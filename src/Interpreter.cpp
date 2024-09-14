#include <fmt/core.h>

#include "Interpreter.h"
#include "Lox.h"
#include "LoxClass.h"

#include <iostream>

namespace Lox
{
    std::any clock(Interpreter&, const std::vector<std::any>&)
    {
        std::time_t t = std::time(nullptr);
        return static_cast<double>(t);
    }

    Interpreter::Interpreter(std::ostream& out) : out(out), globals(std::make_shared<Environment>()), 
    globalEnvironment(globals.get()) 
    {
        globals->define("clock", std::forward<LoxFunction>(LoxFunction{0, &clock}));
        environment = globals;
    }

    Interpreter::~Interpreter() = default;
    
    void Interpreter::interpret(const std::vector<std::shared_ptr<Stmt>>& statements)
    {
        try {
            for(const auto& ptr : statements)
            {
                assert(ptr != nullptr);
                execute(ptr);
            }
        } catch (RuntimeError error) {
            Lox::ReportRuntimeError(error);
        }
    }

    Environment& Interpreter::getGlobalsEnvironment()
    {
        assert(globalEnvironment);
        return *globalEnvironment;
    }

    void Interpreter::execute(std::shared_ptr<Stmt> stmt)
    {
        stmt->accept(*this);
    }

    void Interpreter::executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, 
            std::shared_ptr<Environment> Lenvironment)
    {
        EnterEnvironmentGuard ee{*this, Lenvironment};
        for(const auto& statementPtr : statements) {
          assert(statementPtr != nullptr);
          execute(statementPtr);
        }
    }

    void Interpreter::resolve(const std::shared_ptr<Expr>& expr, int depth)
    {
        locals[expr] = depth;
    }

    std::any Interpreter::visit_block_stmt(std::shared_ptr<Block> stmt)
    {
        auto env = std::make_shared<Environment>(this->environment);
        executeBlock(stmt->getStmt(), env);
        return {}; 
    }

    std::any Interpreter::visit_class_stmt(std::shared_ptr<Class> stmt)
    {
        environment->define(stmt->getName().lexeme, std::any{});
        auto klass = std::make_shared<LoxClass>(stmt->getName().lexeme);
        environment->assign(stmt->getName(), klass);
        return {};
    }

    std::any Interpreter::visit_expression_stmt(std::shared_ptr<Expression> stmt)
    {
        evaluate(stmt->expr);
        return {};
    }

    std::any Interpreter::visit_if_stmt(std::shared_ptr<If> stmt)
    {
        if(isTruthy(evaluate(stmt->condition)))
        {
            execute(stmt->thenBranch);
        } else if (stmt->elseBranch != nullptr)
        {
            execute(stmt->elseBranch);
        }
        return {};
    }

    std::any Interpreter::visit_function_stmt(std::shared_ptr<Function> stmt)
    {
//        const Callable function(&stmt, std::make_unique<Environment>(environment.get()));
        //static_assert(std::is_copy_constructible_v<Callable>);
        //auto fun = Callable(&stmt, std::make_shared<Environment>(*environment));
        auto fun = std::make_shared<LoxFunction>(stmt, environment);
        environment->define(stmt->getName().lexeme, fun);
        return {};
    }

    std::any Interpreter::visit_print_stmt(std::shared_ptr<Print> stmt)
    {
        std::any value = evaluate(stmt->expr);
        // Using cout here because idk how to use the fmt library
        out << stringify(value) << std::endl;
        return {};
    }

    std::any Interpreter::visit_return_stmt(std::shared_ptr<Return> stmt)
    {
        std::any value;
        if(stmt->value.get() != nullptr) 
        {
            value = evaluate(stmt->value);
        }

        throw ReturnException(value);
    }

    std::any Interpreter::visit_var_stmt(std::shared_ptr<Var> stmt)
    {
      std::any value;
      if (stmt->initializer != nullptr)
      {
        value = evaluate(stmt->initializer);
      }

      environment->define(stmt->getName().lexeme, value);
      return {};
    }

    std::any Interpreter::visit_while_stmt(std::shared_ptr<While> stmt)
    {
        while(isTruthy(evaluate(stmt->condition)))
        {
            execute(stmt->body);
        }

        return {};
    }

    std::any Interpreter::visit_assign_expr(std::shared_ptr<Assign> expr)
    {
      std::any value = evaluate(expr->value);
      assert(environment != nullptr);
      if(locals.find(expr) != locals.end())
      {
        int distance = locals.at(expr);
        environment->assignAt(distance, expr->name, value);
      } else
      {
        globals->assign(expr->getName(), value);
      }
      return value;
    }

    std::any Interpreter::visit_literal_expr(std::shared_ptr<Literal> expr)
    {
        return expr->getLiteral();
    }

    std::any Interpreter::visit_logical_expr(std::shared_ptr<Logical> expr)
    {
        std::any left = evaluate(expr->left);

        if(expr->getOp().getType() == TokenType::OR)
        {
            if (isTruthy(left)) return left;
        } else
        {
            if (!isTruthy(left)) return left;
        }

        return evaluate(expr->right);
    }

    std::any Interpreter::visit_set_expr(std::shared_ptr<Set> expr)
    {
        std::any object = evaluate(expr->object);

        if (!(object.type() == typeid(std::shared_ptr<LoxInstance>)))
        {
            throw RuntimeError(expr->name, "Only instances have fields.");
        }

        std::any value = evaluate(expr->value);
        std::any_cast<std::shared_ptr<LoxInstance>>(object)->set(expr->name, value);
        return value;
    }

    std::any Interpreter::visit_grouping_expr(std::shared_ptr<Grouping> expr)
    {
        return evaluate(expr->expr);
    }

    std::any Interpreter::visit_unary_expr(std::shared_ptr<Unary> expr)
    {
        const std::any right = evaluate(expr->right);

        switch(expr->getOp().getType())
        {
            case TokenType::MINUS:
                checkNumberOperand(expr->getOp(), right);
                return -(std::any_cast<double>(right));
            case TokenType::BANG:
                return !isTruthy(right);
            default:
            return std::any{};

        }
    }

    std::any Interpreter::visit_variable_expr(std::shared_ptr<Variable> expr)
    {
      assert(environment != nullptr);
      return lookUpVariable(expr->name, expr);
    }

    std::any Interpreter::lookUpVariable(const Token& name, std::shared_ptr<Expr> expr)
    {
        if(locals.find(expr) != locals.end())
        {
            int dist = locals[expr];
            return environment->getAt(dist, name.lexeme);
        }
        else
        {
            return globals->get(name);
        }
    }

    std::any Interpreter::visit_binary_expr(std::shared_ptr<Binary> expr)
    {
        const std::any left = evaluate(expr->left);
        const std::any right = evaluate(expr->right);

        switch(expr->getOp().getType())
        {   
            case TokenType::GREATER:
                checkNumberOperands(expr->getOp(), left, right);
                return std::any_cast<double>(left) > std::any_cast<double>(right);
            case TokenType::GREATER_EQUAL:
                checkNumberOperands(expr->getOp(), left, right);
                return std::any_cast<double>(left) >= std::any_cast<double>(right);
            case TokenType::LESS:
                checkNumberOperands(expr->getOp(), left, right);
                return std::any_cast<double>(left) < std::any_cast<double>(right);
            case TokenType::LESS_EQUAL:
                checkNumberOperands(expr->getOp(), left, right);
                return std::any_cast<double>(left) <= std::any_cast<double>(right);
            case TokenType::BANG_EQUAL:
                return !isEqual(left, right);
            case TokenType::EQUAL_EQUAL:
                return isEqual(left, right);
            case TokenType::MINUS:
                checkNumberOperands(expr->getOp(), left, right);
                return std::any_cast<double>(left) - std::any_cast<double>(right);
            case TokenType::PLUS:
                if(left.type() == typeid(double) && right.type() == typeid(double))
                    return std::any_cast<double>(left) + std::any_cast<double>(right);

                if(left.type() == typeid(std::string) && right.type() == typeid(std::string))
                    return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);

                throw RuntimeError(expr->getOp(),
                    "Operands must be two numbers or two strings.");  
            case TokenType::SLASH:
                checkNumberOperands(expr->getOp(), left, right);
                return std::any_cast<double>(left) / std::any_cast<double>(right);
            case TokenType::STAR:
                checkNumberOperands(expr->getOp(), left, right);
                return std::any_cast<double>(left) * std::any_cast<double>(right); 
        }

        return std::any{};
    }

    std::any Interpreter::visit_call_expr(std::shared_ptr<Call> expr)
    {
        auto callee = std::make_shared<std::any>(evaluate(expr->callee));

        std::vector<std::any> arguments;
        for(const auto& argument : expr->getArguments())
        {
            arguments.push_back(evaluate(argument));
        }

        // This is a terrible solution, but I made the mistake of using std::any so this code is the result
        // Have to check whether the the callee is a function or a class before casting it in to a Callable 
        // pointer
        std::shared_ptr<Callable> function;

        if(callee->type() == typeid(std::shared_ptr<LoxFunction>))
        {
            function = std::any_cast<std::shared_ptr<LoxFunction>>(*callee);
        }
        else if (callee->type() == typeid(std::shared_ptr<LoxClass>))
        {
            function = std::any_cast<std::shared_ptr<LoxClass>>(*callee);
        }
        else
        {
            throw RuntimeError(expr->getParen(), "Can only call functions and classes.");
        }

        if(arguments.size() != function->getArity()) 
        {
            throw RuntimeError(expr->getParen(), fmt::format("Expected {} arguments, but got {}.",
                function->getArity(), arguments.size()));
        }

        return function->call(*this, arguments);
    }

    std::any Interpreter::visit_get_expr(std::shared_ptr<Get> expr)
    {
        std::any object = evaluate(expr->object);
        if(object.type() == typeid(std::shared_ptr<LoxInstance>))
        {
            return std::any_cast<std::shared_ptr<LoxInstance>>(object)->get(expr->name);
        }

        throw RuntimeError(expr->name, "Only instances have properties.");
    }

    std::string Interpreter::stringify(const std::any& object)
    {
        //Add support for print functions
        if(!object.has_value())
            return "nil";
        if(object.type() == typeid(bool))
            return std::any_cast<bool>(object) ? "true" : "false";
        if(object.type() == typeid(double))
        {
            double n = std::any_cast<double>(object);
            if(std::trunc(n) == n) { // is int
                return std::to_string((int)n);
            } else {
                return std::to_string(n);
            }
        }
        if(object.type() == typeid(std::shared_ptr<LoxFunction>))
            return fmt::format("<fn {}>", std::any_cast<std::shared_ptr<LoxFunction>>(object)->getDeclaration()->getName().lexeme);
        if(object.type() == typeid(std::shared_ptr<LoxClass>))
            return fmt::format("<cl {}>", std::any_cast<std::shared_ptr<LoxClass>>(object)->toString());
        if(object.type() == typeid(std::shared_ptr<LoxInstance>))
            return std::any_cast<std::shared_ptr<LoxInstance>>(object)->toString();
        if(object.type() == typeid(std::string)) 
        {
            return std::any_cast<std::string>(object);
        }
        //assert(false);

        return "";
    } 

    std::any Interpreter::evaluate(std::shared_ptr<Expr> expr)
    {
        return expr->accept(*this);
    }

    bool Interpreter::isTruthy(const std::any& object) const
    {
        if(!object.has_value())
        {
            return false;
        }

        if(object.type() == typeid(bool))
        {
            return std::any_cast<bool>(object);
        }

        return true;
    }

    bool Interpreter::isEqual(const std::any& left, const std::any& right) const
    {
        if(!left.has_value() && !right.has_value())
            return true;
        if(!left.has_value())
            return false;
        if(left.type() != right.type())
        {
            return false;
        }
        if(left.type() == typeid(bool))
        {
            return std::any_cast<bool>(left) == std::any_cast<bool>(right);
        }
        if(left.type() == typeid(double))
        {
            return std::any_cast<double>(left) == std::any_cast<double>(right);
        }
        if(left.type() == typeid(std::string))
        {
            return std::any_cast<std::string>(left) == std::any_cast<std::string>(right);
        }

        return false;
    }

    void Interpreter::checkNumberOperand(const Token& op, const std::any& operand) const
    {
        if(operand.type() == typeid(double)) 
            return;
        throw RuntimeError(op, "Operand must be a number.");
    }

    void Interpreter::checkNumberOperands(const Token& op, 
        const std::any& left, const std::any& right) const
    {
        if(left.type() == typeid(double) && right.type() == typeid(double)) return;

        throw RuntimeError(op, "Operands must be numbers.");
    }

    Interpreter::EnterEnvironmentGuard::EnterEnvironmentGuard(Interpreter& i,
          std::shared_ptr<Environment> env)
    : i(i)
    {
      previous = i.environment;
      i.environment = std::move(env);
    }

    Interpreter::EnterEnvironmentGuard::~EnterEnvironmentGuard()
    {
      i.environment = previous;
    }


}

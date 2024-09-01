#include <fmt/core.h>

#include "Interpreter.h"
#include "Lox.h"

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
        globals->define("clock", std::forward<Callable>(Callable{0, &clock}));
        environment = globals;
    }

    Interpreter::~Interpreter() = default;
    
    void Interpreter::interpret(const std::vector<std::shared_ptr<const Stmt>>& statements)
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

    void Interpreter::execute(std::shared_ptr<const Stmt> stmt)
    {
        stmt->accept(*this);
    }

    void Interpreter::executeBlock(const std::vector<std::shared_ptr<const Stmt>>& statements, 
            std::shared_ptr<Environment> Lenvironment)
    {
        EnterEnvironmentGuard ee{*this, Lenvironment};
        for(const auto& statementPtr : statements) {
          assert(statementPtr != nullptr);
          execute(statementPtr);
        }
    }

    void Interpreter::resolve(const std::shared_ptr<const Expr>& expr, int depth)
    {
        locals[expr] = depth;
    }

    std::any Interpreter::visit_block_stmt(std::shared_ptr<const Block> stmt)
    {
        auto env = std::make_shared<Environment>(this->environment);
        executeBlock(stmt->getStmt(), env);
        return {}; 
    }

    std::any Interpreter::visit_expression_stmt(std::shared_ptr<const Expression> stmt)
    {
        evaluate(stmt->expr);
        return {};
    }

    std::any Interpreter::visit_if_stmt(std::shared_ptr<const If> stmt)
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

    std::any Interpreter::visit_function_stmt(std::shared_ptr<const Function> stmt)
    {
//        const Callable function(&stmt, std::make_unique<Environment>(environment.get()));
        //static_assert(std::is_copy_constructible_v<Callable>);
        //auto fun = Callable(&stmt, std::make_shared<Environment>(*environment));
        auto fun = Callable(stmt, environment);
        environment->define(stmt->getName().lexeme, fun);
        return {};
    }

    std::any Interpreter::visit_print_stmt(std::shared_ptr<const Print> stmt)
    {
        std::any value = evaluate(stmt->expr);
        // Using cout here because idk how to use the fmt library
        out << stringify(value) << std::endl;
        return {};
    }

    std::any Interpreter::visit_return_stmt(std::shared_ptr<const Return> stmt)
    {
        std::any value;
        if(stmt->value.get() != nullptr) 
        {
            value = evaluate(stmt->value);
        }

        throw ReturnException(value);
    }

    std::any Interpreter::visit_var_stmt(std::shared_ptr<const Var> stmt)
    {
      std::any value;
      if (stmt->initializer != nullptr)
      {
        value = evaluate(stmt->initializer);
      }

      environment->define(stmt->getName().lexeme, value);
      return {};
    }

    std::any Interpreter::visit_while_stmt(std::shared_ptr<const While> stmt)
    {
        while(isTruthy(evaluate(stmt->condition)))
        {
            execute(stmt->body);
        }

        return {};
    }

    std::any Interpreter::visit_assign_expr(std::shared_ptr<const Assign> expr)
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

    std::any Interpreter::visit_literal_expr(std::shared_ptr<const Literal> expr)
    {
        return expr->getLiteral();
    }

    std::any Interpreter::visit_logical_expr(std::shared_ptr<const Logical> expr)
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

    std::any Interpreter::visit_grouping_expr(std::shared_ptr<const Grouping> expr)
    {
        return evaluate(expr->expr);
    }

    std::any Interpreter::visit_unary_expr(std::shared_ptr<const Unary> expr)
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

    std::any Interpreter::visit_variable_expr(std::shared_ptr<const Variable> expr)
    {
      assert(environment != nullptr);
      return lookUpVariable(expr->name, expr);
    }

    std::any Interpreter::lookUpVariable(const Token& name, std::shared_ptr<const Expr> expr)
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

    std::any Interpreter::visit_binary_expr(std::shared_ptr<const Binary> expr)
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

    std::any Interpreter::visit_call_expr(std::shared_ptr<const Call> expr)
    {
        std::any callee = evaluate(expr->callee);

        std::vector<std::any> arguments;
        for(const auto& argument : expr->getArguments())
        {
            arguments.push_back(evaluate(argument));
        }

        if(callee.type() != typeid(Callable))
        {
            throw RuntimeError(expr->getParen(), "Can only call functions and classes.");
        }

        auto function = std::any_cast<Callable>(callee);

        if(arguments.size() != function.getArity()) 
        {
            throw RuntimeError(expr->getParen(), fmt::format("Expected {} arguments, but got {}.",
                function.getArity(), arguments.size()));
        }

        return function.call(*this, arguments);
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
        if(object.type() == typeid(Callable))
            return fmt::format("<fn {}>", std::any_cast<Callable>(object).getDeclaration()->getName().lexeme);
        if(object.type() == typeid(std::string)) 
        {
            return std::any_cast<std::string>(object);
        }

        return "";
    } 

    std::any Interpreter::evaluate(std::shared_ptr<const Expr> expr)
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

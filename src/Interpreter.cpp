#include <fmt/core.h>

#include "Interpreter.h"
#include "Lox.h"

namespace Lox
{
    Interpreter::Interpreter(std::ostream& out) : out(out), environment(std::make_unique<Environment>()) 
    {}
    //Interpreter::~Interpreter = default;
    void Interpreter::interpret(const std::vector<std::unique_ptr<Stmt>>& statements)
    {
        try {
            for(const auto& ptr : statements)
            {
                assert(ptr != nullptr);
                execute(*ptr);
            }
        } catch (RuntimeError error) {
            Lox::ReportRuntimeError(error);
        }
    }

    void Interpreter::execute(const Stmt& stmt)
    {
        stmt.accept(*this);
    }

    void Interpreter::executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements, 
            std::unique_ptr<Environment> environment)
    {
        EnterEnvironmentGuard ee{*this, std::move(environment)};
        for(const auto& statementPtr : statements) {
          assert(statementPtr != nullptr);
          execute(*statementPtr);
        }
    }


    std::any Interpreter::visit_block_stmt(const Block& stmt)
    {
        executeBlock(stmt.getStmt(), std::make_unique<Environment>(environment.get()));
        return {}; 
    }

    std::any Interpreter::visit_expression_stmt(const Expression& stmt)
    {
        evaluate(stmt.getExpr());
        return {};
    }

    std::any Interpreter::visit_print_stmt(const Print& stmt)
    {
        std::any value = evaluate(stmt.getExpr());
        // Using cout here because idk how to use the fmt library
        out << stringify(value) + "\n";
        return {};
    }

    std::any Interpreter::visit_var_stmt(const Var& stmt)
    {
      std::any value;
      if (stmt.initializer != nullptr)
      {
        value = evaluate(stmt.getInitializer());
      }

      environment->define(stmt.getName().lexeme, value);
      return {};
    }

    std::any Interpreter::visit_assign_expr(const Assign& expr)
    {
      std::any value = evaluate(expr.getValue());
      assert(environment != nullptr);
      environment->assign(expr.getName(), value);
      return value;
    }

    std::any Interpreter::visit_literal_expr(const Literal& expr)
    {
        return expr.getLiteral();
    }

    std::any Interpreter::visit_grouping_expr(const Grouping& expr)
    {
        return evaluate(expr.getExpr());
    }

    std::any Interpreter::visit_unary_expr(const Unary& expr)
    {
        const std::any right = evaluate(expr.getRight());

        switch(expr.getOp().getType())
        {
            case TokenType::MINUS:
                checkNumberOperand(expr.getOp(), right);
                return -(std::any_cast<double>(right));
            case TokenType::BANG:
                return !isTruthy(right);
            default:
            return std::any{};

        }
    }

    std::any Interpreter::visit_variable_expr(const Variable& expr)
    {
      assert(environment != nullptr);
      return environment->get(expr.getName());
    }

    std::any Interpreter::visit_binary_expr(const Binary& expr)
    {
        const std::any left = evaluate(expr.getLeft());
        const std::any right = evaluate(expr.getRight());

        switch(expr.getOp().getType())
        {   
            case TokenType::GREATER:
                checkNumberOperands(expr.getOp(), left, right);
                return std::any_cast<double>(left) > std::any_cast<double>(right);
            case TokenType::GREATER_EQUAL:
                checkNumberOperands(expr.getOp(), left, right);
                return std::any_cast<double>(left) >= std::any_cast<double>(right);
            case TokenType::LESS:
                checkNumberOperands(expr.getOp(), left, right);
                return std::any_cast<double>(left) < std::any_cast<double>(right);
            case TokenType::LESS_EQUAL:
                checkNumberOperands(expr.getOp(), left, right);
                return std::any_cast<double>(left) <= std::any_cast<double>(right);
            case TokenType::BANG_EQUAL:
                return !isEqual(left, right);
            case TokenType::EQUAL_EQUAL:
                return isEqual(left, right);
            case TokenType::MINUS:
                checkNumberOperands(expr.getOp(), left, right);
                return std::any_cast<double>(left) - std::any_cast<double>(right);
            case TokenType::PLUS:
                if(left.type() == typeid(double) && right.type() == typeid(double))
                    return std::any_cast<double>(left) + std::any_cast<double>(right);

                if(left.type() == typeid(std::string) && right.type() == typeid(std::string))
                    return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);

                throw RuntimeError(expr.getOp(),
                    "Operands must be two numbers or two strings.");  
            case TokenType::SLASH:
                checkNumberOperands(expr.getOp(), left, right);
                return std::any_cast<double>(left) / std::any_cast<double>(right);
            case TokenType::STAR:
                checkNumberOperands(expr.getOp(), left, right);
                return std::any_cast<double>(left) * std::any_cast<double>(right); 
        }

        return std::any{};
    }

    std::string Interpreter::stringify(const std::any& object)
    {
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

        if(object.type() == typeid(std::string)) 
        {
            return std::any_cast<std::string>(object);
        }

        return "";
    } 

    std::any Interpreter::evaluate(const Expr& expr)
    {
        return expr.accept(*this);
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
          std::unique_ptr<Environment> env)
    : i(i)
    {
      previous = std::move(i.environment);
      i.environment = std::move(env);
    }

    Interpreter::EnterEnvironmentGuard::~EnterEnvironmentGuard()
    {
      i.environment = std::move(previous);
    }


}

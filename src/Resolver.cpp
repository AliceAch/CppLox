#include "Resolver.h"

namespace Lox
{
    Resolver::Resolver(Interpreter& interpreter)
        : interpreter(interpreter)
    {}

    std::any Resolver::visit_expression_stmt(std::shared_ptr<Expression> stmt)
    {
        resolve(stmt->expr);
        return {};
    }

    std::any Resolver::visit_if_stmt(std::shared_ptr<If> stmt)
    {
        resolve(stmt->condition);
        resolve(stmt->thenBranch);
        if(stmt->elseBranch != nullptr) resolve(stmt->elseBranch);
        return {};
    }

    std::any Resolver::visit_print_stmt(std::shared_ptr<Print> stmt)
    {
        resolve(stmt->expr);
        return {};
    }

    std::any Resolver::visit_return_stmt(std::shared_ptr<Return> stmt)
    {
        if (currentFunction == FNONE)
        {
            Lox::Error(stmt->keyword, "Can't return from type-level code.");
        }

        if (stmt->value != nullptr)
        {
            if (currentFunction == FunctionType::INITIALIZER)
            {
                Lox::Error(stmt->keyword, "Can't return a value from an initializer.");
            }
            resolve(stmt->value);
        }
        return {};
    } 

    std::any Resolver::visit_while_stmt(std::shared_ptr<While> stmt)
    {
        resolve(stmt->condition);
        resolve(stmt->body);
        return {};
    }

    std::any Resolver::visit_binary_expr(std::shared_ptr<Binary> expr)
    {
        resolve(expr->left);
        resolve(expr->right);
        return {};
    }

    std::any Resolver::visit_call_expr(std::shared_ptr<Call> expr)
    {
        resolve(expr->callee);

        for(const auto& argument : expr->arguments)
        {
            resolve(argument);
        }

        return {};
    }

    std::any Resolver::visit_get_expr(std::shared_ptr<Get> expr)
    {
        resolve(expr->object);
        return {};
    }

    std::any Resolver::visit_block_stmt(std::shared_ptr<Block> stmt)
    {
        beginScope();
        resolve(stmt->stmt);
        endScope();
        return {};
    }

    std::any Resolver::visit_class_stmt(std::shared_ptr<Class> stmt)
    {
        ClassType enclosingClass = currentClass;
        currentClass = ClassType::CLASS;
        declare(stmt->getName());
        define(stmt->getName());

        if (stmt->superclass != nullptr)
        {
            if (stmt->name.lexeme == stmt->superclass->name.lexeme)
            {
                Lox::Error(stmt->superclass->name, "A class can't inherit from itself.");
            }
            currentClass = ClassType::SUBCLASS;
            resolve(stmt->superclass);
        }

        if (stmt->superclass != nullptr)
        {
            beginScope();
            scopes.back()["super"] = true;
        }

        beginScope();
        scopes.back()["this"] = true;

        for (auto& method : stmt->methods)
        {
            FunctionType declaration = FunctionType::METHOD;
            if (method->name.lexeme == "init")
            {
                declaration = FunctionType::INITIALIZER;
            }
            resolveFunction(method, declaration);
        }

        endScope();

        if (stmt->superclass != nullptr)
            endScope();

        currentClass = enclosingClass;
        return {};
    }

    std::any Resolver::visit_grouping_expr(std::shared_ptr<Grouping> expr)
    {
        resolve(expr->expr);
        return {};
    }

    std::any Resolver::visit_literal_expr(std::shared_ptr<Literal> expr)
    {
        return {};
    }

    std::any  Resolver::visit_logical_expr(std::shared_ptr<Logical> expr)
    {
        resolve(expr->left);
        resolve(expr->right);
        return {};
    }

    std::any Resolver::visit_set_expr(std::shared_ptr<Set> expr)
    {
        resolve(expr->value);
        resolve(expr->object);
        return {};
    }    

    std::any Resolver::visit_super_expr(std::shared_ptr<Super> expr)
    {
        if (currentClass == ClassType::CNONE)
        {
            Lox::Error(expr->keyword, "Can't use 'super' outside of a class.");
        } else if (currentClass != ClassType::SUBCLASS)
        {
            Lox::Error(expr->keyword, "Can't use 'super' in a class with no superclass.");
        }
        
        resolveLocal(expr, expr->keyword);
        return {};
    }

    std::any Resolver::visit_this_expr(std::shared_ptr<This> expr)
    {
        if (currentClass == ClassType::CNONE)
        {
            Lox::Error(expr->keyword, "Can't use 'this' outside of a class.");
            return {};
        }
        resolveLocal(expr, expr->keyword);
        return {};
    }

    std::any Resolver::visit_unary_expr(std::shared_ptr<Unary> expr)
    {
        resolve(expr->right);
        return {};
    }

    std::any Resolver::visit_var_stmt(std::shared_ptr<Var> stmt) 
    {
        declare(stmt->getName());
        if(stmt->initializer != nullptr)
        {
            resolve(stmt->initializer);
        }
        define(stmt->getName());
        return {};
    }
    
    std::any Resolver::visit_variable_expr(std::shared_ptr<Variable> expr)
    {
        if(!scopes.empty() &&
            scopes.back().find(expr->getName().lexeme) != scopes.back().end() && !scopes.back()[expr->getName().lexeme])
        {
            Lox::Error(expr->getName(), "Can't read local variable in its own initializer.");
        }

        resolveLocal(expr, expr->getName());
        return {};
    }

    std::any Resolver::visit_assign_expr(std::shared_ptr<Assign> expr)
    {
        resolve(expr->value);
        resolveLocal(expr, expr->name);
        return {};
    }

    std::any Resolver::visit_function_stmt(std::shared_ptr<Function> stmt) 
    {
        declare(stmt->name);
        define(stmt->name);

        resolveFunction(stmt, FUNCTION);
        return {};
    }

    void Resolver::resolve(const std::vector<std::shared_ptr<Stmt>>& statements)
    {
        for (auto& statement : statements)
        {
            resolve(statement);
        }
    }
    void Resolver::resolve(const std::shared_ptr<Stmt>& stmt)
    {
        stmt->accept(*this);
    }
    void Resolver::resolve(const std::shared_ptr<Expr>& expr)
    {
        expr->accept(*this);
    }
    void Resolver::resolveFunction(const std::shared_ptr<Function>& function, FunctionType type)
    {
        FunctionType enclosingFunction = currentFunction;
        currentFunction = type;

        beginScope();
        for(Token param : function->getParams())
        {
            declare(param);
            define(param);
        }
        resolve(function->body);
        endScope();
        currentFunction = enclosingFunction;
    }
    void Resolver::beginScope()
    {
        scopes.emplace_back();
    }
    void Resolver::endScope()
    {
        scopes.pop_back();
    }
    void Resolver::declare(const Token& name) 
    {
        if(scopes.empty())
            return;
        if(scopes.back().find(name.lexeme) != scopes.back().end())
        {
            Lox::Error(name, "Already a variable with this name in this scope.");
        }
        scopes.back()[name.lexeme] = false;
    }
    void Resolver::define(const Token& name)
    {
        if(scopes.empty())
            return;
        scopes.back()[name.lexeme] = true;
    }
    void Resolver::resolveLocal(std::shared_ptr<Expr> expr, const Token& name)
    {
        for(int i = scopes.size() - 1; i >= 0; i--)
        {
            if(scopes[i].find(name.lexeme) != scopes[i].end())
            {
                interpreter.resolve(expr, scopes.size() - 1 - i);
                return;
            }
        }
    }
}
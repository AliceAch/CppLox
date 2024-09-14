#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include "Expr/Expr.h"
#include "Stmt/Stmt.h"
#include "Interpreter.h"
#include "Lox.h"

namespace Lox
{
    class Resolver : exprVisitor<std::any>, stmtVisitor<std::any>
    {
    private:
        enum FunctionType
        {
            NONE,
            FUNCTION
        };

    public:
        explicit Resolver(Interpreter& interpreter);

        std::any visit_block_stmt(std::shared_ptr<Block> stmt) override;
        std::any visit_class_stmt(std::shared_ptr<Class> stmt) override;
        std::any visit_expression_stmt(std::shared_ptr<Expression> stmt) override;
        std::any visit_function_stmt(std::shared_ptr<Function> stmt) override;
        std::any visit_if_stmt(std::shared_ptr<If> stmt) override;
        std::any visit_print_stmt(std::shared_ptr<Print> stmt) override;
        std::any visit_return_stmt(std::shared_ptr<Return> stmt) override;
        std::any visit_var_stmt(std::shared_ptr<Var> stmt) override;
        std::any visit_while_stmt(std::shared_ptr<While> stmt) override;
        
        std::any visit_assign_expr(std::shared_ptr<Assign> expr) override;
        std::any visit_literal_expr(std::shared_ptr<Literal> expr) override;
        std::any visit_logical_expr(std::shared_ptr<Logical> expr) override;
        std::any visit_set_expr(std::shared_ptr<Set> expr) override;
        std::any visit_grouping_expr(std::shared_ptr<Grouping> expr) override;
        std::any visit_unary_expr(std::shared_ptr<Unary> expr) override;
        std::any visit_variable_expr(std::shared_ptr<Variable> expr) override;
        std::any visit_binary_expr(std::shared_ptr<Binary> expr) override;
        std::any visit_call_expr(std::shared_ptr<Call> expr) override;
        std::any visit_get_expr(std::shared_ptr<Get> expr) override;
        void resolve(const std::vector<std::shared_ptr<Stmt>>& stmts);
        void resolve(const std::shared_ptr<Stmt>& stmt);
        void resolve(const std::shared_ptr<Expr>& expr);
        void resolveFunction(const std::shared_ptr<Function>& function, FunctionType type);

    private:

        void beginScope();
        void endScope();
        void declare(const Token& name);
        void define(const Token& name); 
        void resolveLocal(std::shared_ptr<Expr> expr, const Token& name);
        Interpreter& interpreter;
        std::vector<std::unordered_map<std::string, bool>> scopes;
        FunctionType currentFunction = NONE;
    };
}

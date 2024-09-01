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

        std::any visit_block_stmt(std::shared_ptr<const Block> stmt) override;
        std::any visit_expression_stmt(std::shared_ptr<const Expression> stmt) override;
        std::any visit_function_stmt(std::shared_ptr<const Function> stmt) override;
        std::any visit_if_stmt(std::shared_ptr<const If> stmt) override;
        std::any visit_print_stmt(std::shared_ptr<const Print> stmt) override;
        std::any visit_return_stmt(std::shared_ptr<const Return> stmt) override;
        std::any visit_var_stmt(std::shared_ptr<const Var> stmt) override;
        std::any visit_while_stmt(std::shared_ptr<const While> stmt) override;
        
        std::any visit_assign_expr(std::shared_ptr<const Assign> expr) override;
        std::any visit_literal_expr(std::shared_ptr<const Literal> expr) override;
        std::any visit_logical_expr(std::shared_ptr<const Logical> expr) override;
        std::any visit_grouping_expr(std::shared_ptr<const Grouping> expr) override;
        std::any visit_unary_expr(std::shared_ptr<const Unary> expr) override;
        std::any visit_variable_expr(std::shared_ptr<const Variable> expr) override;
        std::any visit_binary_expr(std::shared_ptr<const Binary> expr) override;
        std::any visit_call_expr(std::shared_ptr<const Call> expr) override;
        void resolve(const std::vector<std::shared_ptr<const Stmt>>& stmts);
        void resolve(const std::shared_ptr<const Stmt>& stmt);
        void resolve(const std::shared_ptr<const Expr>& expr);
        void resolveFunction(const std::shared_ptr<const Function>& function, FunctionType type);

    private:

        void beginScope();
        void endScope();
        void declare(const Token& name);
        void define(const Token& name); 
        void resolveLocal(std::shared_ptr<const Expr> expr, const Token& name);
        Interpreter& interpreter;
        std::vector<std::unordered_map<std::string, bool>> scopes;
        FunctionType currentFunction = NONE;
    };
}

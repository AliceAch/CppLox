#pragma once

#include <memory>
#include <any>
#include <cassert>

#include <Token.h>
#include <Expr/Expr.h>

namespace Lox{

    class Expression;
    class Print;

    template <typename R>
    class stmtVisitor
    {
    public:
        virtual R visit_expression_stmt(const Expression& stmt) = 0;
        virtual R visit_print_stmt(const Print& stmt) = 0;
    };

    class Stmt 
    {
    public:
        virtual ~Stmt() = default;

        virtual std::any accept(stmtVisitor<std::any>& StmtVisitor) const = 0;
    };

    class Expression : public Stmt{
    public:
        Expression(std::unique_ptr<Expr> expr) 
        : expr(std::move(expr))
        {
            assert(this->expr != nullptr);
        }

        std::any accept(stmtVisitor<std::any>& StmtVisitor) const 
        {
            return StmtVisitor.visit_expression_stmt(*this);
        }

        const Expr& getExpr() const {return *expr;}

        std::unique_ptr<Expr> expr;
    };

    class Print : public Stmt {
    public:
        Print(std::unique_ptr<Expr> expr) 
        : expr(std::move(expr))
        {
            assert(this->expr != nullptr);
        }

        std::any accept(stmtVisitor<std::any>& StmtVisitor) const
        {
            return StmtVisitor.visit_print_stmt(*this);
        }

        const Expr& getExpr() const {return *expr;}

        std::unique_ptr<Expr> expr;
    };
}
#pragma once

#include <memory>
#include <any>
#include <cassert>
#include <vector>

#include <Token.h>
#include <Expr/Expr.h>

namespace Lox
{
    class Block;
    class Expression;
    class Print;
    class Var;

    template <typename R>
    class stmtVisitor
    {
    public:
        virtual R visit_block_stmt(const Block& stmt) = 0;
        virtual R visit_expression_stmt(const Expression& stmt) = 0;
        virtual R visit_print_stmt(const Print& stmt) = 0;
        virtual R visit_var_stmt(const Var& stmt) = 0;
    };

    class Stmt 
    {
    public:
        virtual ~Stmt() = default;

        virtual std::any accept(stmtVisitor<std::any>& StmtVisitor) const = 0;
    };

    class Block : public Stmt
    {
    public:
        Block(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements))
        {}

        std::any accept(stmtVisitor<std::any>& StmtVisitor) const
        {
          return StmtVisitor.visit_block_stmt(*this);
        }

        const std::vector<std::unique_ptr<Stmt>>& getStmt() const
        {
          return statements;
        }

        std::vector<std::unique_ptr<Stmt>> statements;
    };

    class Expression : public Stmt 
    {
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

    class Print : public Stmt 
    {
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

    class Var : public Stmt 
  {
    public:
      Var(Token& name, std::unique_ptr<Expr> initializer)
      : name(name), initializer(std::move(initializer)) {}

      std::any accept(stmtVisitor<std::any>& StmtVisitor) const override
      {
        return StmtVisitor.visit_var_stmt(*this);
      }
      
      bool hasInitializer() const {return initializer != nullptr;}

      const Token& getName() const {return name;}
      const Expr& getInitializer() const {return *initializer;}

      Token name;
      std::unique_ptr<Expr> initializer;

    };
}

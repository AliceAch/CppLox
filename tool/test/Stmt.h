#pragma once

#include <memory>
#include <any>
#include <cassert>
#include <cmath>

#include "Token.h"
#include "<vector>"
#include ""Expr/Expr.h""


namespace Lox
{
  
  struct Block;
  struct Expression;
  struct Print;
  struct Var;

  template<typename R>
  class stmtVisitor
  {
  public:
    ~stmtVisitor() = default;
    
    virtual R visit_block_stmt(const Block& stmt) = 0;
    virtual R visit_expression_stmt(const Expression& stmt) = 0;
    virtual R visit_print_stmt(const Print& stmt) = 0;
    virtual R visit_var_stmt(const Var& stmt) = 0;
  };

  struct Stmt
  {
    virtual ~Stmt() = default;

    virtual std::any accept(stmtVisitor<std::any>& visitor) const = 0;
  };

  struct Block : public Stmt
  {
    Block(std::vector<std::unique_ptr<Stmt>>& stmt)
        : stmt(std::move(stmt))
    { 
    }

    std::any accept(stmt Visitor<std::any>& visitor) const
    { 
      return visitor.visit_block_stmt(*this); 
    }

    const std::vector<std::unique_ptr<Stmt>>&& getStmt() const { return stmt; }

    std::vector<std::unique_ptr<Stmt>>& stmt;
  };

  struct Expression : public Stmt
  {
    Expression(std::unique_ptr<Expr> expr)
        : expr(std::move(expr))
    { assert(this->expr != nullptr);
    }

    std::any accept(stmt Visitor<std::any>& visitor) const
    { 
      return visitor.visit_expression_stmt(*this); 
    }

    const Expr& getExpr() const { return *expr; }

    std::unique_ptr<Expr> expr;
  };

  struct Print : public Stmt
  {
    Print(std::unique_ptr<Expr> expr)
        : expr(std::move(expr))
    { assert(this->expr != nullptr);
    }

    std::any accept(stmt Visitor<std::any>& visitor) const
    { 
      return visitor.visit_print_stmt(*this); 
    }

    const Expr& getExpr() const { return *expr; }

    std::unique_ptr<Expr> expr;
  };

  struct Var : public Stmt
  {
    Var(Token name, std::unique_ptr<Expr> initialiser)
        : name(std::move(name)), initialiser(std::move(initialiser))
    { 
       assert(this->initialiser != nullptr);
    }

    std::any accept(stmt Visitor<std::any>& visitor) const
    { 
      return visitor.visit_var_stmt(*this); 
    }

    const Token& getName() const { return name; }
    const Expr& getInitialiser() const { return *initialiser; }

    Token name;
    std::unique_ptr<Expr> initialiser;
  };

}

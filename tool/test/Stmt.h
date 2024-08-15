#pragma once

#include <memory>
#include <any>
#include <cassert>
#include <cmath>
#include <vector>

#include "Token.h"
#include "Expr/Expr.h"


namespace Lox
{
  
  struct Block;
  struct Expression;
  struct If;
  struct Print;
  struct Var;

  template<typename R>
  class stmtVisitor
  {
  public:
    ~stmtVisitor() = default;
    
    virtual R visit_block_stmt(const Block& stmt) = 0;
    virtual R visit_expression_stmt(const Expression& stmt) = 0;
    virtual R visit_if_stmt(const If& stmt) = 0;
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
    Block(std::vector<std::unique_ptr<Stmt>> stmt)
        : stmt(std::move(stmt))
    { 
    }

    std::any accept(stmtVisitor<std::any>& visitor) const
    { 
      return visitor.visit_block_stmt(*this); 
    }

    const std::vector<std::unique_ptr<Stmt>>& getStmt() const { return stmt; }

    std::vector<std::unique_ptr<Stmt>> stmt;
  };

  struct Expression : public Stmt
  {
    Expression(std::unique_ptr<Expr> expr)
        : expr(std::move(expr))
    { assert(this->expr != nullptr);
    }

    std::any accept(stmtVisitor<std::any>& visitor) const
    { 
      return visitor.visit_expression_stmt(*this); 
    }

    const Expr& getExpr() const { return *expr; }

    std::unique_ptr<Expr> expr;
  };

  struct If : public Stmt
  {
    If(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch))
    { assert(this->condition != nullptr);
       assert(this->thenBranch != nullptr);
       assert(this->elseBranch != nullptr);
    }

    std::any accept(stmtVisitor<std::any>& visitor) const
    { 
      return visitor.visit_if_stmt(*this); 
    }

    const Expr& getCondition() const { return *condition; }
    const Stmt& getThenbranch() const { return *thenBranch; }
    const Stmt& getElsebranch() const { return *elseBranch; }

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;
  };

  struct Print : public Stmt
  {
    Print(std::unique_ptr<Expr> expr)
        : expr(std::move(expr))
    { assert(this->expr != nullptr);
    }

    std::any accept(stmtVisitor<std::any>& visitor) const
    { 
      return visitor.visit_print_stmt(*this); 
    }

    const Expr& getExpr() const { return *expr; }

    std::unique_ptr<Expr> expr;
  };

  struct Var : public Stmt
  {
    Var(Token name, std::unique_ptr<Expr> initializer)
        : name(std::move(name)), initializer(std::move(initializer))
    { 
       assert(this->initializer != nullptr);
    }

    std::any accept(stmtVisitor<std::any>& visitor) const
    { 
      return visitor.visit_var_stmt(*this); 
    }

    const Token& getName() const { return name; }
    const Expr& getInitializer() const { return *initializer; }

    Token name;
    std::unique_ptr<Expr> initializer;
  };

}

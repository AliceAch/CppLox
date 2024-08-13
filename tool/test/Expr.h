#pragma once

#include <memory>
#include <any>
#include <cassert>
#include <cmath>

#include "Token.h"


namespace Lox
{
  
  struct Assign;
  struct Binary;
  struct Grouping;
  struct Literal;
  struct Unary;
  struct Variable;

  template<typename R>
  class exprVisitor
  {
  public:
    ~exprVisitor() = default;
    
    virtual R visit_assign_expr(const Assign& expr) = 0;
    virtual R visit_binary_expr(const Binary& expr) = 0;
    virtual R visit_grouping_expr(const Grouping& expr) = 0;
    virtual R visit_literal_expr(const Literal& expr) = 0;
    virtual R visit_unary_expr(const Unary& expr) = 0;
    virtual R visit_variable_expr(const Variable& expr) = 0;
  };

  struct Expr
  {
    virtual ~Expr() = default;

    virtual std::any accept(exprVisitor<std::any>& visitor) const = 0;
  };

  struct Assign : public Expr
  {
    Assign(Token name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value))
    { 
       assert(this->value != nullptr);
    }

    std::any accept(expr Visitor<std::any>& visitor) const
    { 
      return visitor.visit_assign_expr(*this); 
    }

    const Token& getName() const { return name; }
    const Expr& getValue() const { return *value; }

    Token name;
    std::unique_ptr<Expr> value;
  };

  struct Binary : public Expr
  {
    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right))
    { assert(this->left != nullptr);
       
       assert(this->right != nullptr);
    }

    std::any accept(expr Visitor<std::any>& visitor) const
    { 
      return visitor.visit_binary_expr(*this); 
    }

    const Expr& getLeft() const { return *left; }
    const Token& getOp() const { return op; }
    const Expr& getRight() const { return *right; }

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
  };

  struct Grouping : public Expr
  {
    Grouping(std::unique_ptr<Expr> expr)
        : expr(std::move(expr))
    { assert(this->expr != nullptr);
    }

    std::any accept(expr Visitor<std::any>& visitor) const
    { 
      return visitor.visit_grouping_expr(*this); 
    }

    const Expr& getExpr() const { return *expr; }

    std::unique_ptr<Expr> expr;
  };

  struct Literal : public Expr
  {
    Literal(std::any literal)
        : literal(std::move(literal))
    { 
    }

    std::any accept(expr Visitor<std::any>& visitor) const
    { 
      return visitor.visit_literal_expr(*this); 
    }

    const std::any& getLiteral() const { return literal; }

    std::any literal;
  };

  struct Unary : public Expr
  {
    Unary(Token op, std::unique_ptr<Expr> right)
        : op(std::move(op)), right(std::move(right))
    { 
       assert(this->right != nullptr);
    }

    std::any accept(expr Visitor<std::any>& visitor) const
    { 
      return visitor.visit_unary_expr(*this); 
    }

    const Token& getOp() const { return op; }
    const Expr& getRight() const { return *right; }

    Token op;
    std::unique_ptr<Expr> right;
  };

  struct Variable : public Expr
  {
    Variable(Token name)
        : name(std::move(name))
    { 
    }

    std::any accept(expr Visitor<std::any>& visitor) const
    { 
      return visitor.visit_variable_expr(*this); 
    }

    const Token& getName() const { return name; }

    Token name;
  };

}

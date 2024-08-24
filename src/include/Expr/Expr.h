#pragma once

#include <memory>
#include <any>
#include <cassert>
#include <cmath>
#include <vector>

#include "Token.h"


namespace Lox
{
  
  struct Assign;
  struct Binary;
  struct Call;
  struct Grouping;
  struct Literal;
  struct Logical;
  struct Unary;
  struct Variable;

  template<typename R>
  class exprVisitor
  {
  public:
    ~exprVisitor() = default;
    
    virtual R visit_assign_expr(const Assign& expr) = 0;
    virtual R visit_binary_expr(const Binary& expr) = 0;
    virtual R visit_call_expr(const Call& expr) = 0;
    virtual R visit_grouping_expr(const Grouping& expr) = 0;
    virtual R visit_literal_expr(const Literal& expr) = 0;
    virtual R visit_logical_expr(const Logical& expr) = 0;
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
        : name(name), value(std::move(value))
    { 
       assert(this->value != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) const
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
        : left(std::move(left)), op(op), right(std::move(right))
    { assert(this->left != nullptr);
       
       assert(this->right != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) const
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

  struct Call : public Expr
  {
    Call(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(paren), arguments(std::move(arguments))
    { assert(this->callee != nullptr);
       
       
    }

    std::any accept(exprVisitor<std::any>& visitor) const
    { 
      return visitor.visit_call_expr(*this); 
    }

    const Expr& getCallee() const { return *callee; }
    const Token& getParen() const { return paren; }
    const std::vector<std::unique_ptr<Expr>>& getArguments() const { return arguments; }

    std::unique_ptr<Expr> callee;
    Token paren;
    std::vector<std::unique_ptr<Expr>> arguments;
  };

  struct Grouping : public Expr
  {
    Grouping(std::unique_ptr<Expr> expr)
        : expr(std::move(expr))
    { assert(this->expr != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) const
    { 
      return visitor.visit_grouping_expr(*this); 
    }

    const Expr& getExpr() const { return *expr; }

    std::unique_ptr<Expr> expr;
  };

  struct Literal : public Expr
  {
    Literal(std::any literal)
        : literal(literal)
    { 
    }

    std::any accept(exprVisitor<std::any>& visitor) const
    { 
      return visitor.visit_literal_expr(*this); 
    }

    const std::any& getLiteral() const { return literal; }

    std::any literal;
  };

  struct Logical : public Expr
  {
    Logical(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right))
    { assert(this->left != nullptr);
       
       assert(this->right != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) const
    { 
      return visitor.visit_logical_expr(*this); 
    }

    const Expr& getLeft() const { return *left; }
    const Token& getOp() const { return op; }
    const Expr& getRight() const { return *right; }

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
  };

  struct Unary : public Expr
  {
    Unary(Token op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right))
    { 
       assert(this->right != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) const
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
        : name(name)
    { 
    }

    std::any accept(exprVisitor<std::any>& visitor) const
    { 
      return visitor.visit_variable_expr(*this); 
    }

    const Token& getName() const { return name; }

    Token name;
  };

}

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
    
    virtual R visit_assign_expr(std::shared_ptr<const Assign> expr) = 0;
    virtual R visit_binary_expr(std::shared_ptr<const Binary> expr) = 0;
    virtual R visit_call_expr(std::shared_ptr<const Call> expr) = 0;
    virtual R visit_grouping_expr(std::shared_ptr<const Grouping> expr) = 0;
    virtual R visit_literal_expr(std::shared_ptr<const Literal> expr) = 0;
    virtual R visit_logical_expr(std::shared_ptr<const Logical> expr) = 0;
    virtual R visit_unary_expr(std::shared_ptr<const Unary> expr) = 0;
    virtual R visit_variable_expr(std::shared_ptr<const Variable> expr) = 0;
  };

  struct Expr
  {
    virtual ~Expr() = default;

    virtual std::any accept(exprVisitor<std::any>& visitor) const = 0;
  };

  struct Assign : public Expr, std::enable_shared_from_this<Assign>
  {
    Assign(Token name, std::shared_ptr<const Expr> value)
        : name(name), value(std::move(value))
    { 
       assert(this->value != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) const
    { 
      return visitor.visit_assign_expr(shared_from_this()); 
    }

    const Token& getName() const { return name; }
    const Expr& getValue() const { return *value; }

    Token name;
    std::shared_ptr<const Expr> value;
  };

  struct Binary : public Expr, std::enable_shared_from_this<Binary>
  {
    Binary(std::shared_ptr<const Expr> left, Token op, std::shared_ptr<const Expr> right)
        : left(std::move(left)), op(op), right(std::move(right))
    { assert(this->left != nullptr);
       
       assert(this->right != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) const
    { 
      return visitor.visit_binary_expr(shared_from_this()); 
    }

    const Expr& getLeft() const { return *left; }
    const Token& getOp() const { return op; }
    const Expr& getRight() const { return *right; }

    std::shared_ptr<const Expr> left;
    Token op;
    std::shared_ptr<const Expr> right;
  };

  struct Call : public Expr, std::enable_shared_from_this<Call>
  {
    Call(std::shared_ptr<const Expr> callee, Token paren, std::vector<std::shared_ptr<const Expr>> arguments)
        : callee(std::move(callee)), paren(paren), arguments(arguments)
    { assert(this->callee != nullptr);
       
       
    }

    std::any accept(exprVisitor<std::any>& visitor) const
    { 
      return visitor.visit_call_expr(shared_from_this()); 
    }

    const Expr& getCallee() const { return *callee; }
    const Token& getParen() const { return paren; }
    const std::vector<std::shared_ptr<const Expr>>& getArguments() const { return arguments; }

    std::shared_ptr<const Expr> callee;
    Token paren;
    std::vector<std::shared_ptr<const Expr>> arguments;
  };

  struct Grouping : public Expr, std::enable_shared_from_this<Grouping>
  {
    Grouping(std::shared_ptr<const Expr> expr)
        : expr(std::move(expr))
    { assert(this->expr != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) const
    { 
      return visitor.visit_grouping_expr(shared_from_this()); 
    }

    const Expr& getExpr() const { return *expr; }

    std::shared_ptr<const Expr> expr;
  };

  struct Literal : public Expr, std::enable_shared_from_this<Literal>
  {
    Literal(std::any literal)
        : literal(literal)
    { 
    }

    std::any accept(exprVisitor<std::any>& visitor) const
    { 
      return visitor.visit_literal_expr(shared_from_this()); 
    }

    const std::any& getLiteral() const { return literal; }

    std::any literal;
  };

  struct Logical : public Expr, std::enable_shared_from_this<Logical>
  {
    Logical(std::shared_ptr<const Expr> left, Token op, std::shared_ptr<const Expr> right)
        : left(std::move(left)), op(op), right(std::move(right))
    { assert(this->left != nullptr);
       
       assert(this->right != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) const
    { 
      return visitor.visit_logical_expr(shared_from_this()); 
    }

    const Expr& getLeft() const { return *left; }
    const Token& getOp() const { return op; }
    const Expr& getRight() const { return *right; }

    std::shared_ptr<const Expr> left;
    Token op;
    std::shared_ptr<const Expr> right;
  };

  struct Unary : public Expr, std::enable_shared_from_this<Unary>
  {
    Unary(Token op, std::shared_ptr<const Expr> right)
        : op(op), right(std::move(right))
    { 
       assert(this->right != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) const
    { 
      return visitor.visit_unary_expr(shared_from_this()); 
    }

    const Token& getOp() const { return op; }
    const Expr& getRight() const { return *right; }

    Token op;
    std::shared_ptr<const Expr> right;
  };

  struct Variable : public Expr, std::enable_shared_from_this<Variable>
  {
    Variable(Token name)
        : name(name)
    { 
    }

    std::any accept(exprVisitor<std::any>& visitor) const
    { 
      return visitor.visit_variable_expr(shared_from_this()); 
    }

    const Token& getName() const { return name; }

    Token name;
  };

}

#pragma once

#include <memory>
#include <any>
#include <cassert>
#include <cmath>

#include "Token.h"

namespace Lox
{
class Assign;  
class Binary;
class Grouping;
class Literal;
class Unary;
class Variable;

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

class Expr 
{
public:
    virtual ~Expr() = default;

    // TODO: figure out a way to accept template exprVisitors? (complicated!)
    virtual std::any accept(exprVisitor<std::any>& exprVisitor) const = 0;
};

class Assign : public Expr
{
public:
  Assign(Token name, std::unique_ptr<Expr> value)
  : name(name), value(std::move(value)) 
  {
    assert(this->value != nullptr);
  }
  std::any accept(exprVisitor<std::any>& exprVisitor) const
  {
    return exprVisitor.visit_assign_expr(*this);
  }

  const Token& getName() const { return name; }
  const Expr& getValue() const { return *value; }

  Token name;
  std::unique_ptr<Expr> value;
};

class Binary : public Expr 
{
public:
    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right) :
    left(std::move(left)),
    op(op),
    right(std::move(right))
    {
        assert(this->left != nullptr);
        assert(this->right != nullptr);
    }
    std::any accept(exprVisitor<std::any>& exprVisitor) const  
    {
        return exprVisitor.visit_binary_expr(*this);
    }

    const Expr& getLeftExpr() const { return *left; }
    const Token& getOp() const { return op; }
    const Expr& getRightExpr() const { return *right; }

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

class Grouping : public Expr 
{
public:
    Grouping(std::unique_ptr<Expr> expr) : expr(std::move(expr))
    {
        assert(this->expr != nullptr);
    }
    std::any accept(exprVisitor<std::any>& visitor) const  
    {
        return visitor.visit_grouping_expr(*this);
    }

    const Expr& getExpr() const { return *expr; }

    std::unique_ptr<Expr> expr;
};

class Literal : public Expr 
{
public:
    Literal(std::any literal) : literal(std::move(literal))
    {}

    std::any accept(exprVisitor<std::any>& visitor) const  
    {
        return visitor.visit_literal_expr(*this);
    }

    const std::any& getLiteral() const { return literal; }

    std::any literal;
};

class Unary : public Expr 
{
public:
    Unary(Token op, std::unique_ptr<Expr> right) : op(op), right(std::move(right))
    {
        assert(this->right != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) const  
    {
        return visitor.visit_unary_expr(*this);
    }

    const Token& getOp() const { return op; }
    const Expr& getRightExpr() const { return *right; }

    Token op;
    std::unique_ptr<Expr> right;
};

class Variable : public Expr 
{
public:
  Variable(Token name)
  : name(name) {}

  std::any accept(exprVisitor<std::any>& visitor) const
  {
    return visitor.visit_variable_expr(*this); 
  }

  const Token& getName() const {return name;}

  Token name;
};

} // end of namespace Lox

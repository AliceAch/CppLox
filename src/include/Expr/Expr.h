#pragma once

#include <memory>
#include <any>
#include <cassert>
#include <cmath>

#include "Token.h"

namespace Lox
{
class Binary;
class Grouping;
class Literal;
class Unary;

template<typename R>
class Visitor {
public:
    ~Visitor() = default;

    virtual R visit_binary_expr(const Binary& expr) = 0;
    virtual R visit_grouping_expr(const Grouping& expr) = 0;
    virtual R visit_literal_expr(const Literal& expr) = 0;
    virtual R visit_unary_expr(const Unary& expr) = 0;
};

class Expr {
public:
    virtual ~Expr() = default;

    // TODO: figure out a way to accept template visitors? (complicated!)
    virtual std::any accept(Visitor<std::any>& visitor) const = 0;
};

class Binary : public Expr {
public:
    Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right) :
    left(std::move(left)),
    op(op),
    right(std::move(right))
    {
        assert(this->left != nullptr);
        assert(this->right != nullptr);
    }
    std::any accept(Visitor<std::any>& visitor) const  
    {
        return visitor.visit_binary_expr(*this);
    }

    const Expr& getLeftExpr() const { return *left; }
    const Token& getOp() const { return op; }
    const Expr& getRightExpr() const { return *right; }

    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
};

class Grouping : public Expr {
public:
    Grouping(std::unique_ptr<Expr> expr) : expr(std::move(expr))
    {
        assert(this->expr != nullptr);
    }
    std::any accept(Visitor<std::any>& visitor) const  
    {
        return visitor.visit_grouping_expr(*this);
    }

    const Expr& getExpr() const { return *expr; }

    std::unique_ptr<Expr> expr;
};

class Literal : public Expr {
public:
    Literal(std::any literal) : literal(std::move(literal))
    {}

    std::any accept(Visitor<std::any>& visitor) const  
    {
        return visitor.visit_literal_expr(*this);
    }

    const std::any& getLiteral() const { return literal; }

    std::any literal;
};

class Unary : public Expr {
public:
    Unary(Token op, std::unique_ptr<Expr> right) : op(op), right(std::move(right))
    {
        assert(this->right != nullptr);
    }

    std::any accept(Visitor<std::any>& visitor) const  
    {
        return visitor.visit_unary_expr(*this);
    }

    const Token& getOp() const { return op; }
    const Expr& getRightExpr() const { return *right; }

    Token op;
    std::unique_ptr<Expr> right;
};

} // end of namespace Lox
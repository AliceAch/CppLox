#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <any>

#include "Token.h"


namespace Lox
{

  struct Binary;
  struct Grouping;
  struct Literal;
  struct Unary;

  struct Expr
  {
    virtual ~Expr() = default;

    class Visitor
    {
    public:
      virtual void visit_binary_expr(const Binary& expr) = 0;
      virtual void visit_grouping_expr(const Grouping& expr) = 0;
      virtual void visit_literal_expr(const Literal& expr) = 0;
      virtual void visit_unary_expr(const Unary& expr) = 0;
    };

    virtual void accept(Visitor&) const {}
  };


  struct Binary : public Expr
  {
    Binary(std::unique_ptr<Expr> left_arg, Token op_arg, std::unique_ptr<Expr> right_arg)
        : left(std::move(left_arg)), op(std::move(op_arg)), right(std::move(right_arg))
    {}

    void accept(Visitor& visitor) const override
    { visitor.visit_binary_expr(*this); }

    std::unique_ptr<Expr> left;
   Token op;
   std::unique_ptr<Expr> right;
  };


  struct Grouping : public Expr
  {
    Grouping(std::unique_ptr<Expr> expression_arg)
        : expression(std::move(expression_arg))
    {}

    void accept(Visitor& visitor) const override
    { visitor.visit_grouping_expr(*this); }

    std::unique_ptr<Expr> expression;
  };


  struct Literal : public Expr
  {
    Literal(std::any literal_arg)
        : literal(std::move(literal_arg))
    {}

    void accept(Visitor& visitor) const override
    { visitor.visit_literal_expr(*this); }

    std::any literal;
  };


  struct Unary : public Expr
  {
    Unary(Token op_arg, std::unique_ptr<Expr> right_arg)
        : op(std::move(op_arg)), right(std::move(right_arg))
    {}

    void accept(Visitor& visitor) const override
    { visitor.visit_unary_expr(*this); }

    Token op;
   std::unique_ptr<Expr> right;
  };

}

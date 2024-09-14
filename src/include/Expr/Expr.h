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
  struct Get;
  struct Grouping;
  struct Literal;
  struct Logical;
  struct Set;
  //struct This;
  struct Unary;
  struct Variable;

  template<typename R>
  class exprVisitor
  {
  public:
    ~exprVisitor() = default;
    
    virtual R visit_assign_expr(std::shared_ptr<Assign> expr) = 0;
    virtual R visit_binary_expr(std::shared_ptr<Binary> expr) = 0;
    virtual R visit_call_expr(std::shared_ptr<Call> expr) = 0;
    virtual R visit_get_expr(std::shared_ptr<Get> expr) = 0;
    virtual R visit_grouping_expr(std::shared_ptr<Grouping> expr) = 0;
    virtual R visit_literal_expr(std::shared_ptr<Literal> expr) = 0;
    virtual R visit_logical_expr(std::shared_ptr<Logical> expr) = 0;
    virtual R visit_set_expr(std::shared_ptr<Set> expr) = 0;
    //virtual R visit_this_expr(std::shared_ptr<This> expr) = 0;
    virtual R visit_unary_expr(std::shared_ptr<Unary> expr) = 0;
    virtual R visit_variable_expr(std::shared_ptr<Variable> expr) = 0;
  };

  struct Expr : public std::enable_shared_from_this<Expr>
  {
    Expr() = default;
    virtual ~Expr() = default;

    virtual std::any accept(exprVisitor<std::any>& visitor) = 0;
  };

  struct Assign : public Expr
  {
    Assign(Token name, std::shared_ptr<Expr> value)
        : name(name), value(std::move(value))
    { 
       assert(this->value != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) 
    { 
      return visitor.visit_assign_expr(std::static_pointer_cast<Assign>(shared_from_this())); 
    }

    const Token& getName() const { return name; }
    const Expr& getValue() const { return *value; }

    Token name;
    std::shared_ptr<Expr> value;
  };

  struct Binary : public Expr
  {
    Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right))
    { assert(this->left != nullptr);
       
       assert(this->right != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) 
    { 
      return visitor.visit_binary_expr(std::static_pointer_cast<Binary>(shared_from_this())); 
    }

    const Expr& getLeft() const { return *left; }
    const Token& getOp() const { return op; }
    const Expr& getRight() const { return *right; }

    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;
  };

  struct Call : public Expr
  {
    Call(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(paren), arguments(std::move(arguments))
    { assert(this->callee != nullptr);
       
       
    }

    std::any accept(exprVisitor<std::any>& visitor) 
    { 
      return visitor.visit_call_expr(std::static_pointer_cast<Call>(shared_from_this())); 
    }

    const Expr& getCallee() const { return *callee; }
    const Token& getParen() const { return paren; }
    const std::vector<std::shared_ptr<Expr>>& getArguments() const { return arguments; }

    std::shared_ptr<Expr> callee;
    Token paren;
    std::vector<std::shared_ptr<Expr>> arguments;
  };
  struct Get : public Expr
  {
    Get(std::shared_ptr<Expr> object, Token name)
        : object(std::move(object)), name(name)
    { assert(this->object != nullptr);
       
    }

    std::any accept(exprVisitor<std::any>& visitor) 
    { 
      return visitor.visit_get_expr(std::static_pointer_cast<Get>(shared_from_this())); 
    }

    const Expr& getObject() const { return *object; }
    const Token& getName() const { return name; }

    std::shared_ptr<Expr> object;
    Token name;
  };
  struct Grouping : public Expr
  {
    Grouping(std::shared_ptr<Expr> expr)
        : expr(std::move(expr))
    { assert(this->expr != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) 
    { 
      return visitor.visit_grouping_expr(std::static_pointer_cast<Grouping>(shared_from_this())); 
    }

    const Expr& getExpr() const { return *expr; }

    std::shared_ptr<Expr> expr;
  };

  struct Literal : public Expr
  {
    Literal(std::any literal)
        : literal(literal)
    { 
    }

    std::any accept(exprVisitor<std::any>& visitor) 
    { 
      return visitor.visit_literal_expr(std::static_pointer_cast<Literal>(shared_from_this())); 
    }

    const std::any& getLiteral() const { return literal; }

    std::any literal;
  };

  struct Logical : public Expr
  {
    Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right))
    { assert(this->left != nullptr);
       
       assert(this->right != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) 
    { 
      return visitor.visit_logical_expr(std::static_pointer_cast<Logical>(shared_from_this())); 
    }

    const Expr& getLeft() const { return *left; }
    const Token& getOp() const { return op; }
    const Expr& getRight() const { return *right; }

    std::shared_ptr<Expr> left;
    Token op;
    std::shared_ptr<Expr> right;
  };
  struct Set : public Expr
  {
    Set(std::shared_ptr<Expr> object, Token name, std::shared_ptr<Expr> value)
        : object(std::move(object)), name(name), value(std::move(value))
    { assert(this->object != nullptr);
       
       assert(this->value != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) 
    { 
      return visitor.visit_set_expr(std::static_pointer_cast<Set>(shared_from_this())); 
    }

    const Expr& getObject() const { return *object; }
    const Token& getName() const { return name; }
    const Expr& getValue() const { return *value; }

    std::shared_ptr<Expr> object;
    Token name;
    std::shared_ptr<Expr> value;
  };
/*
  struct This : public Expr
  {
    This(Token keyword)
        : keyword(keyword)
    { 
    }

    std::any accept(exprVisitor<std::any>& visitor) 
    { 
      return visitor.visit_this_expr(std::static_pointer_cast<This>(shared_from_this())); 
    }

    const Token& getKeyword() const { return keyword; }

    Token keyword;
  };
*/
  struct Unary : public Expr
  {
    Unary(Token op, std::shared_ptr<Expr> right)
        : op(op), right(std::move(right))
    { 
       assert(this->right != nullptr);
    }

    std::any accept(exprVisitor<std::any>& visitor) 
    { 
      return visitor.visit_unary_expr(std::static_pointer_cast<Unary>(shared_from_this())); 
    }

    const Token& getOp() const { return op; }
    const Expr& getRight() const { return *right; }

    Token op;
    std::shared_ptr<Expr> right;
  };

  struct Variable : public Expr
  {
    Variable(Token name)
        : name(name)
    { 
    }

    std::any accept(exprVisitor<std::any>& visitor) 
    { 
      return visitor.visit_variable_expr(std::static_pointer_cast<Variable>(shared_from_this())); 
    }

    const Token& getName() const { return name; }

    Token name;
  };

}

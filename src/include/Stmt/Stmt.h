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
  struct Class;
  struct Expression;
  struct Function;
  struct If;
  struct Print;
  struct Return;
  struct Var;
  struct While;

  template<typename R>
  class stmtVisitor
  {
  public:
    ~stmtVisitor() = default;
    
    virtual R visit_block_stmt(std::shared_ptr<Block> stmt) = 0;
    virtual R visit_class_stmt(std::shared_ptr<Class> stmt) = 0;
    virtual R visit_expression_stmt(std::shared_ptr<Expression> stmt) = 0;
    virtual R visit_function_stmt(std::shared_ptr<Function> stmt) = 0;
    virtual R visit_if_stmt(std::shared_ptr<If> stmt) = 0;
    virtual R visit_print_stmt(std::shared_ptr<Print> stmt) = 0;
    virtual R visit_return_stmt(std::shared_ptr<Return> stmt) = 0;
    virtual R visit_var_stmt(std::shared_ptr<Var> stmt) = 0;
    virtual R visit_while_stmt(std::shared_ptr<While> stmt) = 0;
  };

  struct Stmt : public std::enable_shared_from_this<Stmt>
  {
    Stmt() = default;
    virtual ~Stmt() = default;

    virtual std::any accept(stmtVisitor<std::any>& visitor) = 0;
  };

  struct Block : public Stmt
  {
    Block(std::vector<std::shared_ptr<Stmt>> stmt)
        : stmt(std::move(stmt))
    { 
    }

    std::any accept(stmtVisitor<std::any>& visitor) 
    { 
      return visitor.visit_block_stmt(std::static_pointer_cast<Block>(shared_from_this())); 
    }

    const std::vector<std::shared_ptr<Stmt>>& getStmt() const { return stmt; }

    std::vector<std::shared_ptr<Stmt>> stmt;
  };

  struct Class : public Stmt
  {
    Class(Token name, std::vector<std::shared_ptr<Function>> methods)
        : name(name), methods(std::move(methods))
    { 
       
    }

    std::any accept(stmtVisitor<std::any>& visitor) 
    { 
      return visitor.visit_class_stmt(std::static_pointer_cast<Class>(shared_from_this())); 
    }

    const Token& getName() const { return name; }
    const std::vector<std::shared_ptr<Function>>& getMethods() const { return methods; }

    Token name;
    std::vector<std::shared_ptr<Function>> methods;
  };

  struct Expression : public Stmt
  {
    Expression(std::shared_ptr<Expr> expr)
        : expr(std::move(expr))
    { assert(this->expr != nullptr);
    }

    std::any accept(stmtVisitor<std::any>& visitor) 
    { 
      return visitor.visit_expression_stmt(std::static_pointer_cast<Expression>(shared_from_this())); 
    }

    const Expr& getExpr() const { return *expr; }

    std::shared_ptr<Expr> expr;
  };

  struct Function : public Stmt
  {
    Function(Token name, std::vector<Token> params, std::vector<std::shared_ptr<Stmt>> body)
        : name(name), params(params), body(body)
    { 
      assert(name.getType() == TokenType::IDENTIFIER); 
       
    }

    std::any accept(stmtVisitor<std::any>& visitor) 
    { 
      return visitor.visit_function_stmt(std::static_pointer_cast<Function>(shared_from_this())); 
    }

    const Token& getName() const { return name; }
    const std::vector<Token>& getParams() const { return params; }
    const std::vector<std::shared_ptr<Stmt>>& getBody() const { return body; }

    Token name;
    std::vector<Token> params;
    std::vector<std::shared_ptr<Stmt>> body;
  };

  struct If : public Stmt
  {
    If(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch))
    { assert(this->condition != nullptr);
       assert(this->thenBranch != nullptr);
    }

    std::any accept(stmtVisitor<std::any>& visitor) 
    { 
      return visitor.visit_if_stmt(std::static_pointer_cast<If>(shared_from_this())); 
    }

    const Expr& getCondition() const { return *condition; }
    const Stmt& getThenbranch() const { return *thenBranch; }
    const Stmt& getElsebranch() const { return *elseBranch; }

    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::shared_ptr<Stmt> elseBranch;
  };

  struct Print : public Stmt
  {
    Print(std::shared_ptr<Expr> expr)
        : expr(std::move(expr))
    { assert(this->expr != nullptr);
    }

    std::any accept(stmtVisitor<std::any>& visitor) 
    { 
      return visitor.visit_print_stmt(std::static_pointer_cast<Print>(shared_from_this())); 
    }

    const Expr& getExpr() const { return *expr; }

    std::shared_ptr<Expr> expr;
  };

  struct Return : public Stmt
  {
    Return(Token keyword, std::shared_ptr<Expr> value)
        : keyword(keyword), value(std::move(value))
    { 
    }

    std::any accept(stmtVisitor<std::any>& visitor) 
    { 
      return visitor.visit_return_stmt(std::static_pointer_cast<Return>(shared_from_this())); 
    }

    const Token& getKeyword() const { return keyword; }
    const Expr& getValue() const { return *value; }

    Token keyword;
    std::shared_ptr<Expr> value;
  };

  struct Var : public Stmt
  {
    Var(Token name, std::shared_ptr<Expr> initializer)
        : name(name), initializer(std::move(initializer))
    { 
    }

    std::any accept(stmtVisitor<std::any>& visitor) 
    { 
      return visitor.visit_var_stmt(std::static_pointer_cast<Var>(shared_from_this())); 
    }

    const Token& getName() const { return name; }
    const Expr& getInitializer() const { return *initializer; }

    Token name;
    std::shared_ptr<Expr> initializer;
  };

  struct While : public Stmt
  {
    While(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body))
    { assert(this->condition != nullptr);
       assert(this->body != nullptr);
    }

    std::any accept(stmtVisitor<std::any>& visitor) 
    { 
      return visitor.visit_while_stmt(std::static_pointer_cast<While>(shared_from_this())); 
    }

    const Expr& getCondition() const { return *condition; }
    const Stmt& getBody() const { return *body; }

    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> body;
  };

}

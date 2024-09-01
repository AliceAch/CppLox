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
    
    virtual R visit_block_stmt(std::shared_ptr<const Block> stmt) = 0;
    virtual R visit_expression_stmt(std::shared_ptr<const Expression> stmt) = 0;
    virtual R visit_function_stmt(std::shared_ptr<const Function> stmt) = 0;
    virtual R visit_if_stmt(std::shared_ptr<const If> stmt) = 0;
    virtual R visit_print_stmt(std::shared_ptr<const Print> stmt) = 0;
    virtual R visit_return_stmt(std::shared_ptr<const Return> stmt) = 0;
    virtual R visit_var_stmt(std::shared_ptr<const Var> stmt) = 0;
    virtual R visit_while_stmt(std::shared_ptr<const While> stmt) = 0;
  };

  struct Stmt
  {
    virtual ~Stmt() = default;

    virtual std::any accept(stmtVisitor<std::any>& visitor) const = 0;
  };

  struct Block : public Stmt, std::enable_shared_from_this<Block>
  {
    Block(std::vector<std::shared_ptr<const Stmt>> stmt)
        : stmt(std::move(stmt))
    { 
    }

    std::any accept(stmtVisitor<std::any>& visitor) const
    { 
      return visitor.visit_block_stmt(shared_from_this()); 
    }

    const std::vector<std::shared_ptr<const Stmt>>& getStmt() const { return stmt; }

    std::vector<std::shared_ptr<const Stmt>> stmt;
  };

  struct Expression : public Stmt, std::enable_shared_from_this<Expression>
  {
    Expression(std::shared_ptr<const Expr> expr)
        : expr(std::move(expr))
    { assert(this->expr != nullptr);
    }

    std::any accept(stmtVisitor<std::any>& visitor) const
    { 
      return visitor.visit_expression_stmt(shared_from_this()); 
    }

    const Expr& getExpr() const { return *expr; }

    std::shared_ptr<const Expr> expr;
  };

  struct Function : public Stmt, std::enable_shared_from_this<Function>
  {
    Function(Token name, std::vector<Token> params, std::vector<std::shared_ptr<const Stmt>> body)
        : name(name), params(std::move(params)), body(std::move(body))
    { 
      assert(name.getType() == TokenType::IDENTIFIER);
    }

    std::any accept(stmtVisitor<std::any>& visitor) const
    { 
      return visitor.visit_function_stmt(shared_from_this()); 
    }

    const Token& getName() const { return name; }
    const std::vector<Token>& getParams() const { return params; }
    const std::vector<std::shared_ptr<const Stmt>>& getBody() const { return body; }

    Token name;
    std::vector<Token> params;
    std::vector<std::shared_ptr<const Stmt>> body;
  };

  struct If : public Stmt, std::enable_shared_from_this<If>
  {
    If(std::shared_ptr<const Expr> condition, std::shared_ptr<const Stmt> thenBranch, std::shared_ptr<const Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch))
    { assert(this->condition != nullptr);
       assert(this->thenBranch != nullptr);
    }

    std::any accept(stmtVisitor<std::any>& visitor) const
    { 
      return visitor.visit_if_stmt(shared_from_this()); 
    }

    const Expr& getCondition() const { return *condition; }
    const Stmt& getThenbranch() const { return *thenBranch; }
    const Stmt& getElsebranch() const { return *elseBranch; }

    std::shared_ptr<const Expr> condition;
    std::shared_ptr<const Stmt> thenBranch;
    std::shared_ptr<const Stmt> elseBranch;
  };

  struct Print : public Stmt, std::enable_shared_from_this<Print>
  {
    Print(std::shared_ptr<const Expr> expr)
        : expr(std::move(expr))
    { assert(this->expr != nullptr);
    }

    std::any accept(stmtVisitor<std::any>& visitor) const
    { 
      return visitor.visit_print_stmt(shared_from_this()); 
    }

    const Expr& getExpr() const { return *expr; }

    std::shared_ptr<const Expr> expr;
  };

  struct Return : public Stmt, std::enable_shared_from_this<Return>
  {
    Return(Token keyword, std::shared_ptr<const Expr> value)
        : keyword(keyword), value(std::move(value))
    { 
    }

    std::any accept(stmtVisitor<std::any>& visitor) const
    { 
      return visitor.visit_return_stmt(shared_from_this()); 
    }

    const Token& getKeyword() const { return keyword; }
    const Expr& getValue() const { return *value; }

    Token keyword;
    std::shared_ptr<const Expr> value;
  };

  struct Var : public Stmt, std::enable_shared_from_this<Var>
  {
    Var(Token name, std::shared_ptr<const Expr> initializer)
        : name(name), initializer(std::move(initializer))
    { 
    }

    std::any accept(stmtVisitor<std::any>& visitor) const
    { 
      return visitor.visit_var_stmt(shared_from_this()); 
    }

    const Token& getName() const { return name; }
    const Expr& getInitializer() const { return *initializer; }

    Token name;
    std::shared_ptr<const Expr> initializer;
  };

  struct While : public Stmt, std::enable_shared_from_this<While>
  {
    While(std::shared_ptr<const Expr> condition, std::shared_ptr<const Stmt> body)
        : condition(std::move(condition)), body(std::move(body))
    { assert(this->condition != nullptr);
       assert(this->body != nullptr);
    }

    std::any accept(stmtVisitor<std::any>& visitor) const
    { 
      return visitor.visit_while_stmt(shared_from_this()); 
    }

    const Expr& getCondition() const { return *condition; }
    const Stmt& getBody() const { return *body; }

    std::shared_ptr<const Expr> condition;
    std::shared_ptr<const Stmt> body;
  };

}

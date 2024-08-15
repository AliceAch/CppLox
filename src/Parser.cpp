#include "Parser.h"

#include "Lox.h"

#include <fmt/core.h>

namespace Lox 
{

    Parser::Parser(std::vector<Token> tokens)
        :tokens(tokens) 
    {}

    std::vector<std::unique_ptr<Stmt>> Parser::parse()
    {
        // program → declaration * "EOF" ;
        std::vector<std::unique_ptr<Stmt>> statements;
        while(!isAtEnd())
        {
            statements.push_back(declaration());
        }

        return statements;
    }

    std::unique_ptr<Stmt> Parser::declaration()
    {
      // declaration → varDecl | statement ;
      try {
        if(match(TokenType::VAR))
          return varDeclaration();

        return statement();
      } catch(ParseError error)
      {
        synchronize();
        return nullptr;
      }
    }

    std::unique_ptr<Stmt> Parser::statement()
    {
        // statement → ifStmt 
        //             | forStatement
        //             | printStatement 
        //             | exprStatement 
        //             | whileStatement
        //             | block ;
        if(match(TokenType::IF))
            return ifStatement();
        if(match(TokenType::FOR))
            return forStatement();
        if (match(TokenType::PRINT))
            return printStatement();
        if (match(TokenType::WHILE))
            return whileStatement();
        if (match(TokenType::LEFT_BRACE))
            return std::make_unique<Block>(block());

        return exprStatement();
    }

    std::unique_ptr<Stmt> Parser::forStatement()
    {
        //forStmt → "for" "(" ( varDecl | exprStmt | ";" )
        //          expression? ";"
        //          expression? ")" statement ;
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

        std::unique_ptr<Stmt> initializer;
        if (match(TokenType::SEMICOLON)) {
            initializer = nullptr;
        } else if(match(TokenType::VAR))
        {
            initializer = varDeclaration();
        } else 
        {
            initializer = exprStatement();
        }

        std::unique_ptr<Expr> condition;
        if (!check(TokenType::SEMICOLON)) 
        {
            condition = expression();
        }
        consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

        std::unique_ptr<Expr> increment;
        if (!check(TokenType::RIGHT_PAREN))
        {
            increment = expression();
        }
        consume(TokenType::RIGHT_PAREN, "Expect ')' after clauses.");

        std::unique_ptr<Stmt> body = statement();

        if (increment)
        {
            std::vector<std::unique_ptr<Stmt>> statements;
            statements.push_back(std::move(body));
            statements.push_back(std::make_unique<Expression>(std::move(increment)));
            body = std::make_unique<Block>(std::move(statements));
        }

        if (!condition)
            condition = std::make_unique<Literal>(true);
        body = std::make_unique<While>(std::move(condition), std::move(body));

        if(initializer)
        {
            std::vector<std::unique_ptr<Stmt>> statements;
            statements.push_back(std::move(initializer));
            statements.push_back(std::move(body));
            body = std::make_unique<Block>(std::move(statements));
        }

        return body;
    }

    std::unique_ptr<Stmt> Parser::ifStatement()
    {
        // ifStmt → "if" "(" expression ")" statement ( "else" statement )? ;
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
        std::unique_ptr<Expr> condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect '(' after if condition.");

        std::unique_ptr<Stmt> thenBranch = statement();
        std::unique_ptr<Stmt> elseBranch = nullptr;
        if(match(TokenType::ELSE))
            elseBranch = statement();

        return std::make_unique<If>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
    }

    std::unique_ptr<Stmt> Parser::printStatement()
    {
        // printStatement → "print" expression ";" ;
        std::unique_ptr<Expr> value = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after value.");
        return std::make_unique<Print>(std::move(value));
    }

    std::unique_ptr<Stmt> Parser::exprStatement()
    {
        // exprStatement → expression ";" ;
        std::unique_ptr<Expr> expr = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after expression.");
        return std::make_unique<Expression>(std::move(expr));
    }

    std::vector<std::unique_ptr<Stmt>> Parser::block()
    {
      // block → "{" declaration* "}" ;
      std::vector<std::unique_ptr<Stmt>> statements;

      while(!check(TokenType::RIGHT_BRACE) && !isAtEnd())
      {
        statements.push_back(declaration());
      }

      consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
      return statements;
    }

    std::unique_ptr<Stmt> Parser::varDeclaration()
    {
      Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

      std::unique_ptr<Expr> initializer = nullptr;

      if(match(TokenType::EQUAL))
        initializer = expression();

      consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
      return std::make_unique<Var>(name, std::move(initializer));
    }

    std::unique_ptr<Stmt> Parser::whileStatement()
    {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
        std::unique_ptr<Expr> condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
        std::unique_ptr<Stmt> body = statement();

        return std::make_unique<While>(std::move(condition), std::move(body));
    }

    std::unique_ptr<Expr> Parser::expression()
    {
        // expression → assignment ;
        return assignment();
    }

    std::unique_ptr<Expr> Parser::assignment()
    {
        // assignment → IDENTIFIER "=" assignment | logic_or ;
      auto expr = logicalOr();

      if(match(TokenType::EQUAL))
      {
        auto equals = previous();
        auto value = assignment();

        if(auto* varExpr = dynamic_cast<Variable*>(expr.get()); varExpr)
        {
          return std::make_unique<Assign>(varExpr->getName(), std::move(value));
        }

        error(equals, "Invalid assignment target.");
      }

      return expr;
    }

    std::unique_ptr<Expr> Parser::logicalOr()
    {
        // logic_or → logic_and ( "or" logic_and )* ;
        std::unique_ptr<Expr> expr = logicalAnd();

        while(match(TokenType::OR))
        {
            Token op  = previous();
            std::unique_ptr<Expr> right = logicalAnd();
            expr = std::make_unique<Logical>(std::move(expr), op, std::move(right)); 
        }
        return expr;
    }

    std::unique_ptr<Expr> Parser::logicalAnd()
    {
        // logic_and → equality ( "and" equality )* ;
        std::unique_ptr<Expr> expr = equality();

        while(match(TokenType::AND))
        {
            Token op = previous();
            std::unique_ptr<Expr> right = equality();
            expr = std::make_unique<Logical>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expr> Parser::equality()
    {
        // equality → comparison ( ( "!=" | "==" ) comparison )* ;
        std::unique_ptr<Expr> expr = comparison();

        while(match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL))
        {
            Token op = previous();
            std::unique_ptr<Expr> right = comparison();
            expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
        }

        return expr;
    }
    
    std::unique_ptr<Expr> Parser::comparison()
    {
        //comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
        std::unique_ptr<Expr> expr = term();

        while(match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL))
        {
            Token op = previous();
            std::unique_ptr<Expr> right = term();
            expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expr> Parser::term()
    {
        // term → factor ( ( "-" | "+" ) factor )* ;
        std::unique_ptr<Expr> expr = factor();

        while(match(TokenType::MINUS, TokenType::PLUS))
        {
            Token op = previous();
            std::unique_ptr<Expr> right = factor();
            expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expr> Parser::factor()
    {
        // factor → unary ( ( "/" | "*" ) unary )* ;
        std::unique_ptr<Expr> expr = unary();

        while(match(TokenType::SLASH, TokenType::STAR))
        {
            Token op = previous();
            std::unique_ptr<Expr> right = unary();
            expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expr> Parser::unary() 
    {
        // unary → ( "!" | "-" ) unary | primary ;
        if(match(TokenType::BANG, TokenType::MINUS))
        {
            Token op = previous();
            std::unique_ptr<Expr> right = unary();
            return std::make_unique<Unary>(op, std::move(right)); 
        }
        return primary();
    }

    std::unique_ptr<Expr> Parser::primary()
    {
        // primary → NUMBER | STRING | "false" | "true" | "nil"
        // IDENTIFIER | "(" expression ")" ;
        if(match(TokenType::FALSE))
        {
            return std::make_unique<Literal>(false);
        }
        if(match(TokenType::TRUE))
        {
            return std::make_unique<Literal>(true);
        }
        if(match(TokenType::NIL))
        {
            return std::make_unique<Literal>(std::any{});
        }

        if(match(TokenType::NUMBER, TokenType::STRING))
        {
            return std::make_unique<Literal>(previous().literal);
        }

        if(match(TokenType::IDENTIFIER))
        {
          return std::make_unique<Variable>(previous());
        }

        if(match(TokenType::LEFT_PAREN))
        {
            std::unique_ptr<Expr> expr = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
            return std::make_unique<Grouping>(std::move(expr));
        }

        throw error(peek(), "Expect expression.");
    }

    bool Parser::check(TokenType type) const
    {
        if(isAtEnd())
            return false;
        return peek().getType() == type;
    }

    Token Parser::advance()
    {
        if (!isAtEnd())
            current++;
        return previous(); 
    }

    bool Parser::isAtEnd() const
    {
        return peek().getType() == TokenType::TokenEOF;
    }

    Token Parser::peek() const 
    {
        return tokens.at(current);
    }

    Token Parser::previous() const
    {
        return tokens.at(current - 1);
    }

    void Parser::synchronize()
    {
        advance();

        while(!isAtEnd())
        {
            if(previous().getType() == TokenType::SEMICOLON) 
                return;

            switch(peek().getType())
            {
                case TokenType::CLASS:
                case TokenType::FUN:
                case TokenType::VAR:
                case TokenType::FOR:
                case TokenType::IF:
                case TokenType::WHILE:
                case TokenType::PRINT:
                case TokenType::RETURN:
                    return;
            }

            advance();
        }
    }

    Token Parser::consume(TokenType type, const char* message)
    {
        if(check(type)) 
            return advance();

        throw error(peek(), message);
    }

    Parser::ParseError Parser::error(Token token, const char* message) const
    {
        Lox::Error(token, message);
        return ParseError{};
    }
}

#include "Parser.h"

#include "Lox.h"

#include <fmt/core.h>

#define MAX_FUNCTION_ARGUMENTS 255

namespace Lox 
{

    Parser::Parser(std::vector<Token> tokens)
        :tokens(tokens) 
    {}

    std::vector<std::shared_ptr<Stmt>> Parser::parse()
    {
        // program → declaration * "EOF" ;
        std::vector<std::shared_ptr<Stmt>> statements;
        while(!isAtEnd())
        {
            statements.push_back(declaration());
        }

        return statements;
    }

    std::shared_ptr<Stmt> Parser::declaration()
    {
      // declaration → varDecl | funDecl | statement ;
      try {
        if(match(TokenType::CLASS))
            // classDecl → "class" IDENTIFIER ;
            return classDeclaration();
        if(match(TokenType::FUN))
            // funDecl → "fun" function ;
            return function("function");
        if(match(TokenType::VAR))
            // varDecl → "var" varDeclaration;
            return varDeclaration();

        return statement();
      } catch(ParseError error)
      {
        synchronize();
        return nullptr;
      }
    }

    std::shared_ptr<Stmt> Parser::classDeclaration()
    {

        //classDecl → "class" IDENTIFIER ("<" IDENTIFIER)? "{" function* "}" ;
        Token name = consume(TokenType::IDENTIFIER, "Expect class name.");

        std::shared_ptr<Variable> superclass;
        if (match(TokenType::LESS))
        {
            consume(TokenType::IDENTIFIER, "Expect superclass name.");
            superclass = std::make_shared<Variable>(previous());
        }
        consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");

        std::vector<std::shared_ptr<Function>> methods;
        while(!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        {
            methods.push_back(function("method"));
        }

        consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");

        return std::make_shared<Class>(name, superclass, methods);
    }

    std::shared_ptr<Stmt> Parser::statement()
    {
        // statement → ifStmt 
        //             | forStatement
        //             | printStatement 
        //             | exprStatement 
        //             | whileStatement
        //             | block 
        //             | returnStmt ;
        if (match(TokenType::IF))
            return ifStatement();
        if (match(TokenType::FOR))
            return forStatement();
        if (match(TokenType::PRINT))
            return printStatement();
        if (match(TokenType::RETURN))
            return returnStatement();
        if (match(TokenType::WHILE))
            return whileStatement();
        if (match(TokenType::LEFT_BRACE))
            return std::make_shared<Block>(std::move(block()));

        return exprStatement();
    }

    std::shared_ptr<Stmt> Parser::forStatement()
    {
        //forStmt → "for" "(" ( varDecl | exprStmt | ";" )
        //          expression? ";"
        //          expression? ")" statement ;
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

        std::shared_ptr<Stmt> initializer;
        if (match(TokenType::SEMICOLON)) {
            initializer = nullptr;
        } else if(match(TokenType::VAR))
        {
            initializer = varDeclaration();
        } else 
        {
            initializer = exprStatement();
        }

        std::shared_ptr<Expr> condition;
        if (!check(TokenType::SEMICOLON)) 
        {
            condition = expression();
        }
        consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

        std::shared_ptr<Expr> increment;
        if (!check(TokenType::RIGHT_PAREN))
        {
            increment = expression();
        }
        consume(TokenType::RIGHT_PAREN, "Expect ')' after clauses.");

        std::shared_ptr<Stmt> body = statement();

        if (increment)
        {
            std::vector<std::shared_ptr<Stmt>> statements;
            statements.push_back(std::move(body));
            statements.push_back(std::make_shared<Expression>(std::move(increment)));
            body = std::make_shared<Block>(std::move(statements));
        }

        if (!condition)
            condition = std::make_shared<Literal>(true);
        body = std::make_shared<While>(std::move(condition), std::move(body));

        if(initializer)
        {
            std::vector<std::shared_ptr<Stmt>> statements;
            statements.push_back(std::move(initializer));
            statements.push_back(std::move(body));
            body = std::make_shared<Block>(std::move(statements));
        }

        return body;
    }

    std::shared_ptr<Stmt> Parser::ifStatement()
    {
        // ifStmt → "if" "(" expression ")" statement ( "else" statement )? ;
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
        std::shared_ptr<Expr> condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect '(' after if condition.");

        std::shared_ptr<Stmt> thenBranch = statement();
        std::shared_ptr<Stmt> elseBranch = nullptr;
        if(match(TokenType::ELSE))
            elseBranch = statement();

        return std::make_shared<If>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
    }

    std::shared_ptr<Stmt> Parser::printStatement()
    {
        // printStatement → "print" expression ";" ;
        std::shared_ptr<Expr> value = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after value.");
        return std::make_shared<Print>(std::move(value));
    }

    std::shared_ptr<Stmt> Parser::returnStatement()
    {
        // returnStmt → "return" expression? ";" ;
        Token keyword = previous();
        std::shared_ptr<Expr> value;
        if(!check(TokenType::SEMICOLON))
        {
            value = expression();
        }

        consume(TokenType::SEMICOLON, "Expet ';' after return value");
        return std::make_shared<Return>(keyword, std::move(value));
    }

    std::shared_ptr<Stmt> Parser::exprStatement()
    {
        // exprStatement → expression ";" ;
        std::shared_ptr<Expr> expr = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after expression.");
        return std::make_shared<Expression>(std::move(expr));
    }
    
    std::shared_ptr<Function> Parser::function(std::string kind) 
    {
        // function → IDENTIFIER "(" parameters? ")" block ;
        static const auto errNameMissing = fmt::format("Expect {} name.", kind);
        static const auto errLParenMissing = fmt::format("Expect '(' after {} name.", kind);
        static const auto errLBraceMissing = fmt::format("Expect '{{' before {} body.", kind);

        Token name = consume(TokenType::IDENTIFIER, errNameMissing.c_str());
        consume(TokenType::LEFT_PAREN, errLParenMissing.c_str());

        std::vector<Token> parameters;
        if(!check(TokenType::RIGHT_PAREN))
        {
            do 
            {
                if(parameters.size() >= MAX_FUNCTION_ARGUMENTS)
                {
                    error(peek(), "Can't have more than 255 parameters.");
                }

                parameters.push_back(
                    consume(TokenType::IDENTIFIER, "Expect parameter name."));
            } while(match(TokenType::COMMA));
        }
        consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

        consume(TokenType::LEFT_BRACE, errLBraceMissing.c_str());
        std::vector<std::shared_ptr<Stmt>> body = block();

        return std::make_shared<Function>(name, std::move(parameters), std::move(body)); 
    }

    std::vector<std::shared_ptr<Stmt>> Parser::block()
    {
      // block → "{" declaration* "}" ;
      std::vector<std::shared_ptr<Stmt>> statements;

      while(!check(TokenType::RIGHT_BRACE) && !isAtEnd())
      {
        statements.push_back(declaration());
      }

      consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
      return statements;
    }

    std::shared_ptr<Stmt> Parser::varDeclaration()
    {
      // varDeclaration → IDENTIFIER ("=" expression)? ";" ;
      Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

      std::shared_ptr<Expr> initializer = nullptr;

      if(match(TokenType::EQUAL))
        initializer = expression();

      consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
      return std::make_shared<Var>(name, std::move(initializer));
    }

    std::shared_ptr<Stmt> Parser::whileStatement()
    {
        // whileStmt → "while" "(" expression ")" statement ;
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
        std::shared_ptr<Expr> condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
        std::shared_ptr<Stmt> body = statement();

        return std::make_shared<While>(std::move(condition), std::move(body));
    }

    std::shared_ptr<Expr> Parser::expression()
    {
        // expression → assignment ;
        return assignment();
    }

    std::shared_ptr<Expr> Parser::assignment()
    {
        // assignment → (call ".")? IDENTIFIER "=" assignment | logic_or ;
      auto expr = logicalOr();

      if(match(TokenType::EQUAL))
      {
        auto equals = previous();
        auto value = assignment();
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        // Uh oh...
        if(const auto* varExpr = dynamic_cast<Variable*>(expr.get()); varExpr)
        {
          return std::make_shared<Assign>(varExpr->name, std::move(value));
        } else if (const auto* getExpr = dynamic_cast<Get*>(expr.get()); getExpr)
        {
            return std::make_shared<Set>(getExpr->object, getExpr->name, value);
        }

        error(equals, "Invalid assignment target.");
      }

      return expr;
    }

    std::shared_ptr<Expr> Parser::logicalOr()
    {
        // logic_or → logic_and ( "or" logic_and )* ;
        std::shared_ptr<Expr> expr = logicalAnd();

        while(match(TokenType::OR))
        {
            Token op  = previous();
            std::shared_ptr<Expr> right = logicalAnd();
            expr = std::make_shared<Logical>(std::move(expr), op, std::move(right)); 
        }
        return expr;
    }

    std::shared_ptr<Expr> Parser::logicalAnd()
    {
        // logic_and → equality ( "and" equality )* ;
        std::shared_ptr<Expr> expr = equality();

        while(match(TokenType::AND))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = equality();
            expr = std::make_shared<Logical>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::equality()
    {
        // equality → comparison ( ( "!=" | "==" ) comparison )* ;
        std::shared_ptr<Expr> expr = comparison();

        while(match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = comparison();
            expr = std::make_shared<Binary>(std::move(expr), op, std::move(right));
        }

        return expr;
    }
    
    std::shared_ptr<Expr> Parser::comparison()
    {
        //comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
        std::shared_ptr<Expr> expr = term();

        while(match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = term();
            expr = std::make_shared<Binary>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::term()
    {
        // term → factor ( ( "-" | "+" ) factor )* ;
        std::shared_ptr<Expr> expr = factor();

        while(match(TokenType::MINUS, TokenType::PLUS))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = factor();
            expr = std::make_shared<Binary>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::factor()
    {
        // factor → unary ( ( "/" | "*" ) unary )* ;
        std::shared_ptr<Expr> expr = unary();

        while(match(TokenType::SLASH, TokenType::STAR))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = unary();
            expr = std::make_shared<Binary>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::unary() 
    {
        // unary → ( "!" | "-" ) unary | call ;
        if(match(TokenType::BANG, TokenType::MINUS))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = unary();
            return std::make_shared<Unary>(op, std::move(right)); 
        }
        return call();
    }

    std::shared_ptr<Expr> Parser::call()
    {
        // call → primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
        // arguments → expression ( "," expression )* ;
        std::shared_ptr<Expr> expr = primary();

        while(true)
        {
            if(match(TokenType::LEFT_PAREN))
            {
                expr = finishCall(expr);
            } else if (match(TokenType::DOT))
            {
                Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
                expr = std::make_shared<Get>(expr, name);
            } 
            else 
            {
                break;
            }
        }
        return expr;
    }

    std::shared_ptr<Expr> Parser::primary()
    {
        //primary        → "true" | "false" | "nil" | "this"
        //       | NUMBER | STRING | IDENTIFIER | "(" expression ")"
        //       | "super" "." IDENTIFIER ;
        if(match(TokenType::FALSE))
        {
            return std::make_shared<Literal>(false);
        }
        if(match(TokenType::TRUE))
        {
            return std::make_shared<Literal>(true);
        }
        if(match(TokenType::NIL))
        {
            return std::make_shared<Literal>(std::any{});
        }
        if(match(TokenType::NUMBER, TokenType::STRING))
        {
            return std::make_shared<Literal>(previous().literal);
        }
        if(match(TokenType::SUPER))
        {
            Token keyword = previous();
            consume(TokenType::DOT, "Expect '.' after 'super'.");
            Token method = consume(TokenType::IDENTIFIER, "Expect superclass method name");
            return std::make_shared<Super>(keyword, method);
        }
        if(match(TokenType::THIS))
        {
            return std::make_shared<This>(previous());
        }

        if(match(TokenType::IDENTIFIER))
        {
          return std::make_shared<Variable>(previous());
        }

        if(match(TokenType::LEFT_PAREN))
        {
            std::shared_ptr<Expr> expr = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
            return std::make_shared<Grouping>(std::move(expr));
        }

        throw error(peek(), "Expect expression.");
    }

    std::shared_ptr<Expr> Parser::finishCall(std::shared_ptr<Expr>& callee) 
    {
        std::vector<std::shared_ptr<Expr>> arguments;
        if(!check(TokenType::RIGHT_PAREN))
        {
            do {
                if(arguments.size() >= MAX_FUNCTION_ARGUMENTS)
                {
                    error(peek(), "Can't have more than 255 arguments.");
                }
                arguments.push_back(expression());
            } while (match(TokenType::COMMA));
        }

        Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

        return std::make_shared<Call>(std::move(callee), paren, std::move(arguments));
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

#include "Parser.h"

#include "Lox.h"

#include <fmt/core.h>

namespace Lox 
{

    Parser::Parser(std::vector<Token> tokens)
        :tokens(tokens) 
    {}

    std::unique_ptr<Expr> Parser::parse()
    {
        try {
            return expression();
        } catch(ParseError error)
        {
            return NULL;
        }
    }


    std::unique_ptr<Expr> Parser::expression()
    {
        // expression → assignment ;
        return equality();
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
        // unary → ( "!" | "-" ) unary | unary;
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
            return std::make_unique<Literal>(false);
        if(match(TokenType::TRUE))
            return std::make_unique<Literal>(true);
        if(match(TokenType::NIL))
            return std::make_unique<Literal>(std::any{});

        if(match(TokenType::NUMBER, TokenType::STRING))
        {
            return std::make_unique<Literal>(previous().literal);
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
        if(check(type)) return advance();

        throw error(peek(), message);
    }

    Parser::ParseError Parser::error(Token token, const char* message) const
    {
        Lox::Error(token, message);
        return ParseError{};
    }
}
#pragma once

#include <memory>
#include <stdexcept>
#include <vector>
#include "Expr/Expr.h"
#include "Stmt/Stmt.h"
#include "Token.h"

namespace Lox
{
    class Parser 
    {
        public:
        Parser(std::vector<Token> tokens);
        std::vector<std::unique_ptr<Stmt>> parse();

        private:
        bool check(TokenType type) const;

        template<typename... Args>
        bool match(Args... args);

        Token advance();
        Token peek() const;
        Token previous() const;
        bool isAtEnd() const;

        void synchronize();
        Token consume(TokenType type, const char* message);

        class ParseError : public std::runtime_error {
        public:
            ParseError() : std::runtime_error("") {}    
        };

        ParseError error(Token token, const char* message) const;

        std::unique_ptr<Stmt> statement();
        std::unique_ptr<Stmt> printStatement();
        std::unique_ptr<Stmt> exprStatement();

        std::unique_ptr<Expr> expression();
        std::unique_ptr<Expr> assignment();
        std::unique_ptr<Expr> equality();
        std::unique_ptr<Expr> comparison();    
        std::unique_ptr<Expr> term();
        std::unique_ptr<Expr> factor();
        std::unique_ptr<Expr> unary();
        std::unique_ptr<Expr> primary();

        std::vector<Token> tokens;
        int current{0};
    };

    template<typename... Args>
    bool Parser::match(Args... args)
    {
        std::initializer_list<bool> results {check(args)...};
        for(auto r : results){
            if(r) {
                advance();
                return true;
            }
        }
        return false; 
    }
}
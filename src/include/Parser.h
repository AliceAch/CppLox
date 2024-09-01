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
        std::vector<std::shared_ptr<const Stmt>> parse();

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

        std::shared_ptr<const Expr> finishCall(std::shared_ptr<const Expr>& callee);

        class ParseError : public std::runtime_error {
        public:
            ParseError() : std::runtime_error("") {}    
        };

        ParseError error(Token token, const char* message) const;
        
        std::shared_ptr<const Stmt> declaration();
        std::shared_ptr<const Stmt> statement();
        std::shared_ptr<const Stmt> forStatement();
        std::shared_ptr<const Stmt> ifStatement();
        std::shared_ptr<const Stmt> printStatement();
        std::shared_ptr<const Stmt> returnStatement();
        std::shared_ptr<const Stmt> exprStatement();
        std::shared_ptr<const Stmt> function(std::string kind);
        std::vector<std::shared_ptr<const Stmt>> block();
        std::shared_ptr<const Stmt> varDeclaration();
        std::shared_ptr<const Stmt> whileStatement();

        std::shared_ptr<const Expr> expression();
        std::shared_ptr<const Expr> assignment();
        std::shared_ptr<const Expr> logicalOr();
        std::shared_ptr<const Expr> logicalAnd();
        std::shared_ptr<const Expr> equality();
        std::shared_ptr<const Expr> comparison();    
        std::shared_ptr<const Expr> term();
        std::shared_ptr<const Expr> factor();
        std::shared_ptr<const Expr> unary();
        std::shared_ptr<const Expr> call();
        std::shared_ptr<const Expr> primary();

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

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
        std::vector<std::shared_ptr<Stmt>> parse();

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

        std::shared_ptr<Expr> finishCall(std::shared_ptr<Expr>& callee);

        class ParseError : public std::runtime_error {
        public:
            ParseError() : std::runtime_error("") {}    
        };

        ParseError error(Token token, const char* message) const;
        
        std::shared_ptr<Stmt> declaration();
        std::shared_ptr<Stmt> classDeclaration();
        std::shared_ptr<Stmt> statement();
        std::shared_ptr<Stmt> forStatement();
        std::shared_ptr<Stmt> ifStatement();
        std::shared_ptr<Stmt> printStatement();
        std::shared_ptr<Stmt> returnStatement();
        std::shared_ptr<Stmt> exprStatement();
        std::shared_ptr<Function> function(std::string kind);
        std::vector<std::shared_ptr<Stmt>> block();
        std::shared_ptr<Stmt> varDeclaration();
        std::shared_ptr<Stmt> whileStatement();

        std::shared_ptr<Expr> expression();
        std::shared_ptr<Expr> assignment();
        std::shared_ptr<Expr> logicalOr();
        std::shared_ptr<Expr> logicalAnd();
        std::shared_ptr<Expr> equality();
        std::shared_ptr<Expr> comparison();    
        std::shared_ptr<Expr> term();
        std::shared_ptr<Expr> factor();
        std::shared_ptr<Expr> unary();
        std::shared_ptr<Expr> call();
        std::shared_ptr<Expr> primary();

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

#pragma once

#include <any>
#include <vector>
#include <unordered_map>
#include <string>

#include "Token.h"
#include "TokenType.h"

namespace Lox
{
  class Scanner 
  {
    public:
      Scanner(std::string source);
      std::vector<Token> scanTokens();

    private:
      bool isAtEnd() const;
      char advance();
      void addToken(TokenType type);
      void addToken(TokenType type, std::any literal);
      void scanToken();
      void string();
      void number();
      void identifier();
      void comment();
      bool match(char expected);
      char peek() const;
      char peekNext() const;
      std::string source;
      std::vector<Token> tokens;

      std::unordered_map<std::string, TokenType> keywords;

      int start = 0;
      int current = 0;
      int line = 1;
  };
}

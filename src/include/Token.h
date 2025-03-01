#pragma once

#include <any>
#include <string>

#include <TokenType.h>

namespace Lox
{
  class Token
  {
    public:
      Token(TokenType type, std::string lexeme, std::any literal, int line);
      Token(TokenType type, std::string lexeme, int line);
      std::string toString() const;
      std::string literalToString() const;
      std::string lexeme;
      std::any literal;
      TokenType getType() const;
      int getLine() const;
    private:
      TokenType type;
      int line;
  };
}

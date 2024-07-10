#include "Token.h"

namespace Lox
{
  Token::Token(TokenType type, std::string lexeme, int line) :
    Token(type, lexeme, std::any{}, line)
  {}
  Token::Token(TokenType type, std::string lexeme, std::any literal, int line) :
    type(type),
    lexeme(std::move(lexeme)),
    literal(literal),
    line(line)
  {}

  std::string Token::toString() const
  {
    return std::to_string(static_cast<int>(type)) + ", lexeme: '" + lexeme + "' , literal: '" +
      literalToString() + "'";
  }

  std::string Token::literalToString() const
  {
    switch(type) {
      case TokenType::STRING:
        return std::any_cast<std::string>(literal);
      case TokenType::NUMBER:
        return std::to_string(std::any_cast<double>(literal));
      default:
        return "";
    }

  }
}

#include "Lox.h"
#include "Token.h"

#include <iostream>
#include <fmt/ostream.h>

namespace Lox
{
  bool Lox::HadError=false;
  bool Lox::HadRuntimeError = false;

  void Lox::Report(int line, const std::string& where, const std::string& message)
  {
    fmt::print(std::cerr, "[line {}] Error{}: {}\n", line, where, message);
  }

  void Lox::Error(int line, const std::string& message){
    Report(line, "", message);
    HadError = true;
  }

  void Lox::Error(Token token, const std::string& message)
  {
    if(token.getType() == TokenType::TokenEOF) {
      Report(token.getLine(), " at end", message);
    }
    else 
    {
      Report(token.getLine(), fmt::format(" at '{}'", token.lexeme), message);
    }
    HadError = true;
  }

}

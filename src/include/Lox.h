#pragma once

#include <string>

namespace Lox
{
  class Token;
  class RuntimeError;

  class Lox
  {
    public:
      static void Report(int line, const std::string& where, const std::string& message);
      static void Error(int line, const std::string& message);
      static void Error(Token token, const std::string& message);

      static bool HadError;
      static bool HadRuntimeError;

  };

}

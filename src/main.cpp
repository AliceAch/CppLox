#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <fmt/core.h>
#include "Lox.h"
#include "Scanner.h"
#include "Parser.h"
#include "Interpreter.h"
#include "AstPrinter.h"

#define LOX_VERSION "0.0.1"

namespace 
{
  static Lox::Interpreter interpreter(std::cout);
}

void run(const std::string& source) 
{
  Lox::Scanner scanner(source);
  Lox::Parser parser(scanner.scanTokens());
  std::vector<std::unique_ptr<Lox::Stmt>> statements = parser.parse();

  if (Lox::Lox::HadError) {
    return;
  }
  /*
  std::cout << tokens.size() << std::endl;
  for(auto itr = tokens.begin(); itr != tokens.end(); itr++)
    std::cout << (*itr).toString() << " " << std::endl;
    */
  interpreter.interpret(statements);

}

void runFile(const std::string& path)
{
  std::ifstream file{path};
  if (!file.good()) 
  {
    
    fmt::print("Failed to open {}: No such file or directory\n", path);
    return;
  }

  std::string line;
  std::string source;
  while(std::getline(file,line)) {
    source += line+"\n";
  }
  run(source);
  if (Lox::Lox::HadError)
    exit(2);
  if(Lox::Lox::HadRuntimeError)
    exit(3);
}

void runPrompt()
{
  fmt::print("lox v{}\n", LOX_VERSION);

  std::string code;
  while(true) {
    fmt::print("> ");
    if(std::getline(std::cin,code)) {
      run(code);
      Lox::Lox::HadError = false;
    } else{
      fmt::print("\n");
      break;
    }
  }
}

int main(int args, char* argv[])
{
  if (args>2){
    fmt::print("usage: lox [script]\n");
    exit(1);
  } else if(args == 2) {
    runFile(argv[1]);
  } else  {
    runPrompt();
  }
  return 0;
}

/*
int main(int args, char* argv[])
{
  Lox::Expr* expression = new Lox::Binary(
      std::make_unique<Lox::Unary>(
        Lox::Token(Lox::TokenType::MINUS, "-", NULL, 1),
        std::make_unique<Lox::Literal>(123)),
      Lox::Token(Lox::TokenType::STAR, "*", NULL, 1),
      std::make_unique<Lox::Grouping>(std::make_unique<Lox::Literal>(45.67)));
  Lox::AstPrinter printer;
  std::cout << printer.print(*expression) << std::endl;
}
*/

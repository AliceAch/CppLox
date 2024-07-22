#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
// so this interpreter will only work on my system
#include <fmt/core.h>
#include "Lox.h"
#include "Scanner.h"
#include "AstPrinter.h"

#define LOX_VERSION "0.0.1"

void run(const std::string& source) 
{
  Lox::Scanner scanner(source);
  std::vector<Lox::Token> tokens = scanner.scanTokens();
  if (Lox::Lox::HadError) {
    return;
  }
  std::cout << tokens.size() << std::endl;
  for(auto itr = tokens.begin(); itr != tokens.end(); itr++)
    std::cout << (*itr).toString() << " " << std::endl;
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
using namespace Lox;
int main(int args, char* argv[])
{
  Expr* expression = new Binary(
      std::make_unique<Unary>(
        Token(TokenType::MINUS, "-", NULL, 1),
        std::make_unique<Literal>(123)),
      Token(TokenType::STAR, "*", NULL, 1),
      std::make_unique<Grouping>(std::make_unique<Literal>(45.67)));
  AstPrinter printer;
  std::cout << "Printing:" << printer.print(*expression) << std::endl;
}
*/

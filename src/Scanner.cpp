#include "Scanner.h"
#include "Lox.h"
#include <cctype>
#include <fmt/core.h>
// testing comment
namespace Lox 
{
  Scanner::Scanner(std::string source) : source(std::move(source))
  {
    keywords = 
    {
			{"and", TokenType::AND},
      {"class", TokenType::CLASS},
      {"else", TokenType::ELSE},
      {"false", TokenType::FALSE},
      {"for", TokenType::FOR},
      {"fun", TokenType::FUN},
      {"if", TokenType::IF},
      {"nil", TokenType::NIL},
      {"or", TokenType::OR},
      {"print", TokenType::PRINT},
      {"return", TokenType::RETURN},
      {"super", TokenType::SUPER},
      {"this", TokenType::THIS},
      {"true", TokenType::TRUE},
      {"var", TokenType::VAR},
      {"while", TokenType::WHILE},
    };
  }
  std::vector<Token> Scanner::scanTokens() 
  {
    while(!isAtEnd()) {
      // We are at the beginning of the next lexeme.
      start = current;
      scanToken();
    }
    tokens.emplace_back(TokenType::TokenEOF, "", line);
    return tokens;
  }
  void Scanner::scanToken() 
  {
    auto c = advance();
    switch (c) {
      case '(': addToken(TokenType::LEFT_PAREN); break;
      case ')': addToken(TokenType::RIGHT_PAREN); break;
      case '{': addToken(TokenType::LEFT_BRACE); break;
      case '}': addToken(TokenType::RIGHT_BRACE); break;
      case ',': addToken(TokenType::COMMA); break;
      case '.': addToken(TokenType::DOT); break;
      case '-': addToken(TokenType::MINUS); break;
      case '+': addToken(TokenType::PLUS); break;
      case ';': addToken(TokenType::SEMICOLON); break;
      case '*': addToken(TokenType::STAR); break;
      case '!':
        addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
      case '=':
        addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
      case '<':
        addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        break;
      case '>':
        addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        break;
      case '/':
        if (match('/')) {
          // A comment goes until the end of the line.
          while (peek() != '\n' && !isAtEnd())
            advance();
        } else {
          addToken(TokenType::SLASH);
        }
        break;
      case ' ':
      case '\r':
      case '\t':
        // Ignore whitespace
        break;
      case '\n':
        line++;
        break;
      case '"': string(); break;
      default:  
        if(std::isdigit(c)){
          number();
        } else if (std::isalpha(c)){
          identifier();
        } else {
          Lox::Error(line, "Unexpected character."); break;
        }
    }
  }
  void Scanner::string()
  {
    while(peek() != '"' && !isAtEnd())
    {
      if(peek() == '\n') line++;
      advance();
    }

    if (isAtEnd()) 
    {
      Lox::Error(line, "Unterminated string.");
      return;
    }

    // the closing ".
    advance();
    
    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
  }

  void Scanner::number()
  {
    while(std::isdigit(peek())) advance();
    // Look for a fractional part.
    if (peek() == '.' && std::isdigit(peekNext())){
      // Consume the "."
      advance();

      while(std::isdigit(peek())) advance();
    }
    addToken(TokenType::NUMBER, std::stod(source.substr(start, current - start)));
  }

  void Scanner::identifier()
  {
    //char c = peek();
    while(std::isalpha(peek()) || std::isdigit(peek()) || peek() == '_'){ 
			advance();
		}

		const auto text = source.substr(start, current - start);
		if(const auto it = keywords.find(text); it != keywords.end()) {
			addToken(it->second);
		}	else {
			addToken(TokenType::IDENTIFIER);
		}	
  }
  bool Scanner::match(char expected) 
  {
    if (isAtEnd()) 
      return false;
    if (source.at(current) != expected)
      return false;

    current++;
    return true;
  }
  char Scanner::peek() const 
  {
    if (isAtEnd())
      return '\0';
    return source.at(current);
  }
  char Scanner::peekNext() const
  {
    if(current + 1 >= static_cast<int>(source.size())) return '\0';
    return source.at(current+1);
  }
  bool Scanner::isAtEnd() const
  {
    return current >= static_cast<int>(source.size());
  }
  char Scanner::advance()
  {
    current++;
    return source.at(current-1);
  }
  void Scanner::addToken(TokenType type)
  {
    addToken(type, NULL);
  }
  void Scanner::addToken(TokenType type, std::any literal)
  {
    std::string text = source.substr(start, current-start);
    tokens.emplace_back(type, text, literal, line);
  }

}

#include "Environment.h"

#include "RuntimeError.h"
#include "Token.h"

#include <cassert>

#include <fmt/core.h>

namespace Lox
{

  Environment::Environment()
  : enclosing(nullptr)    
  {}
  Environment::Environment(std::shared_ptr<Environment> enclosing)
  : enclosing(enclosing) 
  {
    assert(this->enclosing != nullptr);
  }
  
/*
  Environment::Environment(const Environment& other)
  : enclosing(other.enclosing)
  {}
*/
  const std::any& Environment::get(const Token& name) const
  {
    auto it = values.find(name.lexeme); 
    if(it != values.end())
    {
      return it->second;
    }

    if(enclosing != nullptr)
      return enclosing->get(name);

    throw RuntimeError(name, fmt::format("Undefined variable '{}'.", name.lexeme));
  }

  const std::any& Environment::getAt(int distance, const std::string& name)
  {
    return ancestor(distance)->values.at(name);
  }

  std::shared_ptr<Environment> Environment::ancestor(int distance)
  {
    auto environment = shared_from_this();
    for (int i = 0; i < distance; i++)
    {
      environment = environment->enclosing;
    }

    return environment;
  }

  void Environment::assign(const Token& name, const std::any& value)
  {
    auto it = values.find(name.lexeme);
    if(it != values.end())
    {
      it->second = value;
      return;
    }

    if(enclosing != nullptr)
    {
      enclosing->assign(name, value);
      return;
    }

    throw RuntimeError(name, fmt::format("Undefined variable '{}'.", name.lexeme));
  }

  void Environment::assignAt(int distance, const Token& name, std::any& value)
  {
    ancestor(distance)->values[name.lexeme] = value;
  }

  void Environment::define(const std::string& name, const std::any& value)
  {
    values.emplace(name, value); 
  }
}


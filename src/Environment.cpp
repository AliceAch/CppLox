#include "Environment.h"

#include "RuntimeError.h"
#include "Token.h"

#include <cassert>

#include <fmt/core.h>

namespace Lox
{

  Environment::Environment()
  : enclosing(nullptr) {}

  Environment::Environment(std::shared_ptr<Environment> enclosing)
  : enclosing(std::move(enclosing)) 
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

  void Environment::define(const std::string& name, const std::any& value)
  {
    values.emplace(name, value); 
  }
}


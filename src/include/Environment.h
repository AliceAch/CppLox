#pragma once

#include <any>
#include <string>
#include <unordered_map>
#include <memory>

namespace Lox
{
  class Token;

  class Environment
  {
    public:
    Environment();
    Environment(std::shared_ptr<Environment> enclosing);

    const std::any& get(const Token& name) const;

    void assign(const Token& name, const std::any& value);
    void define(const std::string& name, const std::any& value);
    
    private:
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string, std::any> values;
  };
}

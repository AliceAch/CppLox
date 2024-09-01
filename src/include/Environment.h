#pragma once

#include <any>
#include <string>
#include <unordered_map>
#include <memory>

namespace Lox
{
  class Token;

  class Environment : public std::enable_shared_from_this<Environment>
  {
    public:
    Environment();
    Environment(std::shared_ptr<Environment> enclosing);

    const std::any& get(const Token& name) const;

    const std::any& getAt(int distance, const std::string& name);
    std::shared_ptr<Environment> ancestor(int distance);

    void assign(const Token& name, const std::any& value);
    void assignAt(int distance, const Token& name, std::any& value);

    void define(const std::string& name, const std::any& value);
    
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string, std::any> values;
  };
}

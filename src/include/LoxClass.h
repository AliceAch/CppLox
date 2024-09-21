#pragma once

#include "Callable.h"
#include "LoxInstance.h"

#include <vector>
#include <unordered_map>
#include <string>

namespace Lox
{
    class LoxClass : public Callable, public std::enable_shared_from_this<LoxClass> 
    {
    public:
        LoxClass(const std::string& name, std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods);
        std::shared_ptr<LoxFunction> findMethod(const std::string& name) const;
        std::any call(Interpreter& interpreter, const std::vector<std::any>& arguments) override;
        int getArity() override;

        std::string toString();
        std::string name;
        std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods;
    };
}
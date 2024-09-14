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
        LoxClass(std::string name);
        std::any call(Interpreter& interpreter, const std::vector<std::any>& arguments) override;
        int getArity() const override;

        std::string toString();
        std::string name;
    };
}
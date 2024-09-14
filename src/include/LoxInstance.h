#pragma once

#include "Token.h"

#include <unordered_map>
#include <any>
#include <memory>
#include <string>

namespace Lox
{
    class LoxClass;

    class LoxInstance
    {
    public:
        LoxInstance(const std::shared_ptr<LoxClass>& klass);

        std::any get(const Token& name);
        void set(Token& name, std::any& value);

        std::string toString() ;
    private:
        std::shared_ptr<LoxClass> klass;
        std::unordered_map<std::string, std::any> fields;
    };
}
#include "LoxInstance.h"
#include "LoxClass.h"
#include "RuntimeError.h"

#include <utility>

namespace Lox
{
    LoxInstance::LoxInstance(const std::shared_ptr<LoxClass>& klass)
    : klass(klass)
    {}

    std::any LoxInstance::get(const Token& name)
    {
        if(fields.find(name.lexeme) != fields.end())
            return fields.at(name.lexeme);

        throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
    }

    void LoxInstance::set(Token& name, std::any& value)
    {
        fields[name.lexeme] = value;
    }

    std::string LoxInstance::toString()
    {
        return klass->name + " instance";
    }

}
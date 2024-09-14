#include "LoxClass.h"

namespace Lox
{
    LoxClass::LoxClass(std::string name)
        : name(name)
    {}

    std::any LoxClass::call(Interpreter& interpreter, const std::vector<std::any>& arguments) 
    {
        std::shared_ptr<LoxInstance> instance = std::make_shared<LoxInstance>(std::static_pointer_cast<LoxClass>(shared_from_this()));
        return instance;
    }
    int LoxClass::getArity() const 
    {
        return 0;
    }

    std::string LoxClass::toString()
    {
        return name;
    }

}


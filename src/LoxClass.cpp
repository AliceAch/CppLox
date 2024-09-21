#include "LoxClass.h"

namespace Lox
{
    LoxClass::LoxClass(const std::string& name, std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods)
        : name(name), methods(methods)
    {}

    std::shared_ptr<LoxFunction> LoxClass::findMethod(const std::string& name) const
    {
        if (methods.find(name) != methods.end())
            return methods.at(name);

        return nullptr;
    }

    std::any LoxClass::call(Interpreter& interpreter, const std::vector<std::any>& arguments) 
    {
        std::shared_ptr<LoxInstance> instance = std::make_shared<LoxInstance>(std::static_pointer_cast<LoxClass>(shared_from_this()));
        std::shared_ptr<LoxFunction> initializer = findMethod("init");
        if (initializer != nullptr)
        {
            initializer->bind(instance)->call(interpreter, arguments);
        }
        return instance;
    }
    int LoxClass::getArity() 
    {
        std::shared_ptr<LoxFunction> initializer = findMethod("init");
        if (initializer == nullptr)
            return 0;
        return initializer->getArity();
    }

    std::string LoxClass::toString()
    {
        return name;
    }

}


#pragma once

#include <any>
#include <functional>
#include <memory>
#include <vector>

namespace Lox
{
    class Interpreter;
    class Function;
    class Environment;

    using FuncType = std::function<std::any(Interpreter&, const std::vector<std::any>&)>;

    class Callable
    {
    public:
        Callable(int arity, FuncType f);
        Callable(std::shared_ptr<const Function> declaration, std::shared_ptr<Environment> closure);

        //Callable(const Callable& other);

        std::any call(Interpreter& interpreter, const std::vector<std::any> arguments) const;

        int getArity() const {return arity;}
        const std::shared_ptr<const Function> getDeclaration() const {return declaration;}
    private:
        int arity;
        FuncType f;

        std::shared_ptr<const Function> declaration;
        std::shared_ptr<Environment> closure;
    };

}
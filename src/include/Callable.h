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
    class LoxInstance;

    using FuncType = std::function<std::any(Interpreter&, const std::vector<std::any>&)>;

    class Callable 
    {
    public:
        //Callable(int arity, FuncType f);
        //Callable(std::shared_ptr<Function> declaration, std::shared_ptr<Environment> closure);

        //Callable(const Callable& other);

        virtual std::any call(Interpreter& interpreter, const std::vector<std::any>& arguments) = 0;

        virtual int getArity() = 0;
        //const std::shared_ptr<Function> getDeclaration() const {return declaration;}
        virtual ~Callable() = default;
    };

    class LoxFunction : public Callable
    {
    public:
        FuncType f;
        int arity = 0;

        LoxFunction(int arity, FuncType f);
        LoxFunction(std::shared_ptr<Function> declaration, std::shared_ptr<Environment> closure, bool isInitializer);
        std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> instance);
        std::any call(Interpreter& i, const std::vector<std::any>& arguments) override;
        int getArity() override;
        const std::shared_ptr<Function> getDeclaration() const {return declaration;}
        

    private:
        std::shared_ptr<Function> declaration;
        std::shared_ptr<Environment> closure;
        bool isInitializer;
    };

}
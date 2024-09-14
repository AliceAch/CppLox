#include "Callable.h"

#include "Interpreter.h"
#include "Stmt/Stmt.h"

#include <cassert>

namespace Lox
{
    LoxFunction::LoxFunction(int arity, FuncType f) : arity(arity), f(f), declaration(nullptr)
    {}

    LoxFunction::LoxFunction(std::shared_ptr<Function> declaration, std::shared_ptr<Environment> closure) : 
    declaration(std::move(declaration)), closure(std::move(closure))
    {
    }
/*
    Callable::Callable(const Callable& other)
    :
        arity(other.arity), f(other.f), declaration(other.declaration),
        // This line right here is causing all of the issues
        closure(std::make_shared<Environment>(*other.closure))
    {}
*/
    std::any LoxFunction::call(Interpreter& interpreter, const std::vector<std::any>& arguments)
    {
        if (!declaration) 
        {
            return f(interpreter, arguments);
        }

        const auto& params = declaration->getParams();
        assert(params.size() == arguments.size());

        // There is something wrong with this line
        auto env = std::make_shared<Environment>(closure);
        //auto env = closure;
        for(std::size_t i = 0u; i < params.size(); ++i)
        {
            env->define(params.at(i).lexeme, arguments.at(i));
        }

        try {
            interpreter.executeBlock(declaration->getBody(), env);
        } catch(const ReturnException& v)
        {
            return v.getValue();
        }

        return std::any{};
    }
    int LoxFunction::getArity() const
    {
        return static_cast<int>(declaration->getParams().size());
    }
}
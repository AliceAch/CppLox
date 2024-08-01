#include <memory>
#include "Expr/Expr.h"
#include "RuntimeError.h"


namespace Lox
{
    class Interpreter : Visitor<std::any>
    {
    public:
        Interpreter();
        //~Interpreter();
        void interpret(std::unique_ptr<Expr>& expression);
    private:
        std::any visit_literal_expr(const Literal& expr) override;
        std::any visit_grouping_expr(const Grouping& expr) override;
        std::any visit_unary_expr(const Unary& expr) override;
        std::any visit_binary_expr(const Binary& expr) override;
        
        std::string stringify(const std::any& object);
        std::any evaluate(const Expr& expr);
        bool isTruthy(const std::any& object) const;
        bool isEqual(const std::any& a, const std::any& b) const;
        void checkNumberOperand(const Token& op, const std::any& operand) const; 
        void checkNumberOperands(const Token& op, 
            const std::any& left, const std::any& right) const;
        
        
    };

}
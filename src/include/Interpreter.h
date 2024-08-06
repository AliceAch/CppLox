#include <memory>
#include <iosfwd>
#include <vector>

#include "Expr/Expr.h"
#include "Stmt/Stmt.h"
#include "RuntimeError.h"


namespace Lox
{
    class Interpreter : exprVisitor<std::any>, stmtVisitor<std::any>
    {
    public:
        Interpreter(std::ostream& out);
        //~Interpreter();
        void interpret(const std::vector<std::unique_ptr<Stmt>>& statements);
        void execute(const Stmt& stmt);
    private:
        std::any visit_expression_stmt(const Expression& stmt) override;
        std::any visit_print_stmt(const Print& stmt) override;

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
        
        std::ostream& out;
    };

}
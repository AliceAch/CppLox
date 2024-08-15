#include <memory>
#include <iosfwd>
#include <vector>  

#include "Environment.h"
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
        void executeBlock(const std::vector<std::unique_ptr<Stmt>>& statements, 
            std::unique_ptr<Environment> environment);
    private:
        std::any visit_block_stmt(const Block& stmt) override;
        std::any visit_expression_stmt(const Expression& stmt) override;
        std::any visit_if_stmt(const If& stmt) override;
        std::any visit_print_stmt(const Print& stmt) override;
        std::any visit_var_stmt(const Var& stmt) override;
        std::any visit_while_stmt(const While& stmt) override;
        
        std::any visit_assign_expr(const Assign& expr) override;
        std::any visit_literal_expr(const Literal& expr) override;
        std::any visit_logical_expr(const Logical& expr) override;
        std::any visit_grouping_expr(const Grouping& expr) override;
        std::any visit_unary_expr(const Unary& expr) override;
        std::any visit_variable_expr(const Variable& expr) override;
        std::any visit_binary_expr(const Binary& expr) override;
        
        std::string stringify(const std::any& object);
        std::any evaluate(const Expr& expr);
        bool isTruthy(const std::any& object) const;
        bool isEqual(const std::any& a, const std::any& b) const;
        void checkNumberOperand(const Token& op, const std::any& operand) const; 
        void checkNumberOperands(const Token& op, 
            const std::any& left, const std::any& right) const;
        
        
        // data
        std::unique_ptr<Environment> environment;

        class EnterEnvironmentGuard 
        {
        public:
          EnterEnvironmentGuard(Interpreter& i, std::unique_ptr<Environment> env);
          ~EnterEnvironmentGuard();

        private:
          Interpreter& i;
          std::unique_ptr<Environment> previous;
        };

        std::ostream& out;
    };

}

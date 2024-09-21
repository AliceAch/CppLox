#pragma once

#include <memory>
#include <iosfwd>
#include <vector>  
#include <ctime>

#include "Environment.h"
#include "Expr/Expr.h"
#include "Stmt/Stmt.h"
#include "RuntimeError.h"
#include "ReturnException.h"
#include "Callable.h"


namespace Lox
{
    class Interpreter : exprVisitor<std::any>, stmtVisitor<std::any>
    {
    public:
        Interpreter(std::ostream& out);
        ~Interpreter();
        void interpret(const std::vector<std::shared_ptr<Stmt>>& statements);

        Environment& getGlobalsEnvironment();

        void execute(std::shared_ptr<Stmt> stmt);
        void executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, 
            std::shared_ptr<Environment> environment);
          
        void resolve(const std::shared_ptr<Expr>& expr, int depth);
        std::any lookUpVariable(const Token& name, std::shared_ptr<Expr> expr);

    private:
        std::any visit_block_stmt(std::shared_ptr<Block> stmt) override;
        std::any visit_class_stmt(std::shared_ptr<Class> stmt) override;
        std::any visit_expression_stmt(std::shared_ptr<Expression> stmt) override;
        std::any visit_function_stmt(std::shared_ptr<Function> stmt) override;
        std::any visit_if_stmt(std::shared_ptr<If> stmt) override;
        std::any visit_print_stmt(std::shared_ptr<Print> stmt) override;
        std::any visit_return_stmt(std::shared_ptr<Return> stmt) override;
        std::any visit_var_stmt(std::shared_ptr<Var> stmt) override;
        std::any visit_while_stmt(std::shared_ptr<While> stmt) override;
        
        std::any visit_assign_expr(std::shared_ptr<Assign> expr) override;
        std::any visit_literal_expr(std::shared_ptr<Literal> expr) override;
        std::any visit_logical_expr(std::shared_ptr<Logical> expr) override;
        std::any visit_set_expr(std::shared_ptr<Set> expr) override;
        std::any visit_this_expr(std::shared_ptr<This> expr) override;
        std::any visit_grouping_expr(std::shared_ptr<Grouping> expr) override;
        std::any visit_unary_expr(std::shared_ptr<Unary> expr) override;
        std::any visit_variable_expr(std::shared_ptr<Variable> expr) override;
        std::any visit_binary_expr(std::shared_ptr<Binary> expr) override;
        std::any visit_call_expr(std::shared_ptr<Call> expr) override;
        std::any visit_get_expr(std::shared_ptr<Get> expr) override;
        
        std::string stringify(const std::any& object);
        std::any evaluate(std::shared_ptr<Expr> expr);
        bool isTruthy(const std::any& object) const;
        bool isEqual(const std::any& a, const std::any& b) const;
        void checkNumberOperand(const Token& op, const std::any& operand) const; 
        void checkNumberOperands(const Token& op, 
            const std::any& left, const std::any& right) const;
        
        
        // data
        std::shared_ptr<Environment> globals;
        Environment* globalEnvironment;
        std::shared_ptr<Environment> environment;

        std::unordered_map<std::shared_ptr<Expr>, int> locals;

        class EnterEnvironmentGuard 
        {
        public:
          EnterEnvironmentGuard(Interpreter& i, std::shared_ptr<Environment> env);
          ~EnterEnvironmentGuard();

        private:
          Interpreter& i;
          std::shared_ptr<Environment> previous;
        };

        std::ostream& out;
    };

}

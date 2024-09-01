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
        void interpret(const std::vector<std::shared_ptr<const Stmt>>& statements);

        Environment& getGlobalsEnvironment();

        void execute(std::shared_ptr<const Stmt> stmt);
        void executeBlock(const std::vector<std::shared_ptr<const Stmt>>& statements, 
            std::shared_ptr<Environment> environment);
          
        void resolve(const std::shared_ptr<const Expr>& expr, int depth);
        std::any lookUpVariable(const Token& name, std::shared_ptr<const Expr> expr);

    private:
        std::any visit_block_stmt(std::shared_ptr<const Block> stmt) override;
        std::any visit_expression_stmt(std::shared_ptr<const Expression> stmt) override;
        std::any visit_function_stmt(std::shared_ptr<const Function> stmt) override;
        std::any visit_if_stmt(std::shared_ptr<const If> stmt) override;
        std::any visit_print_stmt(std::shared_ptr<const Print> stmt) override;
        std::any visit_return_stmt(std::shared_ptr<const Return> stmt) override;
        std::any visit_var_stmt(std::shared_ptr<const Var> stmt) override;
        std::any visit_while_stmt(std::shared_ptr<const While> stmt) override;
        
        std::any visit_assign_expr(std::shared_ptr<const Assign> expr) override;
        std::any visit_literal_expr(std::shared_ptr<const Literal> expr) override;
        std::any visit_logical_expr(std::shared_ptr<const Logical> expr) override;
        std::any visit_grouping_expr(std::shared_ptr<const Grouping> expr) override;
        std::any visit_unary_expr(std::shared_ptr<const Unary> expr) override;
        std::any visit_variable_expr(std::shared_ptr<const Variable> expr) override;
        std::any visit_binary_expr(std::shared_ptr<const Binary> expr) override;
        std::any visit_call_expr(std::shared_ptr<const Call> expr) override;
        
        std::string stringify(const std::any& object);
        std::any evaluate(std::shared_ptr<const Expr> expr);
        bool isTruthy(const std::any& object) const;
        bool isEqual(const std::any& a, const std::any& b) const;
        void checkNumberOperand(const Token& op, const std::any& operand) const; 
        void checkNumberOperands(const Token& op, 
            const std::any& left, const std::any& right) const;
        
        
        // data
        std::shared_ptr<Environment> globals;
        Environment* globalEnvironment;
        std::shared_ptr<Environment> environment;

        std::unordered_map<std::shared_ptr<const Expr>, int> locals;

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

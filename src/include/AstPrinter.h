#pragma once

#include <sstream>
#include <vector>

#include "Expr/Expr.h"

namespace Lox
{
  class AstPrinter : exprVisitor<std::any>
  {
    public:
      //AstPrinter();

      std::any visit_binary_expr(std::shared_ptr<const Binary> expr) override;
      std::any visit_grouping_expr(std::shared_ptr<const Grouping> expr) override;
      std::any visit_literal_expr(std::shared_ptr<const Literal> expr) override;
      std::any visit_unary_expr(std::shared_ptr<const Unary> expr) override;

      std::string print(std::shared_ptr<const Expr> expr);

    private:
      std::string paranthesise(const std::string& name, 
          std::initializer_list<const Expr*> exprs);
      
      std::stringstream stream_;
  };
  
}


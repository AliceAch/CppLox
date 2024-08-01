#pragma once

#include <sstream>
#include <vector>

#include "Expr/Expr.h"

namespace Lox
{
  class AstPrinter : Visitor<std::any>
  {
    public:
      //AstPrinter();

      std::any visit_binary_expr(const Binary& expr) override;
      std::any visit_grouping_expr(const Grouping& expr) override;
      std::any visit_literal_expr(const Literal& expr) override;
      std::any visit_unary_expr(const Unary& expr) override;

      std::string print(const Expr& expr);

    private:
      std::string paranthesise(const std::string& name, 
          std::initializer_list<const Expr*> exprs);
      
      std::stringstream stream_;
  };
  
}


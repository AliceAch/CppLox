#pragma once

#include <sstream>
#include <vector>

#include "Expr/Expr.h"

namespace Lox
{
  class AstPrinter : Expr::Visitor
  {
    public:
      AstPrinter() : indent_level_(0) {}

      void visit_binary_expr(const Binary& expr) override;
      void visit_grouping_expr(const Grouping& expr) override;
      void visit_literal_expr(const Literal& expr) override;
      void visit_unary_expr(const Unary& expr) override;

      std::string print(const Expr& expr);

    private:
      void paranthesise(const std::string& name, 
          std::initializer_list<const Expr*> exprs);
      void set_indent(const unsigned int indent);

      unsigned int indent_level_;
      std::string indent_;
      std::stringstream stream_;
  };


  
}


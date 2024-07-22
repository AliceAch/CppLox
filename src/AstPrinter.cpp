#include <iostream>
#include <string>

#include "AstPrinter.h"

namespace Lox { 
  void AstPrinter::visit_binary_expr(const Binary& expr)
  {
    paranthesise(expr.op.lexeme, {expr.left.get(), expr.right.get()});
  }
  void AstPrinter::visit_literal_expr(const Literal& expr)
  {
    // Fix std::any conversion errors here
    if (expr.literal.type() == typeid(double))
      stream_ << std::any_cast<double>(expr.literal);
    else if (expr.literal.type() == typeid(int))
      stream_ << std::any_cast<int>(expr.literal);
    else if (expr.literal.type() == typeid(std::string))
      stream_ << std::any_cast<std::string>(expr.literal);
    else if (expr.literal.type() == typeid(NULL))
      stream_ << "nil";
    else 
      stream_ << " ";
      
  }
  void AstPrinter::visit_grouping_expr(const Grouping& expr)
  {
    paranthesise("group", {expr.expression.get()});
  }
  void AstPrinter::visit_unary_expr(const Unary& expr)
  {
    paranthesise(expr.op.lexeme, {expr.right.get()});
  }
  void AstPrinter::paranthesise(const std::string& name,
      std::initializer_list<const Expr*> exprs)
  {
    stream_ << '(' << name;

    for (const auto expr : exprs) {
      if (expr == nullptr) {
        continue;
      }
      stream_ << ' ';
      // Might have to add this to stream ?
      expr->accept(*this);
    }

    stream_ << ')';
  }


  void AstPrinter::set_indent(const unsigned int indent)
  {
    indent_level_ = indent;
    indent_ = std::string(indent_level_ * 2, ' ');
  }
std::string AstPrinter::print(const Expr& expr)
  {
    expr.accept(*this);
    //stream_ << "this point";
    stream_ << '\n';
    return stream_.str();

  }

}

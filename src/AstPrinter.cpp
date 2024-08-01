#include <iostream>
#include <string>

#include "AstPrinter.h"

namespace Lox { 
  std::any AstPrinter::visit_binary_expr(const Binary& expr)
  {
    return paranthesise(expr.op.lexeme, {expr.left.get(), expr.right.get()});
  }
  std::any AstPrinter::visit_literal_expr(const Literal& expr)
  {
    std::stringstream stream("");
    // Fix std::any conversion errors here
    if (expr.literal.type() == typeid(double)) {
        double n = std::any_cast<double>(expr.literal);
        if (std::trunc(n) == n) { // is int
            return std::to_string((int)n);
        } else {
            return std::to_string(n); // TODO: don't print trailing zeros
        }
    }
    else if (expr.literal.type() == typeid(int))
      stream << std::any_cast<int>(expr.literal);
    else if (expr.literal.type() == typeid(std::string))
      stream << std::any_cast<std::string>(expr.literal);
    else if (expr.literal.type() == typeid(bool))
      stream << std::any_cast<bool>(expr.literal);
    else if (!expr.literal.has_value())
      stream << "nil";
    else 
      stream << "";

    return stream.str();
  }
  std::any AstPrinter::visit_grouping_expr(const Grouping& expr)
  {
    return paranthesise("group", {expr.expr.get()});
  }
  std::any AstPrinter::visit_unary_expr(const Unary& expr)
  {
    return paranthesise(expr.op.lexeme, {expr.right.get()});
  }
  std::string AstPrinter::paranthesise(const std::string& name,
      std::initializer_list<const Expr*> exprs)
  {
    std::stringstream stream("");
    stream << '(' << name;

    for (const auto expr : exprs) {
      if (expr == nullptr) {
        continue;
      }
      stream << ' ';
      // Might have to add this to stream ?
      stream << std::any_cast<std::string>(expr->accept(*this));
    }

    stream << ')';

    return stream.str();
  }

  std::string AstPrinter::print(const Expr& expr)
  {
    stream_ << std::any_cast<std::string>(expr.accept(*this));
    //stream << "this point";
    stream_ << '\n';
    return stream_.str();

  }

}

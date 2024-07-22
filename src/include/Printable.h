#include <iostream>
#include <any>

struct printable_any {
  template <class T>
  printable_any(T&& t) : m_val(std::forward<T>(t)) {
    m_print_fn = [](std::ostream& os, const std::any& val) {
      os << std::any_cast<std::decay_t<T>>(val);
    };
  }

private:
  using print_fn_t = void(*)(std::ostream&, const std::any&);
  std::any m_val;
  print_fn_t m_print_fn;
  
  friend std::ostream& operator<<(std::ostream& os, const printable_any& val) {
    val.m_print_fn(os, val.m_val);
    return os;
  }
};

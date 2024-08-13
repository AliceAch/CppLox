#pragma once

#include <memory>
#include <any>
#include <cassert>
#include <cmath>
#include <vector>

#include "Token.h"
{% for inc in includes %}#include "{{ inc }}"
{% endfor %}

namespace Lox
{
  {% for spec in class_specs %}
  struct {{ spec.name }};{% endfor %}

  template<typename R>
  class {{base_name|lower}}Visitor
  {
  public:
    ~{{base_name|lower}}Visitor() = default;
    {% for spec in class_specs %}
    virtual R visit_{{ spec.name|lower }}_{{ base_name|lower }}(const {{ spec.name }}& {{ base_name|lower }}) = 0;{% endfor %}
  };

  struct {{ base_name }}
  {
    virtual ~{{ base_name }}() = default;

    virtual std::any accept({{ base_name|lower }}Visitor<std::any>& visitor) const = 0;
  };
{% for spec in class_specs %}
  struct {{ spec.name }} : public {{ base_name }}
  {
    {{ spec.name }}({{ spec.arglist }})
        : {{ spec.initialisers }}
    { {{ spec.asserts }}
    }

    std::any accept({{ base_name|lower}}Visitor<std::any>& visitor) const
    { 
      return visitor.visit_{{ spec.name|lower }}_{{ base_name|lower }}(*this); 
    }

    {{ spec.getters }}

    {{ spec.members }}
  };
{% endfor %}
}


#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <any>

#include "Token.h"
{% for inc in includes %}#include "{{ inc }}"
{% endfor %}

namespace Lox
{
{% for spec in class_specs %}
  struct {{ spec.name }};{% endfor %}

  struct {{ base_name }}
  {
    virtual ~{{ base_name }}() = default;

    template<typename R>
    class exprVisitor
    {
    public:{% for spec in class_specs %}
      virtual R visit_{{ spec.name|lower }}_{{ base_name|lower }}(const {{ spec.name }}& {{ base_name|lower }}) = 0;{% endfor %}
    };

    virtual std::any accept(exprVisitor&) const {}
  };
{% for spec in class_specs %}

  struct {{ spec.name }} : public {{ base_name }}
  {
    {{ spec.name }}({{ spec.arglist }})
        : {{ spec.initialisers }}
    {}

    std::any accept(exprVisitor& exprVisitor) const override
    { exprVisitor.visit_{{ spec.name|lower }}_{{ base_name|lower }}(*this); }

    {{ spec.members }}
  };
{% endfor %}
}


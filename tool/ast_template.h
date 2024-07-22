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

    class Visitor
    {
    public:{% for spec in class_specs %}
      virtual void visit_{{ spec.name|lower }}_{{ base_name|lower }}(const {{ spec.name }}& {{ base_name|lower }}) = 0;{% endfor %}
    };

    virtual void accept(Visitor&) const {}
  };
{% for spec in class_specs %}

  struct {{ spec.name }} : public {{ base_name }}
  {
    {{ spec.name }}({{ spec.arglist }})
        : {{ spec.initialisers }}
    {}

    void accept(Visitor& visitor) const override
    { visitor.visit_{{ spec.name|lower }}_{{ base_name|lower }}(*this); }

    {{ spec.members }}
  };
{% endfor %}
}


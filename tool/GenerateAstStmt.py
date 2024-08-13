import os
import sys

import jinja2

def define_ast(output_dir, base_name, types, includes = []):
    class_specs = []
    type_items = sorted(types.items(), key = lambda x: x[0])

    for name, members in type_items:
        arglist = ", ".join(
            "std::unique_ptr<{}> {}".format(t, n)
            if i else "{} {}".format(t, n)
            for t, n , i in members)
        initialisers = ", ".join(
            "{}(std::move({}))".format(n, n)
            for t, n, i in members)
        member_vars = "\n    ".join(
            "std::unique_ptr<{}> {};". format(t, n)
            if i else "{} {};".format(t, n)
            for t, n, i in members)
        asserts = "\n       ".join(
            "assert(this->{} != nullptr);".format(n)
            if i else ""
            for t, n, i in members)
        getters = "\n    ".join(
            "const %s& get%s() const { return *%s; }" % (t, n.capitalize(), n)
            if i else "const %s& get%s() const { return %s; }" % (t, n.capitalize(), n)
            for t, n, i in members)
        class_specs.append(dict(name=name, arglist=arglist, initialisers=initialisers, members=member_vars, asserts = asserts, getters = getters))
        with open("ast_template.h") as f:
            template = jinja2.Template(f.read())
        with open(os.path.join(output_dir, "{}.h".format(base_name)), 'w') as f:
            f.write(template.render(base_name=base_name, class_specs=class_specs, includes=includes))
        

if __name__ == "__main__":
    try:
        output_dir = sys.argv[1]
    except IndexError:
        print("Usage: python3 {} <output directory>".format(sys.argv[0]))
        sys.exit(1)
    define_ast(
        output_dir, "Stmt",
        {
            "Block"      : [("std::vector<std::unique_ptr<Stmt>>", "stmt", False)],
            "Expression" : [("Expr", "expr", True)],
            "Print"      : [("Expr", "expr", True)],
            "Var"        : [("Token", "name", False), ("Expr", "initializer", True)] #make sure to remove the assertation for this member variable
        },
        ["Expr/Expr.h"]
    )

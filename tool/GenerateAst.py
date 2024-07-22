import os
import sys

def defineAst(outputDir, baseName, types):
    fileName = baseName +".h"
    os.chdir(outputDir)
    os.system('touch ' + fileName)
    writer = open(fileName, "w")

    writer.write("#include \"Lox.h\"\n")
    writer.write("#include \"Token.h\"\n")
    writer.write("#include <list>\n")
    writer.write("#include <any>\n")
    writer.write("\n")
    writer.write("class " + baseName + " {\n")
    # The AST classes.
    for t in types:
        className = t.split("|")[0].strip()
        fields = t.split("|")[1].strip()
        defineType(writer, baseName, className, fields)

    writer.write("}\n")
    writer.close()

def defineType(writer, baseName, className, fieldList):
    writer.write("    static class " + className + " : " + baseName + " {\n")
    # Constructor
    writer.write("        " + className + "(" + fieldList + ") {\n")

    # Store parameters in fields
    fields = fieldList.split(", ");
    for field in fields:
        name = field.split(" ")[1]
        writer.write("            this->" + name + " = " + name + ";\n")
    writer.write("        }\n")

    # Fields
    writer.write("\n")
    for field in fields:
        writer.write("        " + field + ";\n")
    writer.write("  }\n")
if __name__ == "__main__":
    if len(sys.argv) != 2:
        sys.stderr.write("Usage: generate_ast <output directory>\n")
        sys.exit(64)
    
    outputDir = sys.argv[1]
    defineAst(outputDir, "Expr", [
        "Binary   | Expr left, Lox::Token operator, Expr right",
        "Grouping | Expr expression",
        "Literal  | std::any value",
        "Unary    | Lox::Token operator, Expr right"
    ])

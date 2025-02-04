# CppLox
## Description
A C++ implementation of the Lox tree-walk interpreter from the book Crafting Interpreters by Robert Nystorm. The book can be found [here](https://craftinginterpreters.com/).

I decided to write the whole project in C++ because of a personal protest of using
Java. But really, The real reason is because the project would be my first time using C++.
Writing Lox was fun and it made me gain a deeper understanding of how many programming 
languages parse and execute the programmer's code. but using C++ sometimes made me
want to tear my hair out because the lack of Java's garbage collector makes things a lot harder. Good thing is C++
has something similar called smart pointers. Despite this I feel like I
dodged a bullet and then that bullet came around to hit me anyways...

CppLox is based off JLox, a fully-fledged scripting language that parses statements using 
recursive descent into an abstract syntax tree that is then interpreted by traversing
this generated AST. Both implementations of Lox are pretty similar to how early versions of python interpreted its 
code but its syntax is similar to C.

## Features
The code should be able to do everything that Robert Nystrom's JLox is able to do.
This includes:
1. Basic expression evaluation
2. Control flow like if statements and while and for loops,
3. Functions
4. Classes and Inheritance

## Building
CMake makes everything pretty hands off: 

First 
```bash
git clone "https://github.com/AliceAch/CppLox.git"
```
### Linux/MacOS
```console
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
```
The Lox executable will be in the src/ directory.
### Windows
The best solution I would offer to build the project is with Visual Studio's cmake 
integration. This article might be helpful:
https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-170#building-cmake-projects

Otherwise you could try to install CMake and then set it as a system variable (I'm not sure if
it automatically does that) and go about it like the Linux/MacOs procedure
## Usage
Call Lox with no arguments to activate the REPL.

```console
$ ./lox
lox v0.0.1
> print("Hello, World!");
Hello, World!
```
Ctrl-z is the escape character to terminate the program.

Or you can pass in a file path to a file with lox code to run.
I'm not going to document the entire lox language, but here is 
a crash course on Lox:

```
//=======Variables=======//

var A = 1 + 2 * 3;      // Sets A to 7
print(A);               // prints 7
print(clock());         // uses a built in function and prints the system clock

//=======If Statements=======//

if (A == 7 or A < 0)    // if statements
{
    var A = 5;
    print("A is 7 or less than 0!");
    print(A);           // Will print 5 because we are in a local scope
}
A = 6;                  // can redefine variables
print(A);               // prints 6

//=======While Loops=======//

while(A < 7)    
{
    print(A);
    A = A + 1;
}

//=======For Loops=======//

var fib = 0;
var temp;

for(var b = 1; fib < 10000; b = temp + b)
{
    print fib;
    temp = fib;
    fib = b;
}

//========Functions=======//

fun add(a, b)
{
    return a + b;
}

print(add(1,2));        // Will print 3

//=======Classes and Inheritance=======//

class Foo               // Define a class
{
    inFoo()
    {
        print "in foo";
    }
}

class Bar < Foo         // Inherit from a class
{
    inBar()
    {
        print "in bar";
    }
}

var bar = Bar();
bar.inFoo();            // Will print "in foo"
bar.inBar();            // Will print "in bar"
```

save this file as test.lox and run with
```console
$ ./lox test.lox
```
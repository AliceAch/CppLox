# CppLox
A c++ implementation of the Lox tree-walk interpreter from the book Crafting Interpreters by Robert Nystorm.
Cmake handles the dependency (libfmt). I decided to write it in c++ because I do not want to learn Java, and
I much prefer writing in c++. I also need more practice working with the language. The project was fun, but sometimes 
frustrating primarily because of the lack of Java's garbage collector makes things a lot harder. However I still do not 
regret not using Java.

In order to build and run cpplox, create a build directory in the respository folder. Cd into it and enter "cmake .." 
Then enter "cmake --build ." or "make" or whatever. cd into the src directory where lox.exe will reside.

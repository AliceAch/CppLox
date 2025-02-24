add_library(lox)

set(GCC_DEBUG_COMPILE_FLAG "-g")
add_definitions(${GCC_DEBUG_COMPILE_FLAG})

target_include_directories(lox
    PUBLIC
        ${lox_SOURCE_DIR}/src/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(lox
    PUBLIC
        fmt::fmt
)

target_sources(lox 
    PUBLIC
        Lox.cpp
        Scanner.cpp
        Token.cpp
        Callable.cpp
        Parser.cpp
        Interpreter.cpp
        Environment.cpp
        Resolver.cpp
        LoxClass.cpp
        LoxInstance.cpp
)

add_executable(lox_repl)
set_target_properties(lox_repl PROPERTIES OUTPUT_NAME "lox")

target_include_directories(lox_repl
    PUBLIC
        ${lox_SOURCE_DIR}/src/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(lox_repl
    PRIVATE
        main.cpp
)

target_link_libraries(lox_repl PRIVATE lox)

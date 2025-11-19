cmake_minimum_required(VERSION 3.26)
project(Rylang)

set(CMAKE_CXX_STANDARD 17)

include_directories(parser)
include_directories(runtime)
include_directories(runtime/environment)
include_directories(runtime/eval)
include_directories(runtime/interpreter)
include_directories(utils)

add_executable(ryc
        parser/ast.hh
        parser/lexer.cc
        parser/lexer.hh
        parser/parser.cc
        parser/parser.hh
        runtime/environment/environment.cc
        runtime/environment/environment.hh
        runtime/eval/expressions.cc
        runtime/eval/expressions.hh
        runtime/eval/statements.cc
        runtime/eval/statements.hh
        runtime/interpreter/interpreter.cc
        runtime/interpreter/interpreter.hh
        runtime/values.cc
        runtime/values.hh
        utils/error.hh
        utils/utils.cc
        utils/utils.hh
        main.cc)

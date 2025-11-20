/*

utils.hh

*/

#pragma once

#include <iostream>

#include "../parser/ast.hh"
#include "../runtime/values.hh"

#include "../runtime/environment/environment.hh"

void print_ast(std::shared_ptr<Stmt> node, int indent);
void print_value(std::shared_ptr<RuntimeValue> node, Environment* env, std::size_t line);

ValueType stoval(const std::string& type_str, std::shared_ptr<Stmt> node, Environment* env, std::size_t line);

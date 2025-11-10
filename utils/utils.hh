/*

utils.hh

*/

#pragma once

#include <iostream>

#include "../parser/ast.hh"
#include "../runtime/values.hh"

void print_ast(std::shared_ptr<Stmt> node, int indent);
void print_value(std::shared_ptr<RuntimeValue> node);
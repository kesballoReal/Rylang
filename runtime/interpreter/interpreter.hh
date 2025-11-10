/*

interpreter.hh

*/

#pragma once

#include <memory>

#include "../environment/environment.hh"
#include "../../parser/parser.hh"
#include "../values.hh"

using RVPtr = std::shared_ptr<RuntimeValue>;
using SPtr = std::shared_ptr<Stmt>;

RVPtr evaluate(SPtr node, Environment* env);
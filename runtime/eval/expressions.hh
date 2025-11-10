/*

expressions.hh

*/

#include <memory>

#include "../environment/environment.hh"
#include "../../parser/parser.hh"
#include "../values.hh"
#include "../interpreter/interpreter.hh"

using RVPtr = std::shared_ptr<RuntimeValue>;

RVPtr eval_binary_expr(std::shared_ptr<ASTBinaryExpr> bin, Environment* env);
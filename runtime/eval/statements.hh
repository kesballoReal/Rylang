/*

statements.hh

*/

#include <memory>

#include "../environment/environment.hh"
#include "../../parser/parser.hh"
#include "../values.hh"
#include "../interpreter/interpreter.hh"

using RVPtr = std::shared_ptr<RuntimeValue>;

RVPtr eval_var_declaration(std::shared_ptr<ASTVarDecl> node, Environment* env, std::size_t line);
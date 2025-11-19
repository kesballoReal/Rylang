/*

statements.hh

*/

#pragma once

#include <memory>

#include "../environment/environment.hh"
#include "../../parser/parser.hh"
#include "../values.hh"
#include "../interpreter/interpreter.hh"

using RVPtr = std::shared_ptr<RuntimeValue>;

RVPtr cast(RVPtr value, ValueType targetType, std::size_t line);
RVPtr eval_var_declaration(std::shared_ptr<ASTVarDecl> node, Environment* env, std::size_t line);
RVPtr eval_if_stmt(std::shared_ptr<ASTIfStmt> node, Environment* env, std::size_t line);
RVPtr eval_block_stmt(std::shared_ptr<ASTBlockStmt> node, Environment* env, std::size_t line);
RVPtr eval_while_stmt(std::shared_ptr<ASTWhileStmt> node, Environment* env, std::size_t line);
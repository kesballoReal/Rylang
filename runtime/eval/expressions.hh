/*

expressions.hh

*/
#pragma once

#include <memory>

#include "../environment/environment.hh"
#include "../../parser/parser.hh"
#include "../values.hh"
#include "../interpreter/interpreter.hh"

using RVPtr = std::shared_ptr<RuntimeValue>;

RVPtr eval_binary_expr(std::shared_ptr<ASTBinaryExpr> bin, Environment* env, std::size_t line);
RVPtr eval_unary_expr(std::shared_ptr<ASTUnaryExpr> unary, Environment* env, std::size_t line);
RVPtr eval_assign_expr(std::shared_ptr<ASTAssignExpr> assign, Environment* env, std::size_t line);
RVPtr eval_member_expr(std::shared_ptr<ASTMemberExpr> node, Environment* env, std::size_t line);
RVPtr eval_call_expr(std::shared_ptr<ASTCallExpr> node, Environment* env, std::size_t line);
RVPtr eval_cast_expr(std::shared_ptr<ASTCastExpr> node, Environment* env, std::size_t line);

RVPtr eval_array_literal(std::shared_ptr<ASTArrayLiteral> arr, Environment* env, std::size_t line);
#include "interpreter.hh"
#include "../../utils/utils.hh"
#include "../../utils/error.hh"

#include "../eval/expressions.hh"
#include "../eval/statements.hh"

#include <iostream>
#include <cmath>

RVPtr evaluate(SPtr node, Environment* env, std::size_t line)
{
    switch (node->kind) {
        case NodeType::VarDeclaration:
        {
            auto var = std::static_pointer_cast<ASTVarDecl>(node);
            return eval_var_declaration(var, env, line);
        }
        case NodeType::IfStmt:
        {
            auto ifs = std::static_pointer_cast<ASTIfStmt>(node);
            return eval_if_stmt(ifs, env, line);
        }
        case NodeType::WhileStmt:
        {
            auto wh = std::static_pointer_cast<ASTWhileStmt>(node);
            return eval_while_stmt(wh, env, line);
        }
        case NodeType::BreakStmt:
            return std::make_shared<BreakValue>();

        case NodeType::ContinueStmt:
            return std::make_shared<ContinueValue>();
        case NodeType::NumericLiteral:
        {
            auto num = std::static_pointer_cast<ASTNumericLiteral>(node);
            if (std::trunc(num->value) == num->value) {
                return std::make_shared<IntValue>(static_cast<int>(num->value));
            }
            return std::make_shared<FloatValue>(num->value);

        }
        case NodeType::BoolLiteral: {
            auto bool_literal = std::static_pointer_cast<ASTBoolLiteral>(node);
            return std::make_shared<BoolValue>(bool_literal->value);
        }
        case NodeType::NullLiteral:
        {
            return std::make_shared<NullValue>();
        }
        case NodeType::IdentifierLiteral:
        {
            auto identifier = std::static_pointer_cast<ASTIdentifierLiteral>(node);
            RVPtr value = env->lookupVar(identifier->name, line);

            if (!value)
            {
                std::string err = "ryc: variable '" + identifier->name + "' is uninitialized!";
                runtime_err(err, line);
            }

            return value;
        }
        case NodeType::StringLiteral:
        {
            auto str = std::static_pointer_cast<ASTStringLiteral>(node);
            return std::make_shared<StringValue>(str->value);
        }
        case NodeType::Program:
        {
            auto program = std::static_pointer_cast<ASTProgram>(node);
            RVPtr last_eval = std::make_shared<NullValue>();

            for (std::size_t i = 0; i < program->body.size(); i++)
            {
                last_eval = evaluate(program->body[i], env, program->body[i]->line);
            }

            return last_eval;
        }
        case NodeType::BlockStmt:
        {
            auto block = std::static_pointer_cast<ASTBlockStmt>(node);
            return eval_block_stmt(block, env, line);

        }
        case NodeType::BinaryExpr:
        {
            auto bin = std::static_pointer_cast<ASTBinaryExpr>(node);
            return eval_binary_expr(bin, env, line);
        }
        case NodeType::UnaryExpr:
        {
            auto unary = std::static_pointer_cast<ASTUnaryExpr>(node);
            return eval_unary_expr(unary, env, line);
        }
        case NodeType::AssignmentExpr:
        {
            auto assign = std::static_pointer_cast<ASTAssignExpr>(node);
            return eval_assign_expr(assign, env, line);
        }
        default:
        {
            std::cerr << "ryc: This AST Node has not yet been setup for interpretation: " << std::endl;
            print_ast(node, 0);
            std::exit(1);
        }
    }
}

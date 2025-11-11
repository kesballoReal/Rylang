#include "interpreter.hh"
#include "../../utils/utils.hh"
#include "../../utils/error.hh"

#include "../eval/expressions.hh"
#include "../eval/statements.hh"

#include <iostream>
#include <cmath>

RVPtr evaluate(SPtr node, Environment* env, std::size_t line)
{
    switch (node->kind)
    {
        case NodeType::VarDeclaration:
        {
            auto var = std::dynamic_pointer_cast<ASTVarDecl>(node);
            if (!var)
            {
                std::cerr << "ryc: internal error: invalid variable node" << std::endl;
                std::exit(1);
            }

            return eval_var_declaration(var, env, line);
        }
        case NodeType::NumericLiteral:
        {
            auto num = std::dynamic_pointer_cast<ASTNumericLiteral>(node);
            if (!num)
            {
                std::cerr << "ryc: internal error: invalid numeric node" << std::endl;
                std::exit(1);
            }
            if (std::floor(num->value) == num->value)
            {
                return std::make_shared<IntValue>(static_cast<int>(num->value));
            }
            return std::make_shared<FloatValue>(num->value);
        }
        case NodeType::NullLiteral:
        {
            return std::make_shared<NullValue>(nullptr);
        }
        case NodeType::IdentifierLiteral:
        {
            auto identifier = std::dynamic_pointer_cast<ASTIdentifierLiteral>(node);
            if (!identifier)
            {
                std::cerr << "ryc: internal error: invalid identifier node" << std::endl;
                std::exit(1);
            }

            RVPtr value = env->lookupVar(identifier->name, line);

            if (!value)
            {
                std::string err = "ryc: variable '" + identifier->name + "' is uninitialized!";
                runtime_err(err, line);
            }

            return value;
        }
        case NodeType::Program:
        {
            auto program = std::dynamic_pointer_cast<ASTProgram>(node);
            if (!program)
            {
                std::cerr << "ryc: internal error: invalid program node" << std::endl;
                std::exit(1);
            }
            RVPtr last_eval = std::make_shared<NullValue>(nullptr);

            for (std::size_t i = 0; i < program->body.size(); i++)
            {
                last_eval = evaluate(program->body[i], env, program->body[i]->line-1);
            }

            return last_eval;
        }
        case NodeType::BinaryExpr:
        {
            auto bin = std::dynamic_pointer_cast<ASTBinaryExpr>(node);
            if (!bin)
            {
                std::cerr << "ryc: internal error: invalid binary node" << std::endl;
                std::exit(1);
            }
            return eval_binary_expr(bin, env, line);
        }     
        default:
        {
            std::cerr << "ryc: This AST Node has not yet been setup for interpretation: " << std::endl;
            print_ast(node, 0);
            std::exit(1);
        }
    }
}
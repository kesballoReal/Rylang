#include "interpreter.hh"

#include "../eval/expressions.hh"

#include <iostream>

RVPtr evaluate(SPtr node, Environment* env)
{
    switch (node->kind)
    {
        case NodeType::NumericLiteral:
        {
            auto num = std::dynamic_pointer_cast<ASTNumericLiteral>(node);
            if (!num)
            {
                std::cerr << "ryc: internal error: invalid numeric node" << std::endl;
                std::exit(1);
            }
            /* Later check for integer/floats */
            return std::make_shared<IntValue>(num->value);
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

            RVPtr value = env->lookupVar(identifier->name);

            if (!value)
            {
                std::cerr << "ryc: variable '" << identifier->name << "' is uninitialized!" << std::endl;
                std::exit(1);
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
                last_eval = evaluate(program->body[i], env);
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
            return eval_binary_expr(bin, env);
        }     
    }
}
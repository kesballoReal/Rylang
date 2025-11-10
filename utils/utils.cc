#include "utils.hh"

void print_ast(std::shared_ptr<Stmt> node, int indent = 0)
{
    std::string pad(indent, ' '); // This is for indentation

    if (!node) {
        std::cout << pad << "nullptr" << std::endl;
        return;
    }

    switch (node->kind)
    {
        case Program:
        {
            std::cout << pad << "Program:" << std::endl;
            auto program = std::dynamic_pointer_cast<ASTProgram>(node);
            if (!program) {
                std::cerr << pad << "[Invalid Program node]" << std::endl;
                break;
            }
            for (auto& stmt : program->body) {
                print_ast(stmt, indent + 2);
            }
            break;
        }

        case BinaryExpr:
        {
            auto bin = std::dynamic_pointer_cast<ASTBinaryExpr>(node);
            if (!bin) {
                std::cerr << pad << "[Invalid BinaryExpr node]" << std::endl;
                break;
            }

            std::cout << pad << "BinaryExpr(" << bin->op << "):" << std::endl;
            std::cout << pad << "  Left:" << std::endl;
            print_ast(bin->left, indent + 4);
            std::cout << pad << "  Right:" << std::endl;
            print_ast(bin->right, indent + 4);
            break;
        }

        case NumericLiteral:
        {
            auto num = std::dynamic_pointer_cast<ASTNumericLiteral>(node);
            if (!num) {
                std::cerr << pad << "[Invalid NumericLiteral node]" << std::endl;
                break;
            }

            std::cout << pad << "NumericLiteral(" << num->value << ")" << std::endl;
            break;
        }
        case IdentifierLiteral:
        {
            auto ident = std::dynamic_pointer_cast<ASTIdentifierLiteral>(node);
            if (!ident)
            {
                std::cerr << pad << "[Invalid IdentifierLiteral node]" << std::endl;
                break;
            }

            std::cout << pad << "IdentifierLiteral(" << ident->name << ")" << std::endl;
            break;
        }
        case NullLiteral:
        {
            std::cout << pad << "NullLiteral()" << std::endl;
            break;
        }
        default:
            std::cout << pad << "[Unknown node type]" << std::endl;
            break;
    }
}

void print_value(std::shared_ptr<RuntimeValue> node)
{
    switch (node->kind)
    {
        case VAL_INT:
        {
            auto num = std::dynamic_pointer_cast<IntValue>(node);
            std::cout << num->value << std::endl;
            break;
        }
        case VAL_FLOAT:
        {
            auto num = std::dynamic_pointer_cast<FloatValue>(node);
            std::cout << num->value << std::endl;
            break;
        }
        case VAL_NULL:
        {
            std::cout << "null" << std::endl;
            break;
        }
    }
}
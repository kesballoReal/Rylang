#include "utils.hh"

#include <iomanip>
#include <cmath>

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
            auto program = std::static_pointer_cast<ASTProgram>(node);
            for (auto& stmt : program->body) {
                print_ast(stmt, indent + 2);
            }
            break;
        }
        case BlockStmt:
        {
            std::cout << pad << "BlockStmt:" << std::endl;
            auto b = std::static_pointer_cast<ASTBlockStmt>(node);
            for (auto& stmt : b->block) {
                print_ast(stmt, indent + 2);
            }
            break;
        }
        case VarDeclaration:
        {
            std::cout << pad << "VarDeclaration:" << std::endl;
            auto var = std::static_pointer_cast<ASTVarDecl>(node);

            std::cout << pad << "  Name:" << std::endl;
            std::cout << pad << "    " << var->name << std::endl;
            std::cout << pad << "  Type:" << std::endl;
            std::cout << pad << "    " << var->type << std::endl;

            if (var->value)
            {
                std::cout << pad << "  Value:" << std::endl;
                print_ast(*var->value, indent + 4);
            }
            std::cout << pad << "  Const: " << (var->is_const ? "True" : "False") << std::endl;
            break;
            
        }
        case IfStmt:
        {
            auto ifs = std::static_pointer_cast<ASTIfStmt>(node);

            std::cout << pad << "IfStmt:" << std::endl;
            std::cout << pad << "  Condition:" << std::endl;
            print_ast(ifs->condition, indent + 4);
            std::cout << pad << "  Then:" << std::endl;
            print_ast(ifs->thenBranch, indent + 4);

            if (ifs->elseBranch)
            {
                std::cout << pad << "  Else:" << std::endl;
                print_ast(*ifs->elseBranch, indent + 4);
            }
            break;
        }
        case WhileStmt:
        {
            auto wh = std::static_pointer_cast<ASTWhileStmt>(node);
            
            std::cout << pad << "WhileStmt:" << std::endl;
            std::cout << pad << "  Condition:" << std::endl;
            print_ast(wh->condition, indent + 4);
            std::cout << pad << "  Do:" << std::endl;
            print_ast(wh->doBranch, indent + 4);
            break;
        }
        case ContinueStmt:
        {
            std::cout << pad << "ContinueStmt:" << std::endl;
            break;
        }
        case BreakStmt:
        {
            std::cout << pad << "BreakStmt:" << std::endl;
            break;
        }
        case BinaryExpr:
        {
            auto bin = std::static_pointer_cast<ASTBinaryExpr>(node);

            std::cout << pad << "BinaryExpr(" << bin->op << "):" << std::endl;
            std::cout << pad << "  Left:" << std::endl;
            print_ast(bin->left, indent + 4);
            std::cout << pad << "  Right:" << std::endl;
            print_ast(bin->right, indent + 4);
            break;
        }
        case UnaryExpr: {
            auto unary = std::static_pointer_cast<ASTUnaryExpr>(node);
            std::cout << pad << "UnaryExpr(" << unary->op << "):" << std::endl;
            std::cout << pad << "  Value:" << std::endl;
            print_ast(unary->operand, indent + 4);
            std::cout << pad << "  isPrefix: " << (unary->prefix ? "true" : "false") << std::endl;
            break;
        }
        case AssignmentExpr: {
            auto assign = std::static_pointer_cast<ASTAssignExpr>(node);
            std::cout << pad << "AssignmentExpr:" << std::endl;
            std::cout << pad << "  Assignee:" << std::endl; 
            print_ast(assign->assignee, indent + 4);
            std::cout << pad << "  Value:" << std::endl;
            print_ast(assign->value, indent + 4);
            break;
        }
        case NumericLiteral:
        {
            auto num = std::static_pointer_cast<ASTNumericLiteral>(node);

            std::cout << pad << "NumericLiteral(" << num->value << ")" << std::endl;
            break;
        }
        case BoolLiteral: {
            auto bool_literal = std::static_pointer_cast<ASTBoolLiteral>(node);
            std::cout << pad << "BoolLiteral(" << bool_literal->value << ")" << std::endl;
            break;

        }
        case IdentifierLiteral:
        {
            auto ident = std::static_pointer_cast<ASTIdentifierLiteral>(node);

            std::cout << pad << "IdentifierLiteral(" << ident->name << ")" << std::endl;
            break;
        }
        case StringLiteral:
        {
            auto str = std::static_pointer_cast<ASTStringLiteral>(node);
            std::cout << pad << "StringLiteral(" << str->value << ")" << std::endl;
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
    switch (node->kind) {
        case VAL_INT:
        {
            auto num = std::static_pointer_cast<IntValue>(node);
            std::cout << num->value << std::endl;
            break;
        }
        case VAL_FLOAT:
        {
            auto num = std::static_pointer_cast<FloatValue>(node);

            if (std::floor(num->value) == num->value) {
                std::cout << static_cast<int>(num->value) << std::endl;
            } else {
                std::cout << num->value << std::endl;
            }

            break;
        }
        case VAL_BOOL: {
            auto bool_lit = std::static_pointer_cast<BoolValue>(node);
            std::cout << (bool_lit->value ? "true" : "false") << std::endl;
            break;
        }
        case VAL_STRING: {
            auto str_lit = std::static_pointer_cast<StringValue>(node);
            std::cout << str_lit->value << std::endl;
            break;
        }
        case VAL_NULL:
        {
            std::cout << "null" << std::endl;
            break;
        }
    }
}

ValueType stoval(const std::string& type_str, std::shared_ptr<Stmt> node, Environment* env, std::size_t line) {
    if (type_str == "int") return VAL_INT;
    if (type_str == "float") return VAL_FLOAT;
    if (type_str == "bool") return VAL_BOOL;
    if (type_str == "string") return VAL_STRING;
    if (type_str == "null") return VAL_NULL;

    std::string err = "Unknown type '" + type_str + "'";
    runtime_err(err, line);
    return VAL_NULL;
}

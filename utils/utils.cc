#include "utils.hh"

#include <iomanip>
#include <cmath>
#include <cstdint>

void print_ast(std::shared_ptr<Stmt> node, int indent = 0)
{
    std::string pad(indent, ' '); // This is for indentation

    if (!node) {
        std::cout << pad << "null" << std::endl;
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
        case ExprStmt:
        {   
            auto expr = std::static_pointer_cast<ASTExprStmt>(node);
            std::cout << pad << "ExprStmt:" << std::endl;
            std::cout << pad << "Value:" << std::endl;
            print_ast(expr->expression, indent + 4);
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
            std::cout << pad << "  isArray:" << (var->is_array ? "True" : "False") << std::endl;
            if (var->is_array && var->array_size) { std::cout << pad << "  ArraySize:" << std::endl; print_ast(*var->array_size, indent + 4); }
            break;

        }
        case FunctionStmt:
        {
            auto func = std::static_pointer_cast<ASTFunctionStmt>(node);
            
            std::cout << pad << "FunctionStmt:" << std::endl;
            std::cout << pad << "  Name:" << func->name << std::endl;
            std::cout << pad << "  Type:" << func->ret_type << std::endl;
            std::cout << pad << "  Params:" << std::endl;

            for (auto& param : func->params) { print_ast(param, indent + 4); }
            std::cout << pad << "  Body:" << std::endl;
            print_ast(func->body, indent + 4);
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
        case ForStmt:
        {
            auto f = std::static_pointer_cast<ASTForStmt>(node);
            
            std::cout << pad << "ForStmt:" << std::endl;
            std::cout << pad << "  Init:" << std::endl;
            print_ast(f->init, indent + 4);
            std::cout << pad << "  Condition:" << std::endl;
            print_ast(f->condition, indent + 4);
            std::cout << pad << "  Update:" << std::endl;
            print_ast(f->update, indent + 4);
            std::cout << pad << "  Body:" << std::endl;
            print_ast(f->body, indent + 4);
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
        case ReturnStmt:
        {
            auto ret = std::static_pointer_cast<ASTReturnStmt>(node);
            std::cout << pad << "ReturnStmt:" << std::endl;
            if (ret->value)
            {
                std::cout << pad << "  Value:" << std::endl;
                print_ast(ret->value, indent + 4);
            }
            break;
        }
        case CastExpr:
        {
            auto cast = std::static_pointer_cast<ASTCastExpr>(node);
            std::cout << pad << "CastStmt:" << std::endl;
            std::cout << pad << "  Type: " << cast->type << std::endl;
            std::cout << pad << "  Target:" << std::endl;
            print_ast(cast->target, indent + 4);
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
        case MemberExpr: {
            auto mem = std::static_pointer_cast<ASTMemberExpr>(node);
            std::cout << pad << "MemberExpr:" << std::endl;
            std::cout << pad << "  Object:" << std::endl;
            print_ast(mem->object, indent + 4);
            std::cout << pad << "  Property:" << std::endl;
            print_ast(mem->property, indent + 4);
            std::cout << pad << "  isComputed: " << (mem->computed ? "True" : "False") << std::endl;
            break;
        }
        case CallExpr:
        {
            auto call = std::static_pointer_cast<ASTCallExpr>(node);
            std::cout << pad << "CallExpr:" << std::endl;
            std::cout << pad << "  Callee:" << std::endl;
            print_ast(call->callee, indent + 4);
            std::cout << pad << "  Args:" << std::endl;

            for (auto& a : call->args) { print_ast(a, indent + 4); }
            break;
        }
        case Param:
        {
            auto param = std::static_pointer_cast<ASTParam>(node);

            std::cout << pad << "Param" << std::endl;
            std::cout << pad << "  Name:" << param->name << std::endl;
            std::cout << pad << "  Type:" << param->type << std::endl;
            std::cout << pad << "  isArray: " << (param->isArray ? "True" : "False") << std::endl;
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
        case CharLiteral:
        {
            auto ch = std::static_pointer_cast<ASTCharLiteral>(node);

            std::cout << pad << "CharLiteral(" << ch->value << ")" << std::endl;
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
        case ArrayLiteral:
        {
            auto arr = std::static_pointer_cast<ASTArrayLiteral>(node);
            std::cout << pad << "ArrayLiteral:" << std::endl;
            std::cout << pad << "  Elements:" << std::endl;
            for (auto& e : arr->elements)
            {
                print_ast(e, indent + 4);
            }
            break;
        }
        default:
            std::cout << pad << "[Unknown node type]" << std::endl;
            break;
    }
}

void print_value(std::shared_ptr<RuntimeValue> node, Environment* env, std::size_t line)
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
        case VAL_ARRAY: {
            auto arr = std::static_pointer_cast<ArrayValue>(node);
            std::cout << std::endl;
            for (auto& e : arr->elements) { print_value(e, env, line); }
            break;
        }
        case VAL_FUNCTION: {
            auto func = std::static_pointer_cast<FunctionValue>(node);
            std::cout << "<function " 
                    << func->declaration->name << "(";

            for (size_t i = 0; i < func->declaration->params.size(); i++) {
                std::cout << func->declaration->params[i];
                if (i + 1 < func->declaration->params.size()) std::cout << ", ";
            }

            std::cout << ") at " << func.get() << ">";
            break;
        }
        case VAL_RETURN:
        {
            auto ret = std::static_pointer_cast<ReturnValue>(node);
            print_value(ret->value, env, line);
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
    if (type_str == "char") return VAL_CHAR;
    if (type_str == "null") return VAL_NULL;
    if (type_str == "void") return VAL_NULL;

    std::string err = "Unknown type '" + type_str + "'";
    runtime_err(err, line);
    return VAL_NULL;
}

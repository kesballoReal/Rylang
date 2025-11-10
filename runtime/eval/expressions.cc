#include "expressions.hh"

#include <iostream>

RVPtr eval_binary_expr(std::shared_ptr<ASTBinaryExpr> bin, Environment* env)
{
    // Evaluate left and right operands
    RVPtr left_val = evaluate(bin->left, env);
    RVPtr right_val = evaluate(bin->right, env);

    // If either operand is null, return null
    if (!left_val || !right_val || left_val->kind == VAL_NULL || right_val->kind == VAL_NULL)
        return std::make_shared<NullValue>(nullptr);

    // Extract numeric values
    double a = 0.0, b = 0.0;
    bool a_is_float = false, b_is_float = false;

    switch (left_val->kind) {
        case VAL_INT:
            a = std::dynamic_pointer_cast<IntValue>(left_val)->value;
            break;
        case VAL_FLOAT:
            a = std::dynamic_pointer_cast<FloatValue>(left_val)->value;
            a_is_float = true;
            break;
        default:
            std::cerr << "ryc: left operand is not numeric!" << std::endl;
            std::exit(1);
    }

    switch (right_val->kind) {
        case VAL_INT:
            b = std::dynamic_pointer_cast<IntValue>(right_val)->value;
            break;
        case VAL_FLOAT:
            b = std::dynamic_pointer_cast<FloatValue>(right_val)->value;
            b_is_float = true;
            break;
        default:
            std::cerr << "ryc: right operand is not numeric!" << std::endl;
            std::exit(1);
    }

    // Determine if result should be float
    bool result_is_float = a_is_float || b_is_float || bin->op == "/";

    double result = 0.0;

    if (bin->op == "+") result = a + b;
    else if (bin->op == "-") result = a - b;
    else if (bin->op == "*") result = a * b;
    else if (bin->op == "/") {
        if (b == 0.0) {
            std::cerr << "ryc: division by zero!" << std::endl;
            std::exit(1);
        }
        result = a / b;
    }
    else {
        std::cerr << "ryc: unknown binary operator '" << bin->op << "'" << std::endl;
        std::exit(1);
    }

    if (result_is_float)
        return std::make_shared<FloatValue>(result);
    else
        return std::make_shared<IntValue>(static_cast<int>(result));
}
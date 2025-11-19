#include "expressions.hh"
#include "statements.hh"

#include "../../utils/error.hh"

#include <iostream>

RVPtr eval_binary_expr(std::shared_ptr<ASTBinaryExpr> bin, Environment* env, std::size_t line)
{
    RVPtr left_val = evaluate(bin->left, env, line);
    RVPtr right_val = evaluate(bin->right, env, line);
    const std::string& op = bin->op;

    if (!left_val || !right_val || left_val->kind == VAL_NULL || right_val->kind == VAL_NULL)
        return std::make_shared<NullValue>();

    // Short-circuit logical operators
    if (op == "&&" || op == "and") {
        bool left_bool = std::dynamic_pointer_cast<BoolValue>(cast(left_val, VAL_BOOL, line))->value;
        if (!left_bool) return std::make_shared<BoolValue>(false);
        bool right_bool = std::dynamic_pointer_cast<BoolValue>(cast(right_val, VAL_BOOL, line))->value;
        return std::make_shared<BoolValue>(left_bool && right_bool);
    }
    if (op == "||" || op == "or") {
        bool left_bool = std::dynamic_pointer_cast<BoolValue>(cast(left_val, VAL_BOOL, line))->value;
        if (left_bool) return std::make_shared<BoolValue>(true);
        bool right_bool = std::dynamic_pointer_cast<BoolValue>(cast(right_val, VAL_BOOL, line))->value;
        return std::make_shared<BoolValue>(left_bool || right_bool);
    }

    // Arithmetic
    if (op == "+") return left_val->add(right_val, line);
    if (op == "-") return left_val->sub(right_val, line);
    if (op == "*") return left_val->mul(right_val, line);
    if (op == "/") return left_val->div(right_val, line);

    if (op == "==") return left_val->eq(right_val, line);
    if (op == "!=") return left_val->neq(right_val, line);
    if (op == ">") return left_val->gt(right_val, line);
    if (op == ">=") return left_val->gte(right_val, line);
    if (op == "<") return left_val->lt(right_val, line);
    if (op == "<=") return left_val->lte(right_val, line);

    runtime_err("unknown binary operator '" + op + "'", line);
    return nullptr;
}

RVPtr eval_unary_expr(std::shared_ptr<ASTUnaryExpr> unary, Environment* env, std::size_t line) {
    auto value = evaluate(unary->operand, env, line);

    if (!value) {
        runtime_err("ryc: null value in unary expression.", line);
    }

    if (unary->op == "-") {
        // Numeric negation
        switch (value->kind) {
            case VAL_INT: {
                int iv = std::dynamic_pointer_cast<IntValue>(value)->value;
                return std::make_shared<IntValue>(-iv);
            }
            case VAL_FLOAT: {
                double fv = std::dynamic_pointer_cast<FloatValue>(value)->value;
                return std::make_shared<FloatValue>(-fv);
            }
            default: {
                runtime_err("ryc: unary '-' can only be applied to numeric types.", line);
            }
        }
    }
    else if (unary->op == "+") {
        switch (value->kind) {
            case VAL_INT: {
                int iv = std::dynamic_pointer_cast<IntValue>(value)->value;
                return std::make_shared<IntValue>(iv);
            }
            case VAL_FLOAT: {
                double fv = std::dynamic_pointer_cast<FloatValue>(value)->value;
                return std::make_shared<FloatValue>(fv);
            }
            default: {
                runtime_err("ryc: unary '+' can only be applied to numeric types.", line);
            }
        }
    }
    else if (unary->op == "!") {
        // Boolean negation
        switch (value->kind) {
            case VAL_INT: {
                int iv = std::dynamic_pointer_cast<IntValue>(value)->value;
                return std::make_shared<BoolValue>(iv == 0);
            }
            case VAL_FLOAT: {
                double fv = std::dynamic_pointer_cast<FloatValue>(value)->value;
                return std::make_shared<BoolValue>(fv == 0.0);
            }
            case VAL_BOOL: {
                bool bv = std::dynamic_pointer_cast<BoolValue>(value)->value;
                return std::make_shared<BoolValue>(!bv);
            }
            default: {
                runtime_err("ryc: unary '!' can only be applied to truthy values.", line);
            }
        }
    }
    else if (unary->op == "++" || unary->op == "--") {
        // Ensure the operand is an identifier (we can only increment variables)
        auto ident = std::dynamic_pointer_cast<ASTIdentifierLiteral>(unary->operand);
        if (!ident) {
            runtime_err(std::string("ryc: " + unary->op + " can only be applied to identifiers."), line);
        }

        // Lookup the current variable value
        RVPtr oldVal = env->lookupVar(ident->name, line);
        if (!oldVal) {
            runtime_err("ryc: cannot " + std::string(unary->op == "++" ? "increment" : "decrement") + " uninitialized variable " + ident->name, line);
        }

        RVPtr newVal = nullptr;

        switch (oldVal->kind) {
            case VAL_INT: {
                int iv = std::dynamic_pointer_cast<IntValue>(oldVal)->value;
                int temp_v = unary->op == "++" ? iv + 1 : iv - 1;
                newVal = std::make_shared<IntValue>(temp_v);
                break;
            }
            case VAL_FLOAT: {
                double fv = std::dynamic_pointer_cast<FloatValue>(oldVal)->value;
                double temp_v = unary->op == "++" ? fv + 1.0 : fv - 1.0;
                newVal = std::make_shared<FloatValue>(temp_v);
                break;
            }
            default:
                runtime_err("ryc: unary '" + unary->op + "' can only be applied to numeric values.", line);
        }

        // Assign the incremented value back into the environment
        env->assignVar(ident->name, newVal, line);

        // Prefix (++x) → return new value
        // Postfix (x++) → return old value
        return unary->prefix ? newVal : oldVal;
    }
    else {
        const std::string err = "ryc: unknown unary operator '" + unary->op + "'.";
        runtime_err(err, line);
    }

    return nullptr;
}

RVPtr eval_assign_expr(std::shared_ptr<ASTAssignExpr> assign, Environment* env, std::size_t line)
{
    RVPtr value = evaluate(assign->value, env, assign->line);
    auto ident = std::dynamic_pointer_cast<ASTIdentifierLiteral>(assign->assignee);
    if (!ident)
    {
        runtime_err("invalid identifier for assignment expression", line);
    }

    return env->assignVar(ident->name, value, line);
}
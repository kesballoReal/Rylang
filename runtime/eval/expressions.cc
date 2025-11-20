#include "expressions.hh"
#include "statements.hh"

#include "../../utils/error.hh"
#include "../../utils/utils.hh"

#include <iostream>

/* Literals */

RVPtr eval_array_literal(std::shared_ptr<ASTArrayLiteral> arr, Environment* env, std::size_t line)
{
    std::vector<RVPtr> elems;

    for (auto &expr : arr->elements)
        elems.push_back(evaluate(expr, env, line));

    return std::make_shared<ArrayValue>(elems);
}

/* ------------------------- */

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
    if (op == "%") return left_val->mod(right_val, line);

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
            case VAL_NULL: {
                return std::make_shared<BoolValue>(true);
            }
            default: {
                runtime_err("ryc: unary '!' can only be applied to truthy values.", line);
            }
        }
    }
    else if (unary->op == "++" || unary->op == "--") {
        RVPtr oldVal = nullptr;
        RVPtr newVal = nullptr;

        if (auto ident = std::dynamic_pointer_cast<ASTIdentifierLiteral>(unary->operand)) {
            // Lookup the variable
            oldVal = env->lookupVar(ident->name, line);
            if (!oldVal) {
                runtime_err("ryc: cannot " + std::string(unary->op == "++" ? "increment" : "decrement") + " uninitialized variable " + ident->name, line);
            }

            switch (oldVal->kind) {
                case VAL_INT:
                    newVal = std::make_shared<IntValue>(
                        unary->op == "++" 
                            ? std::dynamic_pointer_cast<IntValue>(oldVal)->value + 1
                            : std::dynamic_pointer_cast<IntValue>(oldVal)->value - 1
                    );
                    break;
                case VAL_FLOAT:
                    newVal = std::make_shared<FloatValue>(
                        unary->op == "++" 
                            ? std::dynamic_pointer_cast<FloatValue>(oldVal)->value + 1.0
                            : std::dynamic_pointer_cast<FloatValue>(oldVal)->value - 1.0
                    );
                    break;
                default:
                    runtime_err("ryc: unary '" + unary->op + "' can only be applied to numeric values.", line);
            }

            env->assignVar(ident->name, newVal, line);
        }
        else if (auto member = std::dynamic_pointer_cast<ASTMemberExpr>(unary->operand)) {
            // Lookup the array or object
            RVPtr objVal = evaluate(member->object, env, line);

            if (objVal->kind != VAL_ARRAY) {
                runtime_err("ryc: unary '" + unary->op + "' can only be applied to numeric array elements", line);
            }

            auto arrVal = std::dynamic_pointer_cast<ArrayValue>(objVal);

            // Evaluate the index
            auto idxVal = evaluate(member->property, env, line);
            if (idxVal->kind != VAL_INT) {
                runtime_err("ryc: array index must be an integer", line);
            }

            int idx = std::dynamic_pointer_cast<IntValue>(idxVal)->value;

            if (idx < 0 || idx >= static_cast<int>(arrVal->elements.size())) {
                runtime_err("ryc: array index out of bounds", line);
            }

            oldVal = arrVal->elements[idx];

            switch (oldVal->kind) {
                case VAL_INT:
                    newVal = std::make_shared<IntValue>(
                        unary->op == "++" 
                            ? std::dynamic_pointer_cast<IntValue>(oldVal)->value + 1
                            : std::dynamic_pointer_cast<IntValue>(oldVal)->value - 1
                    );
                    break;
                case VAL_FLOAT:
                    newVal = std::make_shared<FloatValue>(
                        unary->op == "++" 
                            ? std::dynamic_pointer_cast<FloatValue>(oldVal)->value + 1.0
                            : std::dynamic_pointer_cast<FloatValue>(oldVal)->value - 1.0
                    );
                    break;
                default:
                    runtime_err("ryc: unary '" + unary->op + "' can only be applied to numeric values.", line);
            }

            arrVal->elements[idx] = newVal;
        }
        else {
            runtime_err("ryc: " + unary->op + " can only be applied to assignable values.", line);
        }

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
    // --- Member / array assignment: x[i] = value ---
    if (auto member = std::dynamic_pointer_cast<ASTMemberExpr>(assign->assignee)) {
        RVPtr objVal = evaluate(member->object, env, line);
        if (objVal->kind != VAL_ARRAY) runtime_err("attempting to index a non-array value", line);

        auto arr = std::dynamic_pointer_cast<ArrayValue>(objVal);
        RVPtr indexVal = evaluate(member->property, env, line);

        if (indexVal->kind != VAL_INT) runtime_err("array index must be an integer", line);
        size_t idx = static_cast<size_t>(std::dynamic_pointer_cast<IntValue>(indexVal)->value);
        if (idx >= arr->elements.size()) runtime_err("array index out of bounds", line);

        RVPtr value = evaluate(assign->value, env, line);
        arr->elements[idx] = value;

        return value;
    }

    // --- Regular assignment
    if (auto ident = std::dynamic_pointer_cast<ASTIdentifierLiteral>(assign->assignee)) {
        RVPtr value = evaluate(assign->value, env, line);

        return env->assignVar(ident->name, value, line);
    }

    runtime_err("invalid assignee in assignment expression", line);
    return nullptr;
}

RVPtr eval_member_expr(std::shared_ptr<ASTMemberExpr> node, Environment* env, std::size_t line)
{
    RVPtr obj = evaluate(node->object, env, line);

    if (!obj) runtime_err("null object in member expression", line);

    if (node->computed) {
        RVPtr prop = evaluate(node->property, env, line);

        int idx = 0;
        if (prop->kind == VAL_INT) {
            idx = std::dynamic_pointer_cast<IntValue>(prop)->value;
        } else {
            runtime_err("array index must be an integer", line);
        }

        auto arr = std::dynamic_pointer_cast<ArrayValue>(obj);
        if (!arr) runtime_err("object is not an array", line);

        if (idx < 0 || idx >= (int)arr->elements.size())
            runtime_err("array index out of bounds", line);

        return arr->elements[idx];
    }
}

RVPtr eval_call_expr(std::shared_ptr<ASTCallExpr> node, Environment* env, std::size_t line)
{
    auto callee = evaluate(node->callee, env, line);

    if (callee->kind != VAL_FUNCTION) {
        runtime_err("attempted to call a non-function value", line);
    }

    std::vector<RVPtr> args;
    for (auto& a : node->args) {
        args.push_back(evaluate(a, env, line));
    }

    // ----------------------------
    // Native function
    if (auto native = std::dynamic_pointer_cast<NativeFunctionValue>(callee)) {
        return native->func(args, env, line);
    }

    // ----------------------------
    // User-defined function
    if (auto func = std::dynamic_pointer_cast<FunctionValue>(callee)) {
        auto local_env = new Environment(func->closure);
        local_env->current_return_type = func->declaration->ret_type;

        for (size_t i = 0; i < func->declaration->params.size(); i++) {
            auto param = func->declaration->params[i];
            RVPtr arg_val = args[i];

            if (param->isArray) {
                if (arg_val->kind != VAL_ARRAY) {
                    runtime_err("expected array argument for parameter '" + param->name + "'", line);
                }
                auto arr = std::dynamic_pointer_cast<ArrayValue>(arg_val);
                
                ValueType elemType;
                if (param->type == "auto") {
                    // Infer element type from the first array element (or null if empty)
                    elemType = arr->elements.empty() ? VAL_NULL : arr->elements[0]->kind;
                } else {
                    elemType = stoval(param->type, func->declaration, env, line);
                }
                
                for (auto& elem : arr->elements) {
                    elem = cast(elem, elemType, line);
                }
                local_env->declareVar(param->name, arr, VAL_ARRAY, true, line);
            } else {
                ValueType expected_type;
                if (param->type == "auto") {
                    // Infer type directly from the argument's kind
                    expected_type = arg_val->kind;
                } else {
                    expected_type = stoval(param->type, func->declaration, env, line);
                }
                
                arg_val = cast(arg_val, expected_type, line);
                local_env->declareVar(param->name, arg_val, expected_type, true, line);
            }
        }

        auto body = std::static_pointer_cast<ASTBlockStmt>(func->declaration->body);
        auto res = eval_block_stmt(body, local_env, line);

        if (res->kind == VAL_RETURN) {
            res = std::dynamic_pointer_cast<ReturnValue>(res)->value;
        
        }
        delete local_env;
        return res;
    }

    runtime_err("unknown function type", line);
    return nullptr;
}

RVPtr eval_cast_expr(std::shared_ptr<ASTCastExpr> node, Environment* env, std::size_t line) {
    auto value = evaluate(node->target, env, line);
    ValueType target_type = stoval(node->type, node, env, line);

    // Null always converts to default values
    if (value->kind == VAL_NULL) {
        switch (target_type) {
            case VAL_INT:    return std::make_shared<IntValue>(0);
            case VAL_FLOAT:  return std::make_shared<FloatValue>(0.0);
            case VAL_BOOL:   return std::make_shared<BoolValue>(false);
            case VAL_STRING: return std::make_shared<StringValue>("null");
            case VAL_ARRAY:  return std::make_shared<ArrayValue>(std::vector<RVPtr>{});
            case VAL_NULL:   return std::make_shared<NullValue>();
            default:         return std::make_shared<NullValue>();
        }
    }

    switch (target_type) {
        case VAL_INT:
            switch (value->kind) {
                case VAL_FLOAT:
                    return std::make_shared<IntValue>(static_cast<int>(std::dynamic_pointer_cast<FloatValue>(value)->value));
                case VAL_BOOL:
                    return std::make_shared<IntValue>(std::dynamic_pointer_cast<BoolValue>(value)->value ? 1 : 0);
                case VAL_STRING: {
                    try {
                        int v = std::stoi(std::dynamic_pointer_cast<StringValue>(value)->value);
                        return std::make_shared<IntValue>(v);
                    } catch (...) {
                        return std::make_shared<NullValue>();
                    }
                }
                case VAL_INT:
                    return value;
                default:
                    return std::make_shared<NullValue>();
            }
            break;

        case VAL_FLOAT:
            switch (value->kind) {
                case VAL_INT:
                    return std::make_shared<FloatValue>(static_cast<double>(std::dynamic_pointer_cast<IntValue>(value)->value));
                case VAL_BOOL:
                    return std::make_shared<FloatValue>(std::dynamic_pointer_cast<BoolValue>(value)->value ? 1.0 : 0.0);
                case VAL_STRING: {
                    try {
                        double v = std::stod(std::dynamic_pointer_cast<StringValue>(value)->value);
                        return std::make_shared<FloatValue>(v);
                    } catch (...) {
                        return std::make_shared<NullValue>();
                    }
                }
                case VAL_FLOAT:
                    return value;
                default:
                    return std::make_shared<NullValue>();
            }
            break;

        case VAL_BOOL:
            switch (value->kind) {
                case VAL_INT:
                    return std::make_shared<BoolValue>(std::dynamic_pointer_cast<IntValue>(value)->value != 0);
                case VAL_FLOAT:
                    return std::make_shared<BoolValue>(std::dynamic_pointer_cast<FloatValue>(value)->value != 0.0);
                case VAL_STRING:
                    return std::make_shared<BoolValue>(!std::dynamic_pointer_cast<StringValue>(value)->value.empty());
                case VAL_BOOL:
                    return value;
                case VAL_ARRAY:
                    return std::make_shared<BoolValue>(!std::dynamic_pointer_cast<ArrayValue>(value)->elements.empty());
                default:
                    return std::make_shared<NullValue>();
            }
            break;

        case VAL_STRING:
            switch (value->kind) {
                case VAL_INT:
                    return std::make_shared<StringValue>(std::to_string(std::dynamic_pointer_cast<IntValue>(value)->value));
                case VAL_FLOAT:
                    return std::make_shared<StringValue>(std::to_string(std::dynamic_pointer_cast<FloatValue>(value)->value));
                case VAL_BOOL:
                    return std::make_shared<StringValue>(std::dynamic_pointer_cast<BoolValue>(value)->value ? "true" : "false");
                case VAL_STRING:
                    return value;
                case VAL_ARRAY: {
                    std::string s = "[";
                    auto arr = std::dynamic_pointer_cast<ArrayValue>(value);
                    for (size_t i = 0; i < arr->elements.size(); ++i) {
                        s += vtostr(arr->elements[i]->kind);
                        if (i + 1 < arr->elements.size()) s += ", ";
                    }
                    s += "]";
                    return std::make_shared<StringValue>(s);
                }
                case VAL_NULL:
                    return std::make_shared<StringValue>("null");
                default:
                    return std::make_shared<NullValue>();
            }
            break;
        case VAL_CHAR:
            switch (value->kind) {
                case VAL_INT:
                    return std::make_shared<CharValue>(static_cast<char>(std::dynamic_pointer_cast<IntValue>(value)->value));
                case VAL_STRING: {
                    auto s = std::dynamic_pointer_cast<StringValue>(value)->value;
                    if (!s.empty()) return std::make_shared<CharValue>(s[0]);
                    return std::make_shared<NullValue>();
                }
                case VAL_CHAR:
                    return value;
                default:
                    return std::make_shared<NullValue>();
            }
        case VAL_ARRAY:
            return std::make_shared<ArrayValue>(std::vector<RVPtr>{value});

        case VAL_NULL:
            return std::make_shared<NullValue>();

        default:
            return std::make_shared<NullValue>();
    }

    return nullptr;
}

#include "statements.hh"
#include "../../utils/utils.hh"
#include "../../utils/error.hh"

RVPtr default_val(ValueType targetType, std::size_t line)
{
    switch (targetType) {
        case VAL_INT:     return std::make_shared<IntValue>(0); break;
        case VAL_FLOAT:   return std::make_shared<FloatValue>(0.0); break;
        case VAL_BOOL:    return std::make_shared<BoolValue>(false); break;
        case VAL_STRING:  return std::make_shared<StringValue>(""); break;
        case VAL_CHAR:    return std::make_shared<CharValue>('\0'); break;
        case VAL_NULL:    return std::make_shared<NullValue>(); break;
        default: runtime_err("unknown variable type", line);
    }

    return nullptr;
}

RVPtr cast(RVPtr value, ValueType targetType, std::size_t line)
{
    if (!value) return std::make_shared<NullValue>();
    if (value->kind == targetType) return value;

    try {
        switch (targetType) {
            case VAL_INT:
                if (value->kind == VAL_FLOAT)
                    return std::make_shared<IntValue>(
                        static_cast<int>(std::dynamic_pointer_cast<FloatValue>(value)->value)
                    );
                if (value->kind == VAL_BOOL)
                    return std::make_shared<IntValue>(
                        std::dynamic_pointer_cast<BoolValue>(value)->value ? 1 : 0
                    );
                if (value->kind == VAL_CHAR)
                    return std::make_shared<IntValue>(
                        static_cast<int>(std::dynamic_pointer_cast<CharValue>(value)->value)
                    );
                break;

            case VAL_FLOAT:
                if (value->kind == VAL_INT)
                    return std::make_shared<FloatValue>(
                        static_cast<double>(std::dynamic_pointer_cast<IntValue>(value)->value)
                    );
                if (value->kind == VAL_BOOL)
                    return std::make_shared<FloatValue>(
                        std::dynamic_pointer_cast<BoolValue>(value)->value ? 1.0 : 0.0
                    );
                if (value->kind == VAL_CHAR)
                    return std::make_shared<FloatValue>(
                        static_cast<double>(std::dynamic_pointer_cast<CharValue>(value)->value)
                    );
                break;

            case VAL_BOOL:
                if (value->kind == VAL_INT)
                    return std::make_shared<BoolValue>(
                        std::dynamic_pointer_cast<IntValue>(value)->value != 0
                    );
                if (value->kind == VAL_FLOAT)
                    return std::make_shared<BoolValue>(
                        std::dynamic_pointer_cast<FloatValue>(value)->value != 0.0
                    );
                if (value->kind == VAL_CHAR)
                    return std::make_shared<BoolValue>(std::dynamic_pointer_cast<CharValue>(value)->value != 0);
                break;

            case VAL_STRING:
                switch (value->kind) {
                    case VAL_INT:
                        return std::make_shared<StringValue>(std::to_string(std::dynamic_pointer_cast<IntValue>(value)->value));
                    case VAL_FLOAT:
                        return std::make_shared<StringValue>(std::to_string(std::dynamic_pointer_cast<FloatValue>(value)->value));
                    case VAL_BOOL:
                        return std::make_shared<StringValue>(std::dynamic_pointer_cast<BoolValue>(value)->value ? "true" : "false");
                    case VAL_CHAR:
                        return std::make_shared<StringValue>(std::string(1, std::dynamic_pointer_cast<CharValue>(value)->value));
                    case VAL_STRING:
                        return value;
                    case VAL_NULL:
                        return std::make_shared<StringValue>("null");
                }
                break;
            case VAL_CHAR:
                if (value->kind == VAL_INT)
                    return std::make_shared<CharValue>(static_cast<char>(std::dynamic_pointer_cast<IntValue>(value)->value));
                if (value->kind == VAL_STRING) {
                    std::string s = std::dynamic_pointer_cast<StringValue>(value)->value;
                    if (!s.empty()) return std::make_shared<CharValue>(s[0]);
                }
                break;
            case VAL_ARRAY:
                if (value->kind == VAL_ARRAY)
                    return value;
                break;

            case VAL_NULL:
                return std::make_shared<NullValue>();
        }
    } catch (...) {
        runtime_err("cannot cast type", line);
    }

    runtime_err("cannot cast type", line);
}

bool is_truthy(RVPtr value)
{
    switch (value->kind) {
        case VAL_INT:
        {
            auto num = std::static_pointer_cast<IntValue>(value);
            return num->value != 0;
        }
        case VAL_FLOAT:
        {
            auto num = std::static_pointer_cast<FloatValue>(value);
            return num->value != 0.0;
        }
        case VAL_BOOL:
        {
            auto b = std::static_pointer_cast<BoolValue>(value);
            return b->value;
        }
        case VAL_STRING:
        {
            auto str = std::static_pointer_cast<StringValue>(value);
            return !str->value.empty();
        }
        case VAL_NULL:
            return false;
        default:
            return true;
    }
}


RVPtr eval_var_declaration(std::shared_ptr<ASTVarDecl> node, Environment* env, std::size_t line)
{
    bool infer_type = (node->type == "auto");
    ValueType type = VAL_NULL;
    RVPtr value = nullptr;

    if (node->is_array) {
        if (!node->value.has_value() || !node->value.value()) {
            runtime_err("cannot declare array without initializer", line);
        }

        auto arrLit = std::dynamic_pointer_cast<ASTArrayLiteral>(node->value.value());
        value = evaluate(arrLit, env, line);

        if (value->kind != VAL_ARRAY) {
            runtime_err("initializer is not an array", line);
        }

        auto arrVal = std::dynamic_pointer_cast<ArrayValue>(value);

        ValueType elemType;
        if (infer_type) {
            if (arrVal->elements.empty()) {
                runtime_err("cannot infer type of empty array", line);
            }
            elemType = arrVal->elements[0]->kind;
            type = elemType;
        } else {
            type = stoval(node->type, node, env, line);
            elemType = type;
        }

        std::size_t declared_size = 0;
        if (node->array_size.has_value()) {
            declared_size = std::dynamic_pointer_cast<IntValue>(
                evaluate(node->array_size.value(), env, line)
            )->value;

            if (arrVal->elements.size() > declared_size) {
                runtime_err("array initializer has more elements than declared size", line);
            }

            while (arrVal->elements.size() < declared_size) {
                arrVal->elements.push_back(default_val(elemType, line));
            }
        }

        for (auto& elem : arrVal->elements) {
            elem = cast(elem, elemType, line);
        }

        type = VAL_ARRAY;

    } else {
        if (!node->value.has_value() || !node->value.value()) {
            if (infer_type)
                runtime_err("cannot infer type for uninitialized 'auto' variable", line);

            type = stoval(node->type, node, env, line);
            value = default_val(type, line);
        } else {
            value = evaluate(node->value.value(), env, line);
            if (infer_type) {
                type = value->kind;
            } else {
                type = stoval(node->type, node, env, line);
                if (!std::dynamic_pointer_cast<ASTCastExpr>(node->value.value())) {
                    value = cast(value, type, line);
                }
            }
        }
    }

    env->declareVar(node->name, value, type, node->is_const, line);
    return value;
}

RVPtr eval_if_stmt(std::shared_ptr<ASTIfStmt> node, Environment* env, std::size_t line)
{
    auto condition = evaluate(node->condition, env, line);

    if (is_truthy(condition))
    {
        return evaluate(node->thenBranch, env, line);
    }
    else if (node->elseBranch.has_value())
    {
        return evaluate(node->elseBranch.value(), env, line);
    }


    return std::make_shared<NullValue>();
}

RVPtr eval_while_stmt(std::shared_ptr<ASTWhileStmt> node, Environment* env, std::size_t line)
{
    RVPtr last_eval = std::make_shared<NullValue>();

    while (true)
    {
        auto condition = evaluate(node->condition, env, line);
        if (!is_truthy(condition)) break;

        RVPtr result = eval_block_stmt(std::static_pointer_cast<ASTBlockStmt>(node->doBranch), env, line);

        if (result->kind == VAL_BREAK) break;
        if (result->kind == VAL_CONTINUE) continue;

        last_eval = result;
    }

    return last_eval;
}

RVPtr eval_for_stmt(std::shared_ptr<ASTForStmt> node, Environment* env, std::size_t line)
{
    RVPtr last_eval = std::make_shared<NullValue>();

    if (node->init) {
        if (auto varDecl = std::dynamic_pointer_cast<ASTVarDecl>(node->init)) {
            eval_var_declaration(varDecl, env, line);
        } else {
            evaluate(std::dynamic_pointer_cast<Expr>(node->init), env, line);
        }
    }

    while (true) {
        if (node->condition) {
            auto condVal = evaluate(node->condition, env, line);
            if (!is_truthy(condVal)) break;
        }

        RVPtr result = eval_block_stmt(std::dynamic_pointer_cast<ASTBlockStmt>(node->body), env, line);

        if (result->kind == VAL_BREAK) break;
        if (result->kind == VAL_CONTINUE) {
        } else {
            last_eval = result;
        }

        if (node->update) {
            evaluate(node->update, env, line);
        }
    }

    return last_eval;
}


RVPtr eval_block_stmt(std::shared_ptr<ASTBlockStmt> node, Environment* env, std::size_t line)
{
    Environment* child_env = new Environment(env);
    child_env->current_return_type = env->current_return_type;

    RVPtr last_eval = std::make_shared<NullValue>();

    for (auto &stmt : node->block)
    {
        RVPtr result = evaluate(stmt, child_env, line);

        if (result->kind == VAL_BREAK || result->kind == VAL_CONTINUE || result->kind == VAL_RETURN)
        {
            delete child_env;
            return result;
        }

        last_eval = result;
    }

    delete child_env;
    return last_eval;
}

RVPtr eval_func_stmt(std::shared_ptr<ASTFunctionStmt> node, Environment* env, std::size_t line)
{
    auto funcVal = std::make_shared<FunctionValue>(node, env);
    funcVal->closure->current_return_type = funcVal->declaration->ret_type;

    env->declareVar(node->name, funcVal, VAL_FUNCTION, true, line);

    return funcVal;
}

RVPtr eval_return_stmt(std::shared_ptr<ASTReturnStmt> node, Environment* env, std::size_t line) {
    RVPtr value = nullptr;

    if (node->value) {
        value = evaluate(node->value, env, line);
    } else {
        value = std::make_shared<NullValue>();
    }

    if (env->current_return_type == "void" && node->value) {
        runtime_err("void functions cannot return a value", line);
    }

    return std::make_shared<ReturnValue>(value);
}

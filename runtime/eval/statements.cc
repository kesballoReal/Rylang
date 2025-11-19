#include "statements.hh"
#include "../../utils/utils.hh"
#include "../../utils/error.hh"

RVPtr cast(RVPtr value, ValueType targetType, std::size_t line)
{
    if (!value) return std::make_shared<NullValue>();
    if (value->kind == targetType) return value;

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
            break;
        case VAL_STRING:
            switch (value->kind) {
                case VAL_INT: {
                    int v = std::dynamic_pointer_cast<IntValue>(value)->value;
                    return std::make_shared<StringValue>(std::to_string(v));
                }
                case VAL_FLOAT: {
                    double v = std::dynamic_pointer_cast<FloatValue>(value)->value;
                    return std::make_shared<StringValue>(std::to_string(v));
                }
                case VAL_BOOL: {
                    bool v = std::dynamic_pointer_cast<BoolValue>(value)->value;
                    return std::make_shared<StringValue>(v ? "true" : "false");
                }
                case VAL_STRING:
                    return value;
                case VAL_NULL:
                    return std::make_shared<StringValue>("null");
            }
            break;
        case VAL_NULL:
            return std::make_shared<NullValue>();
    }

    runtime_err("cannot cast type", line);
    return nullptr;
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

    if (!node->value.has_value() || !node->value.value()) {
        if (infer_type)
            runtime_err("cannot infer type for uninitialized 'auto' variable", line);
    
        type = stoval(node->type, node, env, line);
        switch (type) {
            case VAL_INT:   value = std::make_shared<IntValue>(0); break;
            case VAL_FLOAT: value = std::make_shared<FloatValue>(0.0); break;
            case VAL_BOOL:  value = std::make_shared<BoolValue>(false); break;
            case VAL_NULL:  value = std::make_shared<NullValue>(); break;
            default: runtime_err("unknown variable type", line);
        }
    } else {
        value = evaluate(node->value.value(), env, line);
        if (infer_type) {
            type = value->kind;
        } else {
            type = stoval(node->type, node, env, line);
            value = cast(value, type, line);
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

RVPtr eval_block_stmt(std::shared_ptr<ASTBlockStmt> node, Environment* env, std::size_t line)
{
    Environment* child_env = new Environment(env);
    RVPtr last_eval = std::make_shared<NullValue>();

    for (auto &stmt : node->block)
    {
        RVPtr result = evaluate(stmt, child_env, line);

        if (result->kind == VAL_BREAK || result->kind == VAL_CONTINUE)
        {
            delete child_env;
            return result;
        }

        last_eval = result;
    }

    delete child_env;
    return last_eval;
}

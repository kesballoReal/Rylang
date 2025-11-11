#include "statements.hh"
#include "../../utils/error.hh"

#include <iostream>

/* Helpers */


ValueType stoval(const std::string& type, std::shared_ptr<ASTVarDecl> node, std::size_t line)
{
    if (type == "int")   return VAL_INT;
    if (type == "float") return VAL_FLOAT;
    if (type == "null")  return VAL_NULL;

    if (type == "auto")
    {
        if (!node->value.has_value() || !node->value.value()) {
            std::string err = "cannot infer type for 'auto' variable '" 
                      + node->name + "' without initializer.";
            runtime_err(err, line);
        }

        auto expr = node->value.value();

        switch (expr->kind)
        {
            case NodeType::NumericLiteral:
                return VAL_INT;
            case NodeType::NullLiteral:
                return VAL_NULL;
            default:
                std::string err = "cannot infer type from expression for variable '"
                          + node->name + "'.";
                runtime_err(err, line);
        }
    }

    std::cerr << "ryc: unknown type name '" << type << "' in declaration of variable '"
              << node->name << "'." << std::endl;
    std::exit(1);
}

RVPtr cast(RVPtr value, ValueType targetType, std::size_t line)
{
    if (!value) return std::make_shared<NullValue>(nullptr);

    if (value->kind == targetType) return value;

    if (value->kind == VAL_INT && targetType == VAL_FLOAT) {
        int iv = std::dynamic_pointer_cast<IntValue>(value)->value;
        return std::make_shared<FloatValue>(static_cast<double>(iv));
    }

    if (value->kind == VAL_FLOAT && targetType == VAL_INT) {
        double fv = std::dynamic_pointer_cast<FloatValue>(value)->value;
        return std::make_shared<IntValue>(static_cast<int>(fv));
    }

    std::string err =
        "ryc: cannot implicitly cast from '"
        + vtostr(value->kind)
        + "' to '"
        + vtostr(targetType)
        + "'.";
    runtime_err(err, line);
}

RVPtr eval_var_declaration(std::shared_ptr<ASTVarDecl> node, Environment* env, std::size_t line)
{
    ValueType declaredType = stoval(node->type, node, line);
    RVPtr value = nullptr;

    // Evaluate initializer if present
    if (node->value.has_value() && node->value.value()) {
        value = evaluate(node->value.value(), env, line);

        // Attempt implicit numeric casting if types mismatch
        if (value->kind != declaredType) {
            value = cast(value, declaredType, line);
        }
    } else {
        // No initializer: provide default value for non-nullable types
        switch (declaredType) {
            case VAL_INT:
                value = std::make_shared<IntValue>(0);
                break;
            case VAL_FLOAT:
                value = std::make_shared<FloatValue>(0.0);
                break;
            case VAL_NULL:
                value = std::make_shared<NullValue>(nullptr);
                break;
            default:
                std::string err = "ryc: unknown type for variable '" + node->name + "'";
                runtime_err(err, line);
        }
    }

    // Declare the variable in the environment
    env->declareVar(node->name, value, line);

    return value;
}

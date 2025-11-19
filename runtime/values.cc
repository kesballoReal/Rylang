#include "values.hh"
#include <cmath>

// ---------------------- BoolValue ----------------------
BoolValue::BoolValue(bool v) : RuntimeValue(VAL_BOOL), value(v) {}
RVPtr BoolValue::eq(RVPtr other, std::size_t line) {
    if (other->kind == VAL_BOOL) return std::make_shared<BoolValue>(value == std::dynamic_pointer_cast<BoolValue>(other)->value);
    if (other->kind == VAL_INT) return std::make_shared<BoolValue>((value ? 1 : 0) == std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<BoolValue>((value ? 1.0 : 0.0) == std::dynamic_pointer_cast<FloatValue>(other)->value);
    return std::make_shared<BoolValue>(false);
}
RVPtr BoolValue::not_op(std::size_t line) { return std::make_shared<BoolValue>(!value); }

// ---------------------- FloatValue ----------------------
FloatValue::FloatValue(double v) : RuntimeValue(VAL_FLOAT), value(v) {}
RVPtr FloatValue::neg(std::size_t line) { return std::make_shared<FloatValue>(-value); }
RVPtr FloatValue::pos(std::size_t line) { return std::make_shared<FloatValue>(value); }
RVPtr FloatValue::not_op(std::size_t line) { return std::make_shared<BoolValue>(value == 0.0); }

RVPtr FloatValue::add(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<FloatValue>(value + std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<FloatValue>(value + std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot add Float and non-numeric type", line);
    return nullptr;
}
RVPtr FloatValue::sub(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<FloatValue>(value - std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<FloatValue>(value - std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot subtract Float and non-numeric type", line);
    return nullptr;
}
RVPtr FloatValue::mul(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<FloatValue>(value * std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<FloatValue>(value * std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot multiply Float and non-numeric type", line);
    return nullptr;
}
RVPtr FloatValue::div(RVPtr other, std::size_t line) {
    double b = 0.0;
    if (other->kind == VAL_INT) b = std::dynamic_pointer_cast<IntValue>(other)->value;
    else if (other->kind == VAL_FLOAT) b = std::dynamic_pointer_cast<FloatValue>(other)->value;
    else runtime_err("cannot divide Float and non-numeric type", line);
    if (b == 0.0) runtime_err("Division by zero error", line);
    return std::make_shared<FloatValue>(value / b);
}
RVPtr FloatValue::eq(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<BoolValue>(value == std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<BoolValue>(value == std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot compare Float with non-numeric type", line);
    return nullptr;
}
RVPtr FloatValue::gt(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<BoolValue>(value > std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<BoolValue>(value > std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot compare Float with non-numeric type", line);
    return nullptr;
}
RVPtr FloatValue::gte(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<BoolValue>(value >= std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<BoolValue>(value >= std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot compare Float with non-numeric type", line);
    return nullptr;
}
RVPtr FloatValue::lt(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<BoolValue>(value < std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<BoolValue>(value < std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot compare Float with non-numeric type", line);
    return nullptr;
}
RVPtr FloatValue::lte(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<BoolValue>(value <= std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<BoolValue>(value <= std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot compare Float with non-numeric type", line);
    return nullptr;
}

// ---------------------- IntValue ----------------------
IntValue::IntValue(int v) : RuntimeValue(VAL_INT), value(v) {}
RVPtr IntValue::neg(std::size_t line) { return std::make_shared<IntValue>(-value); }
RVPtr IntValue::pos(std::size_t line) { return std::make_shared<IntValue>(value); }
RVPtr IntValue::not_op(std::size_t line) { return std::make_shared<BoolValue>(value == 0); }

RVPtr IntValue::add(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<IntValue>(value + std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<FloatValue>(value + std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot add Int and non-numeric type", line);
    return nullptr;
}
RVPtr IntValue::sub(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<IntValue>(value - std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<FloatValue>(value - std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot subtract Int and non-numeric type", line);
    return nullptr;
}
RVPtr IntValue::mul(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<IntValue>(value * std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<FloatValue>(value * std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot multiply Int and non-numeric type", line);
    return nullptr;
}
RVPtr IntValue::div(RVPtr other, std::size_t line) {
    double b = 0.0;
    if (other->kind == VAL_INT) b = std::dynamic_pointer_cast<IntValue>(other)->value;
    else if (other->kind == VAL_FLOAT) b = std::dynamic_pointer_cast<FloatValue>(other)->value;
    else runtime_err("cannot divide Int and non-numeric type", line);
    if (b == 0.0) runtime_err("Division by zero", line);
    return std::make_shared<FloatValue>(value / b);
}
RVPtr IntValue::eq(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<BoolValue>(value == std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<BoolValue>(value == std::dynamic_pointer_cast<FloatValue>(other)->value);
    if (other->kind == VAL_BOOL) return std::make_shared<BoolValue>(value == (std::dynamic_pointer_cast<BoolValue>(other)->value ? 1 : 0));
    return std::make_shared<BoolValue>(false);
}
RVPtr IntValue::gt(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<BoolValue>(value > std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<BoolValue>(value > std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot compare Int with non-numeric type", line);
    return nullptr;
}
RVPtr IntValue::gte(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<BoolValue>(value >= std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<BoolValue>(value >= std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot compare Int with non-numeric type", line);
    return nullptr;
}
RVPtr IntValue::lt(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<BoolValue>(value < std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<BoolValue>(value < std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot compare Int with non-numeric type", line);
    return nullptr;
}
RVPtr IntValue::lte(RVPtr other, std::size_t line) {
    if (other->kind == VAL_INT) return std::make_shared<BoolValue>(value <= std::dynamic_pointer_cast<IntValue>(other)->value);
    if (other->kind == VAL_FLOAT) return std::make_shared<BoolValue>(value <= std::dynamic_pointer_cast<FloatValue>(other)->value);
    runtime_err("cannot compare Int with non-numeric type", line);
    return nullptr;
}

// ---------------------- StringValue ----------------------
StringValue::StringValue(std::string v) : RuntimeValue(VAL_STRING), value(v) {}
RVPtr StringValue::add(RVPtr other, std::size_t line) {
    if (other->kind != VAL_STRING)
    {
        runtime_err("cannot add String and non-string type", line);
    }
    return std::make_shared<StringValue>(value + std::static_pointer_cast<StringValue>(other)->value);
}
RVPtr StringValue::eq(RVPtr other, std::size_t line) {
    if (other->kind != VAL_STRING)
    {
        runtime_err("cannot compare String and non-string type", line);
    }
    return std::make_shared<BoolValue>(value == std::static_pointer_cast<StringValue>(other)->value);
}
RVPtr StringValue::gt(RVPtr other, std::size_t line) {
    if (other->kind != VAL_STRING)
    {
        runtime_err("cannot compare String and non-string type", line);
    }
    return std::make_shared<BoolValue>(value > std::static_pointer_cast<StringValue>(other)->value);
}
RVPtr StringValue::gte(RVPtr other, std::size_t line) {
    if (other->kind != VAL_STRING)
    {
        runtime_err("cannot compare String and non-string type", line);
    }
    return std::make_shared<BoolValue>(value < std::static_pointer_cast<StringValue>(other)->value);
}
RVPtr StringValue::lt(RVPtr other, std::size_t line) {
    if (other->kind != VAL_STRING)
    {
        runtime_err("cannot compare String and non-string type", line);
    }
    return std::make_shared<BoolValue>(value >= std::static_pointer_cast<StringValue>(other)->value);
}
RVPtr StringValue::lte(RVPtr other, std::size_t line) {
    if (other->kind != VAL_STRING)
    {
        runtime_err("cannot compare String and non-string type", line);
    }
    return std::make_shared<BoolValue>(value <= std::static_pointer_cast<StringValue>(other)->value);
}
// ---------------------- NullValue ----------------------
NullValue::NullValue() : RuntimeValue(VAL_NULL) {}

// ---------------------- Break&Continue ----------------------

BreakValue::BreakValue() : RuntimeValue(VAL_BREAK) {}
ContinueValue::ContinueValue() : RuntimeValue(VAL_CONTINUE) {}

// ---------------------- Base helper ----------------------
RVPtr RuntimeValue::neq(RVPtr other, std::size_t line) {
    auto result = std::dynamic_pointer_cast<BoolValue>(eq(other, line));
    return std::make_shared<BoolValue>(!result->value);
}

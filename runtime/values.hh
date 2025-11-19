#pragma once

#include <cstddef>
#include <string>
#include <memory>
#include "../utils/error.hh"

// ---------------------- Forward declarations ----------------------
struct RuntimeValue;
struct IntValue;
struct FloatValue;
struct BoolValue;
struct StringValue;
struct NullValue;

using RVPtr = std::shared_ptr<RuntimeValue>;

// Supported runtime types
enum ValueType {
    VAL_INT,
    VAL_FLOAT,
    VAL_BOOL,
    VAL_STRING,
    VAL_NULL,
    VAL_CONTINUE,
    VAL_BREAK,
};

// ---------------------- Base RuntimeValue ----------------------
struct RuntimeValue {
    ValueType kind;

    explicit RuntimeValue(ValueType k) : kind(k) {}
    virtual ~RuntimeValue() = default;

    // Binary operations
    virtual RVPtr add(RVPtr other, std::size_t line) { runtime_err("cannot add these types", line); return nullptr; }
    virtual RVPtr sub(RVPtr other, std::size_t line) { runtime_err("cannot subtract these types", line); return nullptr; }
    virtual RVPtr mul(RVPtr other, std::size_t line) { runtime_err("cannot multiply these types", line); return nullptr; }
    virtual RVPtr div(RVPtr other, std::size_t line) { runtime_err("cannot divide these types", line); return nullptr; }

    virtual RVPtr eq(RVPtr other, std::size_t line) { runtime_err("cannot compare these types", line); return nullptr; }
    virtual RVPtr neq(RVPtr other, std::size_t line);
    virtual RVPtr gt(RVPtr other, std::size_t line) { runtime_err("cannot compare these types", line); return nullptr; }
    virtual RVPtr gte(RVPtr other, std::size_t line) { runtime_err("cannot compare these types", line); return nullptr; }
    virtual RVPtr lt(RVPtr other, std::size_t line) { runtime_err("cannot compare these types", line); return nullptr; }
    virtual RVPtr lte(RVPtr other, std::size_t line) { runtime_err("cannot compare these types", line); return nullptr; }

    // Unary operations
    virtual RVPtr neg(std::size_t line) { runtime_err("cannot negate this type", line); return nullptr; }
    virtual RVPtr pos(std::size_t line) { runtime_err("cannot apply unary +", line); return nullptr; }
    virtual RVPtr not_op(std::size_t line) { runtime_err("cannot apply ! to this type", line); return nullptr; }
};

// ---------------------- BoolValue ----------------------
struct BoolValue final : RuntimeValue {
    bool value;
    explicit BoolValue(bool v);

    RVPtr eq(RVPtr other, std::size_t line) override;
    RVPtr not_op(std::size_t line) override;
};

// ---------------------- FloatValue ----------------------
struct FloatValue final : RuntimeValue {
    double value;
    explicit FloatValue(double v);

    RVPtr add(RVPtr other, std::size_t line) override;
    RVPtr sub(RVPtr other, std::size_t line) override;
    RVPtr mul(RVPtr other, std::size_t line) override;
    RVPtr div(RVPtr other, std::size_t line) override;

    RVPtr eq(RVPtr other, std::size_t line) override;
    RVPtr gt(RVPtr other, std::size_t line) override;
    RVPtr gte(RVPtr other, std::size_t line) override;
    RVPtr lt(RVPtr other, std::size_t line) override;
    RVPtr lte(RVPtr other, std::size_t line) override;

    RVPtr neg(std::size_t line) override;
    RVPtr pos(std::size_t line) override;
    RVPtr not_op(std::size_t line) override;
};

// ---------------------- IntValue ----------------------
struct IntValue final : RuntimeValue {
    int value;
    explicit IntValue(int v);

    RVPtr add(RVPtr other, std::size_t line) override;
    RVPtr sub(RVPtr other, std::size_t line) override;
    RVPtr mul(RVPtr other, std::size_t line) override;
    RVPtr div(RVPtr other, std::size_t line) override;

    RVPtr eq(RVPtr other, std::size_t line) override;
    RVPtr gt(RVPtr other, std::size_t line) override;
    RVPtr gte(RVPtr other, std::size_t line) override;
    RVPtr lt(RVPtr other, std::size_t line) override;
    RVPtr lte(RVPtr other, std::size_t line) override;

    RVPtr neg(std::size_t line) override;
    RVPtr pos(std::size_t line) override;
    RVPtr not_op(std::size_t line) override;
};

// ---------------------- StringValue ----------------------
struct StringValue final : RuntimeValue {
    std::string value;
    explicit StringValue(std::string v);

    RVPtr add(RVPtr other, std::size_t line) override;

    RVPtr eq(RVPtr other, std::size_t line) override;
    RVPtr gt(RVPtr other, std::size_t line) override;
    RVPtr gte(RVPtr other, std::size_t line) override;
    RVPtr lt(RVPtr other, std::size_t line) override;
    RVPtr lte(RVPtr other, std::size_t line) override;
};

// ---------------------- NullValue ----------------------
struct NullValue final : RuntimeValue {
    NullValue();
};

// ---------------------- Break&Continue ----------------------

struct BreakValue : public RuntimeValue {
    explicit BreakValue();
};

struct ContinueValue : public RuntimeValue {
    explicit ContinueValue();
};

// ---------------------- Helper ----------------------
inline std::string vtostr(ValueType type) {
    switch (type) {
        case VAL_INT:   return "int";
        case VAL_FLOAT: return "float";
        case VAL_BOOL:  return "bool";
        case VAL_STRING: return "string";
        case VAL_NULL:  return "null";
        default:        return "unknown";
    }
}


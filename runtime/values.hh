#pragma once

#include <cstddef>
#include <string>
#include <memory>
#include <functional>

#include "../parser/ast.hh"
#include "../utils/error.hh"

struct Environment;

// ---------------------- Forward declarations ----------------------
struct RuntimeValue;
struct IntValue;
struct FloatValue;
struct BoolValue;
struct StringValue;
struct CharValue;
struct ArrayValue;
struct NullValue;
struct FunctionValue;
struct ReturnValue;

using RVPtr = std::shared_ptr<RuntimeValue>;

// Supported runtime types
enum ValueType {
    VAL_INT,
    VAL_FLOAT,
    VAL_BOOL,
    VAL_STRING,
    VAL_CHAR,
    VAL_NULL,
    VAL_ARRAY,
    VAL_CONTINUE,
    VAL_BREAK,
    VAL_FUNCTION,
    VAL_RETURN,
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
    virtual RVPtr mod(RVPtr other, std::size_t line) { runtime_err("cannot module these types", line); return nullptr; }

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

// ---------------------- ArrayValue ----------------------
struct ArrayValue final : RuntimeValue {
    std::vector<RVPtr> elements;
    explicit ArrayValue(std::vector<RVPtr> e);
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
    RVPtr mod(RVPtr other, std::size_t line) override;

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
    RVPtr mod(RVPtr other, std::size_t line) override;

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

// ---------------------- CharValue ----------------------
struct CharValue final : RuntimeValue {
    char value;
    explicit CharValue(char v);

    RVPtr add(RVPtr other, std::size_t line) override;
    RVPtr eq(RVPtr other, std::size_t line) override;
    RVPtr neq(RVPtr other, std::size_t line);

    RVPtr neg(std::size_t line) override;
    RVPtr pos(std::size_t line) override;
    RVPtr not_op(std::size_t line) override;
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

// ---------------------- Return ----------------------
struct ReturnValue : public RuntimeValue {
    RVPtr value;

    explicit ReturnValue(RVPtr v) 
        : RuntimeValue(VAL_RETURN), value(v) {}
};


// ---------------------- FunctionValue ----------------------
struct FunctionValue final : RuntimeValue {
    std::shared_ptr<ASTFunctionStmt> declaration;
    Environment* closure;

    explicit FunctionValue(std::shared_ptr<ASTFunctionStmt> d, Environment* c);
};

struct NativeFunctionValue : public RuntimeValue {
    using FuncType = std::function<RVPtr(const std::vector<RVPtr>&, Environment*, std::size_t)>;

    std::string name;
    FuncType func;

    NativeFunctionValue(const std::string& n, FuncType f)
        : RuntimeValue(VAL_FUNCTION), name(n), func(f) {}
};


// ---------------------- Helper ----------------------
inline std::string vtostr(ValueType type) {
    switch (type) {
        case VAL_INT:      return "int";
        case VAL_FLOAT:    return "float";
        case VAL_BOOL:     return "bool";
        case VAL_STRING:   return "string";
        case VAL_CHAR:     return "char";
        case VAL_ARRAY:    return "array";
        case VAL_FUNCTION: return "function";
        case VAL_NULL:     return "null";
        case VAL_CONTINUE: return "continue";
        case VAL_BREAK:    return "break";
        case VAL_RETURN:   return "return";
        default:           return "unknown";
    }
}


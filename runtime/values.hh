/*

values.hh

*/

#pragma once

#include <cstddef>

enum ValueType { // RuntimeValues we support
    VAL_INT,
    VAL_FLOAT,
    VAL_NULL,
};

struct RuntimeValue {
    virtual ~RuntimeValue() = default;

    ValueType kind;
};

struct IntValue : RuntimeValue {
    int value;

    IntValue(int v) : value(v) {
        kind = ValueType::VAL_INT;
    }
};

struct FloatValue : RuntimeValue {
    double value;

    FloatValue(double v) : value(v) {
        kind = ValueType::VAL_FLOAT;
    }
};

struct NullValue : RuntimeValue {
    std::nullptr_t value;
    NullValue(std::nullptr_t v) : value(v) { kind = ValueType::VAL_NULL; }
};
#pragma once

#include "values.hh"

class NativeRegistry {
private:
    std::unordered_map<std::string, NativeFunctionValue::FuncType> functions;

    NativeRegistry() = default;

public:
    static NativeRegistry& instance() {
        static NativeRegistry inst;
        return inst;
    }

    void register_function(const std::string& name, NativeFunctionValue::FuncType func) {
        functions[name] = func;
    }

    bool has_function(const std::string& name) const {
        return functions.find(name) != functions.end();
    }

    NativeFunctionValue::FuncType get_function(const std::string& name) const {
        auto it = functions.find(name);
        if (it == functions.end()) return nullptr;
        return it->second;
    }

    const auto& all_functions() const { return functions; }
};

void register_default_native_functions();
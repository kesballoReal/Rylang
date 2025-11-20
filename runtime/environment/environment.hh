/*

environment.hh

*/

#pragma once

#include "../values.hh"

#include <optional>
#include <string>
#include <unordered_map>
#include <memory>

using RVPtr = std::shared_ptr<RuntimeValue>;

struct VarInfo {
    RVPtr value;
    bool isConst;
    ValueType type;
};

class Environment {
public:
    explicit Environment(Environment* parent = nullptr)
        : parent(parent) {}

    std::string current_return_type = "void";

    RVPtr declareVar(const std::string& name, RVPtr value, ValueType type, bool isConst, std::size_t line);
    RVPtr assignVar(const std::string& name, RVPtr value, std::size_t line);
    RVPtr lookupVar(const std::string& varname, std::size_t line);
    Environment* resolve(const std::string& varname, std::size_t line);
private:
    Environment* parent = nullptr;
    std::unordered_map<std::string, VarInfo> variables;
};
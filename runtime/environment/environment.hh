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

class Environment {
public:
    explicit Environment(Environment* parent = nullptr)
        : parent(parent) {}

    // Declares a variable
    RVPtr declareVar(const std::string& varname, RVPtr value, std::size_t line);
    RVPtr assignVar(const std::string& varname, RVPtr value, std::size_t line);
    RVPtr lookupVar(const std::string& varname, std::size_t line);
    Environment* resolve(const std::string& varname, std::size_t line);
private:
    Environment* parent = nullptr;
    std::unordered_map<std::string, std::shared_ptr<RuntimeValue>> variables;
};
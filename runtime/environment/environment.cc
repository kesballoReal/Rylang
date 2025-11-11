#include "environment.hh"

#include "../../utils/error.hh"

#include <iostream>



RVPtr Environment::declareVar(const std::string& varname, RVPtr value, std::size_t line)
{
    if (this->variables.find(varname) != this->variables.end())
    {
        // Variable already exists, cant re-declare it.
        std::string err =  "ryc: cannot declare the symbol '" + varname + "', as it is already defined.";
        runtime_err(err, line);
    }

    this->variables[varname] = value;
    return value;
}

RVPtr Environment::assignVar(const std::string& varname, RVPtr value, std::size_t line)
{
    auto env = this->resolve(varname, line);
    env->variables[varname] = value;
    return value;
}

RVPtr Environment::lookupVar(const std::string& varname, std::size_t line)
{
    auto env = this->resolve(varname, line);
    return env->variables[varname];
}

Environment* Environment::resolve(const std::string& varname, std::size_t line)
{
    if (this->variables.find(varname) != this->variables.end())
    {
        return this;
    }
    if (this->parent == nullptr)
    {
        std::string err = "ryc: cannot resolve symbol '" + varname + "', as it does not exist.";
        runtime_err(err, line);
    }
    return this->parent->resolve(varname, line);
}
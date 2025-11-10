#include "environment.hh"

#include <iostream>

RVPtr Environment::declareVar(const std::string& varname, RVPtr value)
{
    if (this->variables.find(varname) != this->variables.end())
    {
        // Variable already exists, cant re-declare it.
        std::cerr << "ryc: cannot declare the symbol '" << varname << "', as it is already defined." << std::endl;
        std::exit(1);
    }

    this->variables[varname] = value;
    return value;
}

RVPtr Environment::assignVar(const std::string& varname, RVPtr value)
{
    auto env = this->resolve(varname);
    env->variables[varname] = value;
    return value;
}

RVPtr Environment::lookupVar(const std::string& varname)
{
    auto env = this->resolve(varname);
    return env->variables[varname];
}

Environment* Environment::resolve(const std::string& varname)
{
    if (this->variables.find(varname) != this->variables.end())
    {
        return this;
    }
    if (this->parent == nullptr)
    {
        std::cerr << "ryc: cannot resolve symbol '" << varname << "', as it does not exist." << std::endl;
        std::exit(1);
    }
    return this->parent->resolve(varname);
}
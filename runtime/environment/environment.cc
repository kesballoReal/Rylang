#include <iostream>

#include "environment.hh"
#include "statements.hh"

#include "../../utils/error.hh"

RVPtr Environment::declareVar( const std::string& name,RVPtr value,ValueType type, bool isConst,std::size_t line){
    if (variables.find(name) != variables.end())
        runtime_err("ryc: cannot redeclare variable '" + name + "'", line);

    variables[name] = VarInfo{
        .value = value,
        .isConst = isConst,
        .type = type
    };
    return value;
}

RVPtr Environment::assignVar(const std::string& name, RVPtr value, std::size_t line)
{
    Environment* env = resolve(name, line);
    VarInfo& info = env->variables[name];

    if (info.isConst)
        runtime_err("ryc: cannot assign to constant variable '" + name + "'", line);

    // Cast to declared type
    info.value = cast(value, info.type, line);

    return info.value;
}

RVPtr Environment::lookupVar(const std::string& name, std::size_t line)
{
    return resolve(name, line)->variables[name].value;
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

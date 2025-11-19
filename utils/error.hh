#pragma once

#include <iostream>
#include <string>

#include "../parser/lexer.hh"

inline void syntax_err(const std::string& err, std::size_t l)
{
    std::cout << "ryc: Syntax Error: line " << l << ", " << err << std::endl;
    std::exit(1);
}

inline void runtime_err(const std::string& err, std::size_t l)
{
    std::cout << "ryc: Runtime Error: line " << l << ", " << err << std::endl;
    std::exit(1);
}

#include <iostream>
#include <fstream>
#include <string>

#include "parser/lexer.hh"
#include "parser/parser.hh"

#include "runtime/values.hh"
#include "runtime/environment/environment.hh"
#include "runtime/interpreter/interpreter.hh"

#include "utils/utils.hh"

#define LEXER_DEBUG 0
#define PARSER_DEBUG 1
#define INTERPRETER_DEBUG 1

int main(int argc, char** argv)
{   
    // ryc <source> command
    if (argc != 2)
    {
        std::cerr << "ryc: usage: ryc <file>" << std::endl;
        std::exit(1);
    }

    // file_path
    std::string f_path = argv[1];

    /* Open file */
    std::ifstream fptr(f_path);
    if (!fptr)
    {
        std::cerr << "ryc: no such file or directory: '" << f_path << '\'' << std::endl;
        return 1;
    }
    std::string src; // Source file to tokenize

    // Read line by line
    std::string l;
    while (std::getline(fptr, l))
    {
        src += l + '\n';
    }

    fptr.close();

    std::vector<Token> tokens = tokenize(src);

    if (LEXER_DEBUG)
    {   
        std::cout << "===== LEXER DEBUG =====" << std::endl;
        for (std::size_t i = 0; i < tokens.size(); i++)
        {
            std::cout << "Type: " << tokens[i].type << ", Value: " << tokens[i].value << std::endl;
        }
    }

    auto parser = Parser(tokens);
    auto program = parser.produceAST();

    if (PARSER_DEBUG)
    {   
        std::cout << "===== PARSER DEBUG =====" << std::endl;
        print_ast(program, 0);
    }

    Environment* env = new Environment();

    auto result = evaluate(program, env, 0);

    if (INTERPRETER_DEBUG)
    {   
        std::cout << "===== INTERPRETER DEBUG =====" << std::endl;
        std::cout << "Value: "; print_value(result);
        std::cout << "Type: " << vtostr(result->kind) << std::endl;
    }

    delete env;

    return 0;
}

/*

Example: ill keep this

    int         x               =               10;
    |           |               |               |
    V           V               V               V
    
    Keyword   Identifier        EqualsToken     Number
*/

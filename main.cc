#include <iostream>
#include <fstream>
#include <string>

#include "parser/lexer.hh"
#include "parser/parser.hh"

#include "runtime/values.hh"
#include "runtime/environment/environment.hh"
#include "runtime/interpreter/interpreter.hh"

#include "utils/utils.hh"

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
        src += l + '\n'; // we need '\n' so the content isnt all on a single line
    }

    fptr.close();

    std::vector<Token> tokens = tokenize(src);

    /* Tokens debugging  */

    for (std::size_t i = 0; i < tokens.size(); i++)
    {
        std::cout << "Type: " << tokens[i].type << ", Value: " << tokens[i].value << std::endl;
    }

    auto parser = Parser(tokens);
    auto program = parser.produceAST();
    print_ast(program, 0); // Comment this line to remove the debug for the AST

    Environment* env = new Environment();

    env->declareVar("x", std::make_shared<IntValue>(100));

    auto result = evaluate(program, env);

    print_value(result);

    return 0;
}

/*

Example: ill keep this

    int         x               =               10;
    |           |               |               |
    V           V               V               V
    
    Keyword   Identifier        EqualsToken     Number
*/

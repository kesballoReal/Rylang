#include <iostream>
#include <fstream>
#include <string>

#include "parser/lexer.hh"
#include "parser/parser.hh"

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
    std::string src; // Source file to tokenize

    // Read line by line
    std::string l;
    while (std::getline(fptr, l))
    {
        src += l + '\n'; // we need '\n' so the content isnt all on a single line
    }

    fptr.close();

    std::vector<Token> tokens = tokenize(src);

    /* Test to see if the tokens generated are good */
    /*for (std::size_t i = 0; i < tokens.size(); i++)
    {
        std::cout << "Type: " << tokens[i].type << ", Value: " << tokens[i].value << std::endl;
    }*/

    auto parser = Parser(tokens);
    auto program = parser.produceAST();

    for (std::size_t i = 0; i < program->body.size(); i++)
    {
        std::cout << program->body[i]->kind << std::endl;
    }

    return 0;
}

/*

Example: ill keep this

    int         x               =               10;
    |           |               |               |
    V           V               V               V
    
    Keyword   Identifier        EqualsToken     Number
*/

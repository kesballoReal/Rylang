#include "lexer.hh"

#include <iostream>

std::vector<Token> tokenize(const std::string &src)
{   
    std::vector<Token> tokens;

    // Loops trough the length of src
    for (std::size_t i = 0; i < src.length(); i++)
    {
        char c = src[i];

        if (c == '(') {
            tokens.push_back(Token("(", TokenType::LeftParen));
            continue;
        }
        else if (c == ')')
        {
            tokens.push_back(Token(")", TokenType::RightParen));
            continue;
        }
        else if (c == '=')
        {
            tokens.push_back(Token("=", TokenType::Equals));
            continue;
        }
        else if (c == '+')
        {
            tokens.push_back(Token("+", TokenType::Plus));
            continue;
        }
        else if (c == '-')
        {
            tokens.push_back(Token("-", TokenType::Minus));
            continue;
        }
        else if (c == '*')
        {
            tokens.push_back(Token("*", TokenType::Star));
            continue;
        }
        else if (c == '/')
        {
            tokens.push_back(Token("/", TokenType::Slash));
            continue;
        }       
        else
        {   
            // Skips the spaces in between the characters
            if (i < src.length() && std::isspace(c))
            {
                continue;
            }
            // Checks for identifiers
            if (i < src.length() && std::isalpha(c))
            {
                std::string ident;

                while ((i < src.length() && std::isalpha(src[i])))
                {// like this right? 
                    ident += src[i];
                    i++;
                }

                tokens.push_back(Token(ident, TokenType::Identifier));
                i--;
            }
            // Checks for numbers
            if (i < src.length() && std::isdigit(c))
            {
                std::string num; 
                bool hasDot = false;

                while ((i < src.length() && (std::isdigit(src[i]) || src[i] == '.'))) 
                {
                    if (src[i] == '.') 
                    {   
                        if (hasDot) {
                            std::cerr << "ryc: invalid numeric literal found during lexing: " << num << std::endl;
                            std::exit(1);
                        }
                        hasDot = true;
                    }

                    num += src[i];
                    i++;
                }

                tokens.push_back(Token(num, TokenType::Number));
                i--; 
            }
        }
    }

    tokens.push_back(Token("EoF", TokenType::EoF));
    return tokens;
}
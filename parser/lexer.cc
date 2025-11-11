#include "lexer.hh"
#include "../utils/error.hh"

#include <iostream>

std::unordered_map<std::string, TokenType> keywords = 
{
    {"null", TokenType::NullTok},

    {"var", TokenType::VarTok},
    {"const", TokenType::ConstTok},
    /* Datatypes */
    {"int", TokenType::DataType},
    {"float", TokenType::DataType},
    {"auto", TokenType::DataType}
};

std::vector<Token> tokenize(const std::string &src)
{   
    std::vector<Token> tokens;
    std::size_t line = 1;

    for (std::size_t i = 0; i < src.length(); i++)
    {
        char c = src[i];

        if (c == '\n') { line++; continue; }
        if (std::isspace(c)) continue;

        std::size_t token_line = line; // capture line at start of token

        // Single-character symbols
        if (c == '(') { tokens.push_back(Token("(", TokenType::LeftParen, token_line)); continue; }
        if (c == ')') { tokens.push_back(Token(")", TokenType::RightParen, token_line)); continue; }
        if (c == '=') { tokens.push_back(Token("=", TokenType::Equals, token_line)); continue; }
        if (c == '+') { tokens.push_back(Token("+", TokenType::Plus, token_line)); continue; }
        if (c == '-') { tokens.push_back(Token("-", TokenType::Minus, token_line)); continue; }
        if (c == '*') { tokens.push_back(Token("*", TokenType::Star, token_line)); continue; }
        if (c == '/') { tokens.push_back(Token("/", TokenType::Slash, token_line)); continue; }
        if (c == ':') { tokens.push_back(Token(":", TokenType::Colon, token_line)); continue; }
        if (c == ';') { tokens.push_back(Token(";", TokenType::Semicolon, token_line)); continue; }

        // Identifiers / keywords
        if (std::isalpha(c))
        {
            std::string ident;
            while (i < src.length() && std::isalpha(src[i]))
            {
                ident += src[i];
                i++;
            }
            TokenType type = (keywords.find(ident) == keywords.end()) ? TokenType::Identifier : keywords[ident];
            tokens.push_back(Token(ident, type, token_line));
            i--; // step back after overshoot
            continue;
        }

        // Numbers
        if (std::isdigit(c))
        {
            std::string num;
            bool hasDot = false;
            while (i < src.length() && (std::isdigit(src[i]) || src[i] == '.'))
            {
                if (src[i] == '.')
                {
                    if (hasDot) {
                        syntax_err("invalid numeric literal: " + num, token_line);
                    }
                    hasDot = true;
                }
                num += src[i];
                i++;
            }
            tokens.push_back(Token(num, TokenType::Number, token_line));
            i--; // step back
            continue;
        }

        // Unexpected character
        syntax_err(std::string("unexpected character found while lexing: ") + c, token_line);
    }

    tokens.push_back(Token("EoF", TokenType::EoF, line));
    return tokens;
}
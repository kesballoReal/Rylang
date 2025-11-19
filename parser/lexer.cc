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
    {"bool", TokenType::DataType},
    {"string", TokenType::DataType},
    {"auto", TokenType::DataType},

    {"true", TokenType::TrueTok},
    {"false", TokenType::FalseTok},

    /* Control Flow */
    {"if", TokenType::IfTok},
    {"else", TokenType::ElseTok},
    {"while", TokenType::WhileTok},

    {"continue", TokenType::ContinueTok},
    {"break", TokenType::BreakTok},
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

        // Multi-character symbols
        if (src[i] == '+' && src[i+1] == '+') { tokens.emplace_back("++", TokenType::UnaryOP, token_line); i++; continue; }
        if (src[i] == '-' && src[i+1] == '-') { tokens.emplace_back("--", TokenType::UnaryOP, token_line); i++; continue; }
        if (src[i] == '&' && src[i+1] == '&') { tokens.emplace_back("&&", TokenType::BinaryOP, token_line); i++; continue; }
        if (src[i] == '|' && src[i+1] == '|') { tokens.emplace_back("||", TokenType::BinaryOP, token_line); i++; continue; }
		if (src[i] == '=' && src[i+1] == '=') { tokens.emplace_back("==", TokenType::BinaryOP, token_line); i++; continue; }
		if (src[i] == '!' && src[i+1] == '=') { tokens.emplace_back("!=", TokenType::BinaryOP, token_line); i++; continue; }
		if (src[i] == '>' && src[i+1] == '=') { tokens.emplace_back(">=", TokenType::BinaryOP, token_line); i++; continue; }
		if (src[i] == '<' && src[i+1] == '=') { tokens.emplace_back("<=", TokenType::BinaryOP, token_line); i++; continue; }
        // Single-character symbols
        if (c == '(') { tokens.emplace_back("(", TokenType::LeftParen, token_line); continue; }
        if (c == ')') { tokens.emplace_back(")", TokenType::RightParen, token_line); continue; }
        if (c == '{') { tokens.emplace_back("{", TokenType::LeftBrace, token_line); continue; }
        if (c == '}') { tokens.emplace_back("}", TokenType::RightBrace, token_line); continue; }
        if (c == '=') { tokens.emplace_back("=", TokenType::Equals, token_line); continue; }
        if (c == '+') { tokens.emplace_back("+", TokenType::Plus, token_line); continue; }
        if (c == '-') { tokens.emplace_back("-", TokenType::Minus, token_line); continue; }
        if (c == '*') { tokens.emplace_back("*", TokenType::Star, token_line); continue; }
        if (c == '/') { tokens.emplace_back("/", TokenType::Slash, token_line); continue; }
        if (c == ':') { tokens.emplace_back(":", TokenType::Colon, token_line); continue; }
        if (c == ';') { tokens.emplace_back(";", TokenType::Semicolon, token_line); continue; }
        if (c == '!') { tokens.emplace_back("!", TokenType::UnaryOP, token_line); continue; }
        if (c == '>') { tokens.emplace_back(">", TokenType::BinaryOP, token_line); continue; }
        if (c == '<') { tokens.emplace_back("<", TokenType::BinaryOP, token_line); continue; }

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
            tokens.emplace_back(ident, type, token_line);
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
            tokens.emplace_back(num, TokenType::Number, token_line);
            i--; // step back
            continue;
        }
        if (c == '"') {
            std::string str;
            i++;

            while (i < src.length() && src[i] != '"') {
                if (src[i] == '\\' && i + 1 < src.length() && src[i + 1] == '"') {
                    str += '"';
                    i += 2;
                    continue;
                }
                str += src[i];
                i++;
            }

            if (i >= src.length() || src[i] != '"') {
                syntax_err("unterminated string literal", token_line);
            }

            tokens.emplace_back(str, TokenType::String, token_line);
            continue;
        }

        syntax_err(std::string("unexpected character found while lexing: ") + c, token_line);
    }

    tokens.emplace_back("EoF", TokenType::EoF, line);
    return tokens;
}

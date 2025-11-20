#include "lexer.hh"
#include "../utils/error.hh"

#include <iostream>

std::unordered_map<std::string, TokenType> keywords = 
{
    {"null", TokenType::NullTok},
    {"nullptr", TokenType::NullptrTok},

    {"var", TokenType::VarTok},
    {"const", TokenType::ConstTok},
    /* Datatypes */
    {"int", TokenType::DataType},
    {"float", TokenType::DataType},
    {"bool", TokenType::DataType},
    {"string", TokenType::DataType},
    {"char", TokenType::DataType},
    {"auto", TokenType::DataType},
    {"void", TokenType::VoidTok},

    {"true", TokenType::TrueTok},
    {"false", TokenType::FalseTok},

    /* Control Flow */
    {"if", TokenType::IfTok},
    {"else", TokenType::ElseTok},
    {"while", TokenType::WhileTok},
    {"for", TokenType::ForTok},

    {"func", TokenType::FuncTok},
    {"return", TokenType::ReturnTok},

    {"static_cast", TokenType::CastTok},

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

        if (c == '/' && i + 1 < src.length() && src[i + 1] == '/') {
            i += 2;
            while (i < src.length() && src[i] != '\n') i++;
            line++; // increment line if newline is encountered
            continue;
        }

        // Multi-line comment
        if (c == '/' && i + 1 < src.length() && src[i + 1] == '*') {
            i += 2;
            while (i + 1 < src.length() && !(src[i] == '*' && src[i + 1] == '/')) {
                if (src[i] == '\n') line++;
                i++;
            }
            if (i + 1 >= src.length()) {
                syntax_err("unterminated multi-line comment", line);
            }
            i += 2; // skip past '*/'
            continue;
        }
        // Multi-character symbols
        if (src[i] == '+' && src[i+1] == '+') { tokens.emplace_back("++", TokenType::UnaryOP, token_line); i++; continue; }
        if (src[i] == '-' && src[i+1] == '-') { tokens.emplace_back("--", TokenType::UnaryOP, token_line); i++; continue; }
        if (src[i] == '&' && src[i+1] == '&') { tokens.emplace_back("&&", TokenType::BinaryOP, token_line); i++; continue; }
        if (src[i] == '|' && src[i+1] == '|') { tokens.emplace_back("||", TokenType::BinaryOP, token_line); i++; continue; }
		if (src[i] == '=' && src[i+1] == '=') { tokens.emplace_back("==", TokenType::BinaryOP, token_line); i++; continue; }
		if (src[i] == '!' && src[i+1] == '=') { tokens.emplace_back("!=", TokenType::BinaryOP, token_line); i++; continue; }
		if (src[i] == '>' && src[i+1] == '=') { tokens.emplace_back(">=", TokenType::BinaryOP, token_line); i++; continue; }
		if (src[i] == '<' && src[i+1] == '=') { tokens.emplace_back("<=", TokenType::BinaryOP, token_line); i++; continue; }
        if (src[i] == '-' && src[i+1] == '>') { tokens.emplace_back("->", TokenType::Arrow, token_line); i++; continue; }
        // Single-character symbols
        if (c == '(') { tokens.emplace_back("(", TokenType::LeftParen, token_line); continue; }
        if (c == ')') { tokens.emplace_back(")", TokenType::RightParen, token_line); continue; }
        if (c == '{') { tokens.emplace_back("{", TokenType::LeftBrace, token_line); continue; }
        if (c == '}') { tokens.emplace_back("}", TokenType::RightBrace, token_line); continue; }
        if (c == '[') { tokens.emplace_back("[", TokenType::LeftBracket, token_line); continue; }
        if (c == ']') { tokens.emplace_back("]", TokenType::RightBracket, token_line); continue; }
        if (c == '=') { tokens.emplace_back("=", TokenType::Equals, token_line); continue; }
        if (c == '&') { tokens.emplace_back("&", TokenType::Ampersand, token_line); continue; }
        if (c == '+') { tokens.emplace_back("+", TokenType::Plus, token_line); continue; }
        if (c == '-') { tokens.emplace_back("-", TokenType::Minus, token_line); continue; }
        if (c == '*') { tokens.emplace_back("*", TokenType::Star, token_line); continue; }
        if (c == '/') { tokens.emplace_back("/", TokenType::Slash, token_line); continue; }
        if (c == '%') { tokens.emplace_back("%", TokenType::Modulus, token_line); continue; }
        if (c == ':') { tokens.emplace_back(":", TokenType::Colon, token_line); continue; }
        if (c == ';') { tokens.emplace_back(";", TokenType::Semicolon, token_line); continue; }
        if (c == ',') { tokens.emplace_back(",", TokenType::Comma, token_line); continue; }
        if (c == '!') { tokens.emplace_back("!", TokenType::UnaryOP, token_line); continue; }
        if (c == '>') { tokens.emplace_back(">", TokenType::BinaryOP, token_line); continue; }
        if (c == '<') { tokens.emplace_back("<", TokenType::BinaryOP, token_line); continue; }

        // Identifiers / keywords
        if (std::isalpha(c))
        {
            std::string ident;
            bool has_alnum = false;

            while (i < src.length() && (std::isalnum(src[i]) || src[i] == '_'))
            {
                if (std::isalnum(src[i])) has_alnum = true;
                ident += src[i];
                i++;
            }

            if (!has_alnum)
            {
                syntax_err("invalid identifier literal", line);
            }

            TokenType type = (keywords.find(ident) == keywords.end()) ? TokenType::Identifier : keywords[ident];
            tokens.emplace_back(ident, type, token_line);
            i--;
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
            i++; // skip opening quote

            while (i < src.length() && src[i] != '"') {
                if (src[i] == '\\') {
                    i++; // move to escape char
                    if (i >= src.length()) syntax_err("unterminated escape sequence", token_line);
                    switch(src[i]) {
                        case 'n': str += '\n'; break;
                        case 't': str += '\t'; break;
                        case '"': str += '"'; break;
                        case '\\': str += '\\'; break;
                        default:
                            syntax_err(std::string("unknown escape sequence: \\") + src[i], token_line);
                    }
                    // Don't increment i again here; the end-of-loop i++ will handle moving past this char
                } else {
                    str += src[i];
                }
                i++;
            }

            if (i >= src.length() || src[i] != '"') {
                syntax_err("unterminated string literal", token_line);
            }

            tokens.emplace_back(str, TokenType::String, token_line);
            continue;
        }

        if (c == '\'') {
            std::string ch;
            i++; 

            if (i >= src.length()) {
                syntax_err("unterminated character literal", line);
            }

            char char_val;
            if (src[i] == '\\') {
                i++;
                if (i >= src.length()) syntax_err("unterminated escape sequence", line);

                switch (src[i]) {
                    case 'n': char_val = '\n'; break;
                    case 't': char_val = '\t'; break;
                    case '\'': char_val = '\''; break;
                    case '\\': char_val = '\\'; break;
                    default:
                        const std::string msg = "unknown escape sequence: \\" + line + src[i];
                        syntax_err(msg, line);
                }
            } else {
                char_val = src[i];
            }
            i++;

            if (i >= src.length() || src[i] != '\'') {
                syntax_err("unterminated character literal", line);
            }
            ch += char_val;

            tokens.emplace_back(ch, TokenType::Character, token_line);
            continue;
        }


        syntax_err(std::string("unexpected character found while lexing: ") + c, token_line);
    }

    tokens.emplace_back("EoF", TokenType::EoF, line);
    return tokens;
}

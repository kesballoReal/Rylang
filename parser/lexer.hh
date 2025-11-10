/*

lexer.hh 

*/

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

// TokenType enum. What tokens we support in our language
enum TokenType {
    Number,
    Identifier,

    NullTok,

    LeftParen,
    RightParen,

    Plus,
    Minus,
    Star,
    Slash,

    Equals,

    EoF // This is for letting us know where the file ends. EndOfFILE
};


extern std::unordered_map<std::string, TokenType> keywords;

// This will shape our tokens. Every token has a value and a type
struct Token {
    std::string value;
    TokenType type;

    // Constructor
    Token(std::string v, TokenType t) : value(v), type(t) {};
};

// This function takes in input a string (our file given in main.cc) and it outputs a vector of tokens
std::vector<Token> tokenize(const std::string &src);
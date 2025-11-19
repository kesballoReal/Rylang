/*

lexer.hh 

*/

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

// TokenType enum. What tokens we support in our language
enum TokenType {
    Number,
    Identifier,
    String,

    DataType,

    NullTok,
    BoolTok,

    TrueTok,
    FalseTok,

    VarTok,
    ConstTok,

    LeftParen,
    RightParen,

    LeftBrace,
    RightBrace,

    Plus,
    Minus,
    Star,
    Slash,
  
    BinaryOP,
    UnaryOP,

    IfTok,
    ElseTok,
    WhileTok,

    ContinueTok,
    BreakTok,

    Equals,
    Colon,
    Semicolon,

    EoF // This is for letting us know where the file ends. EndOfFILE
};


extern std::unordered_map<std::string, TokenType> keywords;

// This will shape our tokens. Every token has a value and a type
struct Token {
    std::string value;
    TokenType type;
    std::size_t line;

    // Constructor
    Token(std::string v, TokenType t, std::size_t l) : value(std::move(v)), type(t), line(l) {};
};

// This function takes in input a string (our file given in main.cc) and it outputs a vector of tokens
std::vector<Token> tokenize(const std::string &src);

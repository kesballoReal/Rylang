#include "parser.hh"

#include <iostream>

Parser::Parser(std::vector<Token>& tokens) {
    this->tokens = tokens;
}

bool Parser::not_eof()
{
    return this->tokens[this->current].type != TokenType::EoF;
}

Token Parser::at()
{
    return this->tokens[this->current];
}

Token Parser::eat()
{
    return this->tokens[this->current++];
}

Token Parser::expect(TokenType type, const std::string& msg)
{
    auto tok = this->eat();

    if (tok.type != type)
    {
        std::cerr << "ryc: " << msg << std::endl;
        std::exit(1);
    }

    return tok;
}

std::shared_ptr<Expr> Parser::parse_expr()
{
    switch (this->tokens[current].type)
    {
        case TokenType::Number:
        {
            return std::make_shared<ASTNumericLiteral>(std::stof(this->eat().value));
        }
        case TokenType::LeftParen:
        {
            this->eat();
            auto expr = this->parse_expr();
            this->expect(TokenType::RightParen, "expected ')' after expression.");
            return expr;
        }
    }
}

std::shared_ptr<Stmt> Parser::parse_stmt()
{
    switch (this->tokens[current].type)
    {
        default:
        {
            return this->parse_expr();
        }
    }
}

std::shared_ptr<ASTProgram> Parser::produceAST()
{
    auto program = std::make_shared<ASTProgram>();

    while (not_eof()) // While we are not at the end of the file
    {
        program->body.push_back(parse_stmt());
    }

    return program;
}


#include "parser.hh"

#include "../utils/error.hh"

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

Token Parser::expect(TokenType type, const std::string& msg, std::size_t l)
{
    auto tok = this->eat();

    if (tok.type != type)
    {
        syntax_err(msg, l);
    }

    return tok;
}

std::shared_ptr<Expr> Parser::parse_primary_expr()
{
    switch (this->tokens[current].type)
    {
        case TokenType::Number:
        {
            return std::make_shared<ASTNumericLiteral>(std::stof(this->eat().value), this->at().line);
        }
        case TokenType::Identifier:
        {
            return std::make_shared<ASTIdentifierLiteral>(this->eat().value, this->at().line);
        }
        case TokenType::LeftParen:
        {
            this->eat();
            auto expr = this->parse_expr();
            this->expect(TokenType::RightParen, "expected ')' after expression.", this->at().line);
            return expr;
        }
        case TokenType::NullTok:
        {
            this->eat();
            return std::make_shared<ASTNullLiteral>(this->at().line);
        }
        default:
        {   
            std::string err = "unexpected token found while parsing: " + this->tokens[current].value;
            syntax_err(err, this->at().line);
            break;
        }
    }
}

std::shared_ptr<Expr> Parser::parse_multiplicative_expr()
{
    auto left = this->parse_primary_expr();

    while (this->at().type == TokenType::Star || this->at().type == TokenType::Slash)
    {
        std::string op = this->eat().value;
        auto right = this->parse_primary_expr();
        left = std::make_shared<ASTBinaryExpr>(left, right, op, this->at().line);
    }

    return left;
}

std::shared_ptr<Expr> Parser::parse_additive_expr()
{
   auto left = this->parse_multiplicative_expr(); 

    while (this->at().type == TokenType::Plus || this->at().type == TokenType::Minus)
    {
        std::string op = this->eat().value;
        auto right = this->parse_multiplicative_expr();
        left = std::make_shared<ASTBinaryExpr>(left, right, op, this->at().line); 
    }
   return left;
}

std::shared_ptr<Expr> Parser::parse_expr()
{
    return this->parse_additive_expr();
}

std::shared_ptr<Stmt> Parser::parse_var_declaration()
{
    bool is_const = this->at().type == TokenType::ConstTok;
    if (is_const) this->eat();

    this->eat();
    std::string name = this->expect(TokenType::Identifier, "expected variable name after 'var'", this->at().line).value;

    this->expect(TokenType::Colon, "expected ':' after variable name", this->at().line);
    std::string type = this->expect(TokenType::DataType, "expected variable type following ':'", this->at().line).value;

    std::shared_ptr<Expr> value = nullptr;
    if (this->at().type == TokenType::Equals)
    {
        this->eat();
        value = this->parse_expr();
    }
    else
    {
        if (type == "auto") {
            syntax_err("cannot declare variable of type 'auto' without an initializer.", this->at().line);
        }
        if (is_const)
        {
            syntax_err("cannot declare const variable without an initializer.", this->at().line);
        }
    }
    this->expect(TokenType::Semicolon, "expected ';' after variable declaration", this->at().line);
    return std::make_shared<ASTVarDecl>(name, type, value, is_const, this->at().line);
}

std::shared_ptr<Stmt> Parser::parse_stmt()
{
    switch (this->tokens[current].type)
    {
        case TokenType::VarTok:
        case TokenType::ConstTok:
            return this->parse_var_declaration();

        default:
        {
            auto expr = parse_expr();
            this->expect(TokenType::Semicolon, "expected ';' after expression statement", this->at().line);
            return expr;
        }
    }
}


std::shared_ptr<ASTProgram> Parser::produceAST()
{
    auto program = std::make_shared<ASTProgram>(std::vector<std::shared_ptr<Stmt>>{}, this->at().line);

    while (not_eof()) // While we are not at the end of the file
    {
        program->body.push_back(parse_stmt());
    }

    return program;
}


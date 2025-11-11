#pragma once

#include "lexer.hh"
#include "ast.hh"

#include <memory>
#include <vector>

class Parser {
public:
    Parser(std::vector<Token>& tokens);
    std::shared_ptr<ASTProgram> produceAST();

private:
    bool not_eof();
    Token at();
    Token eat();
    Token expect(TokenType type, const std::string& msg, std::size_t l);

    std::shared_ptr<Stmt> parse_stmt();
    std::shared_ptr<Stmt> parse_var_declaration();

    std::shared_ptr<Expr> parse_expr();
    std::shared_ptr<Expr> parse_primary_expr();
    std::shared_ptr<Expr> parse_additive_expr();
    std::shared_ptr<Expr> parse_multiplicative_expr();

    std::vector<Token> tokens;
    size_t current = 0;
};

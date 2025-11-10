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
    Token expect(TokenType type, const std::string& msg);

    std::shared_ptr<Stmt> parse_stmt();
    std::shared_ptr<Expr> parse_expr();

    std::vector<Token> tokens;
    size_t current = 0;
};

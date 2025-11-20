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
    Token expect_str(std::string v, const std::string& msg, std::size_t l);

    std::shared_ptr<Stmt> parse_stmt();
    std::shared_ptr<Stmt> parse_block(std::size_t line);
    std::shared_ptr<Stmt> parse_if_stmt();
    std::shared_ptr<Stmt> parse_while_stmt();
    std::shared_ptr<Stmt> parse_for_stmt();
    std::shared_ptr<Stmt> parse_var_declaration();
    std::shared_ptr<Stmt> parse_func_stmt();

    std::vector<std::shared_ptr<ASTParam>> parse_func_params(std::size_t line);

    std::shared_ptr<Expr> parse_expr();
    std::shared_ptr<Expr> parse_primary_expr();
    std::shared_ptr<Expr> parse_additive_expr();
    std::shared_ptr<Expr> parse_multiplicative_expr();
    std::shared_ptr<Expr> parse_call_expr(std::shared_ptr<Expr> expr);
    std::shared_ptr<Expr> parse_unary_expr();
    std::shared_ptr<Expr> parse_logical_or_expr();
    std::shared_ptr<Expr> parse_logical_and_expr();
    std::shared_ptr<Expr> parse_equality_expr();
    std::shared_ptr<Expr> parse_comparison_expr();
    std::shared_ptr<Expr> parse_assignment_expr();
    std::shared_ptr<Expr> parse_member_expr(std::shared_ptr<Expr> expr);
    std::shared_ptr<Expr> parse_array_literal();
    std::shared_ptr<Expr> parse_array_element();

    std::vector<Token> tokens;
    size_t current = 0;
};

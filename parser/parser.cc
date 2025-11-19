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
    if (this->at().type != type)
        syntax_err(msg, l);

    return this->eat();
}

/* Fold constants */

std::shared_ptr<Expr> fold_constants(
    const std::shared_ptr<Expr>& left,
    const std::shared_ptr<Expr>& right,
    const std::string& op,
    std::size_t line
) {
    // Numbers
    auto lnum = std::dynamic_pointer_cast<ASTNumericLiteral>(left);
    auto rnum = std::dynamic_pointer_cast<ASTNumericLiteral>(right);
    if (lnum && rnum) {
        float result = 0.0f;
        if (op == "+") result = lnum->value + rnum->value;
        else if (op == "-") result = lnum->value - rnum->value;
        else if (op == "*") result = lnum->value * rnum->value;
        else if (op == "/") result = rnum->value != 0.0f ? lnum->value / rnum->value
                                                        : throw std::runtime_error("division by zero");
        return std::make_shared<ASTNumericLiteral>(result, line);
    }

    // Strings
    auto lstr = std::dynamic_pointer_cast<ASTStringLiteral>(left);
    auto rstr = std::dynamic_pointer_cast<ASTStringLiteral>(right);
    if (lstr && rstr && op == "+") {
        return std::make_shared<ASTStringLiteral>(lstr->value + rstr->value, line);
    }

    // Booleans
    auto lbool = std::dynamic_pointer_cast<ASTBoolLiteral>(left);
    auto rbool = std::dynamic_pointer_cast<ASTBoolLiteral>(right);
    if (lbool && rbool) {
        bool result = false;
        if (op == "&&") result = lbool->value && rbool->value;
        else if (op == "||") result = lbool->value || rbool->value;
        return std::make_shared<ASTBoolLiteral>(result, line);
    }

    // Return a normal binary expression
    return std::make_shared<ASTBinaryExpr>(left, right, op, line);
}

/* ------------------------------------------------------------------- */

std::shared_ptr<Expr> Parser::parse_primary_expr()
{
    switch (this->tokens[current].type) {
        case TokenType::Number:
        {
            Token tok = this->eat();
            return std::make_shared<ASTNumericLiteral>(std::stof(tok.value), tok.line);
        }
        case TokenType::Identifier:
        {
            Token tok = this->eat();
            return std::make_shared<ASTIdentifierLiteral>(tok.value, tok.line);
        }
        case TokenType::String:
        {
            Token tok = this->eat();
            return std::make_shared<ASTStringLiteral>(tok.value, tok.line);
        }
        case TokenType::LeftParen:
        {
            Token tok = this->eat();
            auto expr = this->parse_expr();
            this->expect(TokenType::RightParen, "expected ')' after expression.", tok.line);
            return expr;
        }
        case TokenType::NullTok:
        {
            Token tok = this->eat();
            return std::make_shared<ASTNullLiteral>(tok.line);
        }
        case TokenType::TrueTok:
        case TokenType::FalseTok: {
            Token tok = this->eat();
            return std::make_shared<ASTBoolLiteral>((tok.value == "true"), tok.line);
        }
        default:
        {   
            const std::string err = "unexpected token found while parsing: " + this->tokens[current].value;
            syntax_err(err, this->at().line);
        }
    }

    return nullptr;
}

std::shared_ptr<Expr> Parser::parse_unary_expr()
{
    if (this->at().type == TokenType::UnaryOP || this->at().type == TokenType::Plus || this->at().type == TokenType::Minus) {
        Token tok = this->eat();
        std::string op = tok.value;
        auto operand = this->parse_primary_expr();
        return std::make_shared<ASTUnaryExpr>(operand, op, true, tok.line);
    }
    auto left = this->parse_primary_expr();
    if (this->at().type == TokenType::UnaryOP) {
        Token tok = this->eat();
        std::string op = tok.value;
        return std::make_shared<ASTUnaryExpr>(left, op, false, tok.line);
    }
    return left;
}

std::shared_ptr<Expr> Parser::parse_multiplicative_expr()
{
    auto left = this->parse_unary_expr();

    while (this->at().type == TokenType::Star || this->at().type == TokenType::Slash)
    {
        Token tok = this->eat();
        std::string op = tok.value;
        auto right = this->parse_unary_expr();
        left = fold_constants(left, right, op, tok.line);
    }

    return left;
}

std::shared_ptr<Expr> Parser::parse_additive_expr()
{
   auto left = this->parse_multiplicative_expr(); 

    while (this->at().type == TokenType::Plus || this->at().type == TokenType::Minus)
    {
        Token tok = this->eat();
        std::string op = tok.value;
        auto right = this->parse_multiplicative_expr();
        left = fold_constants(left, right, op, tok.line);
    }
   return left;
}

std::shared_ptr<Expr> Parser::parse_assignment_expr()
{
    auto left = this->parse_additive_expr();

    while (this->at().type == TokenType::Equals)
    {
        Token tok = this->eat();
        auto right = this->parse_assignment_expr();
        left = std::make_shared<ASTAssignExpr>(left, right, tok.line);
    }

    return left;
}

std::shared_ptr<Expr> Parser::parse_comparison_expr()
{
    auto left = this->parse_assignment_expr();

    while (this->at().type == TokenType::BinaryOP &&
           (this->at().value == ">" || this->at().value == "<" ||
            this->at().value == ">=" || this->at().value == "<="))
    {
        Token tok = this->eat();
        std::string op = tok.value;
        auto right = this->parse_assignment_expr();
        left = fold_constants(left, right, op, tok.line);
    }

    return left;
}

std::shared_ptr<Expr> Parser::parse_equality_expr()
{
	auto left = this->parse_comparison_expr();
	while (this->at().type == TokenType::BinaryOP && (this->at().value == "==" || this->at().value == "!="))
	{
		Token tok = this->eat();
		std::string op = tok.value;
		auto right = this->parse_comparison_expr();
		left = fold_constants(left, right, op, tok.line);
	}
	return left;
}

std::shared_ptr<Expr> Parser::parse_logical_and_expr()
{
    auto left = this->parse_equality_expr();
    while (this->at().type == TokenType::BinaryOP && this->at().value == "&&")
    {
      Token tok = this->eat();
      std::string op = tok.value;
      auto right = this->parse_equality_expr();
      left = fold_constants(left, right, op, tok.line);
    }
  return left;
}

std::shared_ptr<Expr> Parser::parse_logical_or_expr()
{
    auto left = this->parse_logical_and_expr();
    while (this->at().type == TokenType::BinaryOP && this->at().value == "||")
    {
      Token tok = this->eat();
      std::string op = tok.value;
      auto right = this->parse_logical_and_expr();
      left = fold_constants(left, right, op, tok.line);
    }

    return left;
}

std::shared_ptr<Expr> Parser::parse_expr()
{
    return this->parse_logical_or_expr();
}

std::shared_ptr<Stmt> Parser::parse_var_declaration()
{
    std::size_t decl_line = this->at().line;
    bool is_const = this->at().type == TokenType::ConstTok;
    if (is_const) this->eat();

    this->eat();
    std::string name = this->expect(TokenType::Identifier, "expected variable name after 'var'", decl_line).value;

    this->expect(TokenType::Colon, "expected ':' after variable name", decl_line);
    std::string type = this->expect(TokenType::DataType, "expected variable type following ':'", decl_line).value;

    std::shared_ptr<Expr> value = nullptr;
    if (this->at().type == TokenType::Equals)
    {
        this->eat();
        value = this->parse_expr();
    }
    else
    {
        if (type == "auto") {
            syntax_err("cannot declare variable of type 'auto' without an initializer.", decl_line);
        }
        if (is_const)
        {
            syntax_err("cannot declare const variable without an initializer.", decl_line);
        }
    }
    this->expect(TokenType::Semicolon, "expected ';' after variable declaration", decl_line);
    return std::make_shared<ASTVarDecl>(name, type, value, is_const, decl_line);
}

std::shared_ptr<Stmt> Parser::parse_while_stmt()
{
    Token tok = this->eat();

    this->expect(TokenType::LeftParen, "expected '(' after 'while'", tok.line);
    auto condition = this->parse_expr();
    this->expect(TokenType::RightParen, "expected ')' after while condition", tok.line);

    this->expect(TokenType::LeftBrace, "expected '{' to start while body", tok.line);
    auto doBranch = this->parse_block(tok.line);
    this->expect(TokenType::RightBrace, "expected '}' to close while body", tok.line);

    return std::make_shared<ASTWhileStmt>(condition, doBranch, tok.line);
}

std::shared_ptr<Stmt> Parser::parse_if_stmt()
{
    Token tok = this->eat();

    this->expect(TokenType::LeftParen, "expected '(' after 'if'", tok.line);
    auto condition = this->parse_expr();
    this->expect(TokenType::RightParen, "expected ')' after if condition", tok.line);

    this->expect(TokenType::LeftBrace, "expected '{' to start if body", tok.line);
    auto thenBranch = this->parse_block(tok.line);
    this->expect(TokenType::RightBrace, "expected '}' to close if body", tok.line);

    std::optional<std::shared_ptr<ASTBlockStmt>> elseBranch = std::nullopt;

    if (this->at().type == TokenType::ElseTok)
    {
        this->eat(); // consume 'else'

        if (this->at().type == TokenType::IfTok)
        {
            elseBranch = std::static_pointer_cast<ASTBlockStmt>(
                this->parse_if_stmt()
            );
        }
        else
        {
            this->expect(TokenType::LeftBrace, "expected '{' to start else body", tok.line);
            elseBranch = std::static_pointer_cast<ASTBlockStmt>(this->parse_block(tok.line));
            this->expect(TokenType::RightBrace, "expected '}' to close else body", tok.line);
        }
    }

    return std::make_shared<ASTIfStmt>(condition, thenBranch, elseBranch, tok.line);
}


std::shared_ptr<Stmt> Parser::parse_block(std::size_t line)
{   
    std::vector<std::shared_ptr<Stmt>> block;
    while (this->not_eof() && this->at().type != TokenType::RightBrace)
    {
        block.push_back(this->parse_stmt());
    }

    return std::make_shared<ASTBlockStmt>(block, line);
}

std::shared_ptr<Stmt> Parser::parse_stmt()
{
    switch (this->tokens[current].type)
    {
        case TokenType::VarTok:
        case TokenType::ConstTok:
            return this->parse_var_declaration();
        case TokenType::IfTok:
            return this->parse_if_stmt();
        case TokenType::WhileTok:
            return this->parse_while_stmt();
        case TokenType::ContinueTok:
        {
            Token tok = this->eat();
            this->expect(TokenType::Semicolon, "expected ';' after 'continue'", tok.line);
            return std::make_shared<ASTContinueStmt>(tok.line);
        }
        case TokenType::BreakTok:
        {
            Token tok = this->eat();
            this->expect(TokenType::Semicolon, "expected ';' after 'break'", tok.line);
            return std::make_shared<ASTBreakStmt>(tok.line);
        }
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


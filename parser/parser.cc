#include "parser.hh"

#include "../utils/error.hh"

#include <iostream>
#include <cmath>

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

Token Parser::expect_str(std::string v, const std::string& msg, std::size_t l)
{
    if (this->at().value != v)
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
        else if (op == "%") result = std::fmod(lnum->value, rnum->value);
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
        case TokenType::Character:
        {
            Token tok = this->eat();
            return std::make_shared<ASTCharLiteral>(tok.value[0], tok.line);
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
        case TokenType::CastTok:
        {
            // var x: int = static_cast<string>();
            Token tok = this->eat();

            this->expect_str("<", "expected '<' to specify cast target type", tok.line);
            std::string target = this->expect(TokenType::DataType, "expected type after cast", tok.line).value;
            this->expect_str(">", "expected '>' after cast target type", tok.line);
            
            this->expect(TokenType::LeftParen, "expected '(' to specify cast operand", tok.line);
            auto operand = this->parse_expr();
            this->expect(TokenType::RightParen, "expected ')' to close cast method", tok.line);

            return std::make_shared<ASTCastExpr>(target, operand, tok.line);
        }
        default:
        {   
            const std::string err = "unexpected token found while parsing: " + this->tokens[current].value;
            syntax_err(err, this->at().line);
        }
    }

    return nullptr;
}

std::shared_ptr<Expr> Parser::parse_member_expr(std::shared_ptr<Expr> expr) {
    while (true) {
        if (this->at().type == TokenType::LeftBracket) {
            // Array indexing
            this->eat();
            auto index = parse_expr();
            this->expect(TokenType::RightBracket, "expected ']' after array index", index->line);
            expr = std::make_shared<ASTMemberExpr>(expr, index, true, index->line);
        } else break;
    }

    return expr;
}

std::shared_ptr<Expr> Parser::parse_call_expr(std::shared_ptr<Expr> expr) {
    while (true) {
        if (this->at().type == TokenType::LeftParen) {

            Token start = this->eat();
            std::vector<std::shared_ptr<Expr>> args;

            if (this->at().type != TokenType::RightParen) {
                if (this->at().type == TokenType::LeftBrace) {
                    args.push_back(parse_array_literal());
                } else {
                    args.push_back(parse_expr());
                }
                while (this->at().type == TokenType::Comma) {
                    this->eat();
                    if (this->at().type == TokenType::LeftBrace) {
                        args.push_back(parse_array_literal());
                    } else {
                        args.push_back(parse_expr());
                    }
                }
            }

            this->expect(TokenType::RightParen, "expected ')' after call arguments", start.line);

            expr = std::make_shared<ASTCallExpr>(expr, args, start.line);
        }
        else break;
    }

    return expr;
}


std::shared_ptr<Expr> Parser::parse_unary_expr() {
    // Prefix unary operators
    if (this->at().type == TokenType::UnaryOP || this->at().type == TokenType::Plus || this->at().type == TokenType::Minus ||
        this->at().type == TokenType::Star || this->at().type == TokenType::Ampersand) {
        Token tok = this->eat();
        std::string op = tok.value;
        auto operand = parse_unary_expr();
        return std::make_shared<ASTUnaryExpr>(operand, op, true, tok.line);
    }

    auto left = parse_primary_expr();
    left = parse_member_expr(left);
    left = parse_call_expr(left);

    // Postfix unary operators
    if (this->at().type == TokenType::UnaryOP && (this->at().value == "++" || this->at().value == "--")) {
        Token tok = this->eat();
        std::string op = tok.value;
        return std::make_shared<ASTUnaryExpr>(left, op, false, tok.line);
    }

    return left;
}


std::shared_ptr<Expr> Parser::parse_multiplicative_expr()
{
    auto left = this->parse_unary_expr();

    while (this->at().type == TokenType::Star || this->at().type == TokenType::Slash || this->at().type == TokenType::Modulus)
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

std::shared_ptr<Expr> Parser::parse_array_element()
{
    if (this->at().type == TokenType::LeftBrace) {
        return this->parse_array_literal();
    }

    return this->parse_expr();
}


std::shared_ptr<Expr> Parser::parse_array_literal()
{
    if (this->at().type == TokenType::NullTok) {
        Token tok = this->eat();
        return std::make_shared<ASTNullLiteral>(tok.line);
    }

    Token start = this->expect(TokenType::LeftBrace,
        "expected '{' to start array literal.",
        this->at().line);

    std::vector<std::shared_ptr<Expr>> elements;

    // Empty array "{}"
    if (this->at().type == TokenType::RightBrace) {
        this->eat();
        return std::make_shared<ASTArrayLiteral>(elements, start.line);
    }

    elements.push_back(this->parse_array_element());

    while (this->at().type == TokenType::Comma)
    {
        this->eat();

        if (this->at().type == TokenType::RightBrace)
            break;

        elements.push_back(this->parse_array_element());
    }

    // Expect final '}'
    this->expect(TokenType::RightBrace,
                 "expected '}' to close array literal.",
                 start.line);

    return std::make_shared<ASTArrayLiteral>(elements, start.line);
}


std::shared_ptr<Stmt> Parser::parse_var_declaration()
{
    std::size_t decl_line = this->at().line;
    bool is_const = this->at().type == TokenType::ConstTok;
    bool is_array = false;
    if (is_const) this->eat();

    this->eat();
    std::string name = this->expect(TokenType::Identifier, "expected variable name after 'var'", decl_line).value;

    this->expect(TokenType::Colon, "expected ':' after variable name", decl_line);
    std::string type = this->expect(TokenType::DataType, "expected variable type following ':'", decl_line).value;

    std::optional<std::shared_ptr<Expr>> size_array = std::nullopt;
    if (this->at().type == TokenType::LeftBracket)
    {   
        this->eat();
        is_array = true;

        if (this->at().type != TokenType::RightBracket)
        {   
            size_array = this->parse_expr();
        }
        this->expect(TokenType::RightBracket, "expected ']' to close array size specifier.", decl_line);
    }

    std::shared_ptr<Expr> value = nullptr;
    if (this->at().type == TokenType::Equals)
    {
        this->eat();

        if (this->at().type == TokenType::LeftBrace)
            value = this->parse_array_literal();
        else
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
    return std::make_shared<ASTVarDecl>(name, type, value, is_const, is_array, size_array, decl_line);
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

std::shared_ptr<Stmt> Parser::parse_for_stmt() {
    Token tok = this->eat(); // consume 'for'
    this->expect(TokenType::LeftParen, "expected '(' after 'for'", tok.line);

    // Init: variable declaration or expression statement
    std::shared_ptr<Stmt> init = nullptr;
    if (this->at().type != TokenType::Semicolon) {
        if (this->at().type == TokenType::VarTok || this->at().type == TokenType::ConstTok) {
            init = this->parse_var_declaration();
        } else {
            auto expr = this->parse_expr();
            this->expect(TokenType::Semicolon, "expected ';' after for loop initializer", tok.line);
            init = expr;
        }
    } else {
        this->eat(); // skip semicolon
    }

    // Condition
    std::shared_ptr<Expr> condition = nullptr;
    if (this->at().type != TokenType::Semicolon) {
        condition = this->parse_expr();
    }
    this->expect(TokenType::Semicolon, "expected ';' after for loop condition", tok.line);

    // Update
    std::shared_ptr<Expr> update = nullptr;
    if (this->at().type != TokenType::RightParen) {
        update = this->parse_expr();
    }
    this->expect(TokenType::RightParen, "expected ')' after for loop update", tok.line);

    // Body
    std::shared_ptr<Stmt> body;
    this->expect(TokenType::LeftBrace, "expected '{' to start for loop body", tok.line);
    body = this->parse_block(tok.line);
    this->expect(TokenType::RightBrace, "expected '}' after for loop body", tok.line);

    return std::make_shared<ASTForStmt>(init, condition, update, body, tok.line);
}

std::shared_ptr<Stmt> Parser::parse_func_stmt()
{
    Token tok = this->eat();

    std::string name = this->expect(TokenType::Identifier, "expected function name after 'func'", tok.line).value;

    this->expect(TokenType::LeftParen, "expected '(' to open function parameters", tok.line);
    auto params = this->parse_func_params(tok.line);
    this->expect(TokenType::RightParen, "expected ')' to close function parameters", tok.line);

    this->expect(TokenType::Arrow, "expected '->' to declare function return type", tok.line);
    Token ret_tok = this->eat();

    if (ret_tok.type != TokenType::DataType && ret_tok.type != TokenType::VoidTok) {
        syntax_err("expected function return type after '->'", ret_tok.line);
    }

    std::string ret_type = ret_tok.value;

    if (ret_type == "auto") {
        syntax_err("function return type cannot be 'auto'", tok.line);
     }

    this->expect(TokenType::LeftBrace, "expected '{' to start function body", tok.line);
    auto body = this->parse_block(tok.line);
    this->expect(TokenType::RightBrace, "expected '}' to close function body", tok.line);

    return std::make_shared<ASTFunctionStmt>(name, ret_type, params, body, tok.line);
}


std::vector<std::shared_ptr<ASTParam>> Parser::parse_func_params(std::size_t line)
{
    std::vector<std::shared_ptr<ASTParam>> params;

    while (this->not_eof() && this->at().type != TokenType::RightParen)
    {
        this->expect(TokenType::VarTok, "expected 'var' for function parameter", line);
        std::string varname = this->expect(TokenType::Identifier, "expected variable name in function parameters", line).value;
        this->expect(TokenType::Colon, "expected ':' with variable type in function parameters", line);
        std::string type = this->expect(TokenType::DataType, "expected variable type in function parameters", line).value;
        
        bool is_array = false;
        if (this->at().type == TokenType::LeftBracket) {
            this->eat();
            this->expect(TokenType::RightBracket, "expected ']' after array type", line);
            is_array = true;
        }

        params.push_back(std::make_shared<ASTParam>(varname, type, is_array, line));

        if (this->at().type != TokenType::Comma)
        {
            break;
        }
        this->eat();
        continue;
    }

    return params;
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
        case TokenType::ForTok:
            return this->parse_for_stmt();
        case TokenType::FuncTok:
            return this->parse_func_stmt();
        case TokenType::ContinueTok:
        {
            Token tok = this->eat();
            this->expect(TokenType::Semicolon, "expected ';' after 'continue'", tok.line);
            return std::make_shared<ASTContinueStmt>(tok.line);
        }
        case TokenType::ReturnTok:
        {
            Token tok = this->eat();
            std::shared_ptr<Expr> value = nullptr;
            if (this->at().type != TokenType::Semicolon) {
                value = parse_expr();
            }
            this->expect(TokenType::Semicolon, "expected ';' after return statement", tok.line);
            return std::make_shared<ASTReturnStmt>(value, tok.line);
        }
        case TokenType::BreakTok:
        {
            Token tok = this->eat();
            this->expect(TokenType::Semicolon, "expected ';' after 'break'", tok.line);
            return std::make_shared<ASTBreakStmt>(tok.line);
        }
        case TokenType::LeftBrace:
        {
            Token tok = this->eat();
            auto block = this->parse_block(tok.line);
            this->expect(TokenType::RightBrace, "expected '}' to close block statement", tok.line);
            return block;
        }
        default:
        {
            auto expr = parse_expr();
            this->expect(TokenType::Semicolon, "expected ';' after expression statement", this->at().line);
            return std::make_shared<ASTExprStmt>(expr, expr->line);
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


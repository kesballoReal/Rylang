/*

ast.hh

*/

#pragma once

#include <vector>
#include <memory>
#include <string>

enum NodeType { // Nodes our language supports
    Program, // This contains all of our statements, basically all of our program
    // Statements

    // Expressions
    BinaryExpr,
    // Literals
    NumericLiteral,
    IdentifierLiteral,
    NullLiteral,
};

struct Stmt {
    virtual ~Stmt() = default;

    NodeType kind;
};

struct ASTProgram : Stmt {
    std::vector<std::shared_ptr<Stmt>> body;

    explicit ASTProgram(std::vector<std::shared_ptr<Stmt>> b)
        : body(std::move(b)) {
            kind = NodeType::Program;
        }
};


struct Expr : Stmt {};

struct ASTBinaryExpr : Expr {
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
    std::string op;

    ASTBinaryExpr(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r, std::string o)
    : left(std::move(l)), right(std::move(r)), op(std::move(o)) {
        kind = NodeType::BinaryExpr;
    }

};

struct ASTNumericLiteral : Expr {
    double value;

    ASTNumericLiteral(double val) : value(val) {
        kind = NodeType::NumericLiteral;
    }
};

struct ASTIdentifierLiteral : Expr {
    std::string name;

    ASTIdentifierLiteral(std::string n) : name(n) {
        kind = NodeType::IdentifierLiteral;
    }
};

struct ASTNullLiteral : Expr {
    ASTNullLiteral() {
        kind = NodeType::NullLiteral;
    }
};
#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>

enum NodeType { // Nodes our language supports
    Program,         // This contains all statements
    // Statements
    VarDeclaration,
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
    std::size_t line; // Line number for error reporting
};

struct Expr : Stmt {};

struct ASTProgram : Stmt {
    std::vector<std::shared_ptr<Stmt>> body;

    explicit ASTProgram(std::vector<std::shared_ptr<Stmt>> b, std::size_t l = 0)
        : body(std::move(b)) {
        kind = NodeType::Program;
        line = l;
    }
};

struct ASTVarDecl : Stmt {
    std::string name;
    std::string type;
    std::optional<std::shared_ptr<Expr>> value;
    bool is_const;

    ASTVarDecl(std::string n, std::string t, std::shared_ptr<Expr> v, bool c, std::size_t l)
        : name(std::move(n)), type(std::move(t)), value(std::move(v)), is_const(c) {
        kind = NodeType::VarDeclaration;
        line = l;
    }
};

struct ASTBinaryExpr : Expr {
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
    std::string op;

    ASTBinaryExpr(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r, std::string o, std::size_t ln)
        : left(std::move(l)), right(std::move(r)), op(std::move(o)) {
        kind = NodeType::BinaryExpr;
        line = ln;
    }
};

struct ASTNumericLiteral : Expr {
    double value;

    ASTNumericLiteral(double val, std::size_t ln) : value(val) {
        kind = NodeType::NumericLiteral;
        line = ln;
    }
};

struct ASTIdentifierLiteral : Expr {
    std::string name;

    ASTIdentifierLiteral(std::string n, std::size_t ln) : name(std::move(n)) {
        kind = NodeType::IdentifierLiteral;
        line = ln;
    }
};

struct ASTNullLiteral : Expr {
    ASTNullLiteral(std::size_t ln) {
        kind = NodeType::NullLiteral;
        line = ln;
    }
};

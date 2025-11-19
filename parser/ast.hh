#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>

enum NodeType { // Nodes our language supports
    Program,         // This contains all statements
    // Statements
    BlockStmt,
    VarDeclaration,
    IfStmt,
    WhileStmt,
    ContinueStmt,
    BreakStmt,
    // Expressions
    BinaryExpr,
    UnaryExpr,
    AssignmentExpr,
    // Literals
    NumericLiteral,
    IdentifierLiteral,
    StringLiteral,
    BoolLiteral,
    NullLiteral,
};

struct Stmt {
    virtual ~Stmt() = default;

    NodeType kind;
    std::size_t line{}; // Line number for error reporting
};

struct Expr : Stmt {};

struct ASTProgram final : Stmt {
    std::vector<std::shared_ptr<Stmt>> body;

    explicit ASTProgram(std::vector<std::shared_ptr<Stmt>> b, std::size_t l = 0)
        : body(std::move(b)) {
        kind = NodeType::Program;
        line = l;
    }
};

struct ASTBlockStmt final : Stmt {
    std::vector<std::shared_ptr<Stmt>> block;

    explicit ASTBlockStmt(std::vector<std::shared_ptr<Stmt>> b, std::size_t l)
        : block(std::move(b)) {
            kind = NodeType::BlockStmt;
            line = l;
        }
};

struct ASTContinueStmt final : Stmt {
    ASTContinueStmt(std::size_t l) {
        kind = NodeType::ContinueStmt;
        line = l;
    }
};

struct ASTBreakStmt final : Stmt {
    ASTBreakStmt(std::size_t l) {
        kind = NodeType::BreakStmt;
        line = l;
    }
};

struct ASTVarDecl final : Stmt {
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

struct ASTIfStmt final : Stmt {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> thenBranch;
    std::optional<std::shared_ptr<Stmt>> elseBranch;


    ASTIfStmt(std::shared_ptr<Expr> c, std::shared_ptr<Stmt> t, std::optional<std::shared_ptr<Stmt>> e, std::size_t l) :
        condition(std::move(c)), thenBranch(std::move(t)), elseBranch(std::move(e)) {
            kind = NodeType::IfStmt;
            line = l;
        }
};

struct ASTWhileStmt final : Stmt {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Stmt> doBranch;

    ASTWhileStmt(std::shared_ptr<Expr> c, std::shared_ptr<Stmt> b, std::size_t l) : condition(std::move(c)), doBranch(std::move(b)) {
        kind = NodeType::WhileStmt;
        line = l;
    }
};

struct ASTBinaryExpr final : Expr {
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
    std::string op;

    ASTBinaryExpr(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r, std::string o, std::size_t ln)
        : left(std::move(l)), right(std::move(r)), op(std::move(o)) {
        kind = NodeType::BinaryExpr;
        line = ln;
    }
};

struct ASTUnaryExpr final : Expr {
    std::shared_ptr<Expr> operand;
    std::string op;
    bool prefix;

    ASTUnaryExpr(std::shared_ptr<Expr> op, std::string(o), bool p, std::size_t ln) : operand(std::move(op)), op(std::move(o)), prefix(p) {
        kind = NodeType::UnaryExpr;
        line = ln;
    }
};

struct ASTAssignExpr final : Expr {
    std::shared_ptr<Expr> assignee;
    std::shared_ptr<Expr> value;

    ASTAssignExpr(std::shared_ptr<Expr> a, std::shared_ptr<Expr> v, std::size_t ln) : assignee(std::move(a)), value(std::move(v)) {
        kind = NodeType::AssignmentExpr;
        line = ln;
    }
};

struct ASTNumericLiteral final : Expr {
    double value;

    ASTNumericLiteral(double val, std::size_t ln) : value(val) {
        kind = NodeType::NumericLiteral;
        line = ln;
    }
};

struct ASTIdentifierLiteral final : Expr {
    std::string name;

    ASTIdentifierLiteral(std::string n, std::size_t ln) : name(std::move(n)) {
        kind = NodeType::IdentifierLiteral;
        line = ln;
    }
};

struct ASTStringLiteral final : Expr {
    std::string value;

    ASTStringLiteral(std::string v, std::size_t ln) : value(std::move(v)) {
        kind = NodeType::StringLiteral;
        line = ln;
    } 
};

struct ASTBoolLiteral final : Expr {
    bool value;

    ASTBoolLiteral(bool v, std::size_t ln) : value(v) {
        kind = NodeType::BoolLiteral;
        line = ln;
    }
};

struct ASTNullLiteral : Expr {
    ASTNullLiteral(std::size_t ln) {
        kind = NodeType::NullLiteral;
        line = ln;
    }
};

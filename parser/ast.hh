#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>

enum NodeType { // Nodes our language supports
    Program,         // This contains all statements
    ExprStmt,
    // Statements
    BlockStmt,
    VarDeclaration,
    IfStmt,
    WhileStmt,
    ForStmt,
    FunctionStmt,
    ContinueStmt,
    BreakStmt,
    ReturnStmt,
    // Expressions
    BinaryExpr,
    UnaryExpr,
    AssignmentExpr,
    MemberExpr,
    CallExpr,
    Param,
    CastExpr,
    // Literals
    NumericLiteral,
    IdentifierLiteral,
    StringLiteral,
    BoolLiteral,
    CharLiteral,
    ArrayLiteral,
    NullLiteral,
};

struct Stmt {
    virtual ~Stmt() = default;

    NodeType kind;
    std::size_t line{}; // Line number for error reporting
};

struct Expr : Stmt {};

struct ASTExprStmt : public Stmt {
    std::shared_ptr<Expr> expression;

    ASTExprStmt(std::shared_ptr<Expr> expr, std::size_t l)
        : expression(std::move(expr)) {
            kind = NodeType::ExprStmt;
            line = l;
        }
};


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

struct ASTParam final : Expr {
    std::string name;
    std::string type;
    bool isArray;

    ASTParam(std::string n, std::string t, bool i, std::size_t l) : name(n), type(t), isArray(i) {
        kind = NodeType::Param;
        line = l;
    }
};

struct ASTCastExpr final : Expr {
    std::string type;
    std::shared_ptr<Expr> target;

    ASTCastExpr(std::string ty, std::shared_ptr<Expr> t, std::size_t l) : type(ty), target(std::move(t)) {
        kind = NodeType::CastExpr;
        line = l;
    }
};

struct ASTFunctionStmt final : Stmt {
    std::string name;
    std::string ret_type;
    std::vector<std::shared_ptr<ASTParam>> params;
    std::shared_ptr<Stmt> body;

    ASTFunctionStmt(std::string n, std::string t, std::vector<std::shared_ptr<ASTParam>> p, std::shared_ptr<Stmt> b, std::size_t l) :
                    name(n), ret_type(t), params(std::move(p)), body(std::move(b))
    {
        kind = NodeType::FunctionStmt;
        line = l;
    }
};

struct ASTReturnStmt : public Stmt {
    std::shared_ptr<Expr> value;
    ASTReturnStmt(std::shared_ptr<Expr> v, std::size_t l) : value(v) {
        kind = NodeType::ReturnStmt;
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

    bool is_array;
    std::optional<std::shared_ptr<Expr>> array_size;

    ASTVarDecl(std::string n, std::string t, std::shared_ptr<Expr> v, bool c, bool a, std::optional<std::shared_ptr<Expr>> s, std::size_t l)
        : name(std::move(n)), type(std::move(t)), value(std::move(v)), is_const(c), is_array(a), array_size(std::move(s)) {
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

struct ASTForStmt final : Stmt {
    std::shared_ptr<Stmt> init;
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Expr> update;
    std::shared_ptr<Stmt> body;

    ASTForStmt(std::shared_ptr<Stmt> i, std::shared_ptr<Expr> c, std::shared_ptr<Expr> u, std::shared_ptr<Stmt> b, std::size_t ln) :
    init(std::move(i)), condition(std::move(c)), update(std::move(u)), body(std::move(b)) {
        kind = NodeType::ForStmt;
        line = ln;
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

struct ASTMemberExpr final : Expr {
    std::shared_ptr<Expr> object;
    std::shared_ptr<Expr> property;
    bool computed;
    
    ASTMemberExpr(std::shared_ptr<Expr> obj, std::shared_ptr<Expr> prop, bool c, std::size_t l) 
        : object(std::move(obj)), property(std::move(prop)), computed(c) {
        kind = NodeType::MemberExpr;
        line = l;
    }
};

struct ASTCallExpr : Expr {
    std::shared_ptr<Expr> callee;
    std::vector<std::shared_ptr<Expr>> args;

    ASTCallExpr(std::shared_ptr<Expr> c, std::vector<std::shared_ptr<Expr>> a,std::size_t l)
        : callee(std::move(c)), args(std::move(a)) {
            kind = NodeType::CallExpr;
            line = l;
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

struct ASTCharLiteral final : Expr {
    char value;

    ASTCharLiteral(char v, std::size_t ln) : value(std::move(v)) {
        kind = NodeType::CharLiteral;
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

struct ASTArrayLiteral final : Expr {
    std::vector<std::shared_ptr<Expr>> elements;

    ASTArrayLiteral(std::vector<std::shared_ptr<Expr>> e, std::size_t ln) : elements(std::move(e)) {
        kind = NodeType::ArrayLiteral;
        line = ln;
    }
};

struct ASTNullLiteral final : Expr {
    ASTNullLiteral(std::size_t ln) {
        kind = NodeType::NullLiteral;
        line = ln;
    }
};
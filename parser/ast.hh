/*

ast.hh

*/

#include <vector>
#include <memory>
#include <string>

enum NodeType { // Nodes our language supports
    Program, // This contains all of our statements, basically all of our program
    // Statements

    // Expressions
    BinaryExpr,
    // Literals
    NumericLiteral
};

struct Stmt {
    virtual ~Stmt() = default;

    NodeType kind;
};

struct ASTProgram : Stmt {
    std::vector<std::shared_ptr<Stmt>> body; // this contains our statements
};

struct Expr : Stmt {};

struct ASTBinaryExpr : Expr {
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
    std::string op;
};

struct ASTNumericLiteral : Expr {
    double value;

    ASTNumericLiteral(double val) : value(val) {
        kind = NodeType::NumericLiteral;
    }
};
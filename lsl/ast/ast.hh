#ifndef GUARD_LSL_AST_AST_HH_INCLUDED
#define GUARD_LSL_AST_AST_HH_INCLUDED

#include <lsl/types.hh>
#include <vector>
#include <memory>

namespace lsl {


template< typename T, typename U>
std::shared_ptr<T> ast_cast(std::shared_ptr<U> u) {
    return std::static_pointer_cast<T>(u);
}

enum class AstType {
    Function,
    Body,
    Event,
    Call,
    StateChange,
    State,
    UnaryOp,
    Do,
    BinOp,
    BoolOp,
    Assignment,
    AugAssignment,
    Identifier,
    Comparison,
    For,
    While,
    If,
    Variable,
    TypeCast,
    Vector,
    Quaternion,
    List,
    Integer,
    String,
    Key,
    Float,
    VarDecl,
    Script,
    States,
    Globals,
    Return,
    NoOp,
    ExpressionList,
    Jump,
    Label
};

enum class AstCompareOpType {
    Undefined,
    Equals,
    NotEquals,
    GreaterEquals,
    LessEquals,
    Less,
    Greater
};

enum class AstUnaryOpType {
    Undefined,
    Not,
    Add,
    Invert,
    Sub,
    PreInc,
    PreDec,
    PostInc,
    PostDec
};

enum class AstBinOpType {
    Undefined,
    Add,
    Sub,
    BitAnd,
    BitOr,
    BitXor,
    Div,
    LeftShift,
    RightShift,
    Mod,
    Mult,
};

enum class AstBoolOpType {
    Undefined,
    And,
    Or
};

struct Ast {
    Ast() : type(), line(), column() {}

    AstType type;
    uint32_t line;
    uint32_t column;
};
typedef std::shared_ptr<Ast> AstPtr;

template< AstType Type >
struct AstT : Ast {
    AstT(){ type = Type; }
};

struct ExpressionList : AstT<AstType::ExpressionList> {
    std::vector<AstPtr> expressions;
};

struct Jump : AstT<AstType::Jump> {
    String label;
};

struct Label : AstT<AstType::Label> {
    String name;
};

struct NoOp : AstT<AstType::NoOp> {};

struct StateChange : AstT<AstType::StateChange> {
    String state;
};

struct Vector : AstT<AstType::Vector> {
    AstPtr x;
    AstPtr y;
    AstPtr z;
};

struct Call : AstT<AstType::Call> {
    String name;
    std::vector<AstPtr> parameters;
};

struct Quaternion : AstT<AstType::Quaternion> {
    AstPtr x;
    AstPtr y;
    AstPtr z;
    AstPtr s;
};

struct Key : AstT<AstType::Key> {
    String value;
};

struct StringLit : AstT<AstType::String> {
    String value;
};

struct Integer : AstT<AstType::Integer> {
    int32_t value;
};

struct Float : AstT<AstType::Float> {
    double value;
};

struct Variable : AstT<AstType::Variable> {
    String name;
    String member;
};

struct List : AstT<AstType::List> {
    std::vector<AstPtr> elements;
};

struct BinOp : AstT<AstType::BinOp> {
    AstPtr left;
    AstPtr right;
    AstBinOpType op;
};

struct BoolOp : AstT<AstType::BoolOp> {
    AstPtr left;
    AstPtr right;
    AstBoolOpType op;
};

struct UnaryOp : AstT<AstType::UnaryOp> {
    AstUnaryOpType op;
    AstPtr target;
};

struct TypeCast : AstT<AstType::TypeCast> {
    String target_type;
    AstPtr right;
};

struct TypeDecl {
    String type;
    String name;
};

struct VarDecl : AstT<AstType::VarDecl> {
    String type_name;
    String name;
    AstPtr right;
};

struct Comparison : AstT<AstType::Comparison> {
    AstPtr left;
    AstPtr right;
    AstCompareOpType op;
};

struct Assignment : AstT<AstType::Assignment> {
    AstPtr left;
    AstPtr right;
};

struct AugAssignment : AstT<AstType::AugAssignment> {
    AstPtr left;
    AstPtr right;
    AstBinOpType op;
};

// Acts as scope
struct Body : AstT<AstType::Body> {
    std::vector<AstPtr> statements;
};

struct For : AstT<AstType::For> {
    AstPtr  init;
    AstPtr  condition;
    AstPtr  step;
    AstPtr  body;
};

struct While : AstT<AstType::While> {
    AstPtr condition;
    AstPtr body;
};

struct Do : AstT<AstType::Do> {
    AstPtr condition;
    AstPtr body;
};

struct If : AstT<AstType::If> {
    AstPtr condition;
    AstPtr body;
    AstPtr elseIf;
};

struct FunctionLike {
    String name;
    std::vector<TypeDecl> args;
    Body body;
};

struct Function : AstT<AstType::Function>, FunctionLike {
    String returnType;
};

struct Event : AstT<AstType::Event>, FunctionLike {
};

struct StateDef : AstT<AstType::State> {
    String name;
    std::vector<Event> events;
};

struct States : AstT<AstType::States> {
    std::vector<AstPtr> states;
};

struct Globals : AstT<AstType::Globals> {
    std::vector<AstPtr> globals;
};

struct Script : AstT<AstType::Script> {
    Globals globals;
    States states;
};

struct Return : AstT<AstType::Return> {
    AstPtr value;
};

}
#endif //GUARD_LSL_AST_AST_HH_INCLUDED

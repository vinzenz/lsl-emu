#ifndef GUARD_LSL_AST_AST_HH_INCLUDED
#define GUARD_LSL_AST_AST_HH_INCLUDED

#include <cassert>
#include <lsl/types.hh>
#include <vector>
#include <memory>
#include <lsl/runtime/types.hh>
#include <boost/optional/optional.hpp>

namespace lsl {

template< typename T, typename U>
std::shared_ptr<T> ast_cast(std::shared_ptr<U> u) {
    return std::static_pointer_cast<T>(u);
}

#define FOREACH_ASTTYPE(APPLY) \
    APPLY(Function) \
    APPLY(Body) \
    APPLY(Event) \
    APPLY(Call) \
    APPLY(StateChange) \
    APPLY(StateDef) \
    APPLY(UnaryOp) \
    APPLY(Do) \
    APPLY(BinOp) \
    APPLY(BoolOp) \
    APPLY(Assignment) \
    APPLY(AugAssignment) \
    APPLY(Comparison) \
    APPLY(For) \
    APPLY(While) \
    APPLY(If) \
    APPLY(Variable) \
    APPLY(TypeCast) \
    APPLY(Vector) \
    APPLY(Quaternion) \
    APPLY(List) \
    APPLY(Integer) \
    APPLY(StringLit) \
    APPLY(Key) \
    APPLY(Float) \
    APPLY(VarDecl) \
    APPLY(Script) \
    APPLY(States) \
    APPLY(Globals) \
    APPLY(Return) \
    APPLY(NoOp) \
    APPLY(ExpressionList) \
    APPLY(Jump) \
    APPLY(Label)

enum class AstType {
#define AST_ENUM_TYPE(x) x,
    FOREACH_ASTTYPE(AST_ENUM_TYPE)
    __dummy
#undef AST_ENUM_TYPE
};

template< AstType >
struct AstType2Name;

#define AST_TYPE_FWD_DECL(BaseName) struct BaseName;
FOREACH_ASTTYPE(AST_TYPE_FWD_DECL)
#undef AST_TYPE_FWD_DECL

#define AST_TYPE_2_NAME_SPEC(BaseName) \
    template<> \
    struct AstType2Name<AstType::BaseName> { \
        static char const * PrettyName() { return #BaseName; } \
    };

FOREACH_ASTTYPE(AST_TYPE_2_NAME_SPEC)
#undef AST_TYPE_2_NAME_SPEC


struct AstVisitor{
    AstVisitor(){}
    virtual ~AstVisitor(){}

#define DEFINE_VISITOR_FUNCTION(BaseName) \
    virtual void Visit##BaseName(BaseName *){}
    FOREACH_ASTTYPE(DEFINE_VISITOR_FUNCTION)
#undef DEFINE_VISITOR_FUNCTION
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

template< typename Container >
bool all_constant(Container const & c) {
    for(auto const & e : c) {
        if(e && !e->IsConstant()) {
            return false;
        }
    }
    return true;
}

namespace runtime {
    class ScriptValue;
    typedef boost::optional<ScriptValue> OptionalScriptValue;
}


struct Ast {
    Ast() : type(), line(), column() {}
    virtual ~Ast(){}

    virtual void Visit(AstVisitor *) = 0;
    virtual void VisitChildren(AstVisitor *) = 0;
    virtual char const * PrettyName() const = 0;
    virtual bool IsConstant() const { return false; }
    virtual runtime::OptionalScriptValue  EvalConstExpr() const { return {}; }

#define DEFINE_COMMON_ABSTRACT(BaseName)   \
    virtual bool Is##BaseName() const { return false; } \
    virtual BaseName * As##BaseName() { return NULL; }

    FOREACH_ASTTYPE(DEFINE_COMMON_ABSTRACT)
#undef DEFINE_COMMON_ABSTRACT

    AstType type;
    uint32_t line;
    uint32_t column;
};
typedef std::shared_ptr<Ast> AstPtr;

template< AstType, AstType >
struct ast_cast_;

template<AstType Type>
struct ast_cast_<Type, Type>{
    template< typename T, typename U >
    static T * cast(U * const u) {
        return static_cast<T* const>(u);
    }
};

template< AstType, AstType >
struct ast_cast_ {
    template< typename T, typename U >
    static T * cast(U * const) {
        return nullptr;
    }
};

template< AstType Type >
struct AstT : Ast {
    AstT(){ type = Type; }

    virtual char const * PrettyName() const { return AstType2Name<Type>::PrettyName(); }
#define DEFINE_COMMON_IMPL(BaseName)   \
    virtual bool Is##BaseName() const { return AstType::BaseName == Type; } \
    virtual BaseName* As##BaseName() { return ast_cast_<Type, AstType::BaseName>::template cast<BaseName>(this); }

    FOREACH_ASTTYPE(DEFINE_COMMON_IMPL)
#undef DEFINE_COMMON_IMPL
    virtual void VisitChildren(AstVisitor *){}
};

struct ExpressionList : AstT<AstType::ExpressionList> {
    std::vector<AstPtr> expressions;

    virtual void Visit(AstVisitor * v){
        v->VisitExpressionList(this);
    }
    virtual void VisitChildren(AstVisitor * v){
        for(auto & e : expressions) {
            if(e) {
                e->Visit(v);
            }
        }
    }
};

struct Jump : AstT<AstType::Jump> {
    String label;
    virtual void Visit(AstVisitor * v){
        v->VisitJump(this);
    }
};

struct Label : AstT<AstType::Label> {
    String name;
    virtual void Visit(AstVisitor * v){
        v->VisitLabel(this);
    }
};

struct NoOp : AstT<AstType::NoOp> {
    virtual void Visit(AstVisitor * v){
        v->VisitNoOp(this);
    }
};

struct StateChange : AstT<AstType::StateChange> {
    String state;
    virtual void Visit(AstVisitor * v){
        v->VisitStateChange(this);
    }
};

struct Vector : AstT<AstType::Vector> {
    AstPtr x;
    AstPtr y;
    AstPtr z;

    virtual runtime::OptionalScriptValue  EvalConstExpr() const {
        if(x && y && z && IsConstant()) {
            return runtime::ScriptValue{
                runtime::ValueType::Vector,
                runtime::Vector{
                    x->EvalConstExpr().get().as_float(),
                    y->EvalConstExpr().get().as_float(),
                    z->EvalConstExpr().get().as_float()
                },
                false
            };
        }
        return {};
    }

    virtual bool IsConstant() const {
        return x->IsConstant()
            && y->IsConstant()
            && z->IsConstant();
    }

    virtual void Visit(AstVisitor * v){
        v->VisitVector(this);
    }
    virtual void VisitChildren(AstVisitor * v){
        assert(x && y && z);
        x->Visit(v);
        y->Visit(v);
        z->Visit(v);
    }
};

struct Call : AstT<AstType::Call> {
    String name;
    std::vector<AstPtr> parameters;

    virtual void Visit(AstVisitor * v){
        v->VisitCall(this);
    }
    virtual void VisitChildren(AstVisitor * v){
        for(auto & e : parameters) {
            if(e) {
                e->Visit(v);
            }
        }
    }
};

struct Quaternion : AstT<AstType::Quaternion> {
    AstPtr x;
    AstPtr y;
    AstPtr z;
    AstPtr s;

    virtual runtime::OptionalScriptValue  EvalConstExpr() const {
        if(x && y && z && IsConstant()) {
            return runtime::ScriptValue{
                runtime::ValueType::Rotation,
                runtime::Rotation{
                    x->EvalConstExpr().get().as_float(),
                    y->EvalConstExpr().get().as_float(),
                    z->EvalConstExpr().get().as_float(),
                    s->EvalConstExpr().get().as_float()
                },
                false
            };
        }
        return {};
    }

    virtual bool IsConstant() const {
        return x->IsConstant()
            && y->IsConstant()
            && z->IsConstant()
            && s->IsConstant();
    }

    virtual void Visit(AstVisitor * v){
        v->VisitQuaternion(this);
    }
    virtual void VisitChildren(AstVisitor * v){
        assert(x && y && z && s);
        x->Visit(v);
        y->Visit(v);
        z->Visit(v);
        s->Visit(v);
    }
};

struct Key : AstT<AstType::Key> {
    String value;

    virtual runtime::OptionalScriptValue  EvalConstExpr() const {
        return runtime::ScriptValue{
            runtime::ValueType::Key,
            value,
            false
        };
    }

    virtual bool IsConstant() const { return true; }
    virtual void Visit(AstVisitor * v){
        v->VisitKey(this);
    }
};

struct StringLit : AstT<AstType::StringLit> {
    String value;
    virtual runtime::OptionalScriptValue  EvalConstExpr() const {
        return runtime::ScriptValue{
            runtime::ValueType::Key,
            value,
            false
        };
    }
    virtual bool IsConstant() const { return true; }
    virtual void Visit(AstVisitor * v){
        v->VisitStringLit(this);
    }
};

struct Integer : AstT<AstType::Integer> {
    int32_t value;
    virtual runtime::OptionalScriptValue  EvalConstExpr() const {
        return runtime::ScriptValue{
            runtime::ValueType::Integer,
            value,
            false
        };
    }
    virtual bool IsConstant() const { return true; }
    virtual void Visit(AstVisitor * v){
        v->VisitInteger(this);
    }
};

struct Float : AstT<AstType::Float> {
    double value;
    virtual runtime::OptionalScriptValue  EvalConstExpr() const {
        return runtime::ScriptValue{
            runtime::ValueType::Float,
            value,
            false
        };
    }
    virtual bool IsConstant() const { return true; }
    virtual void Visit(AstVisitor * v){
        v->VisitFloat(this);
    }
};

struct Variable : AstT<AstType::Variable> {
    String name;
    String member;

    virtual void Visit(AstVisitor * v){
        v->VisitVariable(this);
    }
};

struct List : AstT<AstType::List> {
    std::vector<AstPtr> elements;

    virtual runtime::OptionalScriptValue  EvalConstExpr() const {
        if(!IsConstant()) {
            return {};
        }
        auto lst = runtime::List();
        lst.reserve(elements.size());
        for(auto const & e : elements) {
            lst.push_back(e->EvalConstExpr().get());
        }
        return runtime::ScriptValue{
            runtime::ValueType::Integer,
            lst,
            false
        };
    }

    virtual bool IsConstant() const { return all_constant(elements); }
    virtual void Visit(AstVisitor * v){
        v->VisitList(this);
    }
};

struct BinOp : AstT<AstType::BinOp> {
    AstPtr left;
    AstPtr right;
    AstBinOpType op;

    virtual bool IsConstant() const {
        return left->IsConstant() && right->IsConstant();
    }
    virtual void Visit(AstVisitor * v){
        v->VisitBinOp(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        left->Visit(v);
        right->Visit(v);
    }
};

struct BoolOp : AstT<AstType::BoolOp> {
    AstPtr left;
    AstPtr right;
    AstBoolOpType op;

    virtual runtime::OptionalScriptValue  EvalConstExpr() const {
        runtime::Integer result = (op == AstBoolOpType::And
            ? left->EvalConstExpr().get().as_bool() && right->EvalConstExpr().get().as_bool()
            : left->EvalConstExpr().get().as_bool() || right->EvalConstExpr().get().as_bool());
        return runtime::ScriptValue{
            runtime::ValueType::Integer,
            result,
            false
        };
    }

    virtual bool IsConstant() const {
        return left->IsConstant() && right->IsConstant();
    }
    virtual void Visit(AstVisitor * v){
        v->VisitBoolOp(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        left->Visit(v);
        right->Visit(v);
    }
};

struct UnaryOp : AstT<AstType::UnaryOp> {
    AstUnaryOpType op;
    AstPtr target;

    virtual bool IsConstant() const {
        return target->IsConstant();
    }
    virtual void Visit(AstVisitor * v){
        v->VisitUnaryOp(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        target->Visit(v);
    }
};

struct TypeCast : AstT<AstType::TypeCast> {
    String target_type;
    AstPtr right;

    virtual runtime::OptionalScriptValue  EvalConstExpr() const {
        if(target_type == "string") {
            return runtime::ScriptValue{
                runtime::ValueType::String,
                right->EvalConstExpr().get().as_string(),
                false
            };
        }
        if(target_type == "key") {
            return runtime::ScriptValue{
                runtime::ValueType::Key,
                right->EvalConstExpr().get().as_string(),
                false
            };
        }
        if(target_type == "integer") {
            return runtime::ScriptValue{
                runtime::ValueType::Integer,
                right->EvalConstExpr().get().as_integer(),
                false
            };
        }
        if(target_type == "float") {
            return runtime::ScriptValue{
                runtime::ValueType::Integer,
                right->EvalConstExpr().get().as_float(),
                false
            };
        }
        if(target_type == "vector") {
            return runtime::ScriptValue{
                runtime::ValueType::Integer,
                right->EvalConstExpr().get().as_vector(),
                false
            };
        }
        if(target_type == "rotation") {
            return runtime::ScriptValue{
                runtime::ValueType::Integer,
                right->EvalConstExpr().get().as_rotation(),
                false
            };
        }
        if(target_type == "list") {
            return runtime::ScriptValue{
                runtime::ValueType::Integer,
                right->EvalConstExpr().get().as_list(),
                false
            };
        }
        return {};
    }

    virtual bool IsConstant() const {
        return right->IsConstant();
    }
    virtual void Visit(AstVisitor * v){
        v->VisitTypeCast(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        right->Visit(v);
    }
};

struct TypeDecl {
    String type;
    String name;
};

struct VarDecl : AstT<AstType::VarDecl> {
    String type_name;
    String name;
    AstPtr right;

    virtual void Visit(AstVisitor * v){
        v->VisitVarDecl(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        right->Visit(v);
    }
};


struct Comparison : AstT<AstType::Comparison> {
    AstPtr left;
    AstPtr right;
    AstCompareOpType op;

    virtual bool IsConstant() const {
        return left->IsConstant() && right->IsConstant();
    }
    virtual void Visit(AstVisitor * v){
        v->VisitComparison(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        left->Visit(v);
        right->Visit(v);
    }
};

struct Assignment : AstT<AstType::Assignment> {
    AstPtr left;
    AstPtr right;

    virtual bool IsConstant() const {
        // TODO: Should this considered be constant if `right` is constant?
        return false;
    }
    virtual void Visit(AstVisitor * v){
        v->VisitAssignment(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        left->Visit(v);
        right->Visit(v);
    }
};

struct AugAssignment : AstT<AstType::AugAssignment> {
    AstPtr left;
    AstPtr right;
    AstBinOpType op;

    virtual bool IsConstant() const {
        return false;
    }
    virtual void Visit(AstVisitor * v){
        v->VisitAugAssignment(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        left->Visit(v);
        right->Visit(v);
    }
};

// Acts as scope
struct Body : AstT<AstType::Body> {
    std::vector<AstPtr> statements;

    virtual void Visit(AstVisitor * v){
        v->VisitBody(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        for(auto & e : statements) {
            if(e) {
                e->Visit(v);
            }
        }
    }
};

struct For : AstT<AstType::For> {
    AstPtr  init;
    AstPtr  condition;
    AstPtr  step;
    AstPtr  body;


    virtual void Visit(AstVisitor * v){
        v->VisitFor(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        if(init) init->Visit(v);
        if(condition) condition->Visit(v);
        if(step) condition->Visit(v);
        if(body) body->Visit(v);
    }
};

struct While : AstT<AstType::While> {
    AstPtr condition;
    AstPtr body;

    virtual void Visit(AstVisitor * v){
        v->VisitWhile(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        if(condition) condition->Visit(v);
        if(body) body->Visit(v);
    }
};

struct Do : AstT<AstType::Do> {
    AstPtr condition;
    AstPtr body;

    virtual void Visit(AstVisitor * v){
        v->VisitDo(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        if(condition) condition->Visit(v);
        if(body) body->Visit(v);
    }
};

struct If : AstT<AstType::If> {
    AstPtr condition;
    AstPtr body;
    AstPtr elseIf;

    virtual void Visit(AstVisitor * v){
        v->VisitIf(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        if(condition) condition->Visit(v);
        if(body) body->Visit(v);
        if(elseIf) elseIf->Visit(v);
    }
};

struct FunctionLike {
    String name;
    std::vector<TypeDecl> args;
    Body body;
};

struct Function : AstT<AstType::Function>, FunctionLike {
    String returnType;

    virtual void Visit(AstVisitor * v){
        v->VisitFunction(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        body.Visit(v);
    }
};

struct Event : AstT<AstType::Event>, FunctionLike {

    virtual void Visit(AstVisitor * v){
        v->VisitEvent(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        body.Visit(v);
    }
};

struct StateDef : AstT<AstType::StateDef> {
    String name;
    std::vector<Event> events;

    virtual void Visit(AstVisitor * v){
        v->VisitStateDef(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        for(auto & e : events) {
            e.Visit(v);
        }
    }
};

struct States : AstT<AstType::States> {
    std::vector<std::shared_ptr<StateDef>> states;

    virtual void Visit(AstVisitor * v){
        v->VisitStates(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        for(auto & e : states) {
            if(e) e->Visit(v);
        }
    }
};

struct Globals : AstT<AstType::Globals> {
    std::vector<AstPtr> globals;

    virtual void Visit(AstVisitor * v){
        v->VisitGlobals(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        for(auto & e : globals) {
            if(e) e->Visit(v);
        }
    }
};

struct Script : AstT<AstType::Script> {
    Globals globals;
    States states;

    virtual void Visit(AstVisitor * v){
        v->VisitScript(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        globals.Visit(v);
        states.Visit(v);
    }
};

struct Return : AstT<AstType::Return> {
    AstPtr value;

    virtual void Visit(AstVisitor * v){
        v->VisitReturn(this);
    }
    virtual void VisitChildren(AstVisitor * v) {
        if(value) value->Visit(v);
    }
};


}
#endif //GUARD_LSL_AST_AST_HH_INCLUDED

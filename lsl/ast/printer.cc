#include <lsl/ast/printer.hh>
#include <cassert>
namespace lsl {
namespace ast {

void print(PrinterState & state, Script const & s) {
    print(state, s.globals);
    print(state, s.states);
}

void print(PrinterState & state, Ast const & a) {
    switch(a.type) {
    case AstType::ExpressionList: print(state, static_cast<ExpressionList const &>(a)); break;
    case AstType::Jump: print(state, static_cast<Jump const &>(a)); break;
    case AstType::Label: print(state, static_cast<Label const &>(a)); break;
    case AstType::NoOp: print(state, static_cast<NoOp const &>(a)); break;
    case AstType::StateChange: print(state, static_cast<StateChange const &>(a)); break;
    case AstType::Vector: print(state, static_cast<Vector const &>(a)); break;
    case AstType::Call: print(state, static_cast<Call const &>(a)); break;
    case AstType::Quaternion: print(state, static_cast<Quaternion const &>(a)); break;
    case AstType::Key: print(state, static_cast<Key const &>(a)); break;
    case AstType::String: print(state, static_cast<StringLit const &>(a)); break;
    case AstType::Integer: print(state, static_cast<Integer const &>(a)); break;
    case AstType::Float: print(state, static_cast<Float const &>(a)); break;
    case AstType::Variable: print(state, static_cast<Variable const &>(a)); break;
    case AstType::List: print(state, static_cast<List const &>(a)); break;
    case AstType::BinOp: print(state, static_cast<BinOp const &>(a)); break;
    case AstType::BoolOp: print(state, static_cast<BoolOp const &>(a)); break;
    case AstType::UnaryOp: print(state, static_cast<UnaryOp const &>(a)); break;
    case AstType::TypeCast: print(state, static_cast<TypeCast const &>(a)); break;
    case AstType::VarDecl: print(state, static_cast<VarDecl const &>(a)); break;
    case AstType::Comparison: print(state, static_cast<Comparison const &>(a)); break;
    case AstType::Assignment: print(state, static_cast<Assignment const &>(a)); break;
    case AstType::AugAssignment: print(state, static_cast<AugAssignment const &>(a)); break;
    case AstType::Body: print(state, static_cast<Body const &>(a)); break;
    case AstType::For: print(state, static_cast<For const &>(a)); break;
    case AstType::While: print(state, static_cast<While const &>(a)); break;
    case AstType::Do: print(state, static_cast<Do const &>(a)); break;
    case AstType::If: print(state, static_cast<If const &>(a)); break;
    case AstType::Function: print(state, static_cast<Function const &>(a)); break;
    case AstType::Event: print(state, static_cast<Event const &>(a)); break;
    case AstType::State: print(state, static_cast<StateDef const &>(a)); break;
    case AstType::States: print(state, static_cast<States const &>(a)); break;
    case AstType::Globals: print(state, static_cast<Globals const &>(a)); break;
    case AstType::Script: print(state, static_cast<Script const &>(a)); break;
    case AstType::Return: print(state, static_cast<Return const &>(a)); break;
    default:
        assert(false);
    }
}

void print(PrinterState & state, ExpressionList const & el) {
    bool first = true;
    for(auto const & e : el.expressions) {
        if(!first) {
            printf(", ");
        }
        print(state, e);
        first = false;
    }
}

void print(PrinterState & state, Jump const & j) {
    state.print_padding();
    printf("jump %s", j.label.c_str());
}

void print(PrinterState &, Label const & l) {
    printf("@%s", l.name.c_str());
}

void print(PrinterState &, NoOp const &) {
}

void print(PrinterState &, StateChange const & s) {
    printf("state %s", s.state.c_str());
}

void print(PrinterState & state, Vector const & v) {
    printf("<");
    print(state, v.x);
    printf(", ");
    print(state, v.y);
    printf(", ");
    print(state, v.z);
    printf(">");
}

void print(PrinterState & state, Call const & c) {
    printf("%s(", c.name.c_str());
    bool first = true;
    for(auto const & e : c.parameters) {
        if(!first) printf(", ");
        first = false;
        print(state, e);
    }
    printf(")");
}

void print(PrinterState & state, List const & l) {
    printf("[");
    bool first = true;
    for(auto const & e : l.elements) {
        if(!first) printf(", ");
        first = false;
        print(state, e);
    }
    printf("]");
}

void print(PrinterState & state, Quaternion const & q) {
    printf("<");
    print(state, q.x);
    printf(", ");
    print(state, q.y);
    printf(", ");
    print(state, q.z);
    printf(", ");
    print(state, q.s);
    printf(">");
}

void print_escaped_string(std::string const & s) {
    printf("\"");
    for(size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        switch(c) {
        case '\b':  printf("\\b"); break;
        case '\f':  printf("\\f"); break;
        case '\n':  printf("\\n"); break;
        case '\r':  printf("\\r"); break;
        case '\t':  printf("\\t"); break;
        case '\a':  printf("\\a"); break;
        case '"':   printf("\\\""); break;
        case '\\':  printf("\\\\"); break;
        case ' ':   printf(" "); break;
        case '\x7f': printf("\\x7f"); break;
        default:
            if(c >= 0 && c <= '\x1f') {
                printf("\\x%.02x", c);
            }
            else {
                printf("%c", c);
            }
            break;
        }
    }
    printf("\"");
}

char const * to_string(AstBinOpType op) {
    switch(op) {
    case AstBinOpType::Add:
        return "+";
    case AstBinOpType::Sub:
        return "-";
    case AstBinOpType::BitAnd:
        return "&";
    case AstBinOpType::BitOr:
        return "|";
    case AstBinOpType::BitXor:
        return "^";
    case AstBinOpType::Div:
        return "/";
    case AstBinOpType::LeftShift:
        return "<<";
    case AstBinOpType::RightShift:
        return ">>";
    case AstBinOpType::Mod:
        return "%";
    case AstBinOpType::Mult:
        return "*";
    default:
    assert(false && "Unknown AstBinOpType op");
    }
    return nullptr;
}

char const * to_string(AstUnaryOpType op) {
    switch(op) {
    case AstUnaryOpType::Not:
        return "!";
    case AstUnaryOpType::Add:
        return "+";
    case AstUnaryOpType::Invert:
        return "~";
    case AstUnaryOpType::Sub:
        return "-";
    case AstUnaryOpType::PreInc:
        return "++";
    case AstUnaryOpType::PreDec:
        return "--";
    case AstUnaryOpType::PostInc:
        return "++";
    case AstUnaryOpType::PostDec:
        return "--";
    default:
        assert(false && "Unknown AstUnaryOpType op");
    }
    return nullptr;
}

char const * to_string(AstBoolOpType op) {
    return op == AstBoolOpType::And ? "&&" : "||";
}

char const * to_string(AstCompareOpType op) {
    switch(op) {
        case AstCompareOpType::Equals:
            return "==";
        case AstCompareOpType::NotEquals:
            return "!=";
        case AstCompareOpType::GreaterEquals:
            return "<=";
        case AstCompareOpType::LessEquals:
            return ">=";
        case AstCompareOpType::Less:
            return "<";
        case AstCompareOpType::Greater:
            return ">";
        default:
            assert(false && "Unknown AstCompareType op");
    }
    return nullptr;
}

void print(PrinterState &, Key const & k) {
    printf("(key)");
    print_escaped_string(k.value);
}

void print(PrinterState &, StringLit const & s) {
    print_escaped_string(s.value);
}

void print(PrinterState &, Integer const & i) {
    printf("%d", i.value);
}

void print(PrinterState &, Float const & f) {
    printf("%.9g", f.value);
}

void print(PrinterState & state, BinOp const & b) {
    bool binop = b.left->type == AstType::BinOp;
    if(binop) printf("(");
    print(state, b.left);
    if(binop) printf(")");
    printf(" %s ", to_string(b.op));
    print(state, b.right);
}

void print(PrinterState & state, BoolOp const & b) {
    print(state, b.left);
    printf(" %s ", to_string(b.op));
    print(state, b.right);
}

void print(PrinterState & state, UnaryOp const & u) {
    if(u.op == AstUnaryOpType::PostInc || u.op == AstUnaryOpType::PostDec) {
        print(state, u.target);
        printf("%s", to_string(u.op));
    }
    else {
        printf("%s", to_string(u.op));
        print(state, u.target);
    }
}

void print(PrinterState & state, TypeCast const & t) {
    printf("(%s)(", t.target_type.c_str());
    print(state, t.right);
    printf(")");
}

void print(PrinterState &, TypeDecl const & t) {
    printf("%s %s", t.type.c_str(), t.name.c_str());
}

void print(PrinterState & state, VarDecl const & v) {
    printf("%s %s", v.type_name.c_str(), v.name.c_str());
    if(v.right) {
        printf(" = ");
        print(state, v.right);
    }
}

void print(PrinterState & state, Comparison const & c) {
    print(state, c.left);
    printf(" %s ", to_string(c.op));
    print(state, c.right);
}

void print(PrinterState & state, Assignment const & a) {
    print(state, a.left);
    printf(" = ");
    print(state, a.right);
}

void print(PrinterState & state, AugAssignment const & a) {
    print(state, a.left);
    printf(" %s= ", to_string(a.op));
    print(state, a.right);
}

void print(PrinterState & state, Body const & b) {
    printf("{\n");
    state.indent();
    for(auto const & e : b.statements) {
        state.print_padding();
        print(state, e);
        switch(e->type) {
            case AstType::For:
            case AstType::If:
            case AstType::Do:
            case AstType::While:
                break;
            default:
                printf(";\n");
        }

    }
    state.outdent();
    state.print_padding();
    printf("}");
}

void print_body(PrinterState & state, Ast const & e) {
    if(e.type != AstType::Body) {
        printf(" {\n");
        state.indent();
        state.print_padding();
        print(state, e);
        printf(";\n");
        state.outdent();
        state.print_padding();
        printf("}");
    }
    else {
        print(state, e);
    }
}

void print_body(PrinterState & state, AstPtr const & e) {
    print_body(state, *e);
}

void print(PrinterState & state, For const & f) {
    printf("for(");
    print(state, f.init);
    printf("; ");
    print(state, f.condition);
    printf("; ");
    print(state, f.step);
    printf(") ");
    printf("\n");
}

void print(PrinterState & state, While const & w) {
    printf("while(");
    print(state, w.condition);
    printf(") ");
    print_body(state, w.body);
    printf("\n");
}

void print(PrinterState & state, Do const & d) {
    printf("do ");
    print_body(state, d.body);
    state.print_padding();
    printf("while(");
    print(state, d.condition);
    printf(");\n\n");
}

void print(PrinterState & state, If const & i) {
    printf("if(");
    print(state, i.condition);
    printf(") ");
    print_body(state, i.body);
    printf("\n");
}

void print_fun(PrinterState & state, FunctionLike const & f) {
    printf("%s(", f.name.c_str());
    bool first = true;
    for(auto const & a : f.args) {
        if(!first) printf(", ");
        first = false;
        print(state, a);
    }
    printf(") ");
    print(state, f.body);
    printf("\n");
}

void print(PrinterState & state, Function const & f) {
    if(!f.returnType.empty()) {
        printf("%s ", f.returnType.c_str());
    }
    print_fun(state, f);
}

void print(PrinterState & state, Event const & e) {
    state.print_padding();
    print_fun(state, e);
}

void print(PrinterState & state, StateDef const & s) {
    if(s.name == "default") {
        printf("default {\n");
    }
    else {
        printf("state %s {\n", s.name.c_str());
    }

    state.indent();
    for(auto const & e : s.events) {
        print(state, e);
        printf("\n");
    }
    state.outdent();
    state.print_padding();
    printf("}\n");
}

void print(PrinterState & state, Globals const & g) {
    AstType type = AstType::VarDecl;
    for(auto const & v : g.globals) {
        if(v->type != type) {
            printf("\n");
        }
        print(state, v);
        if(v->type != AstType::Function) {
            printf(";\n");
        }
        else {
            printf("\n");
        }
        type = v->type;
    }
    printf("\n");
}

void print(PrinterState & state, Return const & r) {
    if(r.value) {
        printf("return ");
        print(state, r.value);
    }
    else {
        printf("return");
    }
}

void print(PrinterState & state, States const & s) {
    for(auto const & ste : s.states) {
        print(state, ste);
        printf("\n");
    }
}

void print(PrinterState &, Variable const & v) {
    printf("%s%s%s", v.name.c_str(), v.member.empty() ? "" : ".", v.member.c_str());
}

}
}

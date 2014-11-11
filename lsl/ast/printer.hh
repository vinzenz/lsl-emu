#ifndef GUARD_LSL_AST_PRINTER_HH_INCLUDED
#define GUARD_LSL_AST_PRINTER_HH_INCLUDED

#include <lsl/ast/ast.hh>

namespace lsl {
namespace ast {
struct PrinterState{
    int depth;

    void indent() { ++depth; }
    void outdent(){ --depth; }
    void print_padding() {
        for(int i = 0; i < depth; ++i) {
            printf("    ");
        }
    }
};

template< typename T >
void print(PrinterState & state, std::shared_ptr<T> const & s) {
    print(state, *s);
}

void print(PrinterState & state, Script const &);
void print(PrinterState & state, Ast const &);
void print(PrinterState & state, ExpressionList const &);
void print(PrinterState & state, Jump const &);
void print(PrinterState & state, Label const &);
void print(PrinterState & state, NoOp const &);
void print(PrinterState & state, StateChange const &);
void print(PrinterState & state, Vector const &);
void print(PrinterState & state, List const &);
void print(PrinterState & state, Call const &);
void print(PrinterState & state, Quaternion const &);
void print(PrinterState & state, Key const &);
void print(PrinterState & state, StringLit const &);
void print(PrinterState & state, Integer const &);
void print(PrinterState & state, Float const &);
void print(PrinterState & state, BinOp const &);
void print(PrinterState & state, BoolOp const &);
void print(PrinterState & state, UnaryOp const &);
void print(PrinterState & state, TypeCast const &);
void print(PrinterState & state, TypeDecl const &);
void print(PrinterState & state, VarDecl const &);
void print(PrinterState & state, Comparison const &);
void print(PrinterState & state, Assignment const &);
void print(PrinterState & state, AugAssignment const &);
void print(PrinterState & state, Body const &);
void print(PrinterState & state, For const &);
void print(PrinterState & state, While const &);
void print(PrinterState & state, Do const &);
void print(PrinterState & state, If const &);
void print(PrinterState & state, FunctionLike const &);
void print(PrinterState & state, Function const &);
void print(PrinterState & state, Event const &);
void print(PrinterState & state, StateDef const &);
void print(PrinterState & state, Globals const &);
void print(PrinterState & state, Return const &);
void print(PrinterState & state, States const &);
void print(PrinterState & state, Variable const & v);

}}

#endif //GUARD_LSL_AST_PRINTER_HH_INCLUDED
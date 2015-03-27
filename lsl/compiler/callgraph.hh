#ifndef GUARD_LSL_COMPILER_CALLGRAPH_HH_INCLUDED
#define GUARD_LSL_COMPILER_CALLGRAPH_HH_INCLUDED

#include <lsl/ast/ast.hh>

namespace lsl {

class CallGraphVisitor : lsl::AstVisitor {
    CallGraphVisitor();
    virtual ~CallGraphVisitor();
};

class CallGraphNode {
    AstPtr function_;
};

class CallGraph {

};

}

#endif //GUARD_LSL_COMPILER_CALLGRAPH_HH_INCLUDED

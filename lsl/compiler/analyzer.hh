#ifndef GUARD_LSL_COMPILER_ANALYZER_HH_INCLUDED
#define GUARD_LSL_COMPILER_ANALYZER_HH_INCLUDED

#include <map>
#include <lsl/ast/ast.hh>
#include <tuple>

namespace lsl {

struct EventInfo {
    std::vector<std::tuple<runtime::ValueType, String>> Parameters;
    String Name;
    AstPtr AstNode;
};

struct FunctionInfo : EventInfo {
    runtime::ValueType ReturnType;
    std::size_t Called; // function calls
};

struct VarInfo {
    runtime::ValueType Type;
    String Name;
    std::size_t References; // variable usages
    AstPtr AstNode;
};

struct BlockInfo {
    bool Entered;

};

struct StateInfo {
    String Name;
    std::map<String, EventInfo> Events;
    AstPtr AstNode;
    std::size_t Called; // state xyz calls
};

struct SourceInfo {
    std::map<String, VarInfo> Globals;
    std::map<String, FunctionInfo> Functions;
    std::map<String, StateInfo> States;
};

class Analyzer {
protected:lsl::
    SourceInfo source_info_;
    lsl::Script & script_;
public:
    Analyzer(lsl::Script & script);
    virtual ~Analyzer();

    void Perform();

    lsl::Script & Script();
    SourceInfo & Info();
};

}

#endif //GUARD_LSL_COMPILER_ANALYZER_HH_INCLUDED

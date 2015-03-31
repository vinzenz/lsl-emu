#ifndef GUARD_LSL_COMPILER_ANALYZER_HH_INCLUDED
#define GUARD_LSL_COMPILER_ANALYZER_HH_INCLUDED

#include <map>
#include <unordered_map>
#include <lsl/ast/ast.hh>
#include <lsl/compiler/libinfo.hh>
#include <tuple>

namespace lsl {

struct VarInfo {
    runtime::ValueType Type;
    String Name;
    std::size_t References; // variable usages
    bool Initialized;
    bool IsParameter;
    Ast* AstNode;    

    VarInfo()
    : Type(runtime::ValueType::Void)
    , Name()
    , References(0)
    , Initialized(false)
    , IsParameter(false)
    , AstNode(0)
    {}
};

struct EventInfo;
struct BlockInfo {
    EventInfo * Within;
    BlockInfo * Parent;
    std::map<String, VarInfo> Variables;

    BlockInfo()
    : Within(nullptr)
    , Parent(nullptr)
    , Variables()
    {}
};

struct EventInfo {
    bool IsEvent;
    std::vector<std::tuple<runtime::ValueType, String>> Parameters;
    String Name;
    Ast* AstNode;
    std::vector<std::shared_ptr<BlockInfo>> Blocks;
    std::map<String, VarInfo> ParamVars;
    std::vector<EventInfo*> Calls;
    std::vector<EventInfo*> Callers;

    EventInfo()
    : IsEvent(true)
    , Parameters()
    , Name()
    , AstNode(nullptr)
    , Blocks()
    {}
};

struct FunctionInfo : EventInfo {
    runtime::ValueType ReturnType;
    std::size_t Called; // function calls

    FunctionInfo()
    : EventInfo()
    , ReturnType(runtime::ValueType::Void)
    , Called(0)
    {
        IsEvent = false;
    }
};


struct StateInfo {
    String Name;
    std::map<String, EventInfo> Events;
    Ast* AstNode;
    std::size_t Called; // state xyz calls

    StateInfo()
    : Name()
    , Events()
    , AstNode(nullptr)
    , Called(0)
    {}
};

struct SourceInfo {
    std::map<String, VarInfo> Globals;
    std::map<String, FunctionInfo> Functions;
    std::map<String, StateInfo> States;
};

enum class AnalyzerError {
    ParameterShadowingVariableDeclaration, // Warning?
    CanOnlyAssignToVariable,
    UsageOfPreviouslyUndeclaredVariable,
    DuplicatedStateDeclaration,
    DuplicatedGlobalDeclaration,
    DuplicatedFunctionDeclaration,
    UsageOfUndeclaredState,
    CanNotReturnValueFromVoidFunction,
    CanNotReturnValueFromEvent,
    NotEnoughParametersForCall,
    TooManyParametersForCall,
    ParameterDoesNotMatchForCall,
    ExpressionExpected,
    InvalidOperands,
};

struct AnalyzerErrorInfo {
    Ast*            AstNode;
    EventInfo *     Within;
    AnalyzerError   Error;
};

class Analyzer {
protected:    
    lsl::SourceInfo source_info_;
    lsl::Script & script_;
    std::vector<AnalyzerErrorInfo> errors_;
public:
    Analyzer(lsl::Script & script);
    virtual ~Analyzer();

    void Perform();

    lsl::Script & Script();
    SourceInfo & Info();
    std::vector<AnalyzerErrorInfo> & Errors() {
        return errors_;
    }


    typedef std::unordered_map<std::string, LibInfo> LibFunMap;
    LibFunMap & Lib() {
        static LibFunMap m;
        return m;
    }
};

}

#endif //GUARD_LSL_COMPILER_ANALYZER_HH_INCLUDED

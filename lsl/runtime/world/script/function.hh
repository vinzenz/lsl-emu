#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_FUNCTION_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_FUNCTION_HH_INCLUDED

#include <memory>
#include <lsl/runtime/world/script_fwd.hh>
#include <lsl/runtime/world/script/value.hh>
#include <lsl/runtime/world/script/call.hh>
#include <lsl/runtime/world/script/eval.hh>
#include <lsl/ast/ast.hh>

namespace lsl {
namespace runtime {
namespace script {

struct ScriptFunction {
    ScriptRef script;
    lsl::Function ast;
    CompiledScriptCall compiled;

    ScriptFunction(ScriptRef const & script,
                   lsl::Function const & f)
    : script(script)
    , ast(f)
    {}

    bool returns() const {
        return !ast.returnType.empty();
    }

    void call(ScriptContext const & context, ScriptFunctionCall const & call) {
        call_returns(context, call);
    }

    ScriptValue call_returns(ScriptContext const & context,
                             ScriptFunctionCall const & call) {
        if(!compiled) {
            compiled = eval_call(script, ast, call);
        }
        return compiled(script, context, call).get();
    }
};

typedef std::shared_ptr<ScriptFunction> ScriptFunctionPtr;

}}}

#endif //GUARD_LSL_RUNTIME_WORLD_SCRIPT_FUNCTION_HH_INCLUDED

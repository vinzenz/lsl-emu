#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_FUNCTION_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_FUNCTION_HH_INCLUDED

#include <memory>
#include <lsl/runtime/world/script_fwd.hh>
#include <lsl/runtime/world/script/value.hh>
#include <lsl/runtime/world/script/eval.hh>
#include <lsl/ast/ast.hh>

namespace lsl {
namespace runtime {
namespace script {

struct ScriptFunction {
    CompiledScriptFunction compiled;

    ScriptFunction(ScriptRef script, lsl::Function const & f)
    : compiled(eval_function(script, f))
    {}

    ScriptFunction(CompiledScriptFunction const & compiled)
    : compiled(compiled)
    {}

    ValueType returns() const {
        return compiled.result_type;
    }

    CallResult call(ScriptFunctionCall const & call) {
        return compiled.function(call);
    }
};

typedef std::shared_ptr<ScriptFunction> ScriptFunctionPtr;

}}}

#endif //GUARD_LSL_RUNTIME_WORLD_SCRIPT_FUNCTION_HH_INCLUDED

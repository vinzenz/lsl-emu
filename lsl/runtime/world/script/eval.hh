#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_EVAL_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_EVAL_HH_INCLUDED

#include <boost/optional.hpp>
#include <lsl/runtime/world/script/value.hh>
#include <lsl/runtime/world/script_fwd.hh>
#include <lsl/ast/ast.hh>
#include <lsl/runtime/world/script/call.hh>

namespace lsl {
namespace runtime {
namespace script {

typedef boost::optional<ScriptValue> CallResult;
typedef std::function<CallResult(ScriptRef script, ScriptContext const & context, ScriptFunctionCall const & call)> CompiledScriptCall;

CompiledScriptCall eval_call(ScriptRef script, lsl::Function const & ast, ScriptFunctionCall const & call);
}}}

#endif //GUARD_LSL_RUNTIME_WORLD_SCRIPT_EVAL_HH_INCLUDED

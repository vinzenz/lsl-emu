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

struct CompiledScriptFunction {
    ValueType result_type;
    std::vector<ValueType> parameters;
    std::function<CallResult(ScriptFunctionCall const &)> function;
};


CompiledScriptFunction eval_function(ScriptRef script, lsl::Function const & ast);
}}}

#endif //GUARD_LSL_RUNTIME_WORLD_SCRIPT_EVAL_HH_INCLUDED

#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_CALL_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_CALL_HH_INCLUDED

#include <lsl/runtime/world/script/value.hh>
#include <lsl/runtime/world/script/context.hh>

namespace lsl {
namespace runtime {
namespace script {

struct ScriptFunctionCall {
    std::shared_ptr<ScriptContext>  context;
    std::vector<ScriptValue>        arguments;
};

}}}

#endif //GUARD_LSL_RUNTIME_WORLD_SCRIPT_CALL_HH_INCLUDED

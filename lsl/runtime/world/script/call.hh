#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_CALL_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_CALL_HH_INCLUDED

#include <lsl/runtime/world/script_fwd.hh>
#include <lsl/runtime/world/script/value.hh>
#include <functional>

namespace lsl {
namespace runtime {
namespace script {

typedef boost::optional<ScriptValue> CallResult;

struct ScriptFunctionCall {
    ScriptRef                                                      caller;
    ScriptValue::list_type                                         arguments;
};

}}}

#endif //GUARD_LSL_RUNTIME_WORLD_SCRIPT_CALL_HH_INCLUDED

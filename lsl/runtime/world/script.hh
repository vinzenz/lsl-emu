#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_SCRIPT_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_SCRIPT_HH_INCLUDED

#include <boost/variant.hpp>
#include <lsl/runtime/world/script/value.hh>
#include <lsl/runtime/world/script/state.hh>
#include <lsl/runtime/world/script/function.hh>

namespace lsl {
namespace runtime {
namespace script {

struct Script {
    std::unordered_map<String, ScriptValue>         globals;
    std::unordered_map<String, ScriptFunctionPtr>   functions;
    std::unordered_map<String, StatePtr>            states;
    String                                          current_state;
    String                                          prim_key;

    Script(String const & prim_key)
    : globals()
    , functions()
    , states()
    , current_state("default")
    , prim_key(prim_key)
    {}

    StatePtr get_state() {
        return states[current_state];
    }
};

}}}

#endif //GUARD_LSL_RUNTIME_WORLD_SCRIPT_SCRIPT_HH_INCLUDED

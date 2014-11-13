#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_SCRIPT_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_SCRIPT_HH_INCLUDED

#include <boost/variant.hpp>
#include <lsl/runtime/world/script/value.hh>
#include <lsl/runtime/world/script/state.hh>
#include <lsl/runtime/world/script/function.hh>

namespace lsl {
namespace runtime {
namespace script {

struct Library {
    std::unordered_map<String, ScriptFunctionPtr>   functions;
};

inline Library & get_script_library() {
    static Library library;
    return library;
}

struct Script {
    std::unordered_map<String, ScriptValue>         globals;
    std::unordered_map<String, ScriptValue>         globals_clean;
    std::unordered_map<String, ScriptFunctionPtr>   functions;
    std::unordered_map<String, StatePtr>            states;
    String                                          current_state;
    String                                          prim_key;
    Library                                         library;

    Script(String const & prim_key)
    : globals()
    , functions()
    , states()
    , current_state("default")
    , prim_key(prim_key)
    , library(get_script_library())
    {

    }

    void dispatch_event(String const & event, ScriptValue::list_type const & arguments) {
        try {
            get_state()->dispatch_event(event, arguments);
        }
        catch(StateChangeException const & e) {
            if(e.state() == current_state) {
                return;
            }
            try {
                get_state()->dispatch_event("state_exit", {});
            }
            catch(StateChangeException const & e) {
                current_state = e.state();
            }
            current_state = e.state();
            return dispatch_event("state_entry", {});
        }
    }

    StatePtr get_state() {
        return states[current_state];
    }

    void reset() {
        globals = globals_clean;
        current_state = "default";
    }
};

}}}

#endif //GUARD_LSL_RUNTIME_WORLD_SCRIPT_SCRIPT_HH_INCLUDED

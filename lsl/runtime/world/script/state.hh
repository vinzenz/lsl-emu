// Copyright 2014 Vinzenz Feenstra
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_STATE_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_STATE_HH_INCLUDED

#include <unordered_map>
#include <lsl/runtime/world/script/call.hh>
#include <lsl/runtime/world/script_fwd.hh>
#include <lsl/runtime/world/script/eval.hh>
#include <tuple>


namespace lsl {
namespace runtime {
namespace script {

typedef std::tuple<ValueType, std::vector<ValueType>> Signature;

template< typename... Args>
Signature extract_signature(ValueType returns, Args... args) {
    return Signature{
        returns, {args...}
    };
}

template< typename... Args>
inline CompiledScriptFunction make_event(String name, Args... args) {
    auto sig = extract_signature(ValueType::Void, args...);
    return {
        std::get<0>(sig),
        std::get<1>(sig),
        [name](ScriptFunctionCall const &) -> CallResult {
            printf("Non defined event '%s' called\n", name.c_str());
            return CallResult();
        }
    };
}


std::unordered_map<String, CompiledScriptFunction> & get_event_repository();

struct State {

    String name;
    ScriptRef script;
    std::unordered_map<String, CompiledScriptFunction> events;

    State(String const & name, ScriptRef s)
    : name(name)
    , script(s)
    , events()
    {}

    void dispatch_event(String const & event, ScriptValue::list_type const & arguments) {
        if(events.count(event)) {
            events[event].function({script, arguments});
        }
        else {
            get_event_repository()[event].function({script, arguments});
        }
    }

    // attach( key id )
    // control( key id, integer level, integer edge )

    // experience_permissions( key agent_id )
    // experience_permissions_denied( key agent_id, integer reason )

    // http_request( key request_id, string method, string body )
    // http_response( key request_id, integer status, list metadata, string body )

    // land_collision( vector pos )
    // land_collision_end(vector pos)
    // land_collision_start(vector pos)

    // collision( integer num_detected )
    // collision_end(integer total_number)
    // collision_start(integer total_number)

    // at_rot_target(integer handle, rotation target_rot, rotation our_rot);
    // at_target( integer target_num, vector target_pos, vector our_pos )
    // not_at_rot_target( )
    // not_at_target( )

    // moving_end( )
    // moving_start( )

    // sensor(integer num_detected)
    // no_sensor( )

    // object_rez(key id)
    // on_rez( integer start_param )
    // path_update( integer type, list reserved )
    // remote_data( integer event_type, key channel, key message_id, string sender, integer data, string data )
    // run_time_permissions( integer perm )

    // money( key id, integer amount )
    // transaction_result( key id, integer success, string data )

    // touch( integer num_detected )
    // touch_end( integer num_detected )
    // touch_start( integer num_detected )

    // timer( )

    // link_message( integer sender_num, integer num, string str, key id )
    // listen( integer channel, string name, key id, string message )

    // changed( integer change )
    // dataserver( key query_id, string data )
    // email( string time, string address, string subject, string message, integer num_left )

    // state_entry( )
    // state_exit( )
};
typedef std::shared_ptr<State> StatePtr;

}}}

#endif //GUARD_LSL_RUNTIME_WORLD_SCRIPT_STATE_HH_INCLUDED

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
#include <lsl/runtime/world/script/state.hh>


namespace lsl {
namespace runtime {
namespace script {
std::unordered_map<String, CompiledScriptFunction> & get_event_repository() {
    static auto repository = std::unordered_map<String, CompiledScriptFunction>();
    if(repository.empty()) {
        repository["at_rot_target"]                 = make_event("at_rot_target", ValueType::Integer, ValueType::Rotation, ValueType::Rotation);
        repository["at_target"]                     = make_event("at_target", ValueType::Integer, ValueType::Vector, ValueType::Vector);
        repository["attach"]                        = make_event("attach", ValueType::Key);
        repository["changed"]                       = make_event("changed", ValueType::Integer);
        repository["collision"]                     = make_event("collision", ValueType::Integer);
        repository["collision_end"]                 = make_event("collision_end", ValueType::Integer);
        repository["collision_start"]               = make_event("collision_start", ValueType::Integer);
        repository["control"]                       = make_event("control", ValueType::Key, ValueType::Integer, ValueType::Integer);
        repository["dataserver"]                    = make_event("dataserver", ValueType::Key, ValueType::String);
        repository["email"]                         = make_event("email", ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::Integer);
        repository["experience_permissions"]        = make_event("experience_permissions", ValueType::Key);
        repository["experience_permissions_denied"] = make_event("experience_permissions_denied", ValueType::Key, ValueType::Integer);
        repository["http_request"]                  = make_event("http_request", ValueType::Key, ValueType::String, ValueType::String);
        repository["http_response"]                 = make_event("http_response", ValueType::Key, ValueType::Integer, ValueType::List, ValueType::String);
        repository["land_collission"]               = make_event("land_collission", ValueType::Vector);
        repository["land_collission_end"]           = make_event("land_collission_end", ValueType::Vector);
        repository["land_collission_start"]         = make_event("land_collission_start", ValueType::Vector);
        repository["link_message"]                  = make_event("link_message", ValueType::Integer, ValueType::Integer, ValueType::String, ValueType::Key);
        repository["listen"]                        = make_event("listen", ValueType::Integer, ValueType::String, ValueType::Key, ValueType::String);
        repository["money"]                         = make_event("money", ValueType::Key, ValueType::Integer);
        repository["moving_end"]                    = make_event("moving_end");
        repository["moving_start"]                  = make_event("moving_start");
        repository["no_sensor"]                     = make_event("no_sensor");
        repository["not_at_rot_target"]             = make_event("not_at_rot_target");
        repository["not_at_target"]                 = make_event("not_at_target");
        repository["object_rez"]                    = make_event("object_rez", ValueType::Key);
        repository["on_rez"]                        = make_event("on_rez", ValueType::Integer);
        repository["path_update"]                   = make_event("path_update", ValueType::Integer, ValueType::List);
        repository["remote_data"]                   = make_event("remote_data", ValueType::Integer, ValueType::Key, ValueType::Key, ValueType::String, ValueType::Integer, ValueType::String);
        repository["runtime_permissions"]           = make_event("runtime_permissions", ValueType::Integer);
        repository["sensor"]                        = make_event("sensor", ValueType::Integer);
        repository["state_entry"]                   = make_event("state_entry");
        repository["state_exit"]                    = make_event("state_exit");
        repository["timer"]                         = make_event("timer");
        repository["touch"]                         = make_event("touch", ValueType::Integer);
        repository["touch_end"]                     = make_event("touch_end", ValueType::Integer);
        repository["touch_start"]                   = make_event("touch_start", ValueType::Integer);
        repository["transaction_result"]            = make_event("transaction_result", ValueType::Key, ValueType::Integer, ValueType::String);
    }
    return repository;
}

}}}

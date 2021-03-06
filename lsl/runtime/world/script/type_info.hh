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
#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_TYPE_INFO_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_TYPE_INFO_HH_INCLUDED

#include <lsl/runtime/world/script/value.hh>

namespace lsl {
namespace runtime {
namespace script {

template< typename T >
struct ScriptTypeInfo;

template<>
struct ScriptTypeInfo<ScriptValue::string_type> {
    static char const * name() {
        return "string";
    }
};
template<>
struct ScriptTypeInfo<ScriptValue::vector_type> {
    static char const * name() {
        return "vector";
    }
};

template<>
struct ScriptTypeInfo<ScriptValue::rotation_type> {
    static char const * name() {
        return "rotation";
    }
};

template<>
struct ScriptTypeInfo<ScriptValue::list_type> {
    static char const * name() {
        return "string";
    }
};

template<>
struct ScriptTypeInfo<ScriptValue::float_type> {
    static char const * name() {
        return "float";
    }
};

template<>
struct ScriptTypeInfo<ScriptValue::integer_type> {
    static char const * name() {
        return "integer";
    }
};

}}}

#endif // GUARD_LSL_RUNTIME_WORLD_SCRIPT_TYPE_INFO_HH_INCLUDED

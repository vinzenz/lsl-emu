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
#ifndef GUARD_LSL_RUNTIME_WORLD_VALUE_CONVERT_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_VALUE_CONVERT_HH_INCLUDED

#include <lsl/runtime/world/script/error.hh>
#include <lsl/runtime/types.hh>

namespace lsl {
namespace runtime {
namespace script {

template< typename T >
struct converter {
    typedef T type;

    static T from(String const & );
    static T from(Key const & );
    static T from(Integer);
    static T from(Float);
    static T from(Vector);
    static T from(Rotation);
    static T from(List const & );
};

template class converter<Key>;
template class converter<String>;
template class converter<Integer>;
template class converter<Float>;
template class converter<Vector>;
template class converter<Rotation>;
template class converter<List>;

template< typename T, typename U>
T convert(T &, U const & b) {
    return converter<T>::from(b);
}


/*
    String     as_key() const;
    Integer    as_integer() const;
    Float      as_float() const;
    Vector     as_vector() const;
    Rotation   as_rotation() const;
    List       as_list() const;
*/


}}}

#endif // GUARD_LSL_RUNTIME_WORLD_SCRIPT_VALUE_CONVERT_HH_INCLUDED

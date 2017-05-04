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
#ifndef GUARD_LSL_RUNTIME_WORLD_SIMULATOR_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SIMULATOR_HH_INCLUDED

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <lsl/runtime/world/object.hh>

namespace lsl {
namespace runtime {

struct by_key{};
struct by_position{};
struct by_random{};

typedef boost::multi_index::multi_index_container<
    Prim,
    boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<by_key>,
            boost::multi_index::member<Prim, Key, &Prim::key>>,
        boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<by_position>,
            boost::multi_index::member<Prim, Vector, &Prim::position>>,
        boost::multi_index::random_access<
            boost::multi_index::tag<by_random>
        >
    >
> prim_container;

typedef std::unordered_map<
    Key, LinkSet
> objects_container;

struct Simulator {
    prim_container      prims;
    objects_container   objects;
    String              region_name;
    Vector              region_pos;
};

}}

#endif //GUARD_LSL_RUNTIME_WORLD_SIMULATOR_HH_INCLUDED

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
            boost::multi_index::member<Prim, String, &Prim::key>>,
        boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<by_position>,
            boost::multi_index::member<Prim, Vector, &Prim::position>>,
        boost::multi_index::random_access<
            boost::multi_index::tag<by_random>
        >
    >
> prim_container;

typedef std::unordered_map<
    String, LinkSet
> objects_container;

struct Simulator {
    prim_container      prims;
    objects_container   objects;
};

}}

#endif //GUARD_LSL_RUNTIME_WORLD_SIMULATOR_HH_INCLUDED

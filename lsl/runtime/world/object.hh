#ifndef GUARD_LSL_RUNTIME_WORLD_OBJECT_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_OBJECT_HH_INCLUDED

#include <lsl/runtime/world/prim.hh>

namespace lsl {
namespace runtime {

struct LinkSet {
    LinkSet(PrimRef self)
    : children()
    {
        children.push_back(self);
        self.get().root = self.get().key;
    }

    std::vector<PrimRef> children;
};

}}

#endif //GUARD_LSL_RUNTIME_WORLD_OBJECT_HH_INCLUDED

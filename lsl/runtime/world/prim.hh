#ifndef GUARD_LSL_RUNTIME_WORLD_PRIM_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_PRIM_HH_INCLUDED

namespace lsl {
namespace runtime {

struct Prim {
    String      key;
    Inventory   inventory;
    Quaternion  rotation;
    Vector      position;
    Vector      velocity;
};

}
}

#endif //GUARD_LSL_RUNTIME_WORLD_PRIM_HH_INCLUDED

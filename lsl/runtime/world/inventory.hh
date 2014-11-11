#ifndef GUARD_LSL_RUNTIME_WORLD_INVENTORY_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_INVENTORY_HH_INCLUDED


namespace lsl {
namespace runtime {

enum class InventoryType {
    None,
    Animation,
    Bodypart,
    Clothing,
    Gesture,
    Landmark,
    Notecard,
    Object,
    Script,
    Sound,
    Texture,
    All = Animation|Bodypart|Gesture|Landmark|Notecard|Object|Script|Sound|Texture,
};

struct Inventory {
    InventoryType type;
    
};

}
}

#endif //GUARD_LSL_RUNTIME_WORLD_INVENTORY_HH_INCLUDED
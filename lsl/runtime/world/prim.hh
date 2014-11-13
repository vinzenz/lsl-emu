#ifndef GUARD_LSL_RUNTIME_WORLD_PRIM_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_PRIM_HH_INCLUDED

#include <lsl/runtime/types.hh>
#include <boost/ref.hpp>

namespace lsl {
namespace runtime {

struct PrimFace {
    String  texture;
    Vector  repeats;
    Vector  offsets;
    Float   rotation;

    Vector  color;
    Integer shiny;
    Integer bump;
    Integer fullbright;
    Integer glow;
    Integer texgen;
};

struct PrimFlexible {
    Integer boolean;
    Integer softness;
    Float   gravity;
    Float   wind;
    Float   friction;
    Float   tension;
    Vector  force;
};

enum PrimType {
    PRIM_TYPE_BOX         = 0,
    PRIM_TYPE_CYLINDER    = 1,
    PRIM_TYPE_PRISM       = 2,
    PRIM_TYPE_SPHERE      = 3,
    PRIM_TYPE_TORUS       = 4,
    PRIM_TYPE_TUBE        = 5,
    PRIM_TYPE_RING        = 6,
    PRIM_TYPE_SCULPT      = 7
};

struct PrimBasic {
    Integer hole_shape;
    Vector  cut;
    Float   hollow;
    Vector  twist;
};

struct PrimTypeA : PrimBasic {
    Vector  top_size;
    Vector  top_shear;
};

struct PrimTypeB : PrimBasic {
    Vector dimple;
};

struct PrimTypeC : PrimBasic {
    Vector hole_size;
    Vector top_size;
    Vector top_shear;
    Vector advanced_cut;
    Vector taper;
    Float  revolutions;
    Float  radius;
    Float  radius_offset;
    Float  skew;
};

struct PrimSculpt {
    String  map;
    Integer type;
};

struct PrimShape {
    Integer type;
    union {
        PrimTypeA box;
        PrimTypeA cylinder;
        PrimTypeA prism;
        PrimTypeB sphere;
        PrimTypeC torus;
        PrimTypeC tube;
        PrimTypeC ring;
    } values;
    PrimSculpt  sculpt;
};

struct Prim {
    String      key;
//    Inventory   inventory;
    Quaternion  rotation;
    Vector      position;
    Vector      velocity;
    String      root;

    String      name;
    String      description;

    String      owner;
    String      group;
    String      creator;

    Integer     equivalence;
    Integer     flags; // PHYSICS, PHANTOM, TEMP_ON_REZ

    Vector      slice;
    Integer     physical_shape_type;
    Integer     material;
    Vector      position_local;
    Rotation    rotation_local;
    Vector      size;
    std::vector<PrimFace> textures;

    Vector      omega_axis;
    Float       omega_spin_rate;
    Float       omega_gain;

    bool operator < (Prim const & p) const {
        return key < p.key;
    }
};
typedef boost::reference_wrapper<Prim> PrimRef;

}
}

#endif //GUARD_LSL_RUNTIME_WORLD_PRIM_HH_INCLUDED

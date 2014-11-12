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

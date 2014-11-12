#ifndef GUARD_LSL_RUNTIME_WORLD_VALUE_CONVERT_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_VALUE_CONVERT_HH_INCLUDED

#include <lsl/runtime/world/script/error.hh>

namespace lsl {
namespace runtime {
namespace script {

template< typename T, typename U>
T convert(T & a, U const & b) {
    throw ScriptConversionError(a, b);
    return T();
}

}}}

#endif // GUARD_LSL_RUNTIME_WORLD_SCRIPT_VALUE_CONVERT_HH_INCLUDED

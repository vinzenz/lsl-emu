#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_FWD_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_FWD_HH_INCLUDED

#include <boost/ref.hpp>

namespace lsl {
namespace runtime {
namespace script {

struct Script;
typedef boost::reference_wrapper<Script> ScriptRef;

}}}

#endif //GUARD_LSL_RUNTIME_WORLD_SCRIPT_FWD_HH_INCLUDED

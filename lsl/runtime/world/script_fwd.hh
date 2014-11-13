#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_FWD_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_FWD_HH_INCLUDED

#include <memory>
#include <boost/ref.hpp>

namespace lsl {
namespace runtime {
namespace script {

struct Script;
typedef boost::reference_wrapper<Script> ScriptRef;
typedef std::shared_ptr<Script> ScriptPtr;

}}}

#endif //GUARD_LSL_RUNTIME_WORLD_SCRIPT_FWD_HH_INCLUDED

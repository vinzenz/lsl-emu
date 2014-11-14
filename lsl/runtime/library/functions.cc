#include <lsl/runtime/library/functions.hh>
#include <cmath>
#include <random>

namespace lsl {
namespace runtime {
namespace lib {


Float llFrand(ScriptRef, Float upper)
{
    std::default_random_engine e;
    std::uniform_real_distribution<Float> unif(Float(0.),upper);
    return unif(e);
}

}}}

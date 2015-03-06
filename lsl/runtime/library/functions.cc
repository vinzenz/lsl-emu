#include <lsl/runtime/library/functions.hh>
#include <cmath>
#include <random>

namespace lsl {
namespace runtime {
namespace lib {


Float llFrand(ScriptRef, Float upper)
{
    std::random_device e;
    std::uniform_real_distribution<Float> unif(Float(0.),upper);
    return unif(e);
}

}}}

#ifndef GUARD_LSL_RUNTIME_TYPES_HH_INCLUDED
#define GUARD_LSL_RUNTIME_TYPES_HH_INCLUDED

#include <lsl/runtime/vector.hh>
#include <lsl/runtime/quaternion.hh>
#include <sstream>

namespace lsl {
namespace runtime {

typedef std::string String;


inline String to_string(float val) {
    std::ostringstream ostr;
    ostr << std::setiosflags(std::ios::fixed) << std::setprecision(6) << val;
    return ostr.str();

}

inline String to_string(Vector val) {
    std::ostringstream ostr;    
    ostr << '<' << std::setiosflags(std::ios::fixed) << std::setprecision(5)
         << val.x
         << ", "
         << val.y
         << ", "
         << val.z
         << '>';
    return ostr.str();
}

inline String to_string(Quaternion val) {
    std::ostringstream ostr;
    ostr << '<' << std::setiosflags(std::ios::fixed) << std::setprecision(5)
         << val.x
         << ", "
         << val.y
         << ", "
         << val.z
         << ", "
         << val.s
         << '>';
    return ostr.str();
}

enum class ValueType {
    Void,
    String,
    Key,
    Float,
    Integer,
    Vector,
    Rotation
};

struct Variable {
    ValueType  type;
    String     name;
};

struct Value {
    Variable signature;    
    String      s;
    String      k;
    float       f;
    int64_t     i;
    Vector      v;
    Quaternion  r;
};

struct Scope {    
};

struct Function {
    std::string           name;
    ValueType             returns;
    std::vector<Variable> signature;
    Scope                 body;
};

}}

#endif //GUARD_LSL_RUNTIME_TYPES_HH_INCLUDED
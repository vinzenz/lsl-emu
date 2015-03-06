#ifndef GUARD_LSL_RUNTIME_TYPES_HH_INCLUDED
#define GUARD_LSL_RUNTIME_TYPES_HH_INCLUDED

#include <lsl/runtime/vector.hh>
#include <lsl/runtime/quaternion.hh>
#include <boost/variant.hpp>
#include <boost/ref.hpp>
#include <sstream>

namespace lsl {
namespace runtime {

typedef std::string String;
typedef int32_t     Integer;
typedef double      Float;
typedef String      Key;
typedef Quaternion  Rotation;



inline String to_string(Integer val) {
    std::ostringstream ostr;
    ostr << val;
    return ostr.str();

}

inline String to_string(Float val) {
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

enum class ValueType : lsl::runtime::Integer {
    Void        = 0,
    Integer     = 1,
    Float       = 2,
    String      = 3,
    Key         = 4,
    Vector      = 5,
    Rotation    = 6,
    List        = 7
};

struct ScriptValue {
    typedef String                  string_type;
    typedef Integer                 integer_type;
    typedef Float                   float_type;
    typedef Vector                  vector_type;
    typedef Quaternion              rotation_type;

    typedef boost::make_recursive_variant<
        string_type,
        rotation_type,
        vector_type,
        integer_type,
        float_type,
        boost::reference_wrapper<ScriptValue>,// References to Variables global or local
        boost::reference_wrapper<float_type>, // Members of Vector or Quaternion
        std::vector<ScriptValue>
    >::type value_type;

    typedef std::vector<ScriptValue> list_type;

    ValueType   type;
    value_type  value;
    bool        reference;

    string_type     as_key() const;
    string_type     as_string() const;
    integer_type    as_integer() const;
    float_type      as_float() const;
    vector_type     as_vector() const;
    rotation_type   as_rotation() const;
    list_type       as_list() const;

    rotation_type & get_rotation();
    vector_type & get_vector();
    string_type & get_string();
    integer_type & get_integer();
    float_type & get_float();
    list_type & get_list();
    boost::reference_wrapper<float_type> & get_member_ref();
    boost::reference_wrapper<ScriptValue> & get_ref();
};
bool operator==(ScriptValue const & left, ScriptValue const & right);
// bool operator<(ScriptValue const & left, ScriptValue const & right);

typedef ScriptValue::list_type List;

}}

#endif //GUARD_LSL_RUNTIME_TYPES_HH_INCLUDED

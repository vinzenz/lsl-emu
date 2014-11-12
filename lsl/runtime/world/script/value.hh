#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_VALUE_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_VALUE_HH_INCLUDED

#include <boost/variant.hpp>
#include <boost/ref.hpp>
#include <lsl/runtime/types.hh>

namespace lsl {
namespace runtime {
namespace script {


struct ScriptValue {
    typedef String                  string_type;
    typedef int32_t                 integer_type;
    typedef double                  float_type;
    typedef Vector                  vector_type;
    typedef Quaternion              rotation_type;

    typedef boost::make_recursive_variant<
        String,
        Quaternion,
        Vector,
        integer_type,
        float_type,
        boost::reference_wrapper<ScriptValue>,// References to Variables global or local
        boost::reference_wrapper<float_type>, // Members of Vector or Quaternion
        std::vector<boost::recursive_variant_>>::type value_type;

    typedef std::vector<value_type> list_type;

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
};

}}}

#endif // GUARD_LSL_RUNTIME_WORLD_SCRIPT_VALUE_HH_INCLUDED

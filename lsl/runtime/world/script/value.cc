#include <lsl/runtime/world/script/value.hh>
#include <lsl/runtime/world/script/value_convert.hh>

namespace lsl {
namespace runtime {
namespace script {

template< typename ResultType >
struct value_visitor : boost::static_visitor<ResultType> {
    // Variables
    ResultType operator()(boost::reference_wrapper<ScriptValue> const & v) const {
        return boost::apply_visitor(*this, v.get().value);
    }

    // Members of Vector or Quaternion
    ResultType operator()(boost::reference_wrapper<ScriptValue::float_type> const & v) const {
        return (*this)(v.get());
    }

    ResultType operator()(ResultType const & v) const {
        return v;
    }

    template< typename T >
    typename std::enable_if<!boost::is_reference_wrapper<T>::value, ResultType>::type
    operator()(T const & v) const {
        ResultType r;
        return convert(r, v);
    }
};

ScriptValue::string_type ScriptValue::as_key() const {
    return boost::apply_visitor(value_visitor<string_type>(), value);
}

ScriptValue::string_type ScriptValue::as_string() const {
    return boost::apply_visitor(value_visitor<string_type>(), value);
}

ScriptValue::integer_type ScriptValue::as_integer() const {
    return boost::apply_visitor(value_visitor<integer_type>(), value);
}

ScriptValue::float_type ScriptValue::as_float() const {
    return boost::apply_visitor(value_visitor<float_type>(), value);
}

ScriptValue::vector_type ScriptValue::as_vector() const {
    return boost::apply_visitor(value_visitor<vector_type>(), value);
}

ScriptValue::rotation_type ScriptValue::as_rotation() const {
    return boost::apply_visitor(value_visitor<rotation_type>(), value);
}

ScriptValue::list_type ScriptValue::as_list() const {
    return boost::apply_visitor(value_visitor<list_type>(), value);
}

ScriptValue::rotation_type & ScriptValue::get_rotation() {
    return boost::get<ScriptValue::rotation_type>(value);
}

ScriptValue::vector_type & ScriptValue::get_vector() {
    return boost::get<ScriptValue::vector_type>(value);
}

ScriptValue::string_type & ScriptValue::get_string() {
    return boost::get<ScriptValue::string_type>(value);
}

ScriptValue::integer_type & ScriptValue::get_integer() {
    return boost::get<ScriptValue::integer_type>(value);
}

ScriptValue::float_type & ScriptValue::get_float() {
    return boost::get<ScriptValue::float_type>(value);
}

ScriptValue::list_type & ScriptValue::get_list() {
    return boost::get<ScriptValue::list_type>(value);
}

}}}

#include <lsl/runtime/world/script/value.hh>
#include <lsl/runtime/world/script/value_convert.hh>

namespace lsl {
namespace runtime {

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


template< typename ResultType >
struct reference_visitor : boost::static_visitor<ResultType&> {
    // Variables
    ResultType& operator()(boost::reference_wrapper<ScriptValue> & v) const {
        return boost::apply_visitor(*this, v.get().value);
    }

    // Members of Vector or Quaternion
    template< typename T >
    typename std::enable_if<std::is_same<ResultType, T>::value, ResultType>::type &
    operator()(boost::reference_wrapper<T> & v) const {
        return v.get();
    }

    template< typename T >
    typename std::enable_if<!std::is_same<ResultType, T>::value, ResultType>::type &
    operator()(boost::reference_wrapper<T> &) const {
        static ResultType t;
        throw ScriptRuntimeError("Invalid instruction");
        return t;
    }

    ResultType& operator()(ResultType & v) const {
        return v;
    }

    template< typename T >
    typename std::enable_if<!boost::is_reference_wrapper<T>::value, ResultType>::type &
    operator()(T &) const {
        throw ScriptRuntimeError("Invalid instruction");
        static ResultType t;
        return t;
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
    return boost::apply_visitor(reference_visitor<ScriptValue::rotation_type>(), value);
}

ScriptValue::vector_type & ScriptValue::get_vector() {
    return boost::apply_visitor(reference_visitor<ScriptValue::vector_type>(), value);
}

ScriptValue::string_type & ScriptValue::get_string() {
    return boost::apply_visitor(reference_visitor<ScriptValue::string_type>(), value);
}

ScriptValue::integer_type & ScriptValue::get_integer() {
    return boost::apply_visitor(reference_visitor<ScriptValue::integer_type>(), value);
}

ScriptValue::float_type & ScriptValue::get_float() {
    return boost::apply_visitor(reference_visitor<ScriptValue::float_type>(), value);
}

ScriptValue::list_type & ScriptValue::get_list() {
    return boost::apply_visitor(reference_visitor<ScriptValue::list_type>(), value);
}

boost::reference_wrapper<ScriptValue::float_type> & ScriptValue::get_member_ref() {
    return boost::get<boost::reference_wrapper<ScriptValue::float_type>>(value);
}

boost::reference_wrapper<ScriptValue> & ScriptValue::get_ref() {
    return boost::get<boost::reference_wrapper<ScriptValue>>(value);
}

}}

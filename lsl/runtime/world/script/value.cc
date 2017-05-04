// Copyright 2014 Vinzenz Feenstra
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <lsl/runtime/world/script/value.hh>
#include <lsl/runtime/world/script/value_convert.hh>
#include <lsl/runtime/world/script/error.hh>

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
        return script::convert(r, v);
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
        throw script::ScriptRuntimeError("Invalid instruction");
        return t;
    }

    ResultType& operator()(ResultType & v) const {
        return v;
    }

    template< typename T >
    typename std::enable_if<!boost::is_reference_wrapper<T>::value, ResultType>::type &
    operator()(T &) const {
        throw script::ScriptRuntimeError("Invalid instruction");
        static ResultType t;
        return t;
    }
};
ScriptValue::key_type ScriptValue::as_key() const {
    return boost::apply_visitor(value_visitor<key_type>(), value);
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

ScriptValue::integer_type ScriptValue::as_bool() const {
    switch(type) {
    case ValueType::String:
        return as_string().empty() ? 0 : 1;
    case ValueType::Key:
        return as_key().empty() ? 0 : as_key() == "00000000-0000-0000-0000-000000000000" ? 0 : 1;
    case ValueType::Rotation:
        return as_rotation() != Quaternion{0., 0., 0., 1.};
    case ValueType::Vector:
        return !(as_vector() == Vector{0., 0., 0.});
    case ValueType::Integer:
    case ValueType::Float:
        return (as_integer() != 0 ? 1 : 0);
    case ValueType::List:
        return as_list().empty() ? 0 : 1;
    default:
        break;
    }
    return 0;
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


ScriptValue::key_type & ScriptValue::get_key() {
    return boost::apply_visitor(reference_visitor<ScriptValue::key_type>(), value);
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

struct comparison_visitor : boost::static_visitor < bool > {
    comparison_visitor(ScriptValue::value_type const & right)
        : right_(right)
    {}

    template< typename T >
    bool operator()(T const & left) const {
        return left == boost::get<T>(right_.get());
    }

    bool operator()(boost::reference_wrapper<ScriptValue::float_type> const & left) const {
        return left.get() == boost::get<boost::reference_wrapper<ScriptValue::float_type>>(right_.get()).get();
    }

    template< typename T >
    bool operator()(boost::reference_wrapper<T> const & left) const {
        return boost::apply_visitor(comparison_visitor(boost::get<boost::reference_wrapper<T>>(right_.get()).get().value), left.get().value);
    }

    boost::reference_wrapper<ScriptValue::value_type const> right_;
};

bool operator==(ScriptValue const & left, ScriptValue const & right) {
    return boost::apply_visitor(comparison_visitor(right.value), left.value);
}

struct less_visitor : boost::static_visitor < bool > {
    less_visitor(ScriptValue::value_type const & right)
        : right_(right)
    {}

    template< typename T >
    bool operator()(T const & left) const {
        return left < boost::get<T>(right_.get());
    }

    bool operator()(boost::reference_wrapper<ScriptValue::float_type> const & left) const {
        return left.get() < boost::get<boost::reference_wrapper<ScriptValue::float_type>>(right_.get()).get();
    }

    template< typename T >
    bool operator()(boost::reference_wrapper<T> const & left) const {
        return boost::apply_visitor(less_visitor(boost::get<boost::reference_wrapper<T>>(right_.get()).get().value), left.get().value);
    }

    boost::reference_wrapper<ScriptValue::value_type const> right_;
};

bool operator<(ScriptValue const & left, ScriptValue const & right) {
    if(left.type != right.type) {
        return left.as_string() < right.as_string();
    }
    return boost::apply_visitor(less_visitor(right.value), left.value);
}

}}

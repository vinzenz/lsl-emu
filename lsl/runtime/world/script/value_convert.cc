#include <lsl/runtime/world/script/value_convert.hh>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/regex.hpp>

namespace lsl {
namespace runtime {
namespace script {

template<>
String converter<String>::from(String const & s) {
    return s;
}

template<>
String converter<String>::from(Integer i) {
    return to_string(i);
}

template<>
String converter<String>::from(List const & l) {
    String result;
    for(auto const & e: l) {
        result.append(e.as_string());
    }
    return result;
}

template<>
String converter<String>::from(Float i) {
    return to_string(i);
}

template<>
String converter<String>::from(Vector v) {
    return to_string(v);
}

template<>
String converter<String>::from(Rotation r) {
    return to_string(r);
}

///////////////////////////////////////////////////////////////////////

template<>
Integer converter<Integer>::from(String const & s) {
    static char const * INT_REGEX =
        "(0[xX][0-9a-fA-F]+)|((-|\\+)?[1-9][0-9]*)"
    ;
    static boost::regex cast_regex(INT_REGEX);
    boost::smatch results;
    if(!boost::regex_match(s, results, cast_regex) || results.empty()) {
        return 0;
    }
    return Integer(boost::multiprecision::cpp_int(results.str()));
}

template<>
Integer converter<Integer>::from(List const &) {
    throw ScriptConversionError(List(), Integer());
    return Integer();
}

template<>
Integer converter<Integer>::from(Integer i) {
    return i;
}

template<>
Integer converter<Integer>::from(Float f) {
    return Integer(f);
}

template<>
Integer converter<Integer>::from(Vector) {
    throw ScriptConversionError(Vector(), Integer());
    return Integer();
}

template<>
Integer converter<Integer>::from(Rotation) {
    throw ScriptConversionError(Rotation(), Integer());
    return Integer();
    }

///////////////////////////////////////////////////////////////////////

template<>
Float converter<Float>::from(String const & s) {
    // ^ *(\\+|-)?([0-9]+\\.?[0-9]*|\\.[0-9]+)([eE](\\+|-)?[0-9]+)?"
    static char const * FLT_REGEX =
        "(\\+|-)?([0-9]+\\.?[0-9]*|\\.[0-9]+)([eE](\\+|-)?[0-9]+)?"
    ;
    String inp = boost::trim_left_copy(s);
    static boost::regex cast_regex(FLT_REGEX);
    boost::smatch results;
    if(!boost::regex_match(inp, results, cast_regex) || results.empty()) {
        String tmp = inp.substr(0, 3);
        boost::algorithm::to_lower(tmp);
        if(tmp == "inf") {
            return INFINITY;
        }
        else if(tmp == "nan") {
            return NAN;
        }
        return 0;
    }
    return Float(boost::multiprecision::cpp_dec_float_50(results.str()));
}

template<>
Float converter<Float>::from(List const &) {
    throw ScriptConversionError(List(), Float());
    return Float();
}

template<>
Float converter<Float>::from(Integer i) {
    return Float(i);
}

template<>
Float converter<Float>::from(Float f) {
    return f;
}

template<>
Float converter<Float>::from(Vector) {
    throw ScriptConversionError(Float(), Vector());
    return Float();
}

template<>
Float converter<Float>::from(Rotation) {
    throw ScriptConversionError(Rotation(), Float());
    return Float();
}

///////////////////////////////////////////////////////////////////////

template<>
Vector converter<Vector>::from(String const & s) {
    String inp = boost::algorithm::trim_left_copy(s);
    std::vector<String> parts;
    boost::algorithm::split(parts, inp, boost::algorithm::is_any_of("<,>"));
    Vector result;
    if(parts.size() >= 3) {
        result.x = converter<Float>::from(parts[0]);
        result.y = converter<Float>::from(parts[1]);
        result.z = converter<Float>::from(parts[2]);
    }
    return result;
}

template<>
Vector converter<Vector>::from(List const &) {
    throw ScriptConversionError(List(), Vector());
    return Vector();
}

template<>
Vector converter<Vector>::from(Integer) {
    throw ScriptConversionError(Vector(), Integer());
    return Vector();
}

template<>
Vector converter<Vector>::from(Float) {
    throw ScriptConversionError(Float(), Vector());
    return Vector();
}

template<>
Vector converter<Vector>::from(Vector v) {
    return v;
}

template<>
Vector converter<Vector>::from(Rotation) {
    throw ScriptConversionError(Rotation(), Vector());
    return Vector();
}

///////////////////////////////////////////////////////////////////////

template<>
Rotation converter<Rotation>::from(String const & s) {
    String inp = boost::algorithm::trim_left_copy(s);
    std::vector<String> parts;
    boost::algorithm::split(parts, inp, boost::algorithm::is_any_of("<,>"));
    Rotation result = identity(Rotation());
    if(parts.size() >= 4) {
        result.x = converter<Float>::from(parts[0]);
        result.y = converter<Float>::from(parts[1]);
        result.z = converter<Float>::from(parts[2]);
        result.s = converter<Float>::from(parts[3]);
    }
    return result;
}

template<>
Rotation converter<Rotation>::from(List const &) {
    throw ScriptConversionError(List(), Rotation());
    return Rotation();
}

template<>
Rotation converter<Rotation>::from(Integer) {
    throw ScriptConversionError(Integer(), Rotation());
    return Rotation();
}

template<>
Rotation converter<Rotation>::from(Float) {
    throw ScriptConversionError(Float(), Rotation());
    return Rotation();
}

template<>
Rotation converter<Rotation>::from(Rotation v) {
    return v;
}

template<>
Rotation converter<Rotation>::from(Vector) {
    throw ScriptConversionError(Vector(), Rotation());
    return Rotation();
}

///////////////////////////////////////////////////////////////////////

template<>
List converter<List>::from(String const & s) {
    (void)s;
#if 0
    String inp = boost::algorithm::trim_left_copy(s);
    std::vector<String> parts;
    boost::algorithm::split(parts, inp, boost::algorithm::is_any_of("[]"));
    if(inp.size() >= 2) {
        List result;
        inp = parts.front();
        parts.clear();
    }
    return List();
#endif
    throw ScriptRuntimeError("Conversion from string to list are not implemented yet.");
    return List();
}

template<>
List converter<List>::from(List const & l) {
    return l;
}

template<>
List converter<List>::from(Integer i) {
    List l;
    ScriptValue v;
    v.type = ValueType::Integer;
    v.reference = false;
    v.value = i;
    l.push_back(v);
    return l;
}

template<>
List converter<List>::from(Float f) {
    List l;
    ScriptValue v;
    v.type = ValueType::Float;
    v.reference = false;
    v.value = f;
    l.push_back(v);
    return l;
}

template<>
List converter<List>::from(Vector v) {
    List l;
    ScriptValue sv;
    sv.type = ValueType::Vector;
    sv.reference = false;
    sv.value = v;
    l.push_back(sv);
    return l;
}

template<>
List converter<List>::from(Rotation r) {
    List l;
    ScriptValue v;
    v.type = ValueType::Rotation;
    v.reference = false;
    v.value = r;
    l.push_back(v);
    return l;
}

}}}

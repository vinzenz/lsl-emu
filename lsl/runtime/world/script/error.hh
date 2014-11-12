#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_ERROR_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_ERROR_HH_INCLUDED

#include <stdexcept>
#include <lsl/runtime/world/script/type_info.hh>
#include <lsl/utils.hh>

namespace lsl {
namespace runtime {
namespace script {

class ScriptError : public std::runtime_error {
public:
    ScriptError(std::string const & message, int line = -1, int column = -1)
    : std::runtime_error(message)
    , line_(line)
    , column_(column)
    {}

    int line() const {
        return line_;
    }

    void line(int l) {
        line_ = l;
    }

    int column() const {
        return column_;
    }

    void column(int c) {
        column_ = c;
    }
protected:
    int         line_;
    int         column_;
};



class ScriptConversionError : public ScriptError {
public:
    template< typename T, typename U >
    ScriptConversionError(T const &, U const &, int line = -1, int col = -1)
    : ScriptError(format("Cannot convert '%s' to type '%s'", ScriptTypeInfo<T>::name(), ScriptTypeInfo<U>::name()), line, col)
    {}
};

class ScriptRuntimeError : public ScriptError {
public:
    ScriptRuntimeError(std::string const & message, int line = -1, int col = -1)
    : ScriptError(message, line, col)
    {}
};

}}}

#endif // GUARD_LSL_RUNTIME_WORLD_SCRIPT_ERROR_HH_INCLUDED

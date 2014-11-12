#ifndef GUARD_LSL_UTILS_HH_INCLUDED
#define GUARD_LSL_UTILS_HH_INCLUDED

#include <string.h>
#include <lsl/runtime/types.hh>

namespace lsl {
    template< typename... Args >
    std::string format(char const * fmt, Args... args) {
        char buffer[1024] = {0};
        sprintf(buffer, fmt, args...);
        return buffer;
    }
}

#endif //GUARD_LSL_UTILS_HH_INCLUDED

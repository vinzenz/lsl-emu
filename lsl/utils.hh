#ifndef GUARD_LSL_UTILS_HH_INCLUDED
#define GUARD_LSL_UTILS_HH_INCLUDED

#include <string.h>
#include <lsl/runtime/types.hh>

namespace lsl {
    template< typename... Args >
    std::string format(char const * fmt, Args... args) {
        char buffer[1024] = {0};
#if defined(WIN32)
        sprintf_s(buffer, 1024, fmt, args...);        
#else
        sprintf(buffer, fmt, args...);
#endif
        return buffer;
    }
}

#endif //GUARD_LSL_UTILS_HH_INCLUDED

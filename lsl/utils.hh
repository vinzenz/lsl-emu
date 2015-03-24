#ifndef GUARD_LSL_UTILS_HH_INCLUDED
#define GUARD_LSL_UTILS_HH_INCLUDED

#include <cstdint>
#include <array>
#include <string.h>
#include <lsl/runtime/types.hh>
#include <lsl/md5.hh>
#include <iterator>

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

    template< typename ForwardIterT >
    std::array<std::uint8_t, 16> to_md5(ForwardIterT start, ForwardIterT end) {
        std::array<std::uint8_t, 16> result;
        MD5_CTX ctx;
        MD5Init(&ctx);
        for(;start != end; ++start) {
            auto element = static_cast<typename std::iterator_traits<ForwardIterT>::value_type>(*start);
            MD5Update(&ctx, reinterpret_cast<uint8_t*>(&element), static_cast<unsigned>(sizeof(element)));
        }
#if defined(_MSC_VER)
        MD5Final(result._Elems, &ctx);
#else
        MD5Final(result._M_elems, &ctx);
#endif
        return result;
    }

}

#endif //GUARD_LSL_UTILS_HH_INCLUDED

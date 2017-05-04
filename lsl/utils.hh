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
        MD5Final(result.data(), &ctx);
#endif
        return result;
    }

}

#endif //GUARD_LSL_UTILS_HH_INCLUDED

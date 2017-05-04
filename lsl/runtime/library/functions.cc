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
#include <lsl/runtime/library/functions.hh>
#include <cmath>
#include <random>

namespace lsl {
namespace runtime {
namespace lib {


Float llFrand(ScriptRef, Float upper)
{
    std::random_device e;
    std::uniform_real_distribution<Float> unif(Float(0.),upper);
    return unif(e);
}

inline static bool is_reserved(char c)
{
    switch (c) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case 'a': case 'b': case 'c': case 'd': case 'e':
        case 'f': case 'g': case 'h': case 'i': case 'j':
        case 'k': case 'l': case 'm': case 'n': case 'o':
        case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
        case 'A': case 'B': case 'C': case 'D': case 'E':
        case 'F': case 'G': case 'H': case 'I': case 'J':
        case 'K': case 'L': case 'M': case 'N': case 'O':
        case 'P': case 'Q': case 'R': case 'S': case 'T':
        case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
        case '-': case '.': case '_': case '~':
        return false;
        default:
    break;

    }
    return true;
}

String llEscapeURL(ScriptRef, String s) {
    static char const LOOKUP[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    String r;
    r.reserve(s.size() * 3);// Worst case
    for(char c : s) {
        if(is_reserved(c)) {
            r.push_back('%');
            r.push_back(LOOKUP[c >> 4]);
            r.push_back(LOOKUP[c & 0x0F]);
        } else {
            r.push_back(c);
        }
    }
    return r;
}

inline char to_nibble(char c) {
    c = tolower(c);
    if(c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }
    return c - '0';
}

String llUnescapeURL(ScriptRef, String s) {
    String r;
    r.reserve(s.size());
    for(size_t i = 0; i < s.size(); ++i) {
        switch(s[i]) {
        case '%':
            if(i + 2 < s.size() - 1) {
                if(isxdigit(s[i + 1]) && isxdigit(s[i + 2])) {
                    r.push_back(to_nibble(s[i + 1]) << 4 | to_nibble(s[i + 2]));
                    i += 2;
                    break;
                }
            }
        default:
            r.push_back(s[i]);
        }
    }
    return r;
}

}}}

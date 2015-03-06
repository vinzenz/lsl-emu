#include <lsl/runtime/library/functions.hh>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/uuid/sha1.hpp>
#include <lsl/utils.hh>
#include <cmath>
#include <random>

namespace lsl {
namespace runtime {
namespace lib {

bool CorrectIndex(size_t max, Integer & s, Integer & e) {
    if(max == 0) {
        return false;
    }
    Integer imax = static_cast<std::make_signed<size_t>::type>(max);
    if(s < 0) {
        s = imax + s;
    }
    if(e < 0) {
        e = imax + e;
    }
    if(s < 0 || s >= imax) {
        return false;
    }
    if(e < 0) {
        return false;
    }
    if(e >= imax) {
        e = imax;
    }
    if(s > e) {
        s = s ^ e;
        e = s ^ e;
        s = s ^ e;
    }
    return true;
}

String llGetSubString(ScriptRef, String str, Integer s, Integer e) {
    if(!CorrectIndex(str.size(), s, e)) {
        return String();
    }
    return str.substr(s, (e - s + 1));
}

String llDeleteSubString(ScriptRef, String str, Integer s, Integer e) {
    if(!CorrectIndex(str.size(), s, e)) {
        return String();
    }
    return str.erase(s, (e - s + 1));
}

String llInsertString(ScriptRef, String str, Integer p, String s) {
    if(p < 0) {
        p = 0;
    }
    if(p >= Integer(str.size())) {
        str.insert(str.end(), s.begin(), s.end());
    }
    else {
        str.insert(str.begin() + p, s.begin(), s.end());
    }
    return str;
}

String llToUpper(ScriptRef, String str) {
    boost::algorithm::to_upper(str);
    return str;
}

String llToLower(ScriptRef, String str) {
    boost::algorithm::to_lower(str);
    return str;
}

Integer llSubStringIndex(ScriptRef, String haystack, String needle) {
    String::size_type pos = haystack.find(needle);
    if(pos == String::npos) {
        return -1;
    }
    return Integer(pos);
}

String llIntegerToBase64(ScriptRef, Integer a) {
    char const * input = reinterpret_cast<char const *>(&a);
    typedef boost::archive::iterators::base64_from_binary <
        boost::archive::iterators::transform_width <
        char const *, 6, 8
        >
    > to_base64_iter;
    return String(to_base64_iter(input), to_base64_iter(input + sizeof(Integer)));
}

Integer llBase64ToInteger(ScriptRef, String a) {
    size_t remove_padding = 0;
    while(a.size() < remove_padding && *(a.cbegin() + remove_padding) == '=')
        ++remove_padding;
    typedef boost::archive::iterators::binary_from_base64 <
        boost::archive::iterators::transform_width <
        char const *, 8, 6
        >
    > to_base64_iter;
    Integer result = 0;
    std::vector<char> output(to_base64_iter(a.c_str()), to_base64_iter(a.c_str() + a.size() - remove_padding));
    size_t to_copy = std::min(sizeof(Integer), output.size());
    memcpy(&result, output.data(), to_copy);
    return result;
}

String llStringToBase64(ScriptRef, String a) {
    typedef boost::archive::iterators::base64_from_binary<
        char const *
    > to_base64_iter;
    return String(to_base64_iter(a.c_str()), to_base64_iter(a.c_str() + a.size()));
}
String llBase64ToString(ScriptRef, String a) {
    size_t remove_padding = 0;
    while(a.size() < remove_padding && *(a.cbegin() + remove_padding) == '=')
        ++remove_padding;
    typedef boost::archive::iterators::binary_from_base64<
        char const *
    > to_base64_iter;
    return String(to_base64_iter(a.c_str()), to_base64_iter(a.c_str() + a.size() - remove_padding));
}

String llMD5String(ScriptRef, String a, Integer nonce) {
    a += format(":%d", nonce);
    auto result = to_md5(begin(a), end(a));
    return format("%.32x", result);
}

String llSHA1String(ScriptRef, String s) {
    auto sha1 = boost::uuids::detail::sha1();
    unsigned hashed[5]{0, 0, 0, 0};
    sha1.process_block(s.c_str(), s.c_str() + s.size());
    sha1.get_digest(hashed);
    return format("%.40x", hashed);
}

String llXorBase64Strings(ScriptRef s, String a, String b) {
    return llXorBase64StringsCorrect(s, a, b);
}

String llXorBase64StringsCorrect(ScriptRef s, String a, String b) {
    b = llBase64ToString(s, b);
    if(b.empty()) {
        return a;
    }
    while(b.size() < a.size())
        b = b + b;
    a = llBase64ToString(s, a);
    for(size_t i = 0; i < a.size(); ++i) {
        a[i] ^= b[i];
    }
    return llStringToBase64(s, a);
}

String llXorBase64(ScriptRef s, String a, String b) {
    return llXorBase64StringsCorrect(s, a, b);
}

}}}

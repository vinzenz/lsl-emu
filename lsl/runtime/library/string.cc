#include <lsl/runtime/library/functions.hh>
#include <boost/algorithm/string/case_conv.hpp>
#include <cmath>
#include <random>

namespace lsl {
namespace runtime {
namespace lib {

bool CorrectIndex(size_t max, Integer & s, Integer & e) {
    if(s < 0) {
        s = max - s;
    }
    if(e < 0) {
        e = max + s;
    }
    if(s < 0 || s >= max) {
        return false;
    }
    if(e < 0 || e >= max) {
        return false;
    }
    if(s > e) {
        return false;
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

String llIntegerToBase64(ScriptRef, Integer);
Integer llBase64ToInteger(ScriptRef, String);

String llStringToBase64(ScriptRef, String);
String llBase64ToString(ScriptRef, String);

String llMD5String(ScriptRef, String, Integer);
String llSHA1String(ScriptRef, String, Integer);

String llXorBase64Strings(ScriptRef, String, String);
String llXorBase64StringsCorrect(ScriptRef, String, String);


}}}

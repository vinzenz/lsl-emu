#include <lsl/runtime/library/functions.hh>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/classification.hpp>
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

bool CorrectIndex(size_t max, Integer & s, Integer & e);


List llListSort(ScriptRef, List l, Integer stride, Integer ascending) {
    printf("TODO: Not implemented - llListSort");
    if(ascending) {
        std::sort(l.begin(), l.end(), std::greater<ScriptValue>());
    } else {
        std::sort(l.begin(), l.end(), std::less<ScriptValue>());
    }
    return l;
}

Integer  llGetListLength(ScriptRef, List l) {
    return Integer(l.size());
}

Integer  llList2Integer(ScriptRef, List l, Integer i) {
    Integer e = i;
    if(CorrectIndex(l.size(), i, e)) {
        return l.at(i).as_integer();
    }
    return Integer();
}

Float    llList2Float(ScriptRef, List l, Integer i) {
    Integer e = i;
    if(CorrectIndex(l.size(), i, e)) {
        return l.at(i).as_float();
    }
    return Float();
}

String   llList2String(ScriptRef, List l, Integer i) {
    Integer e = i;
    if(CorrectIndex(l.size(), i, e)) {
        return l.at(i).as_string();
    }
    return String();
}


Key      llList2Key(ScriptRef, List l, Integer i) {
    Integer e = i;
    if(CorrectIndex(l.size(), i, e)) {
        return l.at(i).as_key();
    }
    return Key();
}

Vector   llList2Vector(ScriptRef, List l, Integer i) {
    Integer e = i;
    if(CorrectIndex(l.size(), i, e)) {
        return l.at(i).as_vector();
    }
    return Vector();
}

Rotation llList2Rot(ScriptRef, List l, Integer i) {
    Integer e = i;
    if(CorrectIndex(l.size(), i, e)) {
        return l.at(i).as_rotation();
    }
    return identity(Rotation());
}

List     llList2List(ScriptRef, List l, Integer b, Integer e) {
    if(CorrectIndex(l.size(), b, e)) {
        List(l.begin() + b, l.begin() + e + 1);
    }
    return l;
}

List     llDeleteSubList(ScriptRef, List l, Integer b, Integer e) {
    if(CorrectIndex(l.size(), b, e)) {
        l.erase(l.begin() + b, l.begin() + e + 1);
    }
    return l;
}

Integer  llGetListEntryType(ScriptRef, List l, Integer i) {
    Integer e = i;
    if(CorrectIndex(l.size(), i, e)) {
        return Integer(l[i].type);
    }
    return Integer(ValueType::Void);
}

static String dumpList2String(List l, String sep) {
    bool first = true;
    String res;
    for(auto const & e : l) {
        if(!first) res += sep;
        else first = false;
        res += e.as_string();
    }
    return res;
}

String   llList2CSV(ScriptRef, List l) {
    return dumpList2String(l, ", ");
}

List     llCSV2List(ScriptRef, String s) {
    std::vector<std::string> splres;
    boost::algorithm::split(splres, s, boost::algorithm::is_any_of(","));
    List res;
    res.reserve(splres.size());
    for(auto const & elem : splres) {
        res.push_back(ScriptValue());
        res.back().value = boost::algorithm::trim_copy(elem);
        res.back().type = ValueType::String;
        res.back().reference = false;
    }
    return res;
}

List     llListRandomize(ScriptRef, Integer stride) {

}

List     llList2ListStrided(ScriptRef, List, Integer, Integer, Integer) {

}

List     llListReplaceList(ScriptRef r, List source, List ins, Integer s, Integer e) {
    if(CorrectIndex(source.size(), s, e)) {
        source.erase(source.begin() + s, source.begin() + e + 1);
        return llListInsertList(r, source, ins, s);
    }
    return source;
}

List     llListInsertList(ScriptRef, List l, List n, Integer i) {
    l.insert(l.begin() + i, n.begin(), n.end());
    return l;
}

Integer  llListFindList(ScriptRef, List l, List f) {
    auto r = std::search(l.begin(), l.end(), f.begin(), f.end());
    if(r == l.end()) {
        return -1;
    }
    return Integer(r - l.begin());
}

static List llParseString2ListImpl(ScriptRef, String str, List sep, List spac, bool compress) {
    return List();
}

List     llParseString2List(ScriptRef r, String str, List sep, List spac) {
    return llParseString2ListImpl(r, str, sep, spac, true);
}

List     llParseStringKeepNulls(ScriptRef r, String str, List sep, List spac) {
    return llParseString2ListImpl(r, str, sep, spac, false);
}

String   llDumpList2String(ScriptRef, List l, String s) {
    return dumpList2String(l, s);
}


}}}

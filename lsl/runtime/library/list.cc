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
#include <algorithm>
#include <vector>
#include <tuple>
#include <type_traits>

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

typedef std::vector<std::string> SList;
typedef std::vector<std::tuple<std::string, bool>> SUList;
typedef std::string String;

inline SUList unify(SList && sep) {
    SUList result;
    for(auto && v : sep) {
        if(v.empty()) continue;
        result.emplace_back(std::move(v), true);
    }
    return result;
}

inline std::tuple<String::size_type, SUList::value_type> find_next(String::size_type idx, String const & s, SUList & usep) {
    auto cur = std::tuple<String::size_type, SUList::value_type>();
    for(auto & e : usep) {
        if(std::get<1>(e)) {
            String::size_type pos = s.find(std::get<0>(e), idx);
            if(pos != String::npos) {
                if(pos < std::get<0>(cur) || !std::get<1>(std::get<1>(cur))) {
                    cur = std::make_tuple(pos, e);
                }
            } else {
                std::get<1>(e) = false;
            }
        }
    }
    return cur;
}

template< typename T >
inline bool found(T const & t) {
    return std::get<1>(std::get<1>(t));
}

inline SList parse2list(String s, SList sep, SList spac, bool compact) {
    auto res = SList();
    if(s.empty()) {
        return res;
    }
    // Drop all seperators and spacers that are not even present
    auto usep = unify(std::move(sep));
    auto uspac = unify(std::move(spac));

    auto idx = String::size_type(0);
    auto lidx = idx;
    while(lidx != String::npos && lidx < s.size() - 1) {
        auto r1 = find_next(lidx, s, usep);
        auto r2 = find_next(lidx, s, uspac);
        bool use_r1 = false;
        bool use_r2 = false;
        if(found(r1) && found(r2)) {
            size_t p1 = std::get<0>(r1);
            size_t p2 = std::get<0>(r2);
            if(std::get<0>(r1) <= std::get<0>(r2)) {
                // r1 matches better
                use_r1 = true;
            } else {
                use_r2 = true;
            }
        } else if(found(r1)) {
            use_r1 = true;
        } else if(found(r2)) {
            use_r2 = true;
        }
        if(use_r1) {
            res.push_back(s.substr(lidx, std::get<0>(r1) - lidx));
            lidx = std::get<0>(std::get<1>(r1)).size() + std::get<0>(r1);
        } else if(use_r2) {
            res.push_back(s.substr(lidx, std::get<0>(r2) - lidx));
            res.push_back(std::get<0>(std::get<1>(r2)));
            lidx = std::get<0>(std::get<1>(r2)).size() + std::get<0>(r2);
        } else {
            res.push_back(s.substr(lidx));
            break;
        }
    }
    if(compact){
        res.erase(std::remove_if(res.begin(), res.end(), [](String const & s) -> bool { return s.empty(); }), res.end());
    }
    return res;
}

inline SList toSList(List const & l) {
    auto res = SList();
    res.reserve(l.size());
    for(auto const & e : l) {
        res.emplace_back(e.as_string());
    }
    return res;
}

inline List toList(SList const & l) {
    auto res = List();
    res.reserve(l.size());
    for(auto const & e : l) {
        res.emplace_back(ValueType::String, e, false);
    }
    return res;
}

static List llParseString2ListImpl(ScriptRef, String str, List sep, List spac, bool compress) {
    return toList(parse2list(str, toSList(sep), toSList(spac), compress));
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

List     llCSV2List(ScriptRef, String s) {
    SList splres;
    boost::algorithm::split(splres, s, boost::algorithm::is_any_of(","));
    return toList(splres);
}

}}}

#include <lsl/parser/parser.hh>
#include <lsl/ast/api.hh>
#include <lsl/ast/printer.hh>
#include <lsl/runtime/types.hh>
#include <lsl/runtime/world/script.hh>
#include <lsl/runtime/world/simulator.hh>
#include <lsl/runtime/library/functions.hh>
#include <lsl/runtime/world/script/error.hh>
#include <sqlite/connection.hpp>
#include <sqlite/execute.hpp>
#include <sqlite/transaction.hpp>
#include <fstream>

lsl::runtime::ScriptValue MakeList(lsl::runtime::List const & l) {
    lsl::runtime::ScriptValue v;
    v.value = l;
    v.type = lsl::runtime::ValueType::List;
    v.reference = false;
    return v;
}

lsl::runtime::ScriptValue MakeString(std::string const & s) {
    lsl::runtime::ScriptValue v;
    v.type = lsl::runtime::ValueType::String;
    v.value = s;
    v.reference = false;
    return v;
}

lsl::runtime::ScriptValue MakeKey(std::string const & k) {
    lsl::runtime::ScriptValue v;
    v.type = lsl::runtime::ValueType::Key;
    v.value = k;
    v.reference = false;
    return v;
}

lsl::runtime::ScriptValue MakeInt(lsl::runtime::Integer const & i) {
    lsl::runtime::ScriptValue v;
    v.type = lsl::runtime::ValueType::Integer;
    v.value = i;
    v.reference = false;
    return v;
}

lsl::runtime::ScriptValue MakeFloat(lsl::runtime::Float const & f) {
    lsl::runtime::ScriptValue v;
    v.type = lsl::runtime::ValueType::Float;
    v.value = f;
    v.reference = false;
    return v;
}
lsl::runtime::ScriptValue MakeVector(lsl::runtime::Vector const & f) {
    lsl::runtime::ScriptValue v;
    v.type = lsl::runtime::ValueType::Vector;
    v.value = f;
    v.reference = false;
    return v;
}
lsl::runtime::ScriptValue MakeRotation(lsl::runtime::Rotation const & f) {
    lsl::runtime::ScriptValue v;
    v.type = lsl::runtime::ValueType::Rotation;
    v.value = f;
    v.reference = false;
    return v;
}

int main(int argc, char const **argv)
{
    if(argc < 2) {
        return EXIT_FAILURE;
    }
    lsl::Lexer lex(argv[1]);
    lsl::Script scr;
    bool parse_result = false;
    try {
        parse_result = lsl::parse(lex, scr);
    }
    catch(lsl::runtime::script::ScriptError const & e) {
        printf("Compilation failed: %s L: %d C: %d\n", e.what(), e.line(), e.column());
    }

    if(parse_result && argc > 2 && argv[2][0] == 'p') {
        lsl::ast::PrinterState ps{0};
        lsl::ast::print(ps, scr);
        return 0;
    }
    if(!parse_result) {
        return 1;
    }

    //printf("\nLast line: %I64d\nSuccess: %s\n", lex.next().line, parse_result ? "yes" : "no");
    /*
    if(parse_result) {
        printf("Global Variables:\n");
        for(auto const & var : lsl::get_global_variables(scr)) {
            printf("\t%s - %s\n", var.type_name.c_str(), var.name.c_str());
        }
        printf("Functions:\n");
        for(auto const & var : lsl::get_functions(scr)) {
            printf("\t%s %s(", var.returnType.c_str(), var.name.c_str());
            bool first = true;
            for(auto const & arg : var.args) {
                printf("%s%s %s", first ? "": ", ", arg.type.c_str(), arg.name.c_str());
                first = false;
            }
            printf(")\n");
        }
        printf("States:\n");
        for(auto const & ste : lsl::get_states(scr)) {
            printf("\tstate %s\n", ste.name.c_str());
            for(auto const & evt : lsl::get_events(ste)) {
                printf("\t\t%s(", evt.name.c_str());
                bool first = true;
                for(auto const & arg : evt.args) {
                    printf("%s%s %s", first ? "": ", ", arg.type.c_str(), arg.name.c_str());
                    first = false;
                }
                printf(")\n");
            }
        }
    }

    using boost::math::double_constants::degree;
    using boost::math::double_constants::radian;
    using boost::math::double_constants::pi;
    lsl::runtime::Vector v{0.3, 3.0, -2.0};
    //lsl::runtime::Vector v{90., 90., 180.};
    auto qv = from_euler(v * degree);
    auto rv = to_euler(qv) * radian;
    printf("qv  %s\n", to_string(qv).c_str());
    printf("rv  %s\n", to_string(rv).c_str());
    */
    /*
    lsl::ast::PrinterState ps{0};
    lsl::ast::print(ps, scr);
    */
#if 1
    using namespace lsl::runtime::script;
    using lsl::runtime::ValueType;



    auto llSay = CompiledScriptFunction{
        ValueType::Void,
        {ValueType::Integer, ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llSay: %s\n", args.arguments[1].as_string().c_str());
            return CallResult();
        }
    };
    auto llWhisper = CompiledScriptFunction{
        ValueType::Void,
        {ValueType::Integer, ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llWhisper: %s\n", args.arguments[1].as_string().c_str());
            return CallResult();
        }
    };
    auto llOwnerSay = CompiledScriptFunction{
        ValueType::Void,
        {ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llOwnerSay: %s\n", args.arguments[0].as_string().c_str());
            return CallResult();
        }
    };
    auto llInstantMessage = CompiledScriptFunction{
        ValueType::Void,
        {ValueType::Key, ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llInstantMessage(%s): %s\n", args.arguments[0].as_string().c_str(), args.arguments[1].as_string().c_str());
            return CallResult();
        }
    };
    auto llResetScript = CompiledScriptFunction{
        ValueType::Void,
        {},
        [](ScriptFunctionCall const &) -> CallResult {
            printf("llResetScript();\n");
            return CallResult();
        }
    };
    auto llKey2Name = CompiledScriptFunction{
        ValueType::String,
        {ValueType::Key},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llKey2Name(%s);\n", args.arguments[0].as_string().c_str());
            return CallResult(MakeString("Avatar Name"));
        }
    };
#define SOMEGUID "BBDF9334-9A67-11E4-9C22-22F289BEA664"
    auto llGetOwner = CompiledScriptFunction{
        ValueType::Key,
        {},
        [](ScriptFunctionCall const &) -> CallResult {
            printf("llGetOwner();\n");
            return CallResult(MakeKey(SOMEGUID));
        }
    };
    auto llGetInventoryCreator = CompiledScriptFunction{
        ValueType::Key,
        {ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llGetInventoryCreator(%s);\n", args.arguments[0].as_string().c_str());
            return CallResult(MakeKey(SOMEGUID));
        }
    };
    auto llGetScriptName = CompiledScriptFunction{
        ValueType::String,
        {},
        [](ScriptFunctionCall const &) -> CallResult {
            printf("llGetScriptName();\n");
            return CallResult(MakeString("CurrentScript"));
        }
    };
    auto llList2String = CompiledScriptFunction{
        ValueType::String,
        {ValueType::List, ValueType::Integer},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llList2String();\n");
            return CallResult(MakeString(args.arguments[0].as_list().at(args.arguments[1].as_integer()).as_string()));
        }
    };
    auto llList2Integer = CompiledScriptFunction{
        ValueType::Integer,
        {ValueType::List, ValueType::Integer},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llList2Integer();\n");
            auto i = args.arguments[1].as_integer();
            auto l =  args.arguments[0].as_list();
            return CallResult(MakeInt(l.at(i).as_integer()));
        }
    };
    auto llList2Vector = CompiledScriptFunction{
        ValueType::Vector,
        {ValueType::List, ValueType::Integer},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llList2Vector();\n");
            auto i = args.arguments[1].as_integer();
            auto l =  args.arguments[0].as_list();
            return CallResult(MakeVector(l.at(i).as_vector()));
        }
    };
    auto llList2Rot = CompiledScriptFunction{
        ValueType::Rotation,
        {ValueType::List, ValueType::Integer},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llList2Vector();\n");
            auto i = args.arguments[1].as_integer();
            auto l =  args.arguments[0].as_list();
            return CallResult(MakeRotation(l.at(i).as_rotation()));
        }
    };
    auto llList2Float = CompiledScriptFunction{
        ValueType::Float,
        {ValueType::List, ValueType::Integer},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llList2Float();\n");
            auto i = args.arguments[1].as_integer();
            auto l =  args.arguments[0].as_list();
            return CallResult(MakeFloat(l.at(i).as_float()));
        }
    };
    auto llGetListEntryType = CompiledScriptFunction{
        ValueType::Integer,
        {ValueType::List, ValueType::Integer},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeInt(lsl::runtime::lib::llGetListEntryType(args.caller, args.arguments[0].as_list(), args.arguments[1].as_integer())));
        }
    };
    auto llListReplaceList = CompiledScriptFunction{
            ValueType::List,
            {ValueType::List, ValueType::List, ValueType::Integer, ValueType::Integer},
            [](ScriptFunctionCall const & args) -> CallResult {
                printf("llListReplaceList();\n");
                auto idx1 = args.arguments[2].as_integer();
                auto idx2 = args.arguments[3].as_integer();
                auto l =  args.arguments[0].as_list();
                auto l2 = args.arguments[1].as_list();
                return CallResult(MakeList(lsl::runtime::lib::llListReplaceList(args.caller, l, l2, idx1, idx2)));
            }
        };
    auto llGetListLength = CompiledScriptFunction{
        ValueType::Integer,
        {ValueType::List},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llGetListLength();\n");
            return CallResult(MakeInt(lsl::runtime::Integer(args.arguments[0].as_list().size())));
        }
    };
    auto llHTTPRequest = CompiledScriptFunction{
        ValueType::Key,
        {ValueType::String, ValueType::List, ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llHTTPRequest();\n");
            return CallResult(MakeKey(SOMEGUID));
        }
    };
    auto llDetectedKey = CompiledScriptFunction{
        ValueType::Key,
        {ValueType::Integer},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llDetectedKey();\n");
            return CallResult(MakeKey(SOMEGUID));
        }
    };
    auto llDumpList2String = CompiledScriptFunction{
        ValueType::String,
        {ValueType::List, ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llDumpList2String();\n");
            auto res = std::string();
            auto sep = args.arguments[1].as_string();
            auto first = true;
            for(auto const & item : args.arguments[0].as_list()) {
                if(!first) res += sep;
                else first = false;
                res += item.as_string();
            }
            return CallResult(MakeString(res));
        }
    };

    auto llFrand = CompiledScriptFunction{
        ValueType::Float,
        {ValueType::Float},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llFrand();\n");
            return CallResult(MakeFloat(lsl::runtime::lib::llFrand(args.caller, args.arguments[0].as_float())));
        }
    };

    auto llDeleteSubList = CompiledScriptFunction{
        ValueType::List,
        {ValueType::List, ValueType::Integer, ValueType::Integer},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llDeleteSubList()\n");
            return CallResult(MakeList(lsl::runtime::lib::llDeleteSubList(args.caller, args.arguments[0].as_list(), args.arguments[1].as_integer(), args.arguments[2].as_integer())));
        }
    };
    bool timerSet = false;
    auto llSetTimerEvent = CompiledScriptFunction{
        ValueType::Void,
        {ValueType::Float},
        [&timerSet](ScriptFunctionCall const & args) -> CallResult {
            timerSet = args.arguments[0].as_float() != 0;
            printf("llSetTimerEvent - > timer is now: %d\n", timerSet ? 1 : 0);
            return CallResult({});
        }
    };

    auto llSetText = CompiledScriptFunction{
        ValueType::Void,
        {ValueType::String, ValueType::Vector, ValueType::Float},
        [](ScriptFunctionCall const & args) -> CallResult {
            std::ofstream ofs("text.out");
            ofs << args.arguments[1].as_string() + args.arguments[0].as_string();
            return CallResult({});
        }
    };
    auto llListFindList = CompiledScriptFunction{
        ValueType::Integer,
        {ValueType::List, ValueType::List},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeInt(lsl::runtime::lib::llListFindList(args.caller, args.arguments[0].as_list(), args.arguments[1].as_list())));
        }
    };

    auto llListSort = CompiledScriptFunction{
        ValueType::List,
        {ValueType::List, ValueType::Integer, ValueType::Integer},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeList(lsl::runtime::lib::llListSort(args.caller, args.arguments[0].as_list(), args.arguments[1].as_integer(), args.arguments[2].as_integer())));
        }
    };

    auto llList2List = CompiledScriptFunction{
        ValueType::List,
        {ValueType::List, ValueType::Integer, ValueType::Integer},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeList(lsl::runtime::lib::llList2List(args.caller, args.arguments[0].as_list(), args.arguments[1].as_integer(), args.arguments[2].as_integer())));
        }
    };

    auto llBase64ToInteger = CompiledScriptFunction{
        ValueType::Integer,
        {ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeInt(lsl::runtime::lib::llBase64ToInteger(args.caller, args.arguments[0].as_string())));
        }
    };
    auto llStringToBase64 = CompiledScriptFunction{
        ValueType::String,
        {ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeString(lsl::runtime::lib::llStringToBase64(args.caller, args.arguments[0].as_string())));
        }
    };
    auto llSubStringIndex = CompiledScriptFunction{
        ValueType::Integer,
        {ValueType::String, ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {

            return CallResult(MakeInt(lsl::runtime::lib::llSubStringIndex(args.caller, args.arguments[0].as_string(), args.arguments[1].as_string())));
        }
    };
    auto llGetSubString = CompiledScriptFunction{
        ValueType::String,
        {ValueType::String, ValueType::Integer, ValueType::Integer},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeString(lsl::runtime::lib::llGetSubString(args.caller, args.arguments[0].as_string(), args.arguments[1].as_integer(), args.arguments[2].as_integer())));
        }
    };
    auto llStringLength = CompiledScriptFunction{
        ValueType::Integer,
        {ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeInt(lsl::runtime::lib::llStringLength(args.caller, args.arguments[0].as_string())));
        }
    };
    auto llParseStringKeepNulls = CompiledScriptFunction{
        ValueType::List,
        {ValueType::String, ValueType::List, ValueType::List},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeList(lsl::runtime::lib::llParseStringKeepNulls(args.caller, args.arguments[0].as_string(), args.arguments[1].as_list(), args.arguments[2].as_list())));
        }
    };
    auto llParseString2List = CompiledScriptFunction{
        ValueType::List,
        {ValueType::String, ValueType::List, ValueType::List},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeList(lsl::runtime::lib::llParseString2List(args.caller, args.arguments[0].as_string(), args.arguments[1].as_list(), args.arguments[2].as_list())));
        }
    };
    auto llEscapeURL = CompiledScriptFunction{
        ValueType::String,
        {ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeString(lsl::runtime::lib::llEscapeURL(args.caller, args.arguments[0].as_string())));
        }
    };
    auto llUnescapeURL = CompiledScriptFunction{
        ValueType::String,
        {ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeString(lsl::runtime::lib::llUnescapeURL(args.caller, args.arguments[0].as_string())));
        }
    };
    auto llDeleteSubString = CompiledScriptFunction{
        ValueType::String,
        {ValueType::String, ValueType::Integer, ValueType::Integer},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeString(lsl::runtime::lib::llDeleteSubString(args.caller, args.arguments[0].as_string(), args.arguments[1].as_integer(), args.arguments[2].as_integer())));
        }
    };
    auto llLog = CompiledScriptFunction{
        ValueType::Float,
        {ValueType::Float},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeFloat(lsl::runtime::lib::llLog(args.caller, args.arguments[0].as_float())));
        }
    };

    auto llCeil = CompiledScriptFunction{
        ValueType::Float,
        {ValueType::Float},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeFloat(lsl::runtime::lib::llCeil(args.caller, args.arguments[0].as_float())));
        }
    };

    auto llFabs = CompiledScriptFunction{
        ValueType::Float,
        {ValueType::Float},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeFloat(lsl::runtime::lib::llFabs(args.caller, args.arguments[0].as_float())));
        }
    };

    auto llPow = CompiledScriptFunction{
        ValueType::Float,
        {ValueType::Float, ValueType::Float},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeFloat(lsl::runtime::lib::llPow(args.caller, args.arguments[0].as_float(), args.arguments[1].as_float())));
        }
    };

    auto llFloor = CompiledScriptFunction{
        ValueType::Integer,
        {ValueType::Float},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeInt(lsl::runtime::lib::llFloor(args.caller, args.arguments[0].as_float())));
        }
    };
    auto llCSV2List = CompiledScriptFunction{
        ValueType::List,
        {ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeList(lsl::runtime::lib::llCSV2List(args.caller, args.arguments[0].as_string())));
        }
    };
    auto llList2CSV = CompiledScriptFunction{
        ValueType::String,
        {ValueType::List},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeString(lsl::runtime::lib::llList2CSV(args.caller, args.arguments[0].as_list())));
        }
    };
    auto llBase64ToString = CompiledScriptFunction{
        ValueType::String,
        {ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            return CallResult(MakeString(lsl::runtime::lib::llBase64ToString(args.caller, args.arguments[0].as_string())));
        }
    };

    sqlite::connection con("results.db");
    sqlite::execute(con, "CREATE TABLE IF NOT EXISTS poker_result (result INTEGER NOT NULL );", true);
    sqlite::execute inserter(con, "INSERT INTO poker_result (result) VALUES(?);", false);
    sqlite::transaction trans(con, sqlite::transaction_type::deferred);
    auto sqlInsert = CompiledScriptFunction{
        ValueType::Void,
        {ValueType::Integer},
        [&con, &inserter](ScriptFunctionCall const & args) -> CallResult {
            try {
                inserter.clear();
                inserter % args.arguments[0].as_integer();
                inserter();
            } catch(...) {
                printf("ERROR: Failed to insert data!");
            }
            return CallResult();
        }
    };

    get_script_library().functions["llSay"] = std::make_shared<ScriptFunction>(llSay);
    get_script_library().functions["llWhisper"] = std::make_shared<ScriptFunction>(llWhisper);
    get_script_library().functions["llOwnerSay"] = std::make_shared<ScriptFunction>(llOwnerSay);
    get_script_library().functions["llInstantMessage"] = std::make_shared<ScriptFunction>(llInstantMessage);
    get_script_library().functions["llHTTPRequest"] = std::make_shared<ScriptFunction>(llHTTPRequest);
    get_script_library().functions["llResetScript"] = std::make_shared<ScriptFunction>(llResetScript);
    get_script_library().functions["llDetectedKey"] = std::make_shared<ScriptFunction>(llDetectedKey);
    get_script_library().functions["llKey2Name"] = std::make_shared<ScriptFunction>(llKey2Name);
    get_script_library().functions["llGetOwner"] = std::make_shared<ScriptFunction>(llGetOwner);
    get_script_library().functions["llGetInventoryCreator"] = std::make_shared<ScriptFunction>(llGetInventoryCreator);
    get_script_library().functions["llGetScriptName"] = std::make_shared<ScriptFunction>(llGetScriptName);
    get_script_library().functions["llList2String"] = std::make_shared<ScriptFunction>(llList2String);
    get_script_library().functions["llList2Integer"] = std::make_shared<ScriptFunction>(llList2Integer);
    get_script_library().functions["llDeleteSubList"] = std::make_shared<ScriptFunction>(llDeleteSubList);
    get_script_library().functions["llGetListLength"] = std::make_shared<ScriptFunction>(llGetListLength);
    get_script_library().functions["llDumpList2String"] = std::make_shared<ScriptFunction>(llDumpList2String);
    get_script_library().functions["llParseStringKeepNulls"] = std::make_shared<ScriptFunction>(llParseStringKeepNulls);
    get_script_library().functions["llFrand"] = std::make_shared<ScriptFunction>(llFrand);
    get_script_library().functions["llSetTimerEvent"] = std::make_shared<ScriptFunction>(llSetTimerEvent);
    get_script_library().functions["llSetText"] = std::make_shared<ScriptFunction>(llSetText);
    get_script_library().functions["llList2List"] = std::make_shared<ScriptFunction>(llList2List);
    get_script_library().functions["llListReplaceList"] = std::make_shared<ScriptFunction>(llListReplaceList);
    get_script_library().functions["llListSort"] = std::make_shared<ScriptFunction>(llListSort);
    get_script_library().functions["llListFindList"] = std::make_shared<ScriptFunction>(llListFindList);
    get_script_library().functions["sqlInsert"] = std::make_shared<ScriptFunction>(sqlInsert);
    get_script_library().functions["llBase64ToInteger"] = std::make_shared<ScriptFunction>(llBase64ToInteger);
    get_script_library().functions["llStringToBase64"] = std::make_shared<ScriptFunction>(llStringToBase64);
    get_script_library().functions["llGetSubString"] = std::make_shared<ScriptFunction>(llGetSubString);
    get_script_library().functions["llParseString2List"] = std::make_shared<ScriptFunction>(llParseString2List);
    get_script_library().functions["llParseStringKeepNulls"] = std::make_shared<ScriptFunction>(llParseStringKeepNulls);
    get_script_library().functions["llUnescapeURL"] = std::make_shared<ScriptFunction>(llUnescapeURL);
    get_script_library().functions["llEscapeURL"] = std::make_shared<ScriptFunction>(llEscapeURL);
    get_script_library().functions["llDeleteSubString"] = std::make_shared<ScriptFunction>(llDeleteSubString);
    get_script_library().functions["llLog"] = std::make_shared<ScriptFunction>(llLog);
    get_script_library().functions["llFloor"] = std::make_shared<ScriptFunction>(llFloor);
    get_script_library().functions["llCeil"] = std::make_shared<ScriptFunction>(llCeil);
    get_script_library().functions["llFabs"] = std::make_shared<ScriptFunction>(llFabs);
    get_script_library().functions["llPow"] = std::make_shared<ScriptFunction>(llPow);
    get_script_library().functions["llStringLength"] = std::make_shared<ScriptFunction>(llStringLength);
    get_script_library().functions["llGetListEntryType"] = std::make_shared<ScriptFunction>(llGetListEntryType);
    get_script_library().functions["llList2Float"] = std::make_shared<ScriptFunction>(llList2Float);
    get_script_library().functions["llList2Vector"] = std::make_shared<ScriptFunction>(llList2Vector);
    get_script_library().functions["llList2Rot"] = std::make_shared<ScriptFunction>(llList2Rot);
    get_script_library().functions["llSubStringIndex"] = std::make_shared<ScriptFunction>(llSubStringIndex);
    get_script_library().functions["llCSV2List"] = std::make_shared<ScriptFunction>(llCSV2List);
    get_script_library().functions["llList2CSV"] = std::make_shared<ScriptFunction>(llList2CSV);
    get_script_library().functions["llBase64ToString"] = std::make_shared<ScriptFunction>(llBase64ToString);

    auto script = eval_script("Fake", scr);

    script->dispatch_event("state_entry", {});
    trans.commit();
    while(timerSet) {
        script->dispatch_event("timer", {});
    }
    script->dispatch_event("state_exit", {});
#endif
    // auto script = Script("foo");
    lsl::runtime::Simulator simulator;


    return parse_result ? EXIT_SUCCESS : EXIT_FAILURE;
}


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
#include <lsl/compiler/analyzer.hh>

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

#define LOCAL_LSL(NAME, DEF) \
    get_script_library().functions[#NAME] = std::make_shared<ScriptFunction>(lsl::runtime::lib::wrap(DEF))

    using lsl::runtime::Integer;
    using lsl::runtime::List;
    using lsl::runtime::Float;
    using lsl::runtime::String;
    using lsl::runtime::Vector;
    using lsl::runtime::Rotation;

    LOCAL_LSL(llSay, [](ScriptRef, Integer, String s) -> void {
        printf("llSay: %s\n", s.c_str());
    });
    LOCAL_LSL(llWhisper, [](ScriptRef, Integer, String s) -> void {
        printf("llWhisper: %s\n", s.c_str());
    });
    LOCAL_LSL(llShout, [](ScriptRef, Integer, String s) -> void {
        printf("llShout: %s\n", s.c_str());
    });
    LOCAL_LSL(llRegionSay, [](ScriptRef, Integer, String s) -> void {
        printf("llRegionSay: %s\n", s.c_str());
    });
    LOCAL_LSL(llRegionSayTo, [](ScriptRef, String, Integer, String s) -> void {
        printf("llRegionSayTo: %s\n", s.c_str());
    });
    LOCAL_LSL(llOwnerSay, [](ScriptRef, String s) -> void {
        printf("llOwnerSay: %s\n", s.c_str());
    });
    LOCAL_LSL(llInstantMessage, [](ScriptRef, String t, String s) -> void {
        printf("llInstantMessage(%s): %s\n", t.c_str(), s.c_str());
    });
    LOCAL_LSL(llResetScript, [](ScriptRef) -> void {
        printf("llResetScript()\n");
    });
    LOCAL_LSL(llKey2Name, [](ScriptRef, String k) -> String {
        printf("llKey2Name(%s);\n", k.c_str());
        return "Avatar Name";
    });
#define SOMEGUID "BBDF9334-9A67-11E4-9C22-22F289BEA664"
    LOCAL_LSL(llGetOwner, [](ScriptRef) -> String {
        printf("llGetOwner()\n");
        return SOMEGUID;
    });
    LOCAL_LSL(llGetInventoryCreator, [](ScriptRef, String n) -> String {
        printf("llGetInventoryCreator(%s);\n", n.c_str());
        return SOMEGUID;
    });
    LOCAL_LSL(llGetScriptName, [](ScriptRef r) -> String{
        return r.get().name;
    });
    LOCAL_LSL(llHTTPRequest, [](ScriptRef, String, List, String) -> String {
        printf("llHTTPRequest();\n");
        return SOMEGUID;
    });
    LOCAL_LSL(llDetectedKey,[](ScriptRef, Integer) -> String {
        printf("llDetectedKey();\n");
        return SOMEGUID;
    });

    bool timerSet = false;
    LOCAL_LSL(llSetTimerEvent, [&timerSet](ScriptRef, Float f) -> void {
        timerSet = f != 0;
        printf("llSetTimerEvent - > timer is now: %d\n", timerSet ? 1 : 0);
    });
    LOCAL_LSL(llSetText, [](ScriptRef, String s, Vector v, Float) -> void {
        std::ofstream ofs("text.out");
        ofs << lsl::runtime::to_string(v) + s;
    });

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

    get_script_library().functions["sqlInsert"] = std::make_shared<ScriptFunction>(sqlInsert);

    #define LSL_LIB_WRAP(LSL_FUN_NAME) \
        get_script_library().functions[#LSL_FUN_NAME] = std::make_shared<ScriptFunction>(lsl::runtime::lib::wrap(lsl::runtime::lib:: LSL_FUN_NAME))
    LSL_LIB_WRAP(llDumpList2String);
    LSL_LIB_WRAP(llEscapeURL);
    LSL_LIB_WRAP(llUnescapeURL);
    LSL_LIB_WRAP(llCeil);
    LSL_LIB_WRAP(llFloor);
    LSL_LIB_WRAP(llFabs);
    LSL_LIB_WRAP(llLog);
    LSL_LIB_WRAP(llPow);
    LSL_LIB_WRAP(llFrand);

    LSL_LIB_WRAP(llParseString2List);
    LSL_LIB_WRAP(llParseStringKeepNulls);

    LSL_LIB_WRAP(llDeleteSubList);
    LSL_LIB_WRAP(llList2List);
    LSL_LIB_WRAP(llListSort);
    LSL_LIB_WRAP(llListReplaceList);
    LSL_LIB_WRAP(llListFindList);
    LSL_LIB_WRAP(llList2Float);
    LSL_LIB_WRAP(llList2Key);
    LSL_LIB_WRAP(llList2String);
    LSL_LIB_WRAP(llList2Integer);
    LSL_LIB_WRAP(llList2Vector);
    LSL_LIB_WRAP(llList2Rot);
    LSL_LIB_WRAP(llGetListEntryType);
    LSL_LIB_WRAP(llGetListLength);

    LSL_LIB_WRAP(llBase64ToString);
    LSL_LIB_WRAP(llBase64ToInteger);
    LSL_LIB_WRAP(llStringToBase64);

    LSL_LIB_WRAP(llGetSubString);
    LSL_LIB_WRAP(llStringLength);
    LSL_LIB_WRAP(llSubStringIndex);
    LSL_LIB_WRAP(llDeleteSubString);

    LSL_LIB_WRAP(llCSV2List);
    LSL_LIB_WRAP(llList2CSV);

    lsl::Analyzer analyzer(scr);
    analyzer.Perform();

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


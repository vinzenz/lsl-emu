#include <lsl/parser/parser.hh>
#include <lsl/ast/api.hh>
#include <lsl/ast/printer.hh>
#include <lsl/runtime/types.hh>
#include <lsl/runtime/world/script.hh>
#include <lsl/runtime/world/simulator.hh>
#include <lsl/runtime/library/functions.hh>
#include <lsl/runtime/world/script/error.hh>

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
            return CallResult(MakeInt(args.arguments[0].as_list().at(args.arguments[1].as_integer()).as_integer()));
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
    auto llParseStringKeepNulls = CompiledScriptFunction{
        ValueType::List,
        {ValueType::String, ValueType::List, ValueType::List},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llParseStringKeepNulls();\n");
            return CallResult(MakeList({}));
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


    get_script_library().functions["llSay"] = std::make_shared<ScriptFunction>(llSay);
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


    auto script = eval_script("Fake", scr);

    script->dispatch_event("state_entry", {});
    script->dispatch_event("state_exit", {});
#endif
    // auto script = Script("foo");
    lsl::runtime::Simulator simulator;


    return parse_result ? EXIT_SUCCESS : EXIT_FAILURE;
}


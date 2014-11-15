#include <lsl/parser/parser.hh>
#include <lsl/ast/api.hh>
#include <lsl/ast/printer.hh>
#include <lsl/runtime/types.hh>
#include <lsl/runtime/world/script.hh>
#include <lsl/runtime/world/simulator.hh>
#include <lsl/runtime/library/functions.hh>
int main(int argc, char const **argv)
{
    if(argc < 2) {
        return EXIT_FAILURE;
    }
    lsl::Lexer lex(argv[1]);
    lsl::Script scr;
    bool parse_result = lsl::parse(lex, scr);
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
    using namespace lsl::runtime::script;
#if 0
    using lsl::runtime::ValueType;
    auto llSay = CompiledScriptFunction{
        ValueType::Void,
        {ValueType::Integer, ValueType::String},
        [](ScriptFunctionCall const & args) -> CallResult {
            printf("llSay: %s\n", args.arguments[1].as_string().c_str());
            return CallResult();
        }
    };
    get_script_library().functions["llSay"] = std::make_shared<ScriptFunction>(llSay);

    auto script = eval_script("Fake", scr);

    script->dispatch_event("state_entry", {});
    script->dispatch_event("state_exit", {});
#endif
    auto script = Script("foo");
    lsl::runtime::Simulator simulator;


    return parse_result ? EXIT_SUCCESS : EXIT_FAILURE;
}


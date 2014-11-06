#include <lsl/parser/parser.hh>
#include <lsl/ast/api.hh>

int main(int argc, char const **argv)
{
    if(argc < 2) {
        return EXIT_FAILURE;
    }
    lsl::Lexer lex(argv[1]);
    lsl::Script scr;
    bool parse_result = lsl::parse(lex, scr);
    printf("\nLast line: %I64d\nSuccess: %s\n", lex.next().line, parse_result ? "yes" : "no");

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

    return parse_result ? EXIT_SUCCESS : EXIT_FAILURE;
}


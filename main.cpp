#include <lsl/parser/parser.hh>

int main(int argc, char const **argv)
{
    if(argc < 2) {
        return EXIT_FAILURE;
    }
    lsl::Lexer lex(argv[1]);
    lsl::Script scr;
    bool parse_result = lsl::parse(lex, scr);
    printf("Last line: %I64d\nSuccess: %s\n", lex.next().line, parse_result ? "yes" : "no");
    return parse_result ? EXIT_SUCCESS : EXIT_FAILURE;
}


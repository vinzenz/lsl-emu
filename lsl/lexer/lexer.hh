// Copyright 2014 Vinzenz Feenstra
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef GUARD_PYPA_TOKENIZER_LEXER_HH_INCLUDED
#define GUARD_PYPA_TOKENIZER_LEXER_HH_INCLUDED

#include <lsl/filebuf.hh>
#include <lsl/lexer/tokendef.hh>
#include <string>
#include <deque>
#include <list>
#include <vector>
#include <stdint.h>

namespace lsl {

struct TokenInfo {
    TokenIdent ident;
    uint32_t line;
    uint32_t column;
    std::string value;
};

enum class LexerInfoLevel {
    Information,
    Warning,
    Error
};

struct LexerInfo {
    TokenInfo info;
    LexerInfoLevel level;
    std::string value;
};

class Lexer {
    enum {
        TabSize = 8,
        AltTabSize = 1,
    };

    lsl::FileBuf file_;
    std::string input_path_;

    uint32_t column_;
    int level_;
    int indent_;
    std::vector<int> indent_stack_;
    std::vector<int> alt_indent_stack_;
    std::deque<char> lex_buffer_;

    std::list<LexerInfo> info_;
    char first_indet_char;
    std::deque<TokenInfo> token_buffer_;
public:
    Lexer(char const * file_path);
    ~Lexer();

    std::string get_name() const;
    std::string get_line(uint64_t idx);

    std::list<LexerInfo> const & info();

    TokenInfo next();

private:
    char skip();
    char skip_comment();
    unsigned line() const { return file_.line(); }
    char next_char();
    void put_char(char c);
    TokenInfo get_string(TokenInfo & tok, char first, char prefix=0);
    TokenInfo get_number(TokenInfo & tok, char first);
    TokenInfo get_number_binary(TokenInfo & tok, char first);
    TokenInfo get_number_hex(TokenInfo & tok, char first);
    TokenInfo get_number_octal(TokenInfo & tok, char first);
    TokenInfo get_number_float(TokenInfo & tok, char first);
    TokenInfo get_number_integer(TokenInfo & tok, char first);
    TokenInfo get_number_complex(TokenInfo & tok, char first);

    bool handle_indentation(bool continuation = false);
    void handle_whitespace(char c);

    TokenInfo make_token(TokenInfo & tok, Token id, TokenKind kind,
                         TokenClass cls = TokenClass::Default);
    TokenInfo make_token(TokenInfo & tok, TokenIdent ident);
    TokenInfo make_invalid(char const * begin, char const * end,
                           Token id = Token::Invalid,
                           TokenKind kind = TokenKind::Error,
                           TokenClass cls = TokenClass::Default);

    bool add_indent_error(bool dedent = false);
    void add_info_item(LexerInfoLevel level, TokenInfo const & t);

    inline void add_information(TokenInfo const & t) {
        add_info_item(LexerInfoLevel::Information, t);
    }

    inline void add_warning(TokenInfo const & t) {
        add_info_item(LexerInfoLevel::Warning, t);
    }

    inline void add_error(TokenInfo const & t) {
        add_info_item(LexerInfoLevel::Error, t);
    }
};

}

#endif //GUARD_PYPA_TOKENIZER_LEXER_HH_INCLUDED


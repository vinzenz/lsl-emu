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
#ifndef GUARD_lsl_PARSER_PARSER_HH_INCLUDED
#define GUARD_lsl_PARSER_PARSER_HH_INCLUDED

#include <lsl/ast/ast.hh>
#include <lsl/lexer/lexer.hh>
#include <lsl/parser/error.hh>
#include <functional>

namespace lsl {

struct ParserOptions {
    ParserOptions()
    : printerrors(true)
    , printdbgerrors(false)
    , error_handler()
    {}

    bool printerrors;
    bool printdbgerrors;
    std::function<void(lsl::Error)> error_handler;
};

bool parse(Lexer & lexer,
           Script & ast,
           ParserOptions options = ParserOptions());

bool parse(Lexer & lexer,
           AstPtr & ast,
           ParserOptions options = ParserOptions());


}

#endif // GUARD_lsl_PARSER_PARSER_HH_INCLUDED


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
#ifndef GUARD_PYPA_TOKENIZER_KEYWORD_HH_INCLUDED
#define GUARD_PYPA_TOKENIZER_KEYWORD_HH_INCLUDED

#include <lsl/lexer/tokens.hh>
#include <lsl/lexer/tokendef.hh>


namespace lsl {
    static const TokenDef KeywordTokens[] = {
        TokenDef(Token::KeywordElse, TokenString("else"), TokenKind::Name, TokenClass::Keyword),
        TokenDef(Token::KeywordFor, TokenString("for"), TokenKind::Name, TokenClass::Keyword),
        TokenDef(Token::KeywordIf, TokenString("if"), TokenKind::Name, TokenClass::Keyword),
        TokenDef(Token::KeywordReturn, TokenString("return"), TokenKind::Name, TokenClass::Keyword),
        TokenDef(Token::KeywordWhile, TokenString("while"), TokenKind::Name, TokenClass::Keyword),

        TokenDef(Token::KeywordState, TokenString("state"), TokenKind::Name, TokenClass::Keyword),
        TokenDef(Token::KeywordDefault, TokenString("default"), TokenKind::Name, TokenClass::Keyword),
        TokenDef(Token::KeywordEvent, TokenString("event"), TokenKind::Name, TokenClass::Keyword),
        TokenDef(Token::KeywordJump, TokenString("jump"), TokenKind::Name, TokenClass::Keyword),
        TokenDef(Token::KeywordDo, TokenString("do"), TokenKind::Do, TokenClass::Keyword),
        TokenDef(Token::KeywordTrue, TokenString("TRUE"), TokenKind::Name, TokenClass::Keyword),
        TokenDef(Token::KeywordFalse, TokenString("FALSE"), TokenKind::Name, TokenClass::Keyword),

        TokenDef(Token::TypeInteger, TokenString("integer"), TokenKind::TypeName, TokenClass::Keyword),
        TokenDef(Token::TypeInteger, TokenString("float"), TokenKind::TypeName, TokenClass::Keyword),
        TokenDef(Token::TypeInteger, TokenString("vector"), TokenKind::TypeName, TokenClass::Keyword),
        TokenDef(Token::TypeRotation, TokenString("rotation"), TokenKind::TypeName, TokenClass::Keyword),
        TokenDef(Token::TypeString, TokenString("string"), TokenKind::TypeName, TokenClass::Keyword),
        TokenDef(Token::TypeKey, TokenString("key"), TokenKind::TypeName, TokenClass::Keyword),
        TokenDef(Token::TypeList, TokenString("list"), TokenKind::TypeName, TokenClass::Keyword),
    };

    inline ConstArray<TokenDef const> Keywords() {
        return { KeywordTokens };
    }
}
#endif // GUARD_PYPA_TOKENIZER_KEYWORD_HH_INCLUDED

#include <lsl/ast/ast.hh>
#include <lsl/lexer/lexer.hh>
#include <lsl/parser/state.hh>
#include <gmp.h>
#include <double-conversion/double-conversion.h>
using namespace lsl;

template< typename T, typename U>
std::shared_ptr<T> ast_cast(std::shared_ptr<U> u) {
    return std::static_pointer_cast<T>(u);
}


template<typename T>
AstPtr error_transform(T const & t) {
    return std::make_shared<T>(t);
}

template<typename T>
AstPtr error_transform(std::shared_ptr<T> const & t) {
    return t;
}

void report_error(State & s) {
    Error & e = s.errors.top();
    if(s.options.error_handler) {
        s.options.error_handler(e);
    }

    if(!s.options.printerrors) {
        return;
    }

    unsigned long long cur_line = e.cur.line;
    fprintf(stderr, " File \"%s\", line %llu\n %s\n ", e.file_name.c_str(), cur_line, e.line.c_str());

    if(e.cur.column == 0) ++e.cur.column;

    for(unsigned i = 0; i < e.cur.column - 1; ++i) {
        fputc(' ', stderr);
    }

    fprintf(stderr, "^\n%s: %s", e.type == ErrorType::SyntaxError ? "SyntaxError" : e.type == ErrorType::SyntaxWarning ? "SyntaxWarning" : "IndentationError", e.message.c_str());

    if(e.detected_line != -1 && s.options.printdbgerrors) {
        fprintf(stderr, "\n-> Reported @%s:%d in %s\n\n", e.detected_file, e.detected_line, e.detected_function);
    }
}

void add_symbol_error(State & s, char const * message, int line, int column, int reported_line, char const * reported_file_name, char const * reported_function) {
    TokenInfo ti = s.tok_cur;
    ti.line = line;
    ti.column = column;
    s.errors.push({ErrorType::SyntaxError, message, s.lexer->get_name(), ti, {}, s.lexer->get_line(line), reported_line, reported_file_name, reported_function });
    report_error(s);
}

void syntax_error_dbg(State & s, AstPtr ast, char const * message, int line = -1, char const * file = 0, char const * function = 0) {
    TokenInfo cur = top(s);
    if(ast && cur.line > ast->line) {
        cur.line = ast->line;
        cur.column = ast->column;
    }
    s.errors.push({ErrorType::SyntaxError, message, s.lexer->get_name(), cur, ast, s.lexer->get_line(cur.line), line, file ? file : "", function ? function : ""});
    report_error(s);
}

#define syntax_error(s, AST_ITEM, msg) syntax_error_dbg(s, error_transform(AST_ITEM), msg, __LINE__, __FILE__, __PRETTY_FUNCTION__)


bool string_to_double(String const & s, double & result) {
    double_conversion::StringToDoubleConverter conv(0, 0.0, 0.0, 0, 0);
    int length = int(s.size());
    int processed = 0;
    result = conv.StringToDouble(s.c_str(), length, &processed);
    return length == processed;
}

bool expression(State &s, AstPtr &target);


typedef bool (*parser_fun_t)(State &, AstPtr &);

bool vector_init(State & s, AstPtr & target, parser_fun_t fun = expression) {
    StateGuard guard(s, target);
    auto ast = create<Vector>(target);
    location(s, ast);
    target = ast;

    if(!expect(s, TokenKind::Less)) {
        return false;
    }

    if(!fun(s, ast->x)) {
        return false;
    }

    if(!expect(s, TokenKind::Comma)) {
        return false;
    }

    if(!fun(s, ast->y)) {
        return false;
    }

    if(!expect(s, TokenKind::Comma)) {
        return false;
    }

    if(!fun(s, ast->z)) {
        return false;
    }

    if(!expect(s, TokenKind::Greater)) {
        return false;
    }

    return guard.commit();
}

bool quaternion_init(State & s, AstPtr & target, parser_fun_t fun = expression){
    StateGuard guard(s, target);
    auto ast = create<Quaternion>(target);
    location(s, ast);
    target = ast;

    if(!expect(s, TokenKind::Less)) {
        return false;
    }

    if(!fun(s, ast->x)) {
        return false;
    }

    if(!expect(s, TokenKind::Comma)) {
        return false;
    }

    if(!fun(s, ast->y)) {
        return false;
    }

    if(!expect(s, TokenKind::Comma)) {
        return false;
    }

    if(!fun(s, ast->z)) {
        return false;
    }


    if(!expect(s, TokenKind::Comma)) {
        return false;
    }

    if(!fun(s, ast->s)) {
        return false;
    }

    if(!expect(s, TokenKind::Greater)) {
        return false;
    }

    return guard.commit();
}

bool list_init(State & s, AstPtr & target, parser_fun_t fun = expression){
    StateGuard guard(s, target);
    auto ast = create<List>(target);
    location(s, ast);
    target = ast;

    if(!expect(s, TokenKind::LeftBracket)) {
        return false;
    }

    AstPtr item;
    while(fun(s, item)) {
        ast->elements.push_back(item);
        item.reset();
        if(!expect(s, TokenKind::Comma)) {
            break;
        }
    }

    if(!expect(s, TokenKind::RightBracket)) {
        return false;
    }

    return guard.commit();
}

bool lvalue(State &s, AstPtr &target) {
    StateGuard guard(s, target);
    auto ast = create<Variable>(target);
    location(s, ast);

    if(!consume_value(s, Token::Identifier, ast->name)) {
        return false;
    }
    if(expect(s, TokenKind::Dot) && !consume_value(s, Token::Identifier, ast->member)) {
        return false;
    }
    return guard.commit();
}


bool integer_constant(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto ast = create<Integer>(target);
    location(s, ast);

    bool minus = expect(s, TokenKind::Minus);

    if(is(s, TokenKind::Number)) {
        MP_INT integ;
        auto value = (minus ? "-" : "") + String(top(s).value);
        int base = value.size() > 2 ? (value[1] == 'x' || value[1] == 'X' ? 16 : 10) : 10;
        mpz_init_set_str(&integ, top(s).value.c_str(), base);
        if(!mpz_fits_slong_p(&integ)) {
            return false;
        }
        ast->value = mpz_get_si(&integ);
        mpz_clear(&integ);
        pop(s);
    }
    else {
        return false;
    }

    return guard.commit();
}

bool float_constant(State & s, AstPtr & target) {
    if(!is(s, Token::NumberFloat)) {
        return false;
    }

    StateGuard guard(s, target);
    auto ast = create<Float>(target);
    location(s, ast);
    bool minus = expect(s, TokenKind::Minus);
    if(!string_to_double((minus ? "-" : "") + top(s).value, ast->value)) {
        return false;
    }
    return guard.commit();
}

bool simple_assignable(State & s, AstPtr & target);
bool simple_assignable_no_list(State & s, AstPtr & target);

bool vector_constant(State & s, AstPtr & target) {
    return vector_init(s, target, simple_assignable);
}

bool quaternion_constant(State & s, AstPtr & target) {
    return quaternion_init(s, target, simple_assignable);
}

bool string_constant(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto ast = create<StringLit>(target);
    location(s, ast);

    if(!consume_value(s, TokenKind::String, ast->value)) {
        return false;
    }

    return guard.commit();
}

bool list_constant(State & s, AstPtr & target) {
    return list_init(s, target, simple_assignable_no_list);
}

bool special_constant(State & s, AstPtr & target) {
    return vector_constant(s, target)
         || quaternion_constant(s, target)
    ;
}

bool boolean_constant(State & s, AstPtr & target) {
    StateGuard guard(s, target);

    if(!is(s, TokenClass::Keyword)) {
        return false;
    }
    auto integer = create<Integer>(target);
    location(s, integer);

    if(expect(s, Token::KeywordTrue)) {
        integer->value = 1;
    }
    else if(expect(s, Token::KeywordFalse)) {
        integer->value = 0;
    }
    else {
        return false;
    }
    return guard.commit();
}

bool constant(State & s, AstPtr & target) {
    return  integer_constant(s, target)
         || float_constant(s, target)
         || string_constant(s, target)
         || boolean_constant(s, target);
    ;
}

bool identifier(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto ast = create<Variable>(target);
    location(s, ast);

    if(!consume_value(s, Token::Identifier, ast->name)) {
        return false;
    }

    if(expect(s, TokenKind::Dot)) {
        if(!consume_value(s, Token::Identifier, ast->member)) {
            return false;
        }
    }

    return guard.commit();
}

bool simple_assignable_no_list(State & s, AstPtr & target) {
    return identifier(s, target)
        || constant(s, target)
        || special_constant(s, target)
        ;
}

bool simple_assignable(State & s, AstPtr & target) {
    return simple_assignable_no_list(s, target)
        || list_constant(s, target)
    ;
}

bool name_type(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto var = create<VarDecl>(target);
    location(s, var);

    if(!consume_value(s, TokenKind::TypeName, var->type_name)) {
        return false;
    }
    if(!consume_value(s, Token::Identifier, var->name)) {
        return false;
    }
    return guard.commit();
}

bool global_variable(State & s, AstPtr & target, bool is_global = true) {
    StateGuard guard(s, target);
    if(name_type(s, target)) {
        if(!is(s, TokenKind::SemiColon)) {
            if(!expect(s, TokenKind::Equal)) {
                return false;
            }
            auto var = ast_cast<VarDecl>(target);
            if(is_global) {
                if(!simple_assignable(s,var->right)) {
                    if(expression(s, var->right)) {
                         if(expect(s, TokenKind::SemiColon)) {
                             syntax_error(s, target, "Global initializer is not a constant");
                             return false;
                         }
                    }
                }
            }
            else {
                if(!expression(s, var->right)) {
                    syntax_error(s, target, "Expected expression right of '='");
                    return false;
                }
            }
        }
    }
    else {
        return false;
    }
    return (!is_global || expect(s, TokenKind::SemiColon)) && guard.commit();
}

bool compound_statement(State &s, std::vector<AstPtr> &target);
bool global_function(State & s, AstPtr & target, bool is_event = false) {
    StateGuard guard(s, target);
    FunctionLike & fun = (is_event ? (FunctionLike &)(*create<Event>(target))
                                   : (FunctionLike&)(*create<Function>(target)));
    location(s, target);

    AstPtr tmp;
    if(!is_event && name_type(s, tmp)) {
        auto nametype = ast_cast<VarDecl>(tmp);
        static_cast<Function&>(fun).name.swap(nametype->name);
        static_cast<Function&>(fun).returnType.swap(nametype->type_name);
    }
    else if(consume_value(s, Token::Identifier, fun.name)) {
        // Nothing todo
    }
    else {
        return false;
    }

    if(!expect(s, TokenKind::LeftParen)) {
        return false;
    }

    do {
        AstPtr tmp;
        if(name_type(s, tmp)) {
            auto var = ast_cast<VarDecl>(tmp);
            TypeDecl type;
            type.name.swap(var->name);
            type.type.swap(var->type_name);
            fun.args.push_back(std::move(type));
        }
        else {
            break;
        }
    } while(expect(s, TokenKind::Comma));

    if(!expect(s, TokenKind::RightParen)) {
        return false;
    }

    return  compound_statement(s, fun.body.statements)
         && guard.commit();
}

bool global(State & s, AstPtr & target) {
    return global_variable(s, target)
        || global_function(s, target);
}


bool lvalue_postop(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto op = create<UnaryOp>(target);
    location(s, op);

    if(!lvalue(s, op->target)) {
        return false;
    }
    if(!expect(s, TokenKind::Increase)) {
        if(!expect(s, TokenKind::Decrease)) {
            return false;
        }
        op->op = AstUnaryOpType::PostDec;
    }
    else {
        op->op = AstUnaryOpType::PostInc;
    }
    return guard.commit();
}

bool call(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto call_  = create<Call>(target);
    location(s, call_);

    if(!consume_value(s, Token::Identifier, call_->name)) {
        return false;
    }
    if(!expect(s, TokenKind::LeftParen)) {
        return false;
    }

    while(!is(s, TokenKind::RightParen)) {
        AstPtr arg;
        if(!expression(s, arg)) {
            return false;
        }
        call_->parameters.push_back(arg);
        if(!expect(s, TokenKind::Comma)) {
            break;
        }
    }

    if(!expect(s, TokenKind::RightParen)) {
        return false;
    }

    return guard.commit();
}

bool unarypostfix_expression(State & s, AstPtr & target) {
    return vector_init(s, target)
        || quaternion_init(s, target)
        || list_init(s, target)
        || call(s, target)
        || constant(s, target)
        || lvalue_postop(s, target)
        || lvalue(s, target)
    ;
}

bool unary_expression(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto ast = create<UnaryOp>(target);
    location(s, ast);

    bool expects_lvalue = false;
    auto unaryOp = AstUnaryOpType();
    switch(top(s).ident.kind()) {
    // '-' expression
    case TokenKind::Minus:
        unaryOp = AstUnaryOpType::Sub;
        break;
    // '+' expression
    case TokenKind::Plus:
        unaryOp = AstUnaryOpType::Add;
        break;
    // '!' expression
    case TokenKind::Not:
        unaryOp = AstUnaryOpType::Not;
        break;
    // '~' expression
    case TokenKind::Tilde:
        unaryOp = AstUnaryOpType::Invert;
        break;
    // '++' lvalue
    case TokenKind::Increase:
        unaryOp = AstUnaryOpType::PreInc;
        expects_lvalue = true;
        break;
    // '--' lvalue
    case TokenKind::Decrease:
        unaryOp = AstUnaryOpType::PreDec;
        expects_lvalue = true;
        break;
    // '(' expression | TypeName ')'
    case TokenKind::LeftParen:
        pop(s);
        if(is(s, TokenKind::TypeName)) {
            // Typecast
            auto cast = create<TypeCast>(target);
            location(s, cast);
            consume_value(s, TokenKind::TypeName, cast->target_type);
            if(!(   lvalue(s, cast->right)
                    ||  constant(s, cast->right)
                    ||  unarypostfix_expression(s, cast->right)
                    ||  expression(s, cast->right)
            ))
            {
                return false;
            }
        }
        else if(!(expression(s, target) && expect(s, TokenKind::RightParen))) {
            return false;
        }
        break;
    default:
        return unarypostfix_expression(s, target) && guard.commit();
    }
    if(unaryOp != AstUnaryOpType::Undefined) {
        if(!((expects_lvalue && lvalue(s, ast->target)))
          || (!expects_lvalue && expression(s, ast->target))) {

        }
    }
    return guard.commit();
}

bool typecast(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto cast = create<TypeCast>(target);
    location(s, cast);

    if(!expect(s, TokenKind::LeftParen)) {
        return false;
    }

    if(!consume_value(s, TokenKind::TypeName, cast->target_type)) {
        return false;
    }

    if(!expect(s, TokenKind::RightParen)) {
        return false;
    }

    return (
            lvalue(s, cast->right)
        ||  constant(s, cast->right)
        || unarypostfix_expression(s, cast->right)
        || ( expect(s, TokenKind::LeftParen) && expression(s, cast->right) && expect(s, TokenKind::RightParen)  )
        )
        &&
    guard.commit();
}

bool assignment(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    AstPtr left;
    if(!lvalue(s, left)) {
        return false;
    }
    if(expect(s, TokenKind::Equal)) {
        auto assign = create<Assignment>(target);
        location(s, assign);
        assign->left = left;
        if(!expression(s, assign->right)) {
            return false;
        }
    }
    else {
        auto augassign = create<AugAssignment>(target);
        location(s, augassign);
        augassign->left = left;
        switch(kind(top(s))) {
        case TokenKind::PlusEqual:
            augassign->op = AstBinOpType::Add;
            break;
        case TokenKind::MinusEqual:
            augassign->op = AstBinOpType::Sub;
            break;
        case TokenKind::StarEqual:
            augassign->op = AstBinOpType::Mult;
            break;
        case TokenKind::SlashEqual:
            augassign->op = AstBinOpType::Div;
            break;
        case TokenKind::BinAndEqual:
            augassign->op = AstBinOpType::BitAnd;
            break;
        case TokenKind::BinOrEqual:
            augassign->op = AstBinOpType::BitOr;
            break;
        case TokenKind::CircumFlexEqual:
            augassign->op = AstBinOpType::BitXor;
            break;
        case TokenKind::PercentEqual:
            augassign->op = AstBinOpType::Mod;
            break;
        case TokenKind::RightShiftEqual:
            augassign->op = AstBinOpType::RightShift;
            break;
        case TokenKind::LeftShiftEqual:
            augassign->op = AstBinOpType::LeftShift;
            break;
        default:
            return false;
        }
        if(!expression(s, augassign->right)) {
            return false;
        }
    }

    return guard.commit();
}

bool comparison(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto comp = create<Comparison>(target);
    location(s, comp);

    if(!expression(s, comp->left)) {
        return false;
    }

    if(    expect(s, TokenKind::EqualEqual)
        || expect(s, TokenKind::NotEqual)
        || expect(s, TokenKind::Less)
        || expect(s, TokenKind::Greater)
        || expect(s, TokenKind::LessEqual)
        || expect(s, TokenKind::GreaterEqual)
    ) {
        if(!expression(s, comp->right)) {
            return false;
        }
    }
    else {
        return false;
    }

    return guard.commit();
}

bool binary_expr(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto binop = create<BinOp>(target);
    location(s, binop);

    if(!expression(s, binop->left)) {
        return false;
    }

    switch(kind(top(s))) {
    case TokenKind::Minus:
        binop->op = AstBinOpType::Add;
        break;
    case TokenKind::Plus:
        binop->op = AstBinOpType::Sub;
        break;
    case TokenKind::Slash:
        binop->op = AstBinOpType::Div;
        break;
    case TokenKind::Star:
        binop->op = AstBinOpType::Mult;
        break;
    case TokenKind::Percent:
        binop->op = AstBinOpType::Mod;
        break;
    case TokenKind::CircumFlex:
        binop->op = AstBinOpType::BitXor;
        break;
    case TokenKind::BinAnd:
        binop->op = AstBinOpType::BitAnd;
        break;
    case TokenKind::BinOr:
        binop->op = AstBinOpType::BitOr;
        break;
    case TokenKind::LeftShift:
        binop->op = AstBinOpType::LeftShift;
        break;
    case TokenKind::RightShift:
        binop->op = AstBinOpType::RightShift;
        break;
    default:
        return false;
    }

    if(!expression(s, binop->right)) {
        return false;
    }

    return guard.commit();
}

bool expression(State & s, AstPtr & target) {
    return     assignment(s, target)
            || unary_expression(s, target)
            || binary_expr(s, target)
            || comparison(s, target)
            ;
}

bool expr_stmt(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    return expression(s, target)
        && expect(s, TokenKind::SemiColon)
        && guard.commit();
}

bool forexpressionlist(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto lst = create<ExpressionList>(target);
    location(s, lst);

    AstPtr item;
    if(!expression(s, item)) {
        clone_location(lst, create<NoOp>(target));
    }
    else if(is(s, TokenKind::Comma)){
        lst->expressions.push_back(item);
        while(expect(s, TokenKind::Comma)) {
            if(!expression(s, item)) {
                syntax_error(s, item, "Expected expression after ','");
                return false;
            }
            lst->expressions.push_back(item);
        }
    }
    else {
        target = item;
    }
    return guard.commit();
}

bool statement(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    location(s, create<NoOp>(target));
    bool needsSemiColon = false;
    if(expect(s, Token::KeywordState)) {
        auto statech = create<StateChange>(target);
        location(s, statech);
        if(!consume_value(s, Token::Identifier, statech->state)) {
            return false;
        }
        needsSemiColon = true;
    }
    else if(expect(s, Token::KeywordJump)) {
        auto jump = create<Jump>(target);
        location(s, jump);
        if(!consume_value(s, Token::Identifier, jump->label)) {
            return false;
        }
        needsSemiColon = true;
    }
    else if(expect(s, TokenKind::At)) {
        auto label = create<Label>(target);
        location(s, label);
        if(!consume_value(s, Token::Identifier, label->name)) {
            return false;
        }
        needsSemiColon = true;
    }
    else if(expect(s, Token::KeywordReturn)) {
        auto retn = create<Return>(target);
        location(s, retn);
        expression(s, retn->value);
        needsSemiColon = true;
    }
    else if(global_variable(s, target, false)) {
        needsSemiColon = true;
    }
    else if(expression(s, target)) {
        needsSemiColon = true;
    }
    else if(is(s, TokenKind::LeftBrace)) {
        auto bdy = create<Body>(target);
        location(s, bdy);
        if(!compound_statement(s, bdy->statements)) {
            return false;
        }
        needsSemiColon = false;
    }
    else if(expect(s, Token::KeywordIf)) {
        auto if_ = create<If>(target);
        location(s, if_);

        if(!expect(s, TokenKind::LeftParen)) {
            return false;
        }

        if(!expression(s, if_->condition)) {
            return false;
        }

        if(!expect(s, TokenKind::RightParen)) {
            return false;
        }

        if(!statement(s, if_->body)) {
            return false;
        }

        if(expect(s, Token::KeywordElse)) {
            if(!statement(s, if_->elseIf)) {
                return false;
            }
        }
    }
    else if(expect(s, Token::KeywordDo)) {
        auto do_ = create<Do>(target);
        location(s, do_);

        if(!statement(s, do_->body)) {
            return false;
        }

        if(expect(s, Token::KeywordWhile)) {
            if(!expect(s, TokenKind::LeftParen)) {
                return false;
            }

            if(!expression(s, do_->condition)) {
                return false;
            }

            if(!expect(s, TokenKind::RightParen)) {
                return false;
            }
        }
    }
    else if(expect(s, Token::KeywordWhile)) {
        auto while_ = create<While>(target);
        location(s, while_);

        if(!expect(s, TokenKind::LeftParen)) {
            return false;
        }

        if(!expression(s, while_->condition)) {
            return false;
        }

        if(!expect(s, TokenKind::RightParen)) {
            return false;
        }

        if(!statement(s, while_->body)) {
            return false;
        }
    }
    else if(expect(s, Token::KeywordFor)) {
        auto for_ = create<For>(target);
        location(s, for_);

        if(!expect(s, TokenKind::LeftParen)) {
            syntax_error(s, for_, "Expected '('");
            return false;
        }

        if(!forexpressionlist(s, for_->init)) {
            return false;
        }

        if(!expect(s, TokenKind::SemiColon)) {
            syntax_error(s, for_, "Expected ';' after 'for' init");
            return false;
        }

        if(!expression(s, for_->condition)) {
            return false;
        }

        if(!expect(s, TokenKind::SemiColon)) {
            syntax_error(s, for_, "Expected ';' after 'for' condition");
            return false;
        }

        if(!forexpressionlist(s, for_->step)) {
            return false;
        }

        if(!expect(s, TokenKind::RightParen)) {
            syntax_error(s, for_, "Expected ')'");
            return false;
        }

        if(!statement(s, for_->body)) {
            return false;
        }
    }

    if(needsSemiColon && !expect(s, TokenKind::SemiColon)) {
        syntax_error(s, target, "Expected ';'");
        return false;
    }

    return guard.commit();
}

bool globals(State & s, Globals & target) {
    StateGuard guard(s);
    while(!is(s, Token::KeywordDefault)) {
        AstPtr glob;
        if(!global(s, glob)) {
            return false;
        }
        target.globals.push_back(glob);
    }
    return guard.commit();
}

bool state_body(State & s, std::vector<Event> & events) {
    StateGuard guard(s);

    if(!expect(s, TokenKind::LeftBrace)) {
        return false;
    }

    while(!expect(s, TokenKind::RightBrace)) {
        AstPtr evt;
        if(!global_function(s, evt, true)) {
            return false;
        }
        events.push_back(*ast_cast<Event>(evt));
    }

    return guard.commit();
}

bool default_state(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto ste = create<StateDef>(target);
    location(s, ste);

    if(!consume_value(s, Token::KeywordDefault, ste->name)) {
        return false;
    }

    return state_body(s, ste->events)
        && guard.commit();
}

bool state(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto ste = create<StateDef>(target);
    location(s, ste);

    if(!expect(s, Token::KeywordState)) {
        syntax_error(s, ste, "Expected keyword state");
        return false;
    }

    if(!consume_value(s, Token::Identifier, ste->name)) {
        syntax_error(s, ste, "Expected identifier for 'state'");
        return false;
    }
    return guard.commit();
}

bool states(State & s, States & states) {
    StateGuard guard(s);
    location(s, states);

    AstPtr defstate;
    if(!default_state(s, defstate)) {
        return false;
    }
    states.states.push_back(defstate);

    while(!is(s, Token::End)) {
        AstPtr ste;
        if(!state(s, ste)) {
            return false;
        }
        states.states.push_back(ste);
    }
    return guard.commit();
}

bool compound_statement(State & s, std::vector<AstPtr> & stmts) {
    StateGuard guard(s);
    if(!expect(s, TokenKind::LeftBrace)) {
        return false;
    }

    while(!is(s, TokenKind::RightBrace)) {
        AstPtr item;
        if(!statement(s, item)) {
            return false;
        }
        stmts.push_back(item);
    }

    if(!expect(s, TokenKind::RightBrace)) {
        return false;
    }

    return guard.commit();
}

bool script(State & s, Script & result) {
    StateGuard guard(s);
    globals(s, result.globals);
    if(!states(s, result.states)) {
        return false;
    }
    return guard.commit();
}

int main()
{
    Lexer lex("/home/vfeenstr/devel/apps/secondlife/lsl-alt/main.lsl");
    std::vector<AstPtr> result;
    State state{&lex, {}, {}, {}, lex.next(), {}, {}};

    Script scr;
    bool parse_result = script(state, scr);
    printf("Last line: %ld\nSuccess: %s\n", lex.next().line, parse_result ? "yes" : "no");
    return 0;
}


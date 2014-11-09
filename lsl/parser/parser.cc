#include <lsl/ast/ast.hh>
#include <lsl/lexer/lexer.hh>
#include <lsl/parser/state.hh>
#include <boost/multiprecision/cpp_int.hpp>
#include <double-conversion/src/double-conversion.h>

namespace lsl {

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
    s.errors.push({ErrorType::SyntaxError, message, s.lexer->get_name(), ti, {}, s.lexer->get_line(line), reported_line, reported_file_name, reported_function});
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

#ifdef _MSC_VER
#define syntax_error(s, AST_ITEM, msg) syntax_error_dbg(s, error_transform(AST_ITEM), msg, __LINE__, __FILE__, __FUNCSIG__)
#else
#define syntax_error(s, AST_ITEM, msg) syntax_error_dbg(s, error_transform(AST_ITEM), msg, __LINE__, __FILE__, __PRETTY_FUNCTION__)
#endif

bool string_to_double(String const & s, double & result) {
    double_conversion::StringToDoubleConverter conv(0, 0.0, 0.0, 0, 0);
    int length = int(s.size());
    int processed = 0;
    result = conv.StringToDouble(s.c_str(), length, &processed);
    return length == processed;
}

bool expression(State &s, AstPtr &target);


typedef bool(*parser_fun_t)(State &, AstPtr &);

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

    auto value =
                 (minus ? "-" : "")
               + (
                        (token(top(s)) == Token::NumberHex ? "0x" : "")
                    +   String(top(s).value)
                 );

    if(is(s, TokenKind::Number)) {
        try {
            boost::multiprecision::checked_int128_t integ(value);
            ast->value = static_cast<decltype(ast->value)>(integ);
        }
        catch(std::runtime_error const &) {
            return false;
        }
        pop(s);
    }
    else {
        return false;
    }

    return guard.commit();
}

bool float_constant(State & s, AstPtr & target) {
    StateGuard guard(s, target);

    bool minus = expect(s, TokenKind::Minus);
    if(!is(s, Token::NumberFloat)) {
        return false;
    }

    auto ast = create<Float>(target);
    location(s, ast);
    if(!string_to_double((minus ? "-" : "") + top(s).value, ast->value)) {
        return false;
    }
    pop(s);
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
    return integer_constant(s, target)
        || boolean_constant(s, target)
        || float_constant(s, target)
        || string_constant(s, target)
    ;
}

bool identifier(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto ast = create<Variable>(target);
    location(s, ast);

    if(!consume_value(s, Token::Identifier, ast->name)) {
        return false;
    }

    return guard.commit();
}

bool simple_assignable_no_list(State & s, AstPtr & target) {
    return identifier(s, target)
        || constant(s, target)
        || special_constant(s, target)
        ;
}

bool cast_or_expression(State & s, AstPtr & target, parser_fun_t cast_inner_fun, parser_fun_t other) {
    StateGuard guard(s, target);
    if(expect(s, TokenKind::LeftParen)) {
        String name;
        if(consume_value(s, TokenKind::TypeName, name)) {
            if(!expect(s, TokenKind::RightParen)) {
                return false;
            }
            auto cast = create<TypeCast>(target);
            location(s, cast);
            cast->target_type.swap(name);
            if(!cast_inner_fun(s, cast->right)) {
                return false;
            }
            return guard.commit();
        }
        else {
            if(other(s, target) && expect(s, TokenKind::RightParen)) {
                return guard.commit();
            }
        }
    }
    return false;
}

bool simple_assignable(State & s, AstPtr & target) {
    return cast_or_expression(s, target, simple_assignable, simple_assignable)
        || simple_assignable_no_list(s, target)
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

bool global_variable(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    if(!name_type(s, target)) {
        return false;
    }
    if(expect(s, TokenKind::Equal)) {
        auto var = ast_cast<VarDecl>(target);
        if(!simple_assignable(s, var->right)) {
            return false;
        }
    }
    return expect(s, TokenKind::SemiColon) && guard.commit();
}

bool declaration(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    if(!name_type(s, target)) {
        return false;
    }

    if(expect(s, TokenKind::Equal)) {
        auto var = ast_cast<VarDecl>(target);
        if(!expression(s, var->right)) {
            return false;
        }
    }

    return guard.commit();
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


bool call(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    auto call_ = create<Call>(target);
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

bool postfix_expr(State &s, AstPtr &target) {
    StateGuard guard(s, target);
    AstPtr tmp;
    if(call(s, tmp)) {
        target = tmp;
    }
    else if(lvalue(s, tmp)) {
        if(cls(top(s)) == TokenClass::Operator) {
            auto post = create<UnaryOp>(target);
            post->target = tmp;
            if(expect(s, TokenKind::Increase)) {
                post->op = AstUnaryOpType::PostInc;
            }
            else if(expect(s, TokenKind::Decrease)) {
                post->op = AstUnaryOpType::PostDec;
            }
            else {
                target.reset();
            }
        }
        // Fallback -> just an lvalue
        if(!target) {
            target = tmp;
        }
    }
    else if(list_init(s, tmp)) {
        target = tmp;
    }
    else if(constant(s, tmp)) {
        target = tmp;
    }
    else if(vector_init(s, tmp)) {
        target = tmp;
    }
    else if(quaternion_init(s, tmp)) {
        target = tmp;
    }
    else {
        return false;
    }
    return bool(target) && guard.commit();
}

bool unary_expr(State &s, AstPtr &target) {
    StateGuard guard(s, target);
    if(cls(top(s)) == TokenClass::Operator) {
        auto unary = create<UnaryOp>(target);
        location(s, unary);
        switch(kind(top(s))) {
        case TokenKind::Increase:
            unary->op = AstUnaryOpType::PreInc;
            break;
        case TokenKind::Decrease:
            unary->op = AstUnaryOpType::PreDec;
            break;
        case TokenKind::Not:
            unary->op = AstUnaryOpType::Not;
            break;
        case TokenKind::Tilde:
            unary->op = AstUnaryOpType::Invert;
            break;
        case TokenKind::Minus:
            unary->op = AstUnaryOpType::Sub;
            break;
        default:
            return postfix_expr(s, target) && guard.commit();
        }
        pop(s);
        return unary_expr(s, unary->target) && guard.commit();
    }
    else {
        return cast_or_expression(s, target, unary_expr, expression)
            && guard.commit();
    }
    return false;
}

bool multiplicative_expr(State &s, AstPtr &target) {
    StateGuard guard(s, target);
    auto binop = create<BinOp>(target);
    location(s, binop);

    if(unary_expr(s, binop->left)) {
        switch(kind(top(s))) {
            case TokenKind::Star:
                binop->op = AstBinOpType::Mult;
                break;
            case TokenKind::Slash:
                binop->op = AstBinOpType::Div;
                break;
            case TokenKind::Percent:
                binop->op = AstBinOpType::Mod;
                break;
            default:
                target = binop->left;
                return guard.commit();
        }
        pop(s);
        return multiplicative_expr(s, binop->right) && guard.commit();
    }
    return false;
}

bool additive_expr(State &s, AstPtr &target) {
    StateGuard guard(s, target);
    auto binop = create<BinOp>(target);
    location(s, binop);

    if(multiplicative_expr(s, binop->left)) {
        switch(kind(top(s))) {
        case TokenKind::Plus:
            binop->op = AstBinOpType::Add;
            break;
        case TokenKind::Minus:
            binop->op = AstBinOpType::Sub;
            break;
        default:
            target = binop->left;
            return guard.commit();
        }
        pop(s);
        return additive_expr(s, binop->right) && guard.commit();
    }
    return false;
}

bool shift_expr(State &s, AstPtr &target) {
    StateGuard guard(s, target);
    auto binop = create<BinOp>(target);
    location(s, binop);

    if(additive_expr(s, binop->left)) {
        switch(kind(top(s))) {
        case TokenKind::LeftShift:
            binop->op = AstBinOpType::LeftShift;
            break;
        case TokenKind::RightShift:
            binop->op = AstBinOpType::RightShift;
            break;
        default:
            target = binop->left;
            return guard.commit();
        }
        pop(s);
        return shift_expr(s, binop->right) && guard.commit();
    }
    return false;
}

bool relational_expr(State &s, AstPtr &target) {
    StateGuard guard(s, target);
    auto comp = create<Comparison>(target);
    location(s, comp);

    if(shift_expr(s, comp->left)) {
        switch(kind(top(s))) {
        case TokenKind::Less:
            comp->op = AstCompareOpType::Less;
            break;
        case TokenKind::Greater:
            comp->op = AstCompareOpType::Greater;
            break;
        case TokenKind::LessEqual:
            comp->op = AstCompareOpType::LessEquals;
            break;
        case TokenKind::GreaterEqual:
            comp->op = AstCompareOpType::GreaterEquals;
            break;
        default:
            target = comp->left;
            return guard.commit();
        }
        pop(s);
        if(!relational_expr(s, comp->right)) {
            target = comp->left;
            unpop(s);
        }
        return guard.commit();
    }
    return false;
}

bool equality_expr(State &s, AstPtr &target) {
    StateGuard guard(s, target);
    auto comp = create<Comparison>(target);
    location(s, comp);

    if(relational_expr(s, comp->left)) {
        switch(kind(top(s))) {
        case TokenKind::EqualEqual:
            comp->op = AstCompareOpType::Equals;
            break;
        case TokenKind::NotEqual:
            comp->op = AstCompareOpType::NotEquals;
            break;
        default:
            target = comp->left;
            return guard.commit();
        }
        pop(s);
        return equality_expr(s, comp->right) && guard.commit();
    }
    return false;
}

bool bitwise_and_expr(State &s, AstPtr &target) {
    StateGuard guard(s, target);
    auto binop = create<BinOp>(target);
    location(s, binop);

    if(equality_expr(s, binop->left)) {
        if(!expect(s, TokenKind::BinAnd)) {
            target = binop->left;
        }
        else {
            binop->op = AstBinOpType::BitAnd;
            if(!bitwise_and_expr(s, binop->right)) {
                return false;
            }
        }
    }
    else {
        return false;
    }
    return guard.commit();
}

bool bitwise_xor_expr(State &s, AstPtr &target) {
    StateGuard guard(s, target);
    auto binop = create<BinOp>(target);
    location(s, binop);

    if(bitwise_and_expr(s, binop->left)) {
        if(!expect(s, TokenKind::CircumFlex)) {
            target = binop->left;
        }
        else {
            binop->op = AstBinOpType::BitXor;
            if(!bitwise_xor_expr(s, binop->right)) {
                return false;
            }
        }
    }
    else {
        return false;
    }
    return guard.commit();
}

bool bitwise_or_expr(State &s, AstPtr &target) {
    StateGuard guard(s, target);
    auto binop = create<BinOp>(target);
    location(s, binop);

    if(bitwise_xor_expr(s, binop->left)) {
        if(!expect(s, TokenKind::BinOr)) {
            target = binop->left;
        }
        else {
            binop->op = AstBinOpType::BitOr;
            if(!bitwise_or_expr(s, binop->right)) {
                return false;
            }
        }
    }
    else {
        return false;
    }
    return guard.commit();
}

bool logical_expr(State &s, AstPtr &target) {
    StateGuard guard(s, target);
    auto boolop = create<BoolOp>(target);
    location(s, boolop);
    boolop->op = AstBoolOpType::Undefined;

    if(bitwise_or_expr(s, boolop->left)) {
        if(expect(s, TokenKind::BoolOr)) {
            boolop->op = AstBoolOpType::Or;
        }
        else if(expect(s, TokenKind::BoolAnd)) {
            boolop->op = AstBoolOpType::And;
        }
        if(boolop->op != AstBoolOpType::Undefined) {
            if(!logical_expr(s, boolop->right)) {
                return false;
            }
        }
        else {
            target = boolop->left;
        }
    }
    else {
        return false;
    }
    return guard.commit();
}

bool assignment_expr(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    AstPtr tmp;
    if(logical_expr(s, tmp)) {
        if(expect(s, TokenKind::Equal)) {
            auto assign = create<Assignment>(target);
            location(s, assign);
            assign->left = tmp;
            return assignment_expr(s, assign->right) && guard.commit();
        }
        else {
            auto assign = create<AugAssignment>(target);
            location(s, assign);
            assign->left = tmp;

            switch(kind(top(s))) {
            case TokenKind::PlusEqual:
                assign->op = AstBinOpType::Add;
                break;
            case TokenKind::MinusEqual:
                assign->op = AstBinOpType::Sub;
                break;
            case TokenKind::StarEqual:
                assign->op = AstBinOpType::Mult;
                break;
            case TokenKind::SlashEqual:
                assign->op = AstBinOpType::Div;
                break;
            case TokenKind::PercentEqual:
                assign->op = AstBinOpType::Mod;
                break;
            default:
                target = tmp;
                return guard.commit();
            }
            pop(s);
            return assignment_expr(s, assign->right) && guard.commit();
        }
    }
    else {
        return false;
    }
    return guard.commit();
}

bool expression(State &s, AstPtr &target) {
    return assignment_expr(s, target);
}

bool statement(State & s, AstPtr & target) {
    StateGuard guard(s, target);
    location(s, create<NoOp>(target));

    if(is(s, TokenKind::SemiColon)) {
        return guard.commit();
    }

    bool needsSemiColon = false;
    if(expect(s, Token::KeywordState)) {
        auto statech = create<StateChange>(target);
        location(s, statech);
        if(expect(s, Token::KeywordDefault)) {
            statech->state = "default";
        }
        else if(!consume_value(s, Token::Identifier, statech->state)) {
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
    else if(declaration(s, target)) {
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
    else {
        return false;
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

    return state_body(s, ste->events)
        && guard.commit();
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

bool parse(Lexer & lexer,
           Script & ast,
           ParserOptions options /* = ParserOptions() */) {
    State state{&lexer, {}, {}, {}, lexer.next(), {}, {}};
    return script(state, ast);
}

bool parse(Lexer & lexer,
           AstPtr & ast,
           ParserOptions options /* = ParserOptions()*/) {
    auto scr = create<Script>(ast);
    return parse(lexer, *scr, options);
}

}

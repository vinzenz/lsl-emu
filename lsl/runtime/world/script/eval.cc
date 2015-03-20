#include <lsl/runtime/world/script/eval.hh>
#include <lsl/runtime/world/script.hh>
#include <lsl/runtime/world/script/error.hh>
#include <lsl/ast/api.hh>
#include <cassert>
#include <unordered_map>
namespace lsl {
namespace runtime {
namespace script {

// TODO: Improve variable usage = Currently only one local stack is available

ScriptValue deref(ScriptValue val) {
    if(!val.reference) {
        return val;
    }
    return deref(val.get_ref().get());
}

struct Scope;
typedef std::function<CallResult(Scope &)> CompiledFunction;
struct CompiledExpression {
    ValueType result_type;
    bool reference;
    CompiledFunction compiled;
};

struct ScopeVariables {
    typedef boost::optional<boost::reference_wrapper<ScopeVariables>> reference_type;
    reference_type parent_;
    std::unordered_map<String, ScriptValue> storage_;

    ScopeVariables(boost::optional<boost::reference_wrapper<ScopeVariables>> parent = {})
    : parent_(parent)
    , storage_()
    {}

    size_t count(String const & name) {
        size_t cnt = storage_.count(name);
        if(cnt == 0 && parent_) {
            return parent_.get().get().count(name);
        }
        return cnt;
    }

    size_t count_local(String const & name) {
        return storage_.count(name);
    }

    ScriptValue & local(String const & name) {
        return storage_[name];
    }

    ScriptValue & operator[](String const & name) {
        if(storage_.count(name) || !count(name)) {
            return storage_[name];
        }
        return parent_.get().get()[name];
    }
};

struct Scope {

    Scope(Ast const & ast, ScriptRef script, boost::optional<boost::reference_wrapper<Scope>> parent = {})
    : parent(parent)
    , script(script)
    , locals(parent ? boost::ref(parent.get().get().locals) : ScopeVariables::reference_type())
    , label_position()
    , statements()
    , ast(ast)
    , return_type(parent ? parent.get().get().return_type : "")
    {}

    boost::optional<boost::reference_wrapper<Scope>> parent;
    ScriptRef script; // For globals and functions and changing state
    ScopeVariables locals;
    std::unordered_map<String, size_t>      label_position;
    std::vector<CompiledExpression>         statements;
    Ast const & ast;
    String return_type;
};

struct float_extractor : boost::static_visitor < ScriptValue::float_type& > {
    ScriptValue::float_type & operator()(boost::reference_wrapper<ScriptValue::float_type> f) const {
        return f.get();
    }

    ScriptValue::float_type & operator()(boost::reference_wrapper<ScriptValue> r) const {
        return r.get().get_float();
    }

    template< typename T >
    ScriptValue::float_type & operator()(T) const {
        static ScriptValue::float_type f = 0.;
        throw ScriptRuntimeError("Invalid instruction");
        return f;
    }
};

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::AstPtr a);
CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::Ast const & a);

CompiledExpression eval_expr(ScriptRef, Scope &, lsl::NoOp const &) {
    return {
        ValueType::Void,
        false,
        [](Scope &) -> CallResult {
            return CallResult();
        }
    };
}

bool valid_key(String const & key) {
    if(key.length() != 36) {
        return false;
    }
    for(size_t i = 0; i < key.size(); ++i) {
        char x = key[i];
        switch(i) {
        case 8:
        case 13:
        case 18:
        case 23:
        if(x != '-')
            return false;
        break;
        default:
        if((x >= 'a' && x <= 'f') || (x >= 'A' && x <= 'F') || (x >= '0' && x <= '9')) {
            break;
        }
        return false;
        }
    }
    return true;
}

bool as_condition(ScriptValue & value) {
    switch(value.type) {
    case ValueType::Float:
    case ValueType::Integer:
    return value.as_integer() != 0;
    case ValueType::String:
    return !value.get_string().empty();
    case ValueType::List:
    return !value.get_list().empty();
    case ValueType::Key:
    return value.get_string() != "00000000-0000-0000-0000-000000000000";
    case ValueType::Rotation:
    return value.get_rotation() != Quaternion{0., 0., 0., 1.};
    case ValueType::Vector:
    return !(value.get_vector() == Vector{0., 0., 0.});
    default:
    assert(false && "Type?");
    break;
    }
    return false;
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::Call const & v) {
    auto fun = ScriptFunctionPtr();
    if(script.get().functions.count(v.name)) {
        fun = script.get().functions[v.name];
    }
    else if(script.get().library.functions.count(v.name)) {
        fun = script.get().library.functions[v.name];
    }
    else {
        throw ScriptError(format("Call to unknown function '%s'", v.name.c_str()), v.line, v.column);
    }

    std::vector<CompiledExpression> arguments;
    arguments.reserve(v.parameters.size());
    for(auto const & arg : v.parameters) {
        arguments.emplace_back(std::move(eval_expr(script, scope, arg)));
        if(arguments.size() > fun->compiled.parameters.size()) {
            throw ScriptError(format("Too many parameters for function '%s'", v.name.c_str()), v.line, v.column);
        }
        if(arguments.back().result_type != fun->compiled.parameters[arguments.size() - 1]) {
            throw ScriptError(format("Type mismatch on function call to '%s'", v.name.c_str()), v.line, v.column);
        }
    }
    if(arguments.size() < fun->compiled.parameters.size()) {
        throw ScriptError(format("Not enough parameters for function '%s'", v.name.c_str()), v.line, v.column);
    }

    return {
        fun->compiled.result_type,
        false,
        [fun, arguments, v](Scope & scope) -> CallResult {
            auto call_args = ScriptValue::list_type();
            call_args.reserve(arguments.size());
            for(auto const & arg : arguments) {
                call_args.emplace_back(std::move(arg.compiled(scope).get()));
            }
            auto res = fun->call(ScriptFunctionCall{scope.script, call_args});
            return res;
        }
    };
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::If const & v) {
    auto condition = eval_expr(script, scope, v.condition);
    auto child_scope1 = Scope(v, script, boost::ref(scope));
    auto body = eval_expr(script, child_scope1, v.body);
    auto child_scope2 = Scope(v, script, boost::ref(scope));
    auto elseif = v.elseIf ? eval_expr(script, child_scope2, v.elseIf) : CompiledExpression();
    return{
        ValueType::Void,
        false,
        [condition, body, elseif, v](Scope & scope) {
            if(as_condition(condition.compiled(scope).get())) {
                auto child_scope = Scope(v, scope.script, boost::ref(scope));
                body.compiled(child_scope);
            }
            else if(elseif.compiled) {
                auto child_scope = Scope(v, scope.script, boost::ref(scope));
                elseif.compiled(child_scope);
            }
            return CallResult();
        }
    };
}


CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::For const & v) {
    auto init = v.init ? eval_expr(script, scope, v.init) : CompiledExpression();
    auto condition = eval_expr(script, scope, v.condition);
    auto step = v.step ? eval_expr(script, scope, v.step) : CompiledExpression();
    auto for_scope = Scope(v, script, boost::ref(scope));
    auto body = eval_expr(script, for_scope, v.body);
    return{
        ValueType::Void,
        false,
        [init, condition, step, body, v](Scope & scope) -> CallResult{
            if(init.compiled) init.compiled(scope);
            for(; as_condition(condition.compiled(scope).get());) {
                auto for_scope = Scope(v, scope.script, boost::ref(scope));
                body.compiled(for_scope);
                if(step.compiled) step.compiled(scope);
            }
            return CallResult();
        }
    };
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::Do const & v) {
    auto condition = eval_expr(script, scope, v.condition);
    auto child_scope = Scope(v, script, boost::ref(scope));
    auto body = eval_expr(script, child_scope, v.body);
    return{
        ValueType::Void,
        false,
        [condition, body, v](Scope & scope) -> CallResult{
            do {
                auto child_scope = Scope(v, scope.script, boost::ref(scope));
                body.compiled(child_scope);
            } while(as_condition(condition.compiled(scope).get()));
            return CallResult();
        }
    };
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::While const & v) {
    auto condition = eval_expr(script, scope, v.condition);
    auto child_scope = Scope(v, script, boost::ref(scope));
    auto body = eval_expr(script, child_scope, v.body);
    return{
        ValueType::Void,
        false,
        [condition, body, v](Scope & scope) -> CallResult{
            while(as_condition(condition.compiled(scope).get())) {
                auto child_scope = Scope(v, scope.script, boost::ref(scope));
                body.compiled(child_scope);
            }
            return CallResult();
        }
    };
}

void init_with_type(ScriptValue & value, String const & type_name) {
    if(type_name == "list") {
        value.value = ScriptValue::list_type();
        value.type = ValueType::List;
    }
    else if(type_name == "key") {
        value.value = ScriptValue::string_type();
        value.type = ValueType::Key;
    }
    else if(type_name == "string") {
        value.value = ScriptValue::string_type();
        value.type = ValueType::String;
    }
    else if(type_name == "vector") {
        value.value = ScriptValue::vector_type();
        value.type = ValueType::Vector;
    }
    else if(type_name == "rotation" || type_name == "quaternion") {
        value.value = ScriptValue::rotation_type();
        value.type = ValueType::Rotation;
    }
    else if(type_name == "float") {
        value.value = ScriptValue::float_type();
        value.type = ValueType::Float;
    }
    else if(type_name == "integer") {
        value.value = ScriptValue::integer_type();
        value.type = ValueType::Integer;
    }
    else {
        throw ScriptError(format("Unknown type '%s'", type_name.c_str()));
    }
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::TypeCast const & v) {
    ScriptValue value;
    value.reference = false;
    init_with_type(value, v.target_type);
    auto right = eval_expr(script, scope, v.right);
    return{
        value.type,
        false,
        [value, right](Scope & scope) -> CallResult {
            auto exec = right.compiled(scope).get();
            auto result = ScriptValue(value);
            switch(value.type) {
            case ValueType::List:
                result.value = exec.as_list();
                break;
            case ValueType::Rotation:
                result.value = exec.as_rotation();
                break;
            case ValueType::Integer:
                result.value = exec.as_integer();
                break;
            case ValueType::Float:
                result.value = exec.as_float();
                break;
            case ValueType::String:
                result.value = exec.as_string();
                break;
            case ValueType::Key:
                result.value = exec.as_key();
                break;
            case  ValueType::Vector:
                result.value = exec.as_vector();
                break;
            default:
                throw ScriptRuntimeError("Invalid instruction");
            }
            return result;
        }
    };
}

CompiledExpression eval_expr(ScriptRef /*script*/, Scope & /*scope*/, lsl::Jump const & /*v*/) {
    throw ScriptError("Jump & Labels are not implemented");
    return{
        ValueType::Void,
        false,
        CompiledFunction()
    };
}

CompiledExpression eval_expr(ScriptRef /*script*/, Scope & /*scope*/, lsl::Label const & /*v*/) {
    throw ScriptError("Jump & Labels are not implemented");
    return{
        ValueType::Void,
        false,
        CompiledFunction()
    };
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::ExpressionList const & v) {
    std::vector<CompiledExpression> expressions;
    expressions.reserve(v.expressions.size());
    for(size_t i = 0; i < v.expressions.size(); ++i) {
        expressions.emplace_back(std::move(eval_expr(script, scope, v.expressions[i])));
    }
    return{
        ValueType::Void,
        false,
        [expressions](Scope & scope) -> CallResult {
            for(size_t i = 0; i < expressions.size(); ++i) {
                expressions[i].compiled(scope);
            }
            return CallResult();
        }
    };
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::VarDecl const & v) {

    String name = v.name;
    if(scope.locals.count_local(name)) {
        throw ScriptError(format("Name '%s' previously declared within scope", v.name.c_str()), v.line, v.column);
    }

    ScriptValue value;
    value.reference = false;
    init_with_type(value, v.type_name);
    scope.locals.local(name) = value;

    if(v.right) {
        auto right = eval_expr(script, scope, v.right);
        bool flt = right.result_type == ValueType::Float;
        bool i = right.result_type == ValueType::Integer;
        bool k = right.result_type == ValueType::Key;
        bool s = right.result_type == ValueType::String;
        if(!i && !flt && !s && !k && right.result_type != value.type) {
            throw ScriptError("Type mismatch", v.line, v.column);
        }
        return{
            ValueType::Void,
            false,
            [right, value, name](Scope & scope) -> CallResult {
                bool flt = right.result_type == ValueType::Float;
                bool i = right.result_type == ValueType::Integer;
                bool k = right.result_type == ValueType::Key;
                bool s = right.result_type == ValueType::String;
                auto rvalue = right.compiled(scope).get();

                scope.locals.local(name) = value;
                if(!flt && value.type == ValueType::Float) {
                    scope.locals.local(name).value = rvalue.as_float();
                }
                else if(!i && value.type == ValueType::Integer) {
                    scope.locals.local(name).value = rvalue.as_integer();
                }
                else if(!s && value.type == ValueType::String) {
                    scope.locals.local(name).value = rvalue.as_string();
                }
                else if(!k && value.type == ValueType::Key) {
                    scope.locals.local(name).value = rvalue.as_key();
                }
                else {
                    if(rvalue.reference) {
                        scope.locals.local(name)= rvalue.get_ref().get();
                    } else {
                        scope.locals.local(name).value = rvalue.value;
                    }
                }
                return CallResult();
            }
        };
    }
    return{
        ValueType::Void,
        false,
        [value, name](Scope & scope) -> CallResult {
            scope.locals.local(name) = value;
            return CallResult();
        }
    };
    // scope.locals.local(v.name)
    // scope.local(v.name);

}

CompiledExpression eval_expr(ScriptRef script, Scope & /*scope*/, lsl::StateChange const & v) {
    String state = v.state;
    if(!script.get().states.count(state)) {
        throw ScriptError(format("state '%s' not defined", state.c_str()), v.line, v.column);
    }
    return CompiledExpression{
        ValueType::Void,
        false,
        [state](Scope &) -> CallResult {
            throw StateChangeException(state);
            return CallResult();
        }
    };
}

template<typename T, typename U>
ScriptValue::integer_type do_compare(AstCompareOpType op, T left, U right) {
    switch(op) {
    case AstCompareOpType::Equals:
        return left == right ? 1 : 0;
    case AstCompareOpType::Greater:
        return left > right ? 1 : 0;
    case AstCompareOpType::Less:
        return left < right ? 1 : 0;
    case AstCompareOpType::GreaterEquals:
        return left >= right ? 1 : 0;
    case AstCompareOpType::LessEquals:
        return left <= right ? 1 : 0;
    case AstCompareOpType::NotEquals:
        return left != right ? 1 : 0;
    default:
        break;
    }
    throw ScriptRuntimeError("Invalid instruction");
    return 0;
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::Comparison const & v) {
    auto left = eval_expr(script, scope, v.left);
    auto right = eval_expr(script, scope, v.right);
    AstCompareOpType op = v.op;
    bool flt = false;
    bool i = false;
    switch(left.result_type) {
    case ValueType::Float:
        flt = true;
        break;
    case ValueType::Integer:
        i = true;
        break;
    default:
        break;
    }
    switch(right.result_type) {
    case ValueType::Integer:
        i = true;
        break;
    case ValueType::Float:
        flt = true;
        break;
    default:
        if(left.result_type != right.result_type) {
            throw ScriptError("Type mismatch", v.line, v.column);
        }
        break;
    }

    if(i || flt) {
        return{
            ValueType::Integer,
            false,
            [left, right, flt, op](Scope & scope) -> CallResult {
                ScriptValue result;
                result.reference = false;
                result.type = ValueType::Integer;
                if(flt)
                    result.value = do_compare(op, left.compiled(scope).get().as_float(), right.compiled(scope).get().as_float());
                else
                    result.value = do_compare(op, left.compiled(scope).get().get_integer(), right.compiled(scope).get().get_integer());
                return result;
            }
        };
    }
    if(op != AstCompareOpType::NotEquals && op != AstCompareOpType::Equals) {
        throw ScriptError("Type mismatch", v.line, v.column);
    }

    return{
        ValueType::Integer,
        false,
        [left, right, op](Scope & scope) -> CallResult {
            ScriptValue result;
            result.reference = false;
            result.type = ValueType::Integer;
            if(left.result_type == ValueType::List) {
                if(op == AstCompareOpType::NotEquals)
                    result.value = left.compiled(scope).get().get_list().size() != right.compiled(scope).get().get_list().size();
                else
                    result.value = left.compiled(scope).get().get_list().size() == right.compiled(scope).get().get_list().size();
            }
            else {
                bool compres = right.compiled(scope).get().value == left.compiled(scope).get().value;
                if(op == AstCompareOpType::NotEquals)
                    result.value =  compres ? 0 : 1;
                else
                    result.value = compres ? 1 : 0;
            }
            return result;
        }
    };
}

struct return_value_visitor : boost::static_visitor<ScriptValue> {
    ScriptValue operator()(boost::reference_wrapper<ScriptValue> const & v) const {
        return boost::apply_visitor(*this, v.get().value);
    }

    ScriptValue operator()(boost::reference_wrapper<ScriptValue::float_type> f) const {
        return ScriptValue(ValueType::Float, f.get(), false);
    }

    ScriptValue operator()(ScriptValue::list_type const & l) const {
        return ScriptValue{ValueType::List, l, false};
    }

    ScriptValue operator()(ScriptValue::integer_type const & i) const {
        return ScriptValue{ValueType::Integer, i, false};
    }

    ScriptValue operator()(ScriptValue::float_type const & f) const {
        return ScriptValue{ValueType::Float, f, false};
    }

    ScriptValue operator()(ScriptValue::string_type const & s) const {
        return ScriptValue{ValueType::String, s, false};
    }

    ScriptValue operator()(ScriptValue::vector_type const & v) const {
        return ScriptValue{ValueType::Vector, v, false};
    }

    ScriptValue operator()(ScriptValue::rotation_type const & r) const {
        return ScriptValue{ValueType::Rotation, r, false};
    }
};


CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::Return const & v) {
    if(v.value && scope.return_type.empty()) {
        throw ScriptError("Cannot return value from this function.", v.line, v.column);
    }
    if(!v.value && !scope.return_type.empty()) {
        throw ScriptError("Cannot return nothing from this function.", v.line, v.column);
    }
    if(v.value) {
        auto res = eval_expr(script, scope, v.value);
        return{
            ValueType::Void,
            false,
            [res](Scope & scope) -> CallResult {
                throw FunctionReturnException(boost::apply_visitor(return_value_visitor(), res.compiled(scope).get().value));
                return CallResult();
            }
        };
    }
    return{
        ValueType::Void,
        false,
        [](Scope & /*scope*/) -> CallResult {
            throw FunctionReturnException(CallResult());
            return CallResult();
        }
    };
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::List const & v) {
    std::vector<CompiledExpression> elements;
    elements.reserve(v.elements.size());
    for(size_t i = 0; i < v.elements.size(); ++i) {
        elements.emplace_back(std::move(eval_expr(script, scope, v.elements[i])));
    }
    return{
        ValueType::List,
        false,
        [elements](Scope & scope) -> CallResult {
            auto target = ScriptValue::list_type();
            target.reserve(elements.size());
            for(size_t i = 0; i < elements.size(); ++i) {
                target.push_back(deref(elements[i].compiled(scope).get()));
            }

            ScriptValue result;
            result.type = ValueType::List;
            result.reference = false;
            result.value = std::move(target);
            return result;
        }
    };
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::AugAssignment const & v) {
    auto binop = std::make_shared<lsl::BinOp>();
    binop->left = v.left;
    binop->right = v.right;
    binop->op = v.op;

    lsl::Assignment assignment;
    assignment.line = v.line;
    assignment.column = v.column;
    assignment.left = v.left;
    assignment.right = binop;

    return eval_expr(script, scope, assignment);
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::Assignment const & v) {
    auto left = eval_expr(script, scope, v.left);
    auto right = eval_expr(script, scope, v.right);
    if(!left.reference) {
        throw ScriptError("Cannot only assign to variable", v.line, v.column);
    }
    if(left.result_type != right.result_type) {
        bool k = false;
        bool s = false;
        bool i = false;
        bool f = false;
        switch(left.result_type) {
        case ValueType::Float:
            f = true;
            break;
        case ValueType::Integer:
            i = true;
            break;
        case ValueType::String:
            s = true;
        case ValueType::Key:
            k = true;
            break;
        default:
            break;
        }

        switch(right.result_type) {
        case ValueType::Key:
        case ValueType::String:
            if(!k && !s) {
                throw ScriptError("Type mismatch", v.line, v.column);
            }
            break;
        case ValueType::Integer:
        case ValueType::Float:
            if(!i && !f) {
                throw ScriptError("Type mismatch", v.line, v.column);
            }
        break;
        default:
            throw ScriptError("Type mismatch", v.line, v.column);
        }
    }

    return{
        left.result_type,
        false,
        [left, right](Scope & s) -> CallResult {
            ScriptValue result;
            auto l = left.compiled(s).get();
            auto r = right.compiled(s).get();
            switch(left.result_type) {
            case ValueType::Integer:
                l.get_ref().get().value = r.as_integer();
                result.value = l.get_ref().get().value;
                break;
            case ValueType::Float:
                result.value = boost::apply_visitor(float_extractor(), l.value) = r.as_float();
                break;
            case ValueType::Key:
                l.get_ref().get().value = r.as_key();
                result.value = l.get_ref().get().value;
                break;
            case ValueType::Vector:
                l.get_ref().get().value = r.as_vector();
                result.value = l.get_ref().get().value;
                break;
            case ValueType::Rotation:
                l.get_ref().get().value = r.as_rotation();
                result.value = l.get_ref().get().value;
                break;
            case ValueType::List:
                l.get_ref().get().value = r.as_list();
                result.value = l.get_ref().get().value;
                break;
            case ValueType::String:
                l.get_ref().get().value = r.as_string();
                result.value = l.get_ref().get().value;
                break;
            default:
                assert(false && "Cannot assign Void");
                throw ScriptRuntimeError("Invalid instruction");
            }
            result.type = left.result_type;
            result.reference = false;
            return result;
        }
    };
}

ScriptValue::list_type append_copy(ScriptValue::list_type const & left, ScriptValue::list_type const & right) {
    ScriptValue::list_type result = left;
    result.insert(result.end(), right.begin(), right.end());
    return result;
}

CompiledExpression combine_list(CompiledExpression const & left, CompiledExpression const & right) {
    return {
        ValueType::List,
        false,
        [left, right](Scope & scope) -> CallResult {
            ScriptValue value;
            value.value = append_copy(left.compiled(scope).get().get_list(),
                                      right.compiled(scope).get().get_list());
            value.reference = false;
            value.type = ValueType::List;
            return value;
        }
    };
}

CompiledExpression combine_string(CompiledExpression const & left, CompiledExpression const & right) {
    return {
        ValueType::String,
        false,
        [left, right](Scope & scope) -> CallResult {
            ScriptValue value;
            value.value = left.compiled(scope).get().get_string()
                        + right.compiled(scope).get().get_string();
            value.reference = false;
            value.type = ValueType::String;
            return value;
        }
    };
}

CompiledExpression mod(CompiledExpression const & left, CompiledExpression const & right, Ast const & v) {
    bool left_ok = (left.result_type == ValueType::Integer || left.result_type == ValueType::Float);
    bool right_ok = (right.result_type == ValueType::Integer || right.result_type == ValueType::Float);
    if(!(left_ok && right_ok)){
        throw ScriptError("Type mismatch", v.line, v.column);
    }
    bool lfloat = left.result_type == ValueType::Float;
    bool rfloat = right.result_type == ValueType::Float;
    if(lfloat || rfloat) {
        // result float
        CompiledFunction impl;
        impl = [lfloat, rfloat, left, right](Scope &s) -> CallResult {
            auto eleft = left.compiled(s).get();
            auto eright = left.compiled(s).get();
            ScriptValue result;
            result.type = ValueType::Float;
            result.reference = false;
            if(lfloat == rfloat) {
                result.value = fmod(eleft.get_float(), eright.get_float());
            }
            else {
                if(lfloat) {
                    result.value = fmod(eleft.get_float(), ScriptValue::float_type(eright.get_integer()));
                }
                else {
                    result.value = fmod(ScriptValue::float_type(eleft.get_integer()), eright.get_float());
                }
            }
            return result;
        };
        return {
            ValueType::Float,
            false,
            impl
        };
    }
    return {
        ValueType::Integer,
        false,
        [left, right](Scope & s) -> CallResult {
            ScriptValue result;
            result.type = ValueType::Integer;
            result.reference = false;
            result.value = left.compiled(s).get().get_integer()
                         % right.compiled(s).get().get_integer();
            return result;
        }
    };
}

CompiledExpression mult(CompiledExpression const & left, CompiledExpression const & right, Ast const & v) {
    bool rotl = false;
    bool rotr = false;
    bool vecl = false;
    bool vecr = false;
    bool flt = false;
    switch(left.result_type) {
    case ValueType::Float:
        flt = true;
        break;
    case ValueType::Rotation:
        rotl = true;
        break;
    case ValueType::Vector:
        vecl = true;
        break;
    case ValueType::Integer:
        break;
    default:
         throw ScriptError("Type mismatch", v.line, v.column);
    }

    switch(right.result_type) {
    case ValueType::Float:
        flt = true;
        break;
    case ValueType::Rotation:
        if(!rotl && !vecl) {
            throw ScriptError("Type mismatch", v.line, v.column);
        }
        rotr = true;
        break;
    case ValueType::Vector:
        vecr = true;
        break;
    case ValueType::Integer:
        break;
    default:
         throw ScriptError("Type mismatch", v.line, v.column);
    }

    if(vecl && vecr) {
        return {
            ValueType::Vector,
            false,
            [left, right](Scope & s) -> CallResult {
                ScriptValue result;
                result.type = ValueType::Vector;
                result.reference = false;
                result.value = left.compiled(s).get().get_vector()
                             * right.compiled(s).get().get_vector();
                return result;
            }
        };
    }
    if(vecl && rotr) {
        return {
            ValueType::Vector,
            false,
            [left, right](Scope & s) -> CallResult {
                ScriptValue result;
                result.type = ValueType::Vector;
                result.reference = false;
                result.value = left.compiled(s).get().get_vector()
                             * right.compiled(s).get().get_rotation();
                return result;
            }
        };
    }
    if(rotl && rotr) {
        return {
            ValueType::Rotation,
            false,
            [left, right](Scope & s) -> CallResult {
                ScriptValue result;
                result.type = ValueType::Rotation;
                result.reference = false;
                result.value = left.compiled(s).get().get_rotation()
                             * right.compiled(s).get().get_rotation();
                return result;
            }
        };
    }
    if(vecl) {
        return {
            ValueType::Vector,
            false,
            [left, right](Scope & s) -> CallResult {
                ScriptValue result;
                result.type = ValueType::Vector;
                result.reference = false;
                result.value = left.compiled(s).get().get_vector()
                             * right.compiled(s).get().as_float();
                return result;
            }
        };
    }
    if(flt) {
        return {
            ValueType::Float,
            false,
            [left, right](Scope & s) -> CallResult {
                ScriptValue result;
                result.type = ValueType::Float;
                result.reference = false;
                result.value = left.compiled(s).get().as_float()
                             * right.compiled(s).get().as_float();
                return result;
            }
        };
    }
    return {
        ValueType::Integer,
        false,
        [left, right](Scope & s) -> CallResult {
            ScriptValue result;
            result.type = ValueType::Integer;
            result.reference = false;
            result.value = left.compiled(s).get().get_integer()
                         * right.compiled(s).get().get_integer();
            return result;
        }
    };
}

CompiledExpression div(CompiledExpression const & left, CompiledExpression const & right, Ast const & v) {
    bool rotl = false;
    bool rotr = false;
    bool vecl = false;
    bool vecr = false;
    bool flt = false;
    switch(left.result_type) {
    case ValueType::Float:
        flt = true;
        break;
    case ValueType::Rotation:
        rotl = true;
        break;
    case ValueType::Vector:
        vecl = true;
        break;
    case ValueType::Integer:
        break;
    default:
         throw ScriptError("Type mismatch", v.line, v.column);
    }

    switch(right.result_type) {
    case ValueType::Float:
        flt = true;
        break;
    case ValueType::Rotation:
        if(!rotl && !vecl) {
            throw ScriptError("Type mismatch", v.line, v.column);
        }
        rotr = true;
        break;
    case ValueType::Vector:
        vecr = true;
        break;
    case ValueType::Integer:
        break;
    default:
         throw ScriptError("Type mismatch", v.line, v.column);
    }

    if(vecl && vecr) {
        return {
            ValueType::Vector,
            false,
            [left, right](Scope & s) -> CallResult {
                ScriptValue result;
                result.type = ValueType::Vector;
                result.reference = false;
                result.value = left.compiled(s).get().get_vector()
                             / right.compiled(s).get().get_vector();
                return result;
            }
        };
    }
    if(vecl && rotr) {
        return {
            ValueType::Vector,
            false,
            [left, right](Scope & s) -> CallResult {
                ScriptValue result;
                result.type = ValueType::Vector;
                result.reference = false;
                result.value = left.compiled(s).get().get_vector()
                             / right.compiled(s).get().get_rotation();
                return result;
            }
        };
    }
    if(rotl && rotr) {
        return {
            ValueType::Rotation,
            false,
            [left, right](Scope & s) -> CallResult {
                ScriptValue result;
                result.type = ValueType::Rotation;
                result.reference = false;
                result.value = left.compiled(s).get().get_rotation()
                             / right.compiled(s).get().get_rotation();
                return result;
            }
        };
    }
    if(vecl) {
        return {
            ValueType::Vector,
            false,
            [left, right](Scope & s) -> CallResult {
                ScriptValue result;
                result.type = ValueType::Vector;
                result.reference = false;
                result.value = left.compiled(s).get().get_vector()
                             / right.compiled(s).get().as_float();
                return result;
            }
        };
    }
    if(flt) {
        return {
            ValueType::Float,
            false,
            [left, right](Scope & s) -> CallResult {
                ScriptValue result;
                result.type = ValueType::Float;
                result.reference = false;
                result.value = left.compiled(s).get().as_float()
                             / right.compiled(s).get().as_float();
                return result;
            }
        };
    }
    return {
        ValueType::Integer,
        false,
        [left, right](Scope & s) -> CallResult {
            ScriptValue result;
            result.type = ValueType::Integer;
            result.reference = false;
            result.value = left.compiled(s).get().get_integer()
                         / right.compiled(s).get().get_integer();
            return result;
        }
    };
}

CompiledExpression sub(CompiledExpression const & left, CompiledExpression const & right, Ast const & v) {
    bool flt = false;
    bool vec = false;
    bool rot = false;
    switch(left.result_type) {
    case ValueType::Float:
        flt = true;
        break;
    case ValueType::Rotation:
        rot = true;
        break;
    case ValueType::Vector:
        vec = true;
        break;
    case ValueType::Integer:
        break;
    default:
         throw ScriptError("Type mismatch", v.line, v.column);
    }

    switch(right.result_type) {
    case ValueType::Vector:
    case ValueType::Rotation:
        if(right.result_type != left.result_type) {
            throw ScriptError("Type mismatch", v.line, v.column);
        }
        break;
    case ValueType::Float:
        flt = true;
    case ValueType::Integer:
        break;
    default:
         throw ScriptError("Type mismatch", v.line, v.column);
    }

    if(vec) {
        return {
            ValueType::Vector,
            false,
            [left, right](Scope & scope) -> CallResult {
                return ScriptValue {
                    ValueType::Vector,
                    left.compiled(scope).get().get_vector() - right.compiled(scope).get().get_vector(),
                    false
                };
            }
        };
    }
    if(rot) {
        return {
            ValueType::Rotation,
            false,
            [left, right](Scope & scope) -> CallResult {
                return ScriptValue {
                    ValueType::Rotation,
                    left.compiled(scope).get().get_rotation() - right.compiled(scope).get().get_rotation(),
                    false
                };
            }
        };
    }
    if(flt) {
        return {
            ValueType::Float,
            false,
            [left, right](Scope & scope) -> CallResult {
                return ScriptValue {
                    ValueType::Float,
                    left.compiled(scope).get().as_float() - right.compiled(scope).get().as_float(),
                    false
                };
            }
        };
    }
    return {
        ValueType::Integer,
        false,
        [left, right](Scope & scope) -> CallResult {
            return ScriptValue {
                ValueType::Integer,
                left.compiled(scope).get().get_integer() - right.compiled(scope).get().get_integer(),
                false
            };
        }
    };
}

CompiledExpression add(CompiledExpression const & left, CompiledExpression const & right, Ast const & v) {
    bool flt = false;
    bool vec = false;
    bool rot = false;
    switch(left.result_type) {
    case ValueType::Float:
        flt = true;
        break;
    case ValueType::Rotation:
        rot = true;
        break;
    case ValueType::Vector:
        vec = true;
        break;
    case ValueType::Integer:
        break;
    default:
         throw ScriptError("Type mismatch", v.line, v.column);
    }

    switch(right.result_type) {
    case ValueType::Vector:
    case ValueType::Rotation:
        if(right.result_type != left.result_type) {
            throw ScriptError("Type mismatch", v.line, v.column);
        }
        break;
    case ValueType::Float:
        flt = true;
    case ValueType::Integer:
        break;
    default:
         throw ScriptError("Type mismatch", v.line, v.column);
    }

    if(vec) {
        return {
            ValueType::Vector,
            false,
            [left, right](Scope & scope) -> CallResult {
                return ScriptValue {
                    ValueType::Vector,
                    left.compiled(scope).get().get_vector() + right.compiled(scope).get().get_vector(),
                    false
                };
            }
        };
    }
    if(rot) {
        return {
            ValueType::Rotation,
            false,
            [left, right](Scope & scope) -> CallResult {
                return ScriptValue {
                    ValueType::Rotation,
                    left.compiled(scope).get().get_rotation() + right.compiled(scope).get().get_rotation(),
                    false
                };
            }
        };
    }
    if(flt) {
        return {
            ValueType::Float,
            false,
            [left, right](Scope & scope) -> CallResult {
                return ScriptValue {
                    ValueType::Float,
                    left.compiled(scope).get().as_float() + right.compiled(scope).get().as_float(),
                    false
                };
            }
        };
    }
    return {
        ValueType::Integer,
        false,
        [left, right](Scope & scope) -> CallResult {
            return ScriptValue {
                ValueType::Integer,
                left.compiled(scope).get().get_integer() + right.compiled(scope).get().get_integer(),
                false
            };
        }
    };
}


CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::BinOp const & v) {
    auto left = eval_expr(script, scope, v.left);
    auto right = eval_expr(script, scope, v.right);
    CompiledExpression impl;
    AstBinOpType op = v.op;
    switch(op) {
    // require string, list, vector, rotation, float or integer
    case AstBinOpType::Add:
        if(left.result_type == ValueType::String || left.result_type == ValueType::List) {
            if(right.result_type != left.result_type) {
                throw ScriptError("Type mismatch", v.line, v.column);
            }
            if(left.result_type == ValueType::String) {
                impl = combine_string(left, right);
            }
            else {
                impl = combine_list(left, right);
            }
            break;
        } else {
            impl = add(left, right, v);
            break;
        }
    // require vector, rotation, float, or integer
    case AstBinOpType::Mult:
        impl = mult(left, right, v);
        break;
    case AstBinOpType::Div:
        impl = div(left, right, v);
        break;
    case AstBinOpType::Sub:
        impl = sub(left, right, v);
        break;
    // require integer or float
    case AstBinOpType::Mod:
        impl = mod(left, right, v);
        break;
    // require integer
    case AstBinOpType::BitAnd:
    case AstBinOpType::BitOr:
    case AstBinOpType::BitXor:
    case AstBinOpType::LeftShift:
    case AstBinOpType::RightShift:
        if(left.result_type != ValueType::Integer) {
            throw ScriptError("Type mismatch", v.line, v.column);
        }
        impl = CompiledExpression{
            ValueType::Integer,
            false,
            [op, left, right](Scope & scope) {
                auto eleft = left.compiled(scope).get().get_integer();
                auto eright = right.compiled(scope).get().get_integer();
                ScriptValue::integer_type result = 0;
                switch(op)  {
                case AstBinOpType::BitAnd:
                    result = eleft & eright;
                    break;
                case AstBinOpType::BitOr:
                    result = eleft | eright;
                    break;
                case AstBinOpType::BitXor:
                    result = eleft ^ eright;
                    break;
                case AstBinOpType::LeftShift:
                    result = eleft << eright;
                    break;
                case AstBinOpType::RightShift:
                    result = eleft >> eright;
                    break;
                default:
                    throw ScriptRuntimeError("Invalid instruction");
                    break;
                }
                return ScriptValue {
                    ValueType::Integer,
                    result,
                    false
                };
            }
        };

    default:
        break;
    }

    return impl;
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::Variable const & v) {
    ScriptValue * var = nullptr;
    std::string name = v.name;
    bool local = false;
    if(scope.locals.count(v.name)) {
        local = true;
        var = &scope.locals[v.name];
    }
    else if(script.get().globals.count(v.name)) {
        var = &script.get().globals[v.name];
    }
    else {
        throw ScriptError(format("Use of undefined variable '%s'", v.name.c_str()), v.line, v.column);
    }

    if(!v.member.empty()) {
        if(var->type == ValueType::Vector) {
            ScriptValue::float_type Vector::* member = 0;
            if(v.member == "x") {
                member = &Vector::x;
            }
            else if(v.member == "y") {
                member = &Vector::y;
            }
            else if(v.member == "z") {
                member = &Vector::z;
            }
            else {
                throw ScriptError(format("Unknown member '%s' for vector", v.member.c_str()), v.line, v.column);
            }
            return {
                ValueType::Float,
                true,
                [local, member, name](Scope & scope) -> CallResult {
                    ScriptValue value;
                    value.type = ValueType::Float;
                    value.reference = true;
                    if(local) {
                        value.value = boost::ref(scope.locals[name].get_vector().*member);
                    }
                    else {
                        value.value = boost::ref(scope.script.get().globals[name].get_vector().*member);
                    }
                    return value;
                }
            };
        }
        else if(var->type == ValueType::Rotation) {
            ScriptValue::float_type Quaternion::* member = 0;
            if(v.member == "x") {
                member = &Quaternion::x;
            }
            else if(v.member == "y") {
                member = &Quaternion::y;
            }
            else if(v.member == "z") {
                member = &Quaternion::z;
            }
            else if(v.member == "s") {
                member = &Quaternion::s;
            }
            else {
                throw ScriptError(format("Unknown member '%s' for rotation", v.member.c_str()), v.line, v.column);
            }
            return {
                ValueType::Float,
                true,
                [local, member, name](Scope & scope) -> CallResult {
                    ScriptValue value;
                    value.type = ValueType::Float;
                    value.reference = true;
                    if(local) {
                        value.value =boost::ref(scope.locals[name].get_rotation().*member);
                    }
                    else {
                        value.value = boost::ref(scope.script.get().globals[name].get_rotation().*member);
                    }
                    return value;
                }
            };
        }
        else {
            throw ScriptError(format("Invalid syntax - Only vector and rotation memebers can be accessed."), v.line, v.column);
        }
    }
    ValueType type = var->type;
    return {
        type,
        true,
        [local, name, type](Scope & scope) -> CallResult {
            ScriptValue result;
            result.type = type;
            result.reference = true;
            if(local) {
                scope.locals[name].reference = false;
                result.value = boost::ref(scope.locals[name]);
            }
            else {
                result.value = boost::ref(scope.script.get().globals[name]);
            }
            return result;
        }
    };
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::UnaryOp const & v) {
    CompiledExpression target = eval_expr(script, scope, v.target);
    std::function<CallResult(Scope &)> impl;
    AstUnaryOpType op = v.op;
    if(target.result_type == ValueType::Rotation || target.result_type == ValueType::Vector) {
        if(op == AstUnaryOpType::Sub) {
            if(target.result_type == ValueType::Rotation) {
                impl = [target](Scope & scope) -> CallResult {
                    ScriptValue value;
                    value.type = ValueType::Rotation;
                    value.reference = false;
                    value.value = target.compiled(scope).get().as_rotation() * -1.;
                    return value;
                };
            }
            else {
                impl = [target](Scope & scope) -> CallResult {
                    ScriptValue value;
                    value.type = ValueType::Vector;
                    value.reference = false;
                    value.value = target.compiled(scope).get().as_vector() * -1;
                    return value;
                };
            }
        }
        else {
            throw ScriptError("Type mismatch", v.line, v.column);
        }
    }
    else {
        if(target.result_type != ValueType::Integer && target.result_type != ValueType::Float) {
            throw ScriptError("Type mismatch - Only integer and float types can use this operator", v.line, v.column);
        }
        switch(op) {
        case AstUnaryOpType::Add:
            throw ScriptError("Syntax error", v.line, v.column);
            break;
        case AstUnaryOpType::Not:
            if(target.result_type != ValueType::Integer) {
                throw ScriptError("Type mismatch - '!' can only be applied to integers", v.line, v.column);
            }
            impl = [target](Scope & scope) {
                ScriptValue res;
                res.type = ValueType::Integer;
                res.reference = false;
                res.value = !target.compiled(scope).get().get_integer();
                return res;
            };

            break;
        case AstUnaryOpType::Invert:
            if(target.result_type != ValueType::Integer) {
                throw ScriptError("Type mismatch - Only integers can be inverted", v.line, v.column);
            }
            break;
        case AstUnaryOpType::Sub:
            impl = [target](Scope & scope) {
                ScriptValue res;
                res.type = target.result_type;
                res.reference = false;
                if(target.result_type == ValueType::Integer)
                    res.value = -target.compiled(scope).get().get_integer();
                else
                    res.value = -target.compiled(scope).get().get_float();
                return res;
            };
            break;
        // Require a reference
        case AstUnaryOpType::PreInc:
        case AstUnaryOpType::PreDec:
        case AstUnaryOpType::PostInc:
        case AstUnaryOpType::PostDec:
            if(!target.reference) {
                throw ScriptError("Expected variable - Not a temporary", v.line, v.column);
            }
            impl = [op, target, v](Scope & scope) {
                ScriptValue res;
                res.type = ValueType::Integer;
                res.reference = false;
                switch(op) {
                default:
                    assert(false && "Invalid operand for this function");
                    throw ScriptRuntimeError("Invalid instruction", v.line, v.column);
                    break;
                case AstUnaryOpType::PreInc:
                    res.value = ++target.compiled(scope).get().get_integer();
                    break;
                case AstUnaryOpType::PreDec:
                    res.value = --target.compiled(scope).get().get_integer();
                    break;
                case AstUnaryOpType::PostInc:
                    res.value = target.compiled(scope).get().get_integer()++;
                    break;
                case AstUnaryOpType::PostDec:
                    res.value = target.compiled(scope).get().get_integer()--;
                    break;
                }
                return res;
            };
            break;
        case AstUnaryOpType::Undefined:
            assert(false && "AstUnaryOpType::Undefined is an invalid operand");
            throw ScriptRuntimeError("Invalid instruction", v.line, v.column);
        }
    }
    return {
        target.result_type,
        false,
        impl,
    };
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::BoolOp const & v) {
    std::tuple<CompiledExpression, CompiledExpression> data(
                eval_expr(script, scope, v.left),
                eval_expr(script, scope, v.right));
    AstBoolOpType op = v.op;
    auto and_compare = [data](Scope & scope) -> ScriptValue::integer_type {
        return ((std::get<0>(data).compiled(scope).get().as_integer()
             &&  std::get<1>(data).compiled(scope).get().as_integer()) ? 1 : 0);
    };
    auto or_compare = [data](Scope & scope) -> ScriptValue::integer_type {
        return ((std::get<0>(data).compiled(scope).get().as_integer()
             ||  std::get<1>(data).compiled(scope).get().as_integer()) ? 1 : 0);
    };
    return {
        ValueType::Integer,
        false,
        [op, and_compare, or_compare](Scope & scope) -> CallResult {
            ScriptValue result;
            result.reference = false;
            result.type = ValueType::Integer;
            result.value = op == AstBoolOpType::Or ? or_compare(scope) : and_compare(scope);
            return result;
        }
    };
}

CompiledExpression eval_expr(ScriptRef, Scope &, lsl::Integer const & v) {
    return {
        ValueType::Integer,
        false,
        [v](Scope &) -> CallResult {
            ScriptValue value;
            value.reference = false;
            value.type = ValueType::Integer;
            value.value = v.value;
            return value;
        }
    };
}

CompiledExpression eval_expr(ScriptRef, Scope &, lsl::StringLit const & v) {
    return {
        ValueType::String,
        false,
        [v](Scope &) -> CallResult {
            ScriptValue value;
            value.reference = false;
            value.type = ValueType::String;
            value.value = v.value;
            return value;
        }
    };
}

CompiledExpression eval_expr(ScriptRef, Scope &, lsl::Key const & v) {
    return {
        ValueType::Key,
        false,
        [v](Scope &) -> CallResult {
            ScriptValue value;
            value.reference = false;
            value.type = ValueType::Key;
            value.value = v.value;
            return value;
        }
    };
}

bool quat_vec_compatible_result(CompiledExpression const & e) {
    return e.result_type == ValueType::Integer
        || e.result_type == ValueType::Float;
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::Quaternion const & v) {
    std::tuple<CompiledExpression,CompiledExpression,CompiledExpression,CompiledExpression> data(
                eval_expr(script, scope, v.x),
                eval_expr(script, scope, v.y),
                eval_expr(script, scope, v.z),
                eval_expr(script, scope, v.s));
    if(!(   quat_vec_compatible_result(std::get<0>(data))
         && quat_vec_compatible_result(std::get<1>(data))
         && quat_vec_compatible_result(std::get<2>(data))
         && quat_vec_compatible_result(std::get<3>(data)))) {
        throw ScriptError("Only numbers can be used to initialize a rotation/quaternion.", v.line, v.column);
    }

    return {
        ValueType::Rotation,
        false,
        [data](Scope & scope) -> CallResult {
            ScriptValue value;
            value.reference = false;
            value.type = ValueType::Rotation;
            value.value = lsl::runtime::Quaternion{
                    std::get<0>(data).compiled(scope).get().as_float(),
                    std::get<1>(data).compiled(scope).get().as_float(),
                    std::get<2>(data).compiled(scope).get().as_float(),
                    std::get<3>(data).compiled(scope).get().as_float()
            };
            return value;
        }
    };
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::Vector const & v) {
    std::tuple<CompiledExpression,CompiledExpression,CompiledExpression> data(
                eval_expr(script, scope, v.x),
                eval_expr(script, scope, v.y),
                eval_expr(script, scope, v.z));
    if(!(   quat_vec_compatible_result(std::get<0>(data))
         && quat_vec_compatible_result(std::get<1>(data))
         && quat_vec_compatible_result(std::get<2>(data)))) {
        throw ScriptError("Only numbers can be used to initialize a vector.", v.line, v.column);
    }
    return {
        ValueType::Vector,
        false,
        [data](Scope & scope) -> CallResult {
            ScriptValue value;
            value.type = ValueType::Vector;
            value.reference = false;
            value.value = lsl::runtime::Vector{
                    std::get<0>(data).compiled(scope).get().as_float(),
                    std::get<1>(data).compiled(scope).get().as_float(),
                    std::get<2>(data).compiled(scope).get().as_float()
            };
            return value;
        }
    };
}

CompiledExpression eval_expr(ScriptRef, Scope &, lsl::Float const & v) {
    return {
        ValueType::Float,
        false,
        [v](Scope &) -> CallResult {
            ScriptValue value;
            value.reference = false;
            value.type = ValueType::Float;
            value.value = v.value;
            return value;
        }
    };
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::Body const & v) {
    auto statements = std::vector<CompiledExpression>();
    statements.reserve(v.statements.size());
    for(auto const & s : v.statements) {
        statements.emplace_back(std::move(eval_expr(script, scope, s)));
    }
    return {
        ValueType::Void,
        false,
        [statements](Scope & scope) -> CallResult {
            for( auto const & s : statements ) {
                s.compiled(scope);
            }
            return CallResult();
        }
    };
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::Ast const & a) {
    switch(a.type) {
    case AstType::ExpressionList: return eval_expr(script, scope, static_cast<ExpressionList const &>(a)); break;
    case AstType::Jump: return eval_expr(script, scope, static_cast<Jump const &>(a)); break;
    case AstType::Label: return eval_expr(script, scope, static_cast<Label const &>(a)); break;
    case AstType::NoOp: return eval_expr(script, scope, static_cast<NoOp const &>(a)); break;
    case AstType::StateChange: return eval_expr(script, scope, static_cast<StateChange const &>(a)); break;
    case AstType::Vector: return eval_expr(script, scope, static_cast<lsl::Vector const &>(a)); break;
    case AstType::Call: return eval_expr(script, scope, static_cast<Call const &>(a)); break;
    case AstType::Quaternion: return eval_expr(script, scope, static_cast<lsl::Quaternion const &>(a)); break;
    case AstType::Key: return eval_expr(script, scope, static_cast<lsl::Key const &>(a)); break;
    case AstType::String: return eval_expr(script, scope, static_cast<StringLit const &>(a)); break;
    case AstType::Integer: return eval_expr(script, scope, static_cast<lsl::Integer const &>(a)); break;
    case AstType::Float: return eval_expr(script, scope, static_cast<lsl::Float const &>(a)); break;
    case AstType::Variable: return eval_expr(script, scope, static_cast<lsl::Variable const &>(a)); break;
    case AstType::List: return eval_expr(script, scope, static_cast<lsl::List const &>(a)); break;
    case AstType::BinOp: return eval_expr(script, scope, static_cast<BinOp const &>(a)); break;
    case AstType::BoolOp: return eval_expr(script, scope, static_cast<BoolOp const &>(a)); break;
    case AstType::UnaryOp: return eval_expr(script, scope, static_cast<UnaryOp const &>(a)); break;
    case AstType::TypeCast: return eval_expr(script, scope, static_cast<TypeCast const &>(a)); break;
    case AstType::VarDecl: return eval_expr(script, scope, static_cast<VarDecl const &>(a)); break;
    case AstType::Comparison: return eval_expr(script, scope, static_cast<Comparison const &>(a)); break;
    case AstType::Assignment: return eval_expr(script, scope, static_cast<Assignment const &>(a)); break;
    case AstType::AugAssignment: return eval_expr(script, scope, static_cast<AugAssignment const &>(a)); break;
    case AstType::For: return eval_expr(script, scope, static_cast<For const &>(a)); break;
    case AstType::While: return eval_expr(script, scope, static_cast<While const &>(a)); break;
    case AstType::Do: return eval_expr(script, scope, static_cast<Do const &>(a)); break;
    case AstType::If: return eval_expr(script, scope, static_cast<If const &>(a)); break;
    case AstType::Return: return eval_expr(script, scope, static_cast<Return const &>(a)); break;
    case AstType::Body: return eval_expr(script, scope, static_cast<Body const &>(a)); break;
    default:
        assert(false);
    }
    return CompiledExpression();
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::AstPtr a) {
    return eval_expr(script, scope, *a);
}

CompiledScriptFunction
eval_function(
    ScriptRef script,
    lsl::Function const & ast_)
{
    auto ast = lsl::Function(ast_);
    CompiledScriptFunction result;

    if(script.get().functions.count(ast.name)) { // || script.get().library.functions(ast.name)) {
        throw ScriptError(format("Function '%s' was already previously declared", ast.name.c_str()), ast.line, ast.column);
    }

    auto scope = Scope(ast, script);
    for(auto const & arg : ast.args) {
        if(scope.locals.count_local(arg.name)) {
            throw ScriptError(format("Name '%s' previously declared within scope", ast.name.c_str()), ast.line, ast.column);
        }
        ScriptValue value;
        value.reference = false;
        init_with_type(value, arg.type);
        scope.locals.local(arg.name) = value;
        result.parameters.push_back(value.type);
    }

    ValueType return_type = ValueType::Void;
    if(!ast.returnType.empty()) {
        ScriptValue value;
        init_with_type(value, ast.returnType);
        return_type = value.type;
        scope.return_type = ast.returnType;
    }

    auto body = eval_expr(script, scope, ast.body);

    result.result_type = return_type;
    result.function = [ast, body](ScriptFunctionCall const & call) -> CallResult {
        if(call.arguments.size() != ast.args.size()) {
            throw ScriptError(format("Attempted to call function '%s' with invalid number of parameters", ast.name.c_str()), ast.line, ast.column);
        }

        auto scope = Scope(ast, call.caller);
        for(size_t i = 0; i < call.arguments.size(); ++i) {
            scope.locals.local(ast.args[i].name) = call.arguments[i];
        }

        try {
            body.compiled(scope);
        }
        catch(FunctionReturnException const & e) {
            return e.result();
        }

        return CallResult();
    };

    return result;
}


CompiledScriptFunction
eval_event(
    ScriptRef script,
    State & state,
    lsl::Event const & ast_)
{
    auto ast = lsl::Event(ast_);
    CompiledScriptFunction result;
    if(!get_event_repository().count(ast.name)) {
        throw ScriptError(format("Event '%s' is not a valid event", ast.name.c_str()), ast.line, ast.column);
    }
    if(state.events.count(ast.name)) {
        throw ScriptError(format("Event '%s' was already previously defined", ast.name.c_str()), ast.line, ast.column);
    }

    auto scope = Scope(ast, script);
    auto eventdef = get_event_repository()[ast.name];
    for(auto const & arg : ast.args) {
        if(scope.locals.count_local(arg.name)) {
            throw ScriptError(format("Name '%s' previously declared within scope", ast.name.c_str()), ast.line, ast.column);
        }
        ScriptValue value;
        value.reference = false;
        init_with_type(value, arg.type);
        scope.locals.local(arg.name) = value;
        if(eventdef.parameters[result.parameters.size()] != value.type) {
            throw ScriptError(format("Wrong type for parameter '%s'", arg.name.c_str()), ast.line, ast.column);
        }
        result.parameters.push_back(value.type);
    }

    auto body = eval_expr(script, scope, ast.body);

    result.result_type = ValueType::Void;
    result.function = [ast, body](ScriptFunctionCall const & call) -> CallResult {
        if(call.arguments.size() != ast.args.size()) {
            throw ScriptError(format("Attempted to call function '%s' with invalid number of parameters", ast.name.c_str()), ast.line, ast.column);
        }

        auto scope = Scope(ast, call.caller);
        for(size_t i = 0; i < call.arguments.size(); ++i) {
            scope.locals.local(ast.args[i].name) = call.arguments[i];
        }

        try {
            body.compiled(scope);
        }
        catch(FunctionReturnException const & e) {
            return e.result();
        }

        return CallResult();
    };

    return result;
}

StatePtr eval_state(
    ScriptRef script,
    lsl::StateDef const & ast_)
{
    auto state = std::make_shared<State>(ast_.name, script);

    for(auto const & e : ast_.events) {
        state->events[e.name] = eval_event(script, *state, e);
    }

    return state;
}

ScriptPtr eval_script(String const & prim_key, lsl::Script const & ast) {
    auto script = std::make_shared<Script>(prim_key);
    auto scope = Scope(ast, boost::ref(*script));
    for(auto const & constant : get_global_variables(ast)) {
        script->globals[constant.name];
        if(constant.right) {
            auto value = eval_expr(boost::ref(*script), scope, constant.right);
            script->globals[constant.name] = value.compiled(scope).get();
        }
    }
    for(auto const & fun : get_functions(ast)) {
        script->functions[fun.name] = std::make_shared<ScriptFunction>(eval_function(boost::ref(*script), fun));
    }

    for(auto const & state : ast.states.states) {
        script->states[state->name];
    }

    for(auto const & state : ast.states.states) {
        script->states[state->name] = eval_state(boost::ref(*script), *state);
    }

    return script;
}


}}}

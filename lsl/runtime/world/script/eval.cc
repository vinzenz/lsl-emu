#include <lsl/runtime/world/script/eval.hh>
#include <lsl/runtime/world/script.hh>
#include <lsl/runtime/world/script/error.hh>
#include <cassert>
#include <unordered_map>
namespace lsl {
namespace runtime {
namespace script {

struct Scope {
    ScriptRef script; // For globals and functions and changing state
    std::unordered_map<String, ScriptValue> locals;
};


typedef std::function<CallResult(Scope &)> CompiledFunction;
struct CompiledExpression {
    ValueType result_type;
    bool reference;
    CompiledFunction compiled;
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

#define NOT_IMPLEMENTED(TYPE) \
CompiledExpression eval_expr(ScriptRef, Scope &, lsl::TYPE const &) { \
    return { \
        ValueType::Void, \
        false, \
        [](Scope &) -> CallResult { \
            assert(false && "Not implemented"); \
            return CallResult(); \
    }}; \
}

NOT_IMPLEMENTED(ExpressionList)
NOT_IMPLEMENTED(TypeCast)
NOT_IMPLEMENTED(Jump)
NOT_IMPLEMENTED(Label)
NOT_IMPLEMENTED(StateChange)
NOT_IMPLEMENTED(Call)
NOT_IMPLEMENTED(List)

NOT_IMPLEMENTED(VarDecl)
NOT_IMPLEMENTED(Comparison)
NOT_IMPLEMENTED(Assignment)
NOT_IMPLEMENTED(AugAssignment)
NOT_IMPLEMENTED(For)
NOT_IMPLEMENTED(While)
NOT_IMPLEMENTED(Do)
NOT_IMPLEMENTED(If)
NOT_IMPLEMENTED(Return)

ScriptValue::list_type append_copy(ScriptValue::list_type const & left, ScriptValue::list_type const & right) {
    ScriptValue::list_type result = left;
    result.insert(result.end(), right.begin(), right.end());
    return result;
}

CompiledExpression combine_list(CompiledExpression const & left, CompiledExpression const & right) {
    return {
        ValueType::List,
        false,
        [left, right](Scope & scope) {
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
        [left, right](Scope & scope) {
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
                        value.value =boost::ref(scope.locals[name].get_vector().*member);
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
            else if(v.member == "x") {
                member = &Quaternion::x;
            }
            else if(v.member == "x") {
                member = &Quaternion::x;
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
        if(target.result_type != ValueType::Integer || target.result_type != ValueType::Float) {
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
                res.type = ValueType::Integer;
                res.reference = false;
                res.value = -target.compiled(scope).get().get_integer();
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
        ValueType::Vector,
        false,
        [v](Scope &) -> CallResult {
            ScriptValue value;
            value.type = ValueType::Float;
            value.value = v.value;
            return value;
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
    case AstType::Key: return eval_expr(script, scope, static_cast<Key const &>(a)); break;
    case AstType::String: return eval_expr(script, scope, static_cast<StringLit const &>(a)); break;
    case AstType::Integer: return eval_expr(script, scope, static_cast<Integer const &>(a)); break;
    case AstType::Float: return eval_expr(script, scope, static_cast<Float const &>(a)); break;
    case AstType::Variable: return eval_expr(script, scope, static_cast<lsl::Variable const &>(a)); break;
    case AstType::List: return eval_expr(script, scope, static_cast<List const &>(a)); break;
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
    default:
        assert(false);
    }
    return CompiledExpression();
}

CompiledExpression eval_expr(ScriptRef script, Scope & scope, lsl::AstPtr a) {
    return eval_expr(script, scope, *a);
}


CompiledScriptCall
eval_call(
    ScriptRef script,
    lsl::Function const & ast,
    ScriptFunctionCall const & call)
{
    CompiledScriptCall result;


    return result;
}

}}}

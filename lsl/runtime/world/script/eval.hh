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
#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_EVAL_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_EVAL_HH_INCLUDED

#include <boost/optional.hpp>
#include <lsl/runtime/world/script/value.hh>
#include <lsl/runtime/world/script_fwd.hh>
#include <lsl/ast/ast.hh>
#include <lsl/runtime/world/script/call.hh>

namespace lsl {
namespace runtime {
namespace script {

struct FunctionReturnException {
    FunctionReturnException(CallResult result)
    : result_(result)
    {}

    CallResult const & result() const {
        return result_;
    }

protected:
    CallResult result_;
};

struct StateChangeException {
    StateChangeException(String name)
    : state_(name)
    {}

    String const & state() const {
        return state_;
    }
protected:
    String state_;
};


struct CompiledScriptFunction {
    ValueType result_type;
    std::vector<ValueType> parameters;
    std::function<CallResult(ScriptFunctionCall const &)> function;
};

ScriptPtr eval_script(String const & prim_key, lsl::Script const & ast);
CompiledScriptFunction eval_function(ScriptRef script, lsl::Function const & ast);

}}}

#endif //GUARD_LSL_RUNTIME_WORLD_SCRIPT_EVAL_HH_INCLUDED

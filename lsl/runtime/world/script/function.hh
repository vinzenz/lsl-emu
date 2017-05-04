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
#ifndef GUARD_LSL_RUNTIME_WORLD_SCRIPT_FUNCTION_HH_INCLUDED
#define GUARD_LSL_RUNTIME_WORLD_SCRIPT_FUNCTION_HH_INCLUDED

#include <memory>
#include <lsl/runtime/world/script_fwd.hh>
#include <lsl/runtime/world/script/value.hh>
#include <lsl/runtime/world/script/eval.hh>
#include <lsl/ast/ast.hh>

namespace lsl {
namespace runtime {
namespace script {

struct ScriptFunction {
    CompiledScriptFunction compiled;

    ScriptFunction(ScriptRef script, lsl::Function const & f)
    : compiled(eval_function(script, f))
    {}

    ScriptFunction(CompiledScriptFunction const & compiled)
    : compiled(compiled)
    {}

    ValueType returns() const {
        return compiled.result_type;
    }

    CallResult call(ScriptFunctionCall const & call) const {
        return compiled.function(call);
    }
};

typedef std::shared_ptr<ScriptFunction> ScriptFunctionPtr;

}}}

#endif //GUARD_LSL_RUNTIME_WORLD_SCRIPT_FUNCTION_HH_INCLUDED

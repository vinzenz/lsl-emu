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
#include <lsl/ast/api.hh>

namespace lsl {

template< typename Type, AstType type, typename Container >
std::vector<Type> get_global(Container const & s) {
    auto result = std::vector<Type>{};

    for(auto const & global : s) {
        if(global && global->type == type) {
            result.push_back(*ast_cast<Type>(global));
        }
    }

    return result;
}

std::vector<VarDecl> get_global_variables(Script const & s) {
    return get_global<VarDecl, AstType::VarDecl>(s.globals.globals);
}

std::vector<StateDef> get_states(Script const & s) {
    return get_global<StateDef, AstType::StateDef>(s.states.states);
}

std::vector<Event> get_events(StateDef const & s) {
    return s.events;
}

std::vector<Function> get_functions(Script const & s) {
    return get_global<Function, AstType::Function>(s.globals.functions);
}

}

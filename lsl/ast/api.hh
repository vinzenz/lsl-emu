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
#ifndef GUARD_LSL_AST_API_HH_INCLUDED
#define GUARD_LSL_AST_API_HH_INCLUDED

#include <lsl/ast/ast.hh>

namespace lsl {

std::vector<VarDecl> get_global_variables(Script const &);
std::vector<StateDef> get_states(Script const &);
std::vector<Event> get_events(StateDef const &);
std::vector<Function> get_functions(Script const &);

}

#endif //GUARD_LSL_AST_API_HH_INCLUDED

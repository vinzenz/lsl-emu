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

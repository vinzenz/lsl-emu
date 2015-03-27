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

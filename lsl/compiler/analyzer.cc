#include <lsl/compiler/analyzer.hh>

namespace lsl {

runtime::ValueType value_type(String const & name) {
    if(name == "string") {
        return runtime::ValueType::String;
    }
    else if(name == "integer") {
        return runtime::ValueType::Integer;
    }
    else if(name == "float") {
        return runtime::ValueType::Float;
    }
    else if(name == "key") {
        return runtime::ValueType::Key;
    }
    else if(name == "vector") {
        return runtime::ValueType::Vector;
    }
    else if(name == "rotatation") {
        return runtime::ValueType::Rotation;
    }
    else if(name == "list") {
        return runtime::ValueType::List;
    }
    return runtime::ValueType::Void;
}

FunctionInfo funinfo(std::shared_ptr<lsl::Function> f) {
    FunctionInfo info;
    for(auto const & arg : f->args) {
        info.Parameters.emplace_back(value_type(arg.type), arg.name);
    }
    info.ReturnType = value_type(f->returnType);
    info.Name = f->name;
    info.AstNode = f;
    return info;
}

EventInfo eventinfo(std::shared_ptr<lsl::Event> f) {
    FunctionInfo info;
    info.Called = 0;
    for(auto const & arg : f->args) {
        info.Parameters.emplace_back(value_type(arg.type), arg.name);
    }
    info.Name = f->name;
    info.AstNode = f;
    return info;
}

VarInfo varinfo(std::shared_ptr<lsl::VarDecl> ast) {
    VarInfo info;
    info.References = 0;
    info.Name = ast->name;
    info.Type = value_type(ast->type_name);
    info.AstNode = ast;
    return info;
}

StateInfo stateinfo(std::shared_ptr<StateDef> ast) {
    StateInfo info;
    for(auto const & e : ast->events) {
        info.Events[e.name] = eventinfo(std::make_shared<Event>(e));
    }
    info.AstNode = ast;
    return info;
}

struct AnalizationVistor : AstVisitor {
    Analyzer * analyzer_;
    AnalizationVistor(Analyzer * analyzer)
    : AstVisitor()
    , analyzer_(analyzer)
    {}

    virtual void VisitScript(Script * script) {
        // Preanalyzing the script
        // -> Extraction of all states, events, functions and globals
        // -> That way we can already check for call signatures etc
        for(auto const & e : script->globals.functions) {
            auto const & f = std::static_pointer_cast<Function>(e);
            analyzer_->Info().Functions[f->name] = funinfo(f);
        }
        for(auto const & e : script->globals.globals) {
            auto const & v = std::static_pointer_cast<VarDecl>(e);
            analyzer_->Info().Globals[v->name] = varinfo(v);
        }
        for(auto const & e : script->states.states) {
            analyzer_->Info().States[e->name] = stateinfo(e);
        }
        // script->VisitChildren(this);

    }
};


Analyzer::Analyzer(lsl::Script & script)
: source_info_()
, script_(script)
{}

Analyzer::~Analyzer()
{}

void Analyzer::Perform() {
    AnalizationVistor visitor(this);
    Script().Visit(&visitor);
}

lsl::Script & Analyzer::Script() {
    return script_;
}

SourceInfo & Analyzer::Info() {
    return source_info_;
}

}

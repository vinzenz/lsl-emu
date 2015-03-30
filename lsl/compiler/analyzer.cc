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

FunctionInfo funinfo(lsl::Function* f) {
    FunctionInfo info;
    for(auto const & arg : f->args) {
        info.Parameters.emplace_back(value_type(arg.type), arg.name);
        info.ParamVars[arg.name] = VarInfo();
        info.ParamVars[arg.name].Name = arg.name;
        info.ParamVars[arg.name].IsParameter = true;
        info.ParamVars[arg.name].Initialized = true;
        info.ParamVars[arg.name].Type = value_type(arg.type);
        info.ParamVars[arg.name].AstNode = f;
    }
    info.ReturnType = value_type(f->returnType);
    info.Name = f->name;
    info.AstNode = f;
    return info;
}
FunctionInfo funinfo(std::shared_ptr<lsl::Function> f) {
    return funinfo(f.get());
}
EventInfo eventinfo(lsl::Event * f) {
    EventInfo info;
    for(auto const & arg : f->args) {
        info.Parameters.emplace_back(value_type(arg.type), arg.name);
        info.ParamVars[arg.name] = VarInfo();
        info.ParamVars[arg.name].Name = arg.name;
        info.ParamVars[arg.name].IsParameter = true;
        info.ParamVars[arg.name].Initialized = true;
        info.ParamVars[arg.name].Type = value_type(arg.type);
        info.ParamVars[arg.name].AstNode = f;
    }
    info.Name = f->name;
    info.AstNode = f;
    return info;
}

EventInfo eventinfo(std::shared_ptr<lsl::Event> f) {
    return eventinfo(f.get());
}

VarInfo varinfo(lsl::VarDecl * ast) {
    VarInfo info;
    info.References = 0;
    info.Name = ast->name;
    info.Initialized = !!ast->right;
    info.IsParameter = false;
    info.Type = value_type(ast->type_name);
    info.AstNode = ast;
    return info;
}

VarInfo varinfo(std::shared_ptr<lsl::VarDecl> ast) {
    return varinfo(ast.get());
}

StateInfo stateinfo(StateDef * ast) {
    StateInfo info;
    for(auto const & e : ast->events) {
        info.Events[e.name] = eventinfo(std::make_shared<Event>(e));
    }
    info.AstNode = ast;
    return info;
}

StateInfo stateinfo(std::shared_ptr<StateDef> ast) {
    return stateinfo(ast.get());
}

struct AnalyzationVistor : AstVisitor {
    std::vector<BlockInfo*> scope_stack_;
    std::vector<EventInfo*> call_stack_;
    StateInfo * current_state_;
    Analyzer * analyzer_;

#if 0
    APPLY(UnaryOp) \
    APPLY(Do) \
    APPLY(BinOp) \
    APPLY(BoolOp) \
    APPLY(Comparison) \
    APPLY(For) \
    APPLY(While) \
    APPLY(If) \
    APPLY(NoOp) \
    APPLY(ExpressionList) \
    APPLY(Jump) \
    APPLY(Label)

    // Unneeded visitations
    APPLY(TypeCast) \
    APPLY(Vector) \
    APPLY(Quaternion) \
    APPLY(List) \
    APPLY(Integer) \
    APPLY(StringLit) \
    APPLY(KeyLit) \
    APPLY(Float) \

#endif
    virtual void VisitList(List * i) { i->VisitChildren(this); }
    virtual void VisitQuaternion(Quaternion * i) { i->VisitChildren(this); }
    virtual void VisitVector(Vector * i) { i->VisitChildren(this); }

    virtual void VisitUnaryOp(UnaryOp * i) { i->VisitChildren(this); }
    virtual void VisitBinOp(BinOp * i) { i->VisitChildren(this); }
    virtual void VisitBoolOp(BoolOp * i) { i->VisitChildren(this); }
    virtual void VisitComparison(Comparison * i) { i->VisitChildren(this); }
    virtual void VisitDo(Do * i) { i->VisitChildren(this); }
    virtual void VisitFor(For * i) { i->VisitChildren(this); }
    virtual void VisitWhile(While * i) { i->VisitChildren(this); }
    virtual void VisitIf(If * i) { i->VisitChildren(this); }
    virtual void VisitExpressionList(ExpressionList * i) { i->VisitChildren(this); }
    virtual void VisitJump(Jump * i) { i->VisitChildren(this); }
    virtual void VisitLabel(Label * i) { i->VisitChildren(this); }
    virtual void VisitTypeCast(TypeCast * i) { i->VisitChildren(this); }

    void CreateError(Ast * ptr, AnalyzerError err) {
        analyzer_->Errors().push_back({ptr, call_stack_.empty() ? nullptr : call_stack_.back(), err});
    }
    void CreateError(AstPtr ptr, AnalyzerError err) {
        CreateError(ptr.get(), err);
    }

    SourceInfo & Info() {
        return analyzer_->Info();
    }

    AnalyzationVistor(Analyzer * analyzer)
    : AstVisitor()
    , analyzer_(analyzer)
    {}

    virtual void VisitFunction(Function * fun) {
        call_stack_.push_back(&Info().Functions[fun->name]);
        fun->VisitChildren(this);
        call_stack_.pop_back();
    }

    virtual void VisitBody(Body * body) {
        if(call_stack_.back()->Blocks.empty()) {
            call_stack_.back()->Blocks.push_back(std::make_shared<BlockInfo>());
            call_stack_.back()->Blocks.back()->Parent = nullptr;
            call_stack_.back()->Blocks.back()->Within = call_stack_.back();
            scope_stack_.push_back(call_stack_.back()->Blocks.back().get());
        } else {
            auto cur = std::make_shared<BlockInfo>();
            cur->Within = call_stack_.back();
            cur->Parent = scope_stack_.back();
            call_stack_.back()->Blocks.push_back(cur);
            scope_stack_.push_back(cur.get());
        }

        body->VisitChildren(this);

        scope_stack_.pop_back();
    }

    virtual void VisitCall(Call * call) {
        if(Info().Functions.count(call->name)) {
            Info().Functions[call->name].Called++;
        }
        call->VisitChildren(this);
    }

    virtual void VisitVarDecl(VarDecl * decl) {
        // Check Fun Params
        if(!scope_stack_.empty()) { // Skipping global declarations (We already went through them earlier)
            for(auto const & e : scope_stack_.back()->Within->Parameters) {
                if(std::get<1>(e) == decl->name) {
                    if(call_stack_.back()->Blocks.front().get() == scope_stack_.back()) {
                        // Not allowed to define another variable by this name
                        CreateError(decl, AnalyzerError::ParameterShadowingVariableDeclaration);
                        break; // No need to continue here we already found the shadowing
                    }
                }
            }
            scope_stack_.back()->Variables[decl->name] = varinfo(decl);
            if(decl->right) {
                scope_stack_.back()->Variables[decl->name].Initialized = true;
            }
        }
        decl->VisitChildren(this);
    }

    VarInfo * FindCurrentVarInfo(String const & name) {
        for(auto it = scope_stack_.rbegin(), e = scope_stack_.rend(); it != e; ++it) {
            if((*it)->Variables.count(name)) {
                return &(*it)->Variables[name];
            }
            if(*it == call_stack_.back()->Blocks.front().get()) {
                break; // We're at the end of the current function scope
            }
        }
        // Check Fun Params
        if(scope_stack_.back()->Within->ParamVars.count(name)) {
            return &scope_stack_.back()->Within->ParamVars[name];
        }
        if(Info().Globals.count(name)) {
            return &Info().Globals[name];
        }
        return nullptr; // NO such variable
    }

    virtual void VisitReturn(Return * ret) {
        if(ret->value && (call_stack_.back()->IsEvent || static_cast<FunctionInfo*>(call_stack_.back())->ReturnType == runtime::ValueType::Void)) {
            // Invalid
            CreateError(ret, call_stack_.back()->IsEvent ? AnalyzerError::CanNotReturnValueFromEvent : AnalyzerError::CanNotReturnValueFromVoidFunction);
        }
    }

    virtual void VisitStateChange(StateChange * change) {
        if(Info().States.count(change->state) == 0) {
            CreateError(change, AnalyzerError::UsageOfUndeclaredState);
        }
    }

    template< typename AstT >
    void VisitAssignmentType(AstT * assignment) {
        if(!assignment->left->IsVariable()) {
            // Usage cannot assign to anything but a variable
            CreateError(assignment->left, AnalyzerError::CanOnlyAssignToVariable);
        } else {
            auto varinfo = FindCurrentVarInfo(assignment->left->AsVariable()->name);
            if(varinfo != nullptr) {
                varinfo->Initialized = true;
            }
            // Usage of previously undeclaraed variable -> Caught by VisitVariable
        }
        assignment->VisitChildren(this);
    }

    virtual void VisitAssignment(Assignment * assignment) {
        VisitAssignmentType(assignment);
    }

    virtual void VisitAugAssignment(AugAssignment * assignment) {
        VisitAssignmentType(assignment);
    }

    virtual void VisitVariable(Variable * var) {
        auto info = FindCurrentVarInfo(var->name);
        if(info == nullptr) {
            // Usage of previously undeclared variable
            CreateError(var, AnalyzerError::UsageOfPreviouslyUndeclaredVariable);
        } else {
            ++info->References;
        }
    }

    virtual void VisitStates(States * s) {
        s->VisitChildren(this);
    }

    virtual void VisitEvent(Event * event) {
        call_stack_.push_back(&current_state_->Events[event->name]);
        event->VisitChildren(this);
        call_stack_.pop_back();
    }

    virtual void VisitStateDef(StateDef * state) {
        current_state_ = &Info().States[state->name];
        state->VisitChildren(this);
        current_state_ = nullptr;
    }

    virtual void VisitGlobals(Globals * globals) {
        globals->VisitChildren(this);
    }

    virtual void VisitScript(Script * script) {
        // Preanalyzing the script
        // -> Extraction of all states, events, functions and globals
        // -> That way we can already check for call signatures etc
        for(auto const & e : script->globals.functions) {
            if(Info().Functions.count(e->AsFunction()->name) > 0) {
                CreateError(e, AnalyzerError::DuplicatedFunctionDeclaration);
            }
            Info().Functions[e->AsFunction()->name] = funinfo(e->AsFunction());
        }
        for(auto const & e : script->globals.globals) {
            if(Info().Globals.count(e->AsVarDecl()->name) > 0) {
                CreateError(e, AnalyzerError::DuplicatedGlobalDeclaration);
            }
            Info().Globals[e->AsVarDecl()->name] = varinfo(e->AsVarDecl());
        }
        for(auto const & e : script->states.states) {
            if(Info().States.count(e->name) > 0) {
                CreateError(e, AnalyzerError::DuplicatedStateDeclaration);
            }
            Info().States[e->name] = stateinfo(e);
        }

        script->VisitChildren(this);
    }
};


Analyzer::Analyzer(lsl::Script & script)
: source_info_()
, script_(script)
{}

Analyzer::~Analyzer()
{}

void Analyzer::Perform() {
    AnalyzationVistor visitor(this);
    Script().Visit(&visitor);
}

lsl::Script & Analyzer::Script() {
    return script_;
}

SourceInfo & Analyzer::Info() {
    return source_info_;
}

}

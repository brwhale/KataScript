#pragma once

namespace KataScript {
    // scope control lets you have object lifetimes
    ScopeRef KataScriptInterpreter::newScope(const string& name, ScopeRef scope) {
        // if the scope exists we just use it as is
        auto iter = scope->scopes.find(name);
        if (iter != scope->scopes.end()) {
            return iter->second;
        } else {
            auto& newScope = scope->scopes[name];
            newScope = make_shared<Scope>(name, scope);
            return newScope;
        }
    }

    ScopeRef KataScriptInterpreter::insertScope(ScopeRef existing, ScopeRef parent) {
        existing->parent = parent;
        // if the scope exists we just use it's spot
        auto iter = parent->scopes.find(existing->name);
        if (iter != parent->scopes.end()) {
            iter->second.swap(existing);
            return iter->second;
        } else {
            auto& newScope = parent->scopes[existing->name];
            newScope = existing;
            return newScope;
        }
    }

    void KataScriptInterpreter::closeScope(ScopeRef& scope) {
        if (scope->parent) {
            if (scope->classScope) {
                scope = scope->parent;
            } else {
                auto name = scope->name;
                scope->functions.clear();
                scope->variables.clear();
                scope->scopes.clear();
                scope = scope->parent;
                scope->scopes.erase(name);
            }
        }
    }

    ScopeRef KataScriptInterpreter::newClassScope(const string& name, ScopeRef scope) {
        auto ref = newScope(name, scope);
        ref->classScope = true;
        return ref;
    }
}

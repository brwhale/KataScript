#pragma once

namespace KataScript {
    // scope control lets you have object lifetimes
    ScopeRef KataScriptInterpreter::newScope(const string& name, ScopeRef scope) {
        // if the scope exists we just use it as is
        auto iter = scope->scopes.find(name);
        if (iter != scope->scopes.end()) {
            return iter->second;
        } else {
            return scope->insertScope(make_shared<Scope>(name, scope));
        }
    }

    ScopeRef KataScriptInterpreter::insertScope(ScopeRef existing, ScopeRef parent) {
        existing->parent = parent;
        return parent->insertScope(existing);
    }

    void KataScriptInterpreter::closeScope(ScopeRef& scope) {
        if (scope->parent) {
            if (scope->isClassScope) {
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
        ref->isClassScope = true;
        return ref;
    }
}

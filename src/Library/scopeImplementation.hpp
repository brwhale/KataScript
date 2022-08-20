#pragma once

namespace KataScript {
    // scope control lets you have object lifetimes
    ScopeRef KataScriptInterpreter::newScope(const string& name) {
        // if the scope exists we just use it as is
        auto iter = currentScope->scopes.find(name);
        if (iter != currentScope->scopes.end()) {
            return iter->second;
        } else {
            auto& newScope = currentScope->scopes[name];
            newScope = make_shared<Scope>(name, currentScope);
            return newScope;
        }
    }

    ScopeRef KataScriptInterpreter::insertScope(ScopeRef existing) {
        existing->parent = currentScope;
        // if the scope exists we just use it's spot
        auto iter = currentScope->scopes.find(existing->name);
        if (iter != currentScope->scopes.end()) {
            iter->second.swap(existing);
            return iter->second;
        } else {
            auto& newScope = currentScope->scopes[existing->name];
            newScope = existing;
            return newScope;
        }
    }

    void KataScriptInterpreter::closeCurrentScope() {
        if (currentScope->parent) {
            if (currentScope->classScope) {
                currentScope = currentScope->parent;
            } else {
                auto name = currentScope->name;
                currentScope->functions.clear();
                currentScope->variables.clear();
                currentScope->scopes.clear();
                currentScope = currentScope->parent;
                currentScope->scopes.erase(name);
            }
        }
    }

    void KataScriptInterpreter::newClassScope(const string& name) {
        currentScope = newScope(name);
        currentScope->classScope = true;
    }
}

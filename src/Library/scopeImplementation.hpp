#pragma once

namespace KataScript {
    // scope control lets you have object lifetimes
    KSScopeRef KataScriptInterpreter::newScope(const string& name) {
        // if the scope exists we just use it as is
        auto iter = currentScope->scopes.find(name);
        if (iter != currentScope->scopes.end()) {
            return iter->second;
        } else {
            auto parent = currentScope;
            auto& newScope = currentScope->scopes[name];
            newScope = make_shared<KSScope>(name, parent);
            return newScope;
        }
    }

    KSScopeRef KataScriptInterpreter::resolveScope(const string& name) {
        // if the scope exists we just use it as is
        auto scope = currentScope;
        unordered_map<string, KSScopeRef>::iterator iter;
        while (scope->scopes.end() == (iter = scope->scopes.find(name)) && scope->parent) {
            scope = scope->parent;
        }
        if (scope->scopes.end() != iter) {
            return iter->second;
        }
        throw KSException("Cannot resolve non-existant scope");
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

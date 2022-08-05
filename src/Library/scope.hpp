#pragma once

namespace KataScript {
    struct Scope {
        // this is the main storage object for all functions and variables
        string name;
        ScopeRef parent;
        bool classScope = false;
        unordered_map<string, ValueRef> variables;
        unordered_map<string, ScopeRef> scopes;
        unordered_map<string, FunctionRef> functions;
        Scope(const string& name_, ScopeRef scope) : name(name_), parent(scope) {}
        Scope(const Scope& o) : name(o.name), parent(o.parent), scopes(o.scopes), functions(o.functions) {
            for (auto&& v : o.variables) {
                variables[v.first] = make_shared<Value>(v.second->value, v.second->type);
            }
        }
    };
}
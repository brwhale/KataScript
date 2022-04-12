#pragma once

namespace KataScript {
    struct KSScope {
        // this is the main storage object for all functions and variables
        string name;
        KSScopeRef parent;
        bool classScope = false;
        unordered_map<string, KSValueRef> variables;
        unordered_map<string, KSScopeRef> scopes;
        unordered_map<string, KSFunctionRef> functions;
        KSScope(const string& name_, KSScopeRef scope) : name(name_), parent(scope) {}
        KSScope(const KSScope& o) : name(o.name), parent(o.parent), scopes(o.scopes), functions(o.functions) {
            for (auto&& v : o.variables) {
                variables[v.first] = make_shared<KSValue>(v.second->value, v.second->type);
            }
        }
    };
}
#pragma once
#include <mutex>

namespace KataScript {
    struct Scope {
        // this is the main storage object for all functions and variables
        string name;
        ScopeRef parent;
        bool classScope = false;
#ifndef KATASCRIPT_THREAD_UNSAFE
        std::mutex varInsert;
        std::mutex scopeInsert;
        std::mutex fncInsert;
#endif
        unordered_map<string, ValueRef> variables;
        unordered_map<string, ScopeRef> scopes;
        unordered_map<string, FunctionRef> functions;

        ValueRef insertVar(const string& n, ValueRef val) {
#ifndef KATASCRIPT_THREAD_UNSAFE
            auto l = std::unique_lock(varInsert);
#endif
            variables[n] = val;
            return val;
        }

        ScopeRef insertScope(ScopeRef val) {
#ifndef KATASCRIPT_THREAD_UNSAFE
            auto l = std::unique_lock(scopeInsert);
#endif
            scopes[val->name] = val;
            return val;
        }

        Scope(const string& name_, ScopeRef scope) : name(name_), parent(scope) {}
        Scope(const Scope& o) : name(o.name), parent(o.parent), scopes(o.scopes), functions(o.functions) {
            // copy vars by value when cloning a scope
            for (auto&& v : o.variables) {
                variables[v.first] = make_shared<Value>(v.second->value, v.second->type);
            }
        }
        Scope(const string& name_, const unordered_map<string, ValueRef>& variables_) : name(name_), variables(variables_) {}
    };
}
#pragma once
#include <mutex>

namespace KataScript {
    struct Scope {
        // this is the main storage object for all functions and variables
        string name;
        ScopeRef parent;
        KataScriptInterpreter* host;
#ifndef KATASCRIPT_THREAD_UNSAFE
        std::mutex varInsert;
        std::mutex scopeInsert;
        std::mutex fncInsert;
#endif
        unordered_map<string, ValueRef> variables;
        unordered_map<string, ScopeRef> scopes;
        unordered_map<string, FunctionRef> functions;
        bool isClassScope = false;

        ValueRef& insertVar(const string& n, ValueRef val) {
#ifndef KATASCRIPT_THREAD_UNSAFE
            auto l = std::unique_lock(varInsert);
#endif
            auto& ref = variables[n];
            ref = val;
            return ref;
        }

        ScopeRef insertScope(ScopeRef val) {
#ifndef KATASCRIPT_THREAD_UNSAFE
            auto l = std::unique_lock(scopeInsert);
#endif
            scopes[val->name] = val;
            return val;
        }

        Scope(KataScriptInterpreter* interpereter) : name("global"), parent(nullptr), host(interpereter) {}
        Scope(const string& name_, KataScriptInterpreter* interpereter) : name(name_), parent(nullptr), host(interpereter) {}
        Scope(const string& name_, ScopeRef scope) : name(name_), parent(scope), host(scope->host) {}
        Scope(const Scope& o) : name(o.name), parent(o.parent), scopes(o.scopes), functions(o.functions), host(o.host) {
            // copy vars by value when cloning a scope
            for (auto&& v : o.variables) {
                variables[v.first] = make_shared<Value>(v.second->value);
            }
        }
        Scope(const string& name_, const unordered_map<string, ValueRef>& variables_) : name(name_), variables(variables_) {}
    };
}
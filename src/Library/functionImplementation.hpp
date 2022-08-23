#pragma once

namespace KataScript {
    ValueRef KataScriptInterpreter::callFunction(const string& name, ScopeRef scope, const List& args) {
        return callFunction(resolveFunction(name, scope), scope, args);
    }

    void each(ExpressionRef collection, function<void(ExpressionRef)> func) {
        func(collection);
        for (auto&& ex : *collection) {
            each(ex, func);
        }
    }

    ValueRef KataScriptInterpreter::callFunction(FunctionRef fnc, ScopeRef scope, const List& args) {
        if (fnc->subexpressions.size()) {
            // get function scope
            scope = newScope(fnc->name, scope);
            auto limit = min(args.size(), fnc->argNames.size());
            vector<string> newVars;
            for (size_t i = 0; i < limit; ++i) {
                auto& ref = scope->variables[fnc->argNames[i]];
                if (ref == nullptr) {
                    newVars.push_back(fnc->argNames[i]);
                }
                ref = args[i];
            }

            ValueRef returnVal = nullptr;
            for (auto&& sub : fnc->subexpressions) {
                if (sub->type == ExpressionType::Return) {
                    returnVal = getValue(sub, scope);
                    break;
                } else {
                    auto result = sub->consolidated(this, scope);
                    if (result->type == ExpressionType::Return) {
                        returnVal = get<ValueRef>(result->expression);
                        break;
                    }
                }
            }

            if (fnc->type == FunctionType::CONSTRUCTOR) {
                for (auto&& vr : newVars) {
                    scope->variables.erase(vr);
                }
                returnVal = make_shared<Value>(make_shared<Class>(scope));
            }

            closeScope(scope);
            return returnVal ? returnVal : make_shared<Value>();
        } else if (fnc->lambda) {
            ValueRef returnVal = nullptr;
            if (fnc->type == FunctionType::CONSTRUCTOR) {
                scope = resolveScope(fnc->name, scope);
                returnVal = make_shared<Value>(make_shared<Class>(scope));
                auto temp = currentClass;
                currentClass = returnVal->getClass();
                fnc->lambda(args);
                currentClass = temp;
            } else {
                scope = newScope(fnc->name, scope);
                returnVal = fnc->lambda(args);
            }
            closeScope(scope);
            return returnVal ? returnVal : make_shared<Value>();
        } else if (fnc->scopedLambda) {
            ValueRef returnVal = nullptr;
            if (fnc->type == FunctionType::CONSTRUCTOR) {
                scope = resolveScope(fnc->name, scope);
                returnVal = make_shared<Value>(make_shared<Class>(scope));
                auto temp = currentClass;
                currentClass = returnVal->getClass();
                fnc->scopedLambda(returnVal->getClass()->functionScope, args);
                currentClass = temp;
            } else {
                scope = newScope(fnc->name, scope);
                returnVal = fnc->scopedLambda(scope, args);
            }
            closeScope(scope);
            return returnVal ? returnVal : make_shared<Value>();
        } else {
            //empty func
            return make_shared<Value>();
        }
    }

    FunctionRef KataScriptInterpreter::newFunction(const string& name, ScopeRef scope, FunctionRef func) {
        bool isConstructor = scope->classScope && scope->name == name && scope->parent;
        auto fnScope = isConstructor ? scope->parent : scope;
        auto& ref = fnScope->functions[name];
        ref = func;
        ref->type = isConstructor
            ? FunctionType::CONSTRUCTOR
            : scope->classScope
            ? FunctionType::MEMBER
            : FunctionType::FREE;
        auto& funcvar = resolveVariable(name, fnScope);
        funcvar->type = Type::Function;
        funcvar->value = ref;
        return ref;
    }

    FunctionRef KataScriptInterpreter::newFunction(
        const string& name,
        ScopeRef scope,
        const vector<string>& argNames,
        const vector<ExpressionRef>& body
    ) {
        return newFunction(name, scope, make_shared<Function>(name, argNames, body));
    }

    FunctionRef KataScriptInterpreter::newClass(const string& name, ScopeRef scope, const unordered_map<string, ValueRef>& variables, const unordered_map<string, ScopedLambda>& functions) {
        scope = newClassScope(name, scope);

        scope->variables = variables;
        FunctionRef ret = nullptr;

        for (auto& func : functions) {
            auto ref = newFunction(func.first, scope, func.second);
            if (func.first == name) {
                ret = ref;
            }
        }

        closeScope(scope);

        if (!ret) {
            throw Exception("Cannot create class with no constructor");
        }

        return ret;
    }

    ValueRef& KataScriptInterpreter::newVariable(const string& name, ScopeRef scope) {
        if (currentClass) {
            auto iter = currentClass->variables.find(name);
            if (iter != currentClass->variables.end()) {
                return iter->second;
            }
            auto& varr = currentClass->variables[name];
            varr = make_shared<Value>();
            return varr;
        }
        auto iter = scope->variables.find(name);
        if (iter != scope->variables.end()) {
            return iter->second;
        }
        auto& varr = scope->variables[name];
        varr = make_shared<Value>();
        return varr;
    }

    // name resolution for variables
    ValueRef& KataScriptInterpreter::resolveVariable(const string& name, ScopeRef scope) {
        if (currentClass) {
            auto iter = currentClass->variables.find(name);
            if (iter != currentClass->variables.end()) {
                return iter->second;
            }
        }
        auto initialScope = scope;
        while (scope) {
            auto iter = scope->variables.find(name);
            if (iter != scope->variables.end()) {
                return iter->second;
            } else {
                scope = scope->parent;
            }
        }
        if (!scope) {
            for (auto m : modules) {
                auto iter = m.scope->variables.find(name);
                if (iter != m.scope->variables.end()) {
                    return iter->second;
                }
            }
        }
        auto& varr = initialScope->variables[name];
        varr = make_shared<Value>();
        return varr;
    }

    // name lookup for callfunction api method
    FunctionRef KataScriptInterpreter::resolveFunction(const string& name, ScopeRef scope) {
        auto initialScope = scope;
        while (scope) {
            auto iter = scope->functions.find(name);
            if (iter != scope->functions.end()) {
                return iter->second;
            } else {
                scope = scope->parent;
            }
        }
        auto& func = initialScope->functions[name];
        func = make_shared<Function>(name);
        return func;
    }

    ScopeRef KataScriptInterpreter::resolveScope(const string& name, ScopeRef scope) {
        auto initialScope = scope;
        while (scope) {
            auto iter = scope->scopes.find(name);
            if (iter != scope->scopes.end()) {
                return iter->second;
            } else {
                if (!scope->parent) {
                    if (scope->name == name) {
                        return scope;
                    }
                }
                scope = scope->parent;
            }
        }
        return initialScope->insertScope(make_shared<Scope>(name, initialScope));
    }
}
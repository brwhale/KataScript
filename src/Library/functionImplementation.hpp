#pragma once

namespace KataScript {
    // call function by name
    ValueRef KataScriptInterpreter::callFunction(const string& name, const List& args) {
        return callFunction(resolveFunction(name), args);
    }

    void each(ExpressionRef collection, function<void(ExpressionRef)> func) {
        func(collection);
        for (auto&& ex : *collection) {
            each(ex, func);
        }
    }

    // call function by reference
    ValueRef KataScriptInterpreter::callFunction(FunctionRef fnc, const List& args) {
        if (fnc->subexpressions.size()) {
            auto oldscope = currentScope;
            // get function scope
            currentScope = newScope(fnc->name);
            auto limit = min(args.size(), fnc->argNames.size());
            vector<string> newVars;
            for (size_t i = 0; i < limit; ++i) {
                auto& ref = currentScope->variables[fnc->argNames[i]];
                if (ref == nullptr) {
                    newVars.push_back(fnc->argNames[i]);
                }
                ref = args[i];
            }

            ValueRef returnVal = nullptr;
            for (auto&& sub : fnc->subexpressions) {
                if (sub->type == ExpressionType::Return) {
                    returnVal = getValue(sub);
                    break;
                } else {
                    auto result = sub->consolidated(this);
                    if (result->type == ExpressionType::Return) {
                        returnVal = get<ValueRef>(result->expression);
                        break;
                    }
                }
            }

            if (fnc->type == FunctionType::CONSTRUCTOR) {
                for (auto&& vr : newVars) {
                    currentScope->variables.erase(vr);
                }
                returnVal = make_shared<Value>(make_shared<Class>(currentScope));
            }

            closeCurrentScope();
            currentScope = oldscope;
            return returnVal ? returnVal : make_shared<Value>();
        } else if (fnc->lambda) {
            auto oldscope = currentScope;
            currentScope = newScope(fnc->name);
            ValueRef returnVal = nullptr;
            if (fnc->type == FunctionType::CONSTRUCTOR) {
                returnVal = make_shared<Value>(make_shared<Class>(currentScope));
                auto temp = currentClass;
                currentClass = returnVal->getClass();
                fnc->lambda(args);
                currentClass = temp;
            } else {
                returnVal = fnc->lambda(args);
            }
            closeCurrentScope();
            currentScope = oldscope;
            return returnVal ? returnVal : make_shared<Value>();
        } else {
            //empty func
            return make_shared<Value>();
        }
    }

    FunctionRef KataScriptInterpreter::newFunction(const string& name, FunctionRef func) {
        bool isConstructor = currentScope->classScope && currentScope->name == name && currentScope->parent;
        auto fnScope = isConstructor ? currentScope->parent : currentScope;
        auto& ref = fnScope->functions[name];
        ref = func;
        ref->type = isConstructor
            ? FunctionType::CONSTRUCTOR
            : currentScope->classScope
            ? FunctionType::MEMBER
            : FunctionType::FREE;
        auto& funcvar = resolveVariable(name, fnScope);
        funcvar->type = Type::Function;
        funcvar->value = ref;
        return ref;
    }

    FunctionRef KataScriptInterpreter::newFunction(
        const string& name,
        const vector<string>& argNames,
        const vector<ExpressionRef>& body
    ) {
        return newFunction(name, make_shared<Function>(name, argNames, body));
    }

    FunctionRef KataScriptInterpreter::newFunction(const string& name, const Lambda& lam) {
        return newFunction(name, make_shared<Function>(name, lam));
    }

    FunctionRef KataScriptInterpreter::newClass(const string& name, const unordered_map<string, ValueRef>& variables, const unordered_map<string, Lambda>& functions) {
        newClassScope(name);

        currentScope->variables = variables;
        FunctionRef ret = nullptr;

        for (auto& func : functions) {
            auto ref = newFunction(func.first, func.second);
            if (func.first == name) {
                ret = ref;
            }
        }

        closeCurrentScope();

        if (!ret) {
            throw Exception("Cannot create class with no constructor");
        }

        return ret;
    }

    ValueRef& KataScriptInterpreter::newVariable(const string& name) {
        if (currentClass) {
            auto iter = currentClass->variables.find(name);
            if (iter != currentClass->variables.end()) {
                return iter->second;
            }
            auto& varr = currentClass->variables[name];
            varr = make_shared<Value>();
            return varr;
        }
        auto iter = currentScope->variables.find(name);
        if (iter != currentScope->variables.end()) {
            return iter->second;
        }
        auto& varr = currentScope->variables[name];
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
        if (!scope) {
            scope = currentScope;
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
        if (!scope) {
            scope = currentScope;
        }
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
        if (!scope) {
            scope = currentScope;
        }
        auto initialScope = scope;
        while (scope) {
            auto iter = scope->scopes.find(name);
            if (iter != scope->scopes.end()) {
                return iter->second;
            } else {
                scope = scope->parent;
            }
        }
        auto& func = initialScope->scopes[name];
        func = make_shared<Scope>(name, initialScope);
        return func;
    }
}
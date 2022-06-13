#pragma once

namespace KataScript {
    // call function by name
    KSValueRef KataScriptInterpreter::callFunction(const string& name, const KSList& args) {
        return callFunction(resolveFunction(name), args);
    }

    void each(KSExpressionRef collection, function<void(KSExpressionRef)> func) {
        func(collection);
        for (auto&& ex : *collection) {
            each(ex, func);
        }
    }

    // call function by reference
    KSValueRef KataScriptInterpreter::callFunction(const KSFunctionRef fnc, const KSList& args) {
        if (fnc->subexpressions.size()) {
            auto oldscope = currentScope;
            // get function scope
            newScope(fnc->name);
            auto limit = min(args.size(), fnc->argNames.size());
            vector<string> newVars;
            for (size_t i = 0; i < limit; ++i) {
                auto& ref = currentScope->variables[fnc->argNames[i]];
                if (ref == nullptr) {
                    newVars.push_back(fnc->argNames[i]);
                }
                ref = args[i];
            }

            KSValueRef returnVal = nullptr;
            for (auto&& sub : fnc->subexpressions) {
                if (sub->type == KSExpressionType::Return) {
                    returnVal = getValue(sub);
                    break;
                } else {
                    auto result = sub->consolidated(this);
                    if (result->type == KSExpressionType::Return) {
                        returnVal = get<KSValueRef>(result->expression);
                        break;
                    }
                }
            }

            if (fnc->type == KSFunctionType::CONSTRUCTOR) {
                for (auto&& vr : newVars) {
                    currentScope->variables.erase(vr);
                }
                returnVal = make_shared<KSValue>(make_shared<KSClass>(currentScope));
            }

            closeCurrentScope();
            currentScope = oldscope;
            return returnVal ? returnVal : make_shared<KSValue>();
        } else if (fnc->lambda) {
            return fnc->lambda(args);
        } else {
            //empty func
            return make_shared<KSValue>();
        }
    }

    KSFunctionRef& KataScriptInterpreter::newFunction(
        const string& name,
        const vector<string>& argNames,
        const vector<KSExpressionRef>& body
    ) {
        bool isConstructor = currentScope->classScope && currentScope->name == name && currentScope->parent;
        auto fnScope = isConstructor ? currentScope->parent : currentScope;
        auto& ref = fnScope->functions[name];
        ref = make_shared<KSFunction>(name, argNames, body);
        ref->type = isConstructor
            ? KSFunctionType::CONSTRUCTOR
            : currentScope->classScope
            ? KSFunctionType::MEMBER
            : KSFunctionType::FREE;
        auto& funcvar = resolveVariable(name, fnScope);
        funcvar->type = KSType::Function;
        funcvar->value = ref;
        return ref;
    }

    KSFunctionRef& KataScriptInterpreter::newFunction(const string& name, const KSLambda& lam) {
        auto& ref = currentScope->functions[name];
        ref = make_shared<KSFunction>(name, lam);
        auto& funcvar = resolveVariable(name);
        funcvar->type = KSType::Function;
        funcvar->value = ref;
        return ref;
    }

    KSFunctionRef& KataScriptInterpreter::newLibraryFunction(
        const string& name,
        const KSLambda& lam,
        KSScopeRef scope) {
        auto oldScope = currentScope;
        currentScope = scope;
        auto& ref = newFunction(name, lam);
        currentScope = oldScope;
        return ref;
    }

    // name resolution for variables
    KSValueRef& KataScriptInterpreter::resolveVariable(const string& name, KSScopeRef scope) {
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
                auto iter = m->variables.find(name);
                if (iter != m->variables.end()) {
                    return iter->second;
                }
            }
        }
        auto& varr = initialScope->variables[name];
        varr = make_shared<KSValue>();
        return varr;
    }

    // name lookup for callfunction api method
    KSFunctionRef KataScriptInterpreter::resolveFunction(const string& name) {
        auto scope = currentScope;
        while (scope) {
            auto iter = scope->functions.find(name);
            if (iter != scope->functions.end()) {
                return iter->second;
            } else {
                scope = scope->parent;
            }
        }
        return make_shared<KSFunction>(name);
    }
}
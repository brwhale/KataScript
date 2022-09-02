#include "KataScript.hpp"
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

    ValueRef KataScriptInterpreter::callFunction(FunctionRef fnc, ScopeRef scope, const List& args, Class* classs) {
        switch (fnc->getBodyType()) {
            case FunctionBodyType::Subexpressions: {
                auto& subexpressions = get<vector<ExpressionRef>>(fnc->body);
                // get function scope
                scope = fnc->type == FunctionType::constructor ? resolveScope(fnc->name, scope) : newScope(fnc->name, scope);
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

                if (fnc->type == FunctionType::constructor) {
                    returnVal = make_shared<Value>(make_shared<Class>(scope));
                    for (auto&& sub : subexpressions) {
                        getValue(sub, scope, returnVal->getClass().get());
                    }
                } else {
                    for (auto&& sub : subexpressions) {
                        if (sub->type == ExpressionType::Return) {
                            returnVal = getValue(sub, scope, classs);
                            break;
                        } else {
                            auto result = consolidated(sub, scope, classs);
                            if (result->type == ExpressionType::Return) {
                                returnVal = get<ValueRef>(result->expression);
                                break;
                            }
                        }
                    }
                }

                if (fnc->type == FunctionType::constructor) {
                    for (auto&& vr : newVars) {
                        scope->variables.erase(vr);
                        returnVal->getClass()->variables.erase(vr);
                    }
                }

                closeScope(scope);
                return returnVal ? returnVal : make_shared<Value>();
            }
            case FunctionBodyType::Lambda: {
                scope = newScope(fnc->name, scope);
                auto returnVal = get<Lambda>(fnc->body)(args);
                closeScope(scope);
                return returnVal ? returnVal : make_shared<Value>();
            }
            case FunctionBodyType::ScopedLambda: {
                scope = newScope(fnc->name, scope);
                auto returnVal = get<ScopedLambda>(fnc->body)(scope, args);
                closeScope(scope);
                return returnVal ? returnVal : make_shared<Value>();
            }
            case FunctionBodyType::ClassLambda: {
                scope = resolveScope(fnc->name, scope);
                if (fnc->type == FunctionType::constructor) {
                    auto returnVal = make_shared<Value>(make_shared<Class>(scope));
                    get<ClassLambda>(fnc->body)(returnVal->getClass().get(), scope, args);
                    closeScope(scope);
                    return returnVal;
                } else if (fnc->type == FunctionType::free && args.size() >= 2 && args[1]->getType() == Type::Class) {
                    // apply function
                    classs = args[1]->getClass().get();
                }
                auto ret = get<ClassLambda>(fnc->body)(classs, scope, args);
                closeScope(scope);
                return ret;
            }
        }

        //empty func
        return make_shared<Value>();
    }

    FunctionRef KataScriptInterpreter::newFunction(const string& name, ScopeRef scope, FunctionRef func) {
        auto& ref = scope->functions[name];
        ref = func;
        if (ref->type == FunctionType::free && scope->isClassScope) {
            ref->type = FunctionType::member;
        }
        auto funcvar = resolveVariable(name, scope);
        funcvar->value = ref;
        return ref;
    }

    FunctionRef KataScriptInterpreter::newFunction(
        const string& name,
        ScopeRef scope,
        const vector<string>& argNames
    ) {
        return newFunction(name, scope, make_shared<Function>(name, argNames));
    }

    FunctionRef KataScriptInterpreter::newConstructor(const string& name, ScopeRef scope, FunctionRef func) {
        auto& ref = scope->functions[name];
        ref = func;
        ref->type = FunctionType::constructor;
        auto funcvar = resolveVariable(name, scope);
        funcvar->value = ref;
        return ref;
    }

    FunctionRef KataScriptInterpreter::newConstructor(
        const string& name,
        ScopeRef scope,
        const vector<string>& argNames
    ) {
        return newConstructor(name, scope, make_shared<Function>(name, argNames));
    }

    FunctionRef KataScriptInterpreter::newClass(const string& name, ScopeRef scope, const unordered_map<string, ValueRef>& variables, const ClassLambda& constructor, const unordered_map<string, ClassLambda>& functions) {
        scope = newClassScope(name, scope);

        scope->variables = variables;
        FunctionRef ret = newConstructor(name, scope->parent, make_shared<Function>(name, constructor));

        for (auto& func : functions) {
            newFunction(func.first, scope, func.second);
        }

        closeScope(scope);

        return ret;
    }

    // name resolution for variables
    ValueRef& KataScriptInterpreter::resolveVariable(const string& name, ScopeRef scope) {
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
        return initialScope->insertVar(name, make_shared<Value>());
    }

    ValueRef& KataScriptInterpreter::resolveVariable(const string& name, Class* classs, ScopeRef scope) {
        auto iter = classs->variables.find(name);
        if (iter != classs->variables.end()) {
            return iter->second;
        }
        return resolveVariable(name, scope);
    }

    FunctionRef KataScriptInterpreter::resolveFunction(const string& name, Class* classs, ScopeRef scope) {
        auto iter = classs->functionScope->functions.find(name);
        if (iter != classs->functionScope->functions.end()) {
            return iter->second;
        }
        return resolveFunction(name, scope);
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
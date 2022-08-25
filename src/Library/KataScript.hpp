// copyright Garrett Skelton 2020
// MIT license
#pragma once

#include <cstdarg>
#include <iostream>
#include <cmath>
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>

#include "exception.hpp"
#include "stringUtils.hpp"
#include "types.hpp"
#include "value.hpp"
#include "expressions.hpp"
#include "scope.hpp"
#include "modules.hpp"

namespace KataScript {
    // state enum for state machine for token by token parsing
    enum class ParseState : uint8_t {
        beginExpression,
        readLine,
        defineVar,
        defineFunc,
        defineClass,
        classArgs,
        funcArgs,
        returnLine,
        ifCall,
        expectIfEnd,
        loopCall,
        forEach,
        importModule
    };

    // finally we have our interpereter
    class KataScriptInterpreter {
        friend Expression;
        vector<Module> modules;
        vector<Module> optionalModules;
        ScopeRef globalScope = make_shared<Scope>("global", nullptr);
        ScopeRef parseScope = globalScope;
        ExpressionRef currentExpression;
        FunctionRef applyFunctionLocation;
        ValueRef applyFunctionVarLocation;
        ValueRef listIndexFunctionVarLocation;
        ValueRef identityFunctionVarLocation;

        ParseState parseState = ParseState::beginExpression;
        vector<string_view> parseStrings;
        int outerNestLayer = 0;
        bool lastStatementClosedScope = false;
        bool lastStatementWasElse = false;
        bool lastTokenEndCurlBraket = false;
        uint64_t currentLine = 0;
        ParseState prevState = ParseState::beginExpression;
        ModulePrivilegeFlags allowedModulePrivileges;

        bool needsToReturn(ExpressionRef expr, ValueRef& returnVal, ScopeRef scope, ClassRef classs);
        ExpressionRef consolidated(ExpressionRef exp, ScopeRef scope, ClassRef classs);

        ExpressionRef getResolveVarExpression(const string& name, bool classScope);
        ExpressionRef getExpression(const vector<string_view>& strings, ScopeRef scope, ClassRef classs);
        ValueRef getValue(const vector<string_view>& strings, ScopeRef scope, ClassRef classs);
        ValueRef getValue(ExpressionRef expr, ScopeRef scope, ClassRef classs);

        void clearParseStacks();
        void closeDanglingIfExpression();
        void parse(string_view token);
        
        ScopeRef newClassScope(const string& name, ScopeRef scope);
        void closeScope(ScopeRef& scope);
        bool closeCurrentExpression();
        FunctionRef newFunction(const string& name, ScopeRef scope, FunctionRef func);
        FunctionRef newFunction(const string& name, ScopeRef scope, const vector<string>& argNames);
        FunctionRef newConstructor(const string& name, ScopeRef scope, FunctionRef func);
        FunctionRef newConstructor(const string& name, ScopeRef scope, const vector<string>& argNames);
        Module* getOptionalModule(const string& name);
        void createStandardLibrary();
        void createOptionalModules();
    public:
        ScopeRef newScope(const string& name, ScopeRef scope);
        ScopeRef insertScope(ScopeRef existing, ScopeRef parent);
        ScopeRef newScope(const string& name) { return newScope(name, globalScope); }
        ScopeRef insertScope(ScopeRef existing) { return insertScope(existing, globalScope); }
        FunctionRef newClass(const string& name, ScopeRef scope, const unordered_map<string, ValueRef>& variables, const ClassLambda& constructor, const unordered_map<string, ClassLambda>& functions);
        FunctionRef newClass(const string& name, const unordered_map<string, ValueRef>& variables, const ClassLambda& constructor, const unordered_map<string, ClassLambda>& functions) { return newClass(name, globalScope, variables, constructor, functions); }
        FunctionRef newFunction(const string& name, ScopeRef scope, const Lambda& lam){ return newFunction(name, scope, make_shared<Function>(name, lam)); }
        FunctionRef newFunction(const string& name, const Lambda& lam) { return newFunction(name, globalScope, lam); }
        FunctionRef newFunction(const string& name, ScopeRef scope, const ScopedLambda& lam) { return newFunction(name, scope, make_shared<Function>(name, lam)); }
        FunctionRef newFunction(const string& name, const ScopedLambda& lam) { return newFunction(name, globalScope, lam); }
        FunctionRef newFunction(const string& name, ScopeRef scope, const ClassLambda& lam) { return newFunction(name, scope, make_shared<Function>(name, lam)); }
        ScopeRef newModule(const string& name, ModulePrivilegeFlags flags, const unordered_map<string, Lambda>& functions);
        ValueRef callFunction(const string& name, ScopeRef scope, const List& args);
        ValueRef callFunction(FunctionRef fnc, ScopeRef scope, const List& args, ClassRef classs = nullptr);
        ValueRef callFunction(const string& name, const List& args) { return callFunction(name, globalScope, args); }
        ValueRef callFunction(FunctionRef fnc, const List& args) { return callFunction(fnc, globalScope, args); }
        template <typename ... Ts>
        ValueRef callFunctionWithArgs(FunctionRef fnc, ScopeRef scope, Ts...args) {
            List argsList = { make_shared<Value>(args)... };
            return callFunction(fnc, scope, argsList);
        }
        template <typename ... Ts>
        ValueRef callFunctionWithArgs(FunctionRef fnc, Ts...args) {
            List argsList = { make_shared<Value>(args)... };
            return callFunction(fnc, globalScope, argsList);
        }

        ValueRef& resolveVariable(const string& name, ClassRef classs, ScopeRef scope);
        ValueRef& resolveVariable(const string& name, ScopeRef scope);
        FunctionRef resolveFunction(const string& name, ScopeRef scope);
        ScopeRef resolveScope(const string& name, ScopeRef scope);

        ValueRef resolveVariable(const string& name) { return resolveVariable(name, globalScope); }
        FunctionRef resolveFunction(const string& name) { return resolveFunction(name, globalScope); }
        ScopeRef resolveScope(const string& name) { return resolveScope(name, globalScope); }
        
        bool readLine(string_view text);
        bool evaluate(string_view script);
        bool evaluateFile(const string& path);
        bool readLine(string_view text, ScopeRef scope);
        bool evaluate(string_view script, ScopeRef scope);
        bool evaluateFile(const string& path, ScopeRef scope);
        void clearState();
        KataScriptInterpreter(ModulePrivilegeFlags priv) : allowedModulePrivileges(priv) 
            { createStandardLibrary(); if (priv) { createOptionalModules(); } }
        KataScriptInterpreter(ModulePrivilege priv) : KataScriptInterpreter(static_cast<ModulePrivilegeFlags>(priv)) { }
        KataScriptInterpreter() : KataScriptInterpreter(ModulePrivilegeFlags()) { }
    };
}

#if defined(__EMSCRIPTEN__) || defined(KATASCRIPT_INTERNAL_PRINT)
#define KATASCRIPT_DO_INTERNAL_PRINT
#endif

#ifdef KATASCRIPT_IMPL

#include "typeConversion.hpp"
#include "parsing.hpp"
#include "scopeImplementation.hpp"
#include "functionImplementation.hpp"
#include "expressionImplementation.hpp"
#include "modulesImplementation.hpp"
#include "optionalModules.hpp"

#endif // KATASCRIPT_IMPL

// copyright Garrett Skelton 2020
// MIT license
#pragma once

#include <cstdarg>
#include <iostream>
#include <cmath>
#include <chrono>
#include <fstream>

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
        ScopeRef currentScope = globalScope;
        ClassRef currentClass = nullptr;
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

        ExpressionRef getExpression(const vector<string_view>& strings);
        ValueRef getValue(const vector<string_view>& strings);

        void clearParseStacks();
        void closeDanglingIfExpression();
        void parse(string_view token);
        ValueRef& newVariable(const string& name);
        
        ValueRef getValue(ExpressionRef expr);
        void newClassScope(const string& name);
        void closeCurrentScope();
        bool closeCurrentExpression();
        ValueRef callFunction(const string& name, const List& args);
        ValueRef callFunction(FunctionRef fnc, const List& args);
        FunctionRef newFunction(const string& name, FunctionRef func);
        FunctionRef newFunction(const string& name, const vector<string>& argNames, const vector<ExpressionRef>& body);
        Module* getOptionalModule(const string& name);
        void createStandardLibrary();
        void createOptionalModules();
    public:
        ScopeRef newScope(const string& name);
        ScopeRef insertScope(ScopeRef existing);
        FunctionRef newClass(const string& name, const unordered_map<string, ValueRef>& variables, const unordered_map<string, Lambda>& functions);
        FunctionRef newFunction(const string& name, const Lambda& lam);
        ScopeRef newModule(const string& name, ModulePrivilegeFlags flags, const unordered_map<string, Lambda>& functions);
        template <typename ... Ts>
        ValueRef callFunction(FunctionRef fnc, Ts...args) {
            List argsList = { make_shared<Value>(args)... };
            return callFunction(fnc, argsList);
        }
        template <typename ... Ts>
        ValueRef callFunctionFromScope(FunctionRef fnc, ScopeRef scope, Ts...args) {
            auto temp = currentScope;
            currentScope = scope;
            auto ret = callFunction(fnc, args...);
            currentScope = temp;
            return ret;
        }
        ValueRef& resolveVariable(const string& name, ScopeRef scope = nullptr);
        FunctionRef resolveFunction(const string& name, ScopeRef scope = nullptr);
        ScopeRef resolveScope(const string& name, ScopeRef scope = nullptr);
        
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

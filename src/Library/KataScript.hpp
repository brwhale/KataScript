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

namespace KataScript {
    // state enum for state machine for token by token parsing
    enum class KSParseState : uint8_t {
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
        friend KSExpression;
        vector<KSScopeRef> modules;
        KSScopeRef globalScope = make_shared<KSScope>("global", nullptr);
        KSScopeRef currentScope = globalScope;
        KSClassRef currentClass = nullptr;
        KSExpressionRef currentExpression;
        KSFunction* applyFunctionLocation = nullptr;

        KSParseState parseState = KSParseState::beginExpression;
        vector<string> parseStrings;
        int outerNestLayer = 0;
        bool lastStatementClosedScope = false;
        bool lastStatementWasElse = false;
        bool lastTokenEndCurlBraket = false;
        uint64_t currentLine = 0;
        KSParseState prevState = KSParseState::beginExpression;

        KSExpressionRef getExpression(const vector<string>& strings);
        KSValueRef getValue(const vector<string>& strings);

        void clearParseStacks();
        void closeDanglingIfExpression();
        void parse(const string& token);
        KSFunctionRef& newLibraryFunction(const string& name, const KSLambda& lam, KSScopeRef scope);
        KSValueRef& newVariable(const string& name);
        KSFunctionRef& newFunction(const string& name, const vector<string>& argNames, const vector<KSExpressionRef>& body);
        KSValueRef getValue(KSExpressionRef expr);
        void newClassScope(const string& name);
        KSScopeRef resolveScope(const string& name);
        void closeCurrentScope();
        bool closeCurrentExpression();
        KSValueRef callFunction(const string& name, const KSList& args);
        KSValueRef callFunction(const KSFunctionRef fnc, const KSList& args);
        KSFunctionRef& newFunction(const string& name, KSFunctionRef func);
    public:
        KSScopeRef newScope(const string& name);
        KSFunctionRef newClass(const string& name, const unordered_map<string, KSValueRef>& variables, const unordered_map<string, KSLambda>& functions);
        KSFunctionRef& newFunction(const string& name, const KSLambda& lam);
        template <typename ... Ts>
        KSValueRef callFunction(const KSFunctionRef fnc, Ts...args) {
            KSList argsList = { make_shared<KSValue>(args)... };
            return callFunction(fnc, argsList);
        }
        KSValueRef& resolveVariable(const string& name, KSScopeRef scope = nullptr);
        KSFunctionRef resolveFunction(const string& name, KSScopeRef scope = nullptr);
        
        bool readLine(const string& text);
        bool evaluate(const string& script);
        bool evaluateFile(const string& path);
        bool readLine(const string& text, KSScopeRef scope);
        bool evaluate(const string& script, KSScopeRef scope);
        bool evaluateFile(const string& path, KSScopeRef scope);
        void clearState();
        KataScriptInterpreter();
    };
}

#ifdef KATASCRIPT_IMPL

#include "typeConversion.hpp"
#include "parsing.hpp"
#include "scopeImplementation.hpp"
#include "functionImplementation.hpp"
#include "expressionImplementation.hpp"
#include "modules.hpp"

#endif // KATASCRIPT_IMPL

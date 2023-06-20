#include "KataScript.hpp"
#pragma once

namespace KataScript {
    ReturnResult KataScriptInterpreter::needsToReturn(ExpressionRef exp, ScopeRef scope, Class* classs) {
        if (exp->type == ExpressionType::Return || exp->type == ExpressionType::Break) {
            return ReturnResult{ getValue(exp, scope, classs), exp->type == ExpressionType::Return ? ReturnType::Return : ReturnType::Break };
        } else {
            auto result = consolidated(exp, scope, classs);
            if (result->type == ExpressionType::Return || result->type == ExpressionType::Break) {
                return ReturnResult{ get<ValueRef>(result->expression), result->type == ExpressionType::Return ? ReturnType::Return : ReturnType::Break };
            }
        }
        return ReturnResult{ nullptr, ReturnType::None };
    }

    ReturnResult KataScriptInterpreter::needsToReturn(const vector<ExpressionRef>& subexpressions, ScopeRef scope, Class* classs) {
        for (auto&& sub : subexpressions) {
            if (auto returnVal = needsToReturn(sub, scope, classs)) {
                return returnVal;
            }
        }
        return ReturnResult{ nullptr, ReturnType::None };
    }

    // walk the tree depth first and replace any function expressions 
    // with a value expression of their results
    ExpressionRef KataScriptInterpreter::consolidated(ExpressionRef exp, ScopeRef scope, Class* classs) {
        switch (exp->type) {
        case ExpressionType::DefineVar:
        {
            auto& def = get<DefineVar>(exp->expression);
            auto& varr = scope->variables[def.name];
            if (def.defineExpression) {
                auto val = getValue(def.defineExpression, scope, classs);
                varr = make_shared<Value>(val->value);
            } else {
                varr = make_shared<Value>();
            }
            return make_shared<Expression>(varr, ExpressionType::Value);
        }
        break;
        case ExpressionType::ResolveVar:
            return make_shared<Expression>(resolveVariable(get<ResolveVar>(exp->expression).name, scope), ExpressionType::Value);
        case ExpressionType::MemberVariable: {
            auto& expr = get<MemberVariable>(exp->expression);
            auto classToUse = classs;
            if (expr.object) {
                auto val = getValue(expr.object, scope, classs);
                if (val->getType() == Type::Class) {
                    auto clRef = val->getClass();
                    classToUse = clRef.get();
                }
            }
            return make_shared<Expression>(resolveVariable(expr.name, classToUse, scope), ExpressionType::Value);
        }
        case ExpressionType::MemberFunctionCall: {
            auto& expr = get<MemberFunctionCall>(exp->expression);
            List args;
            for (auto&& sub : expr.subexpressions) {
                args.push_back(getValue(sub, scope, classs));
            }
            auto val = getValue(expr.object, scope, classs);
            if (val->getType() != Type::Class) {
                args.insert(args.begin(), val);
                return make_shared<Expression>(callFunction(resolveVariable(expr.functionName, scope)->getFunction(), scope, args, classs), ExpressionType::Value);
            }
            auto owningClass = val->getClass();
            return make_shared<Expression>(callFunction(resolveFunction(expr.functionName, owningClass.get(), scope), scope, args, owningClass), ExpressionType::Value);
        }
        case ExpressionType::Return:
            return make_shared<Expression>(getValue(get<Return>(exp->expression).expression, scope, classs), ExpressionType::Value);
            break;
        case ExpressionType::Break:
            return make_shared<Expression>(make_shared<Value>(), ExpressionType::Value);
            break;
        case ExpressionType::FunctionCall:
        {
            List args;
            auto& funcExpr = get<FunctionExpression>(exp->expression);
            
            for (auto&& sub : funcExpr.subexpressions) {
                args.push_back(getValue(sub, scope, classs));
            }

            if (funcExpr.function->getType() == Type::String) {
                funcExpr.function = resolveVariable(funcExpr.function->getString(), scope);
            }
            return make_shared<Expression>(callFunction(funcExpr.function->getFunction(), scope, args, classs), ExpressionType::Value);
        }
        break;
        case ExpressionType::Loop:
        {
            scope = newScope("loop", scope);
            auto& loopexp = get<Loop>(exp->expression);
            if (loopexp.initExpression) {
                getValue(loopexp.initExpression, scope, classs);
            }
            ReturnResult returnVal;
            while (!returnVal && getValue(loopexp.testExpression, scope, classs)->getBool()) {
                returnVal = needsToReturn(loopexp.subexpressions, scope, classs);
                if (!returnVal && loopexp.iterateExpression) {
                    getValue(loopexp.iterateExpression, scope, classs);
                }
            }
            closeScope(scope);
            if (returnVal) {
                return make_shared<Expression>(returnVal.value, returnVal.type == ReturnType::Return ? ExpressionType::Return : ExpressionType::Break);
            } else {
                return make_shared<Expression>(make_shared<Value>(), ExpressionType::Value);
            }
        }
        break;
        case ExpressionType::ForEach:
        {
            scope = newScope("loop", scope);
            auto varr = resolveVariable(get<Foreach>(exp->expression).iterateName, scope);
            auto list = getValue(get<Foreach>(exp->expression).listExpression, scope, classs);
            auto& subs = get<Foreach>(exp->expression).subexpressions;
            ReturnResult returnVal;
            if (list->getType() == Type::Dictionary) {
                for (auto&& in : *list->getDictionary().get()) {
                    if (returnVal) break;
                    *varr = *in.second;
                    returnVal = needsToReturn(subs, scope, classs);
                }
            } else if (list->getType() == Type::List) {
                for (auto&& in : list->getList()) {
                    if (returnVal) break;
                    *varr = *in;
                    returnVal = needsToReturn(subs, scope, classs);
                }
            } else if (list->getType() == Type::Array) {
                auto& arr = list->getArray();
                switch (arr.getType()) {
                case Type::Int: {
                    auto vec = list->getStdVector<Int>();
                    for (auto&& in : vec) {
                        if (returnVal) break;
                        *varr = Value(in);
                        returnVal = needsToReturn(subs, scope, classs);
                    }
                }
                break;
                case Type::Float: {
                    auto vec = list->getStdVector<Float>();
                    for (auto&& in : vec) {
                        if (returnVal) break;
                        *varr = Value(in);
                        returnVal = needsToReturn(subs, scope, classs);
                    }
                }
                break;
                case Type::Vec3: {
                    auto vec = list->getStdVector<vec3>();
                    for (auto&& in : vec) {
                        if (returnVal) break;
                        *varr = Value(in);
                        returnVal = needsToReturn(subs, scope, classs);
                    }
                }
                break;
                case Type::String: {
                    auto vec = list->getStdVector<string>();
                    for (auto&& in : vec) {
                        if (returnVal) break;
                        *varr = Value(in);
                        returnVal = needsToReturn(subs, scope, classs);
                    }
                }
                break;
                default:
                    break;
                }
            }
            closeScope(scope);
            if (returnVal) {
                return make_shared<Expression>(returnVal.value, returnVal.type == ReturnType::Return ? ExpressionType::Return : ExpressionType::Break);
            } else {
                return make_shared<Expression>(make_shared<Value>(), ExpressionType::Value);
            }
        }
        break;
        case ExpressionType::IfElse:
        {
            ReturnResult returnVal;
            for (auto& express : get<IfElse>(exp->expression)) {
                if (!express.testExpression || getValue(express.testExpression, scope, classs)->getBool()) {
                    scope = newScope("ifelse", scope);
                    returnVal = needsToReturn(express.subexpressions, scope, classs);
                    closeScope(scope);
                    break;
                }
            }
            if (returnVal) {
                return make_shared<Expression>(returnVal.value, returnVal.type == ReturnType::Return ? ExpressionType::Return : ExpressionType::Break);
            } else {
                return make_shared<Expression>(make_shared<Value>(), ExpressionType::Value);
            }
        }
        break;
        default:
            break;
        }
        return make_shared<Expression>(get<ValueRef>(exp->expression), ExpressionType::Value);
    }

    // evaluate an expression from tokens
    ValueRef KataScriptInterpreter::getValue(const vector<string_view>& strings, ScopeRef scope, Class* classs) {
        return getValue(getExpression(strings, scope, classs), scope, classs);
    }

    // evaluate an expression from expressionRef
    ValueRef KataScriptInterpreter::getValue(ExpressionRef exp, ScopeRef scope, Class* classs) {
        // copy the expression so that we don't lose it when we consolidate
        return get<ValueRef>(consolidated(exp, scope, classs)->expression);
    }

    // since the 'else' block in  an if/elfe is technically in a different scope
    // ifelse espressions are not closed immediately and instead left dangling
    // until the next expression is anything other than an 'else' or the else is unconditional
    bool KataScriptInterpreter::closeDanglingIfExpression() {
        if (currentExpression && currentExpression->type == ExpressionType::IfElse) {
            if (currentExpression->parent) {
                currentExpression = currentExpression->parent;
            } else {
                getValue(currentExpression, parseScope, nullptr);
                currentExpression = nullptr;
            }
            return true;
        }
        return false;
    }

    bool KataScriptInterpreter::closeCurrentExpression() {
        if (currentExpression) {
            if (currentExpression->type != ExpressionType::IfElse) {
                if (currentExpression->parent) {
                    currentExpression = currentExpression->parent;
                } else {
                    if (currentExpression->type != ExpressionType::FunctionDef) {
                        getValue(currentExpression, parseScope, nullptr);
                    }
                    currentExpression = nullptr;
                }
                return true;
            }
        }
        return false;
    }
}
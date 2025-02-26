#include "KataScript.hpp"
#pragma once

namespace KataScript {
    bool isReturnType(ExpressionType t) {
        return t == ExpressionType::Return || t == ExpressionType::Break || t == ExpressionType::Continue;
    }
    ReturnResult KataScriptInterpreter::needsToReturn(ExpressionRef exp, ScopeRef scope, Class* classs) {
        if (isReturnType(exp->type)) {
            return ReturnResult{ getValue(exp, scope, classs), exp->type};
        } else {
            auto result = consolidated(exp, scope, classs);
            if (isReturnType(result->type)) {
                return ReturnResult{ get<ValueRef>(result->expression), result->type };
            }
        }
        return ReturnResult{ nullptr, ExpressionType::None };
    }

    ReturnResult KataScriptInterpreter::needsToReturn(const vector<ExpressionRef>& subexpressions, ScopeRef scope, Class* classs) {
        for (auto&& sub : subexpressions) {
            if (auto returnVal = needsToReturn(sub, scope, classs)) {
                if (returnVal.type == ExpressionType::Continue) {
                    returnVal.type = ExpressionType::None;
                } // skipping the rest of the subexpressions completes the purpose of Continue, hence we can just set type to none to become falsy
                return returnVal;
            }
        }
        return ReturnResult{ nullptr, ExpressionType::None };
    }

    // walk the tree depth first and replace any function expressions 
    // with a value expression of their results
    ExpressionRef KataScriptInterpreter::consolidated(ExpressionRef exp, ScopeRef scope, Class* classs) {
        switch (exp->type) {
        case ExpressionType::Constant:
            return make_shared<Expression>(make_shared<Value>(get<Constant>(exp->expression).val));
        case ExpressionType::DefineVar: {
            auto& def = get<DefineVar>(exp->expression);
            auto& varr = scope->variables[def.name];
            if (def.defineExpression) {
                auto val = getValue(def.defineExpression, scope, classs);
                varr = make_shared<Value>(val->value);
            } else {
                varr = makeNull();
            }
            return make_shared<Expression>(varr);
        }
        case ExpressionType::ResolveVar:
            return make_shared<Expression>(resolveVariable(get<ResolveVar>(exp->expression).name, scope));
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
            return make_shared<Expression>(resolveVariable(expr.name, classToUse, scope));
        }
        case ExpressionType::MemberFunctionCall: {
            auto& expr = get<MemberFunctionCall>(exp->expression);
            auto val = getValue(expr.object, scope, classs);
            if (val->getType() == Type::ArrayMember) {
                val = val->getArrayMember().getValue();
            }
            auto fncRef = (val->getType() != Type::Class) ? resolveVariable(expr.functionName, scope)->getFunction() : resolveFunction(expr.functionName, val->getClass().get(), scope);
            List args;
            for (auto&& sub : expr.subexpressions) {
                auto subval = getValue(sub, scope, classs);
                args.push_back(subval->getType() == Type::ArrayMember ? subval->getArrayMember().getValue() : subval);
            }
            if (val->getType() != Type::Class) {
                args.insert(args.begin(), val);
                return make_shared<Expression>(callFunction(fncRef, scope, args, classs));
            }
            return make_shared<Expression>(callFunction(fncRef, scope, args, val->getClass()));
        }
        case ExpressionType::Return:
            return make_shared<Expression>(getValue(get<Return>(exp->expression).expression, scope, classs));
        case ExpressionType::Break:
            return make_shared<Expression>(makeNull());
        case ExpressionType::FunctionCall: {
            auto& funcExpr = get<FunctionExpression>(exp->expression);
            if (funcExpr.function->getType() == Type::String) {
                auto resolvedFunction = resolveVariable(funcExpr.function->getString(), scope);                
                if (resolvedFunction->getType() == Type::Null) {
                    throw Exception("Function "s + funcExpr.function->getString() + " was null and cannot be called");
                }
                funcExpr.function = resolvedFunction;
            } else if (funcExpr.function->getType() == Type::Null) {
                if (funcExpr.subexpressions.size() >= 1) {
                    funcExpr.function = getValue(funcExpr.subexpressions.front(), scope, classs);
                    if (funcExpr.function->getType() == Type::ArrayMember) {
                        funcExpr.function = funcExpr.function->getArrayMember().getValue();
                    }
                    if (funcExpr.function->getType() == Type::Null) {
                        // todo: better more descriptive error message
                        // perhaps reconstruct the line of code from the expression?
                        throw Exception("Indirect function call");
                    }
                    funcExpr.subexpressions.erase(funcExpr.subexpressions.begin());                    
                }
            }
            auto fncRef = funcExpr.function->getFunction();
            List args;
            bool isEq = funcExpr.function == setFunctionVarLocation;
            for (auto&& sub : funcExpr.subexpressions) {
                auto val = getValue(sub, scope, classs);
                args.push_back((val->getType() == Type::ArrayMember && !isEq) ? val->getArrayMember().getValue() : val);
                isEq = false;
            }
            return make_shared<Expression>(callFunction(fncRef, scope, args, classs));
        }
        case ExpressionType::Loop: {
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
            if (returnVal && returnVal.type == ExpressionType::Return) {
                return make_shared<Expression>(returnVal.value, ExpressionType::Return);
            } else {
                return make_shared<Expression>(makeNull());
            }
        }
        case ExpressionType::ForEach: {
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
                case Type::Function: {
                    auto vec = list->getStdVector<FunctionRef>();
                    for (auto&& in : vec) {
                        if (returnVal) break;
                        *varr = Value(in);
                        returnVal = needsToReturn(subs, scope, classs);
                    }
                }
                                   break;

                case Type::UserPointer: {
                    auto vec = list->getStdVector<UserPointer>();
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
            if (returnVal && returnVal.type == ExpressionType::Return) {
                return make_shared<Expression>(returnVal.value, ExpressionType::Return);
            } else {
                return make_shared<Expression>(makeNull());
            }
        }
        case ExpressionType::IfElse: {
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
                return make_shared<Expression>(returnVal.value, returnVal.type);
            } else {
                return make_shared<Expression>(makeNull());
            }
        }
        default:
            break;
        }
        return make_shared<Expression>(get<ValueRef>(exp->expression));
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

    bool KataScriptInterpreter::closeCurrentExpression() {
        previousExpression = currentExpression;
        if (currentExpression) {
            if (currentExpression->parent) {
                currentExpression = currentExpression->parent;
            } else {
                if (currentExpression->type != ExpressionType::FunctionDef
                    && currentExpression->type != ExpressionType::IfElse
                    ) {
                    getValue(currentExpression, parseScope, nullptr);
                }
                currentExpression = nullptr;
                return true;
            }
        }
        return false;
    }
}
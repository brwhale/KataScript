#pragma once

namespace KataScript {
    bool KSExpression::needsToReturn(KSExpressionRef exp, KSValueRef& returnVal, KataScriptInterpreter* i) const {
        if (exp->type == KSExpressionType::Return) {
            returnVal = i->getValue(exp);
            return true;
        } else {
            auto result = exp->consolidated(i);
            if (result->type == KSExpressionType::Return) {
                returnVal = get<KSValueRef>(result->expression);
                return true;
            }
        }
        return false;
    }

    // evaluate an expression
    KSExpressionRef KSExpression::consolidated(KataScriptInterpreter* i) const {
        switch (type) {
        case KSExpressionType::DefineVar:
        {
            auto& def = get<KSDefineVar>(expression);
            auto& varr = i->currentScope->variables[def.name];
            if (def.defineExpression) {
                auto val = i->getValue(def.defineExpression);
                varr = make_shared<KSValue>(val->value, val->type);
            } else {
                varr = make_shared<KSValue>();
            }
            return make_shared<KSExpression>(varr, KSExpressionType::Value);
        }
        break;
        case KSExpressionType::ResolveVar:
            return make_shared<KSExpression>(i->resolveVariable(get<KSResolveVar>(expression).name), KSExpressionType::Value);
            break;
        case KSExpressionType::Return:
            return make_shared<KSExpression>(i->getValue(get<KSReturn>(expression).expression), KSExpressionType::Value);
            break;
        case KSExpressionType::FunctionCall:
        {
            KSList args;
            auto& funcExpr = get<KSFunctionExpression>(expression);
            for (auto&& sub : funcExpr.subexpressions) {
                args.push_back(get<KSValueRef>(sub->consolidated(i)->expression));
            }
            if (args.size() && args[0]->type == KSType::Function
                && funcExpr.function->type == KSType::Function
                && funcExpr.function->getFunction().get() == i->applyFunctionLocation) {
                auto funcToUse = args[0]->getFunction();
                args.erase(args.begin());
                return make_shared<KSExpression>(i->callFunction(funcToUse, args), KSExpressionType::Value);
            }
            if (funcExpr.function->type == KSType::Null) {
                if (args.size() && args[0]->type == KSType::Function) {
                    auto funcToUse = args[0]->getFunction();
                    args.erase(args.begin());
                    return make_shared<KSExpression>(i->callFunction(funcToUse, args), KSExpressionType::Value);
                } else if (args.size()) {
                    return make_shared<KSExpression>(args[0], KSExpressionType::Value);
                } else {
                    throw KSException("Unable to call non-existant function");
                }
            }
            return make_shared<KSExpression>(i->callFunction(funcExpr.function->getFunction(), args), KSExpressionType::Value);
        }
        break;
        case KSExpressionType::Loop:
        {
            i->currentScope = i->newScope("loop");
            auto& loopexp = get<KSLoop>(expression);
            if (loopexp.initExpression) {
                i->getValue(loopexp.initExpression);
            }
            KSValueRef returnVal = nullptr;
            while (returnVal == nullptr && i->getValue(loopexp.testExpression)->getBool()) {
                for (auto&& exp : loopexp.subexpressions) {
                    if (needsToReturn(exp, returnVal, i)) {
                        break;
                    }
                }
                if (returnVal == nullptr && loopexp.iterateExpression) {
                    i->getValue(loopexp.iterateExpression);
                }
            }
            i->closeCurrentScope();
            if (returnVal) {
                return make_shared<KSExpression>(returnVal, KSExpressionType::Return);
            } else {
                return make_shared<KSExpression>(make_shared<KSValue>(), KSExpressionType::Value);
            }
        }
        break;
        case KSExpressionType::ForEach:
        {
            i->currentScope = i->newScope("loop");
            auto& varr = i->resolveVariable(get<KSForeach>(expression).iterateName);
            auto list = get<KSValueRef>(get<KSForeach>(expression).listExpression->consolidated(i)->expression);
            KSValueRef returnVal = nullptr;
            if (list->type == KSType::List) {
                for (auto&& in : list->getList()) {
                    *varr = *in;
                    for (auto&& exp : get<KSForeach>(expression).subexpressions) {
                        if (needsToReturn(exp, returnVal, i)) {
                            break;
                        }
                    }
                }
            } else if (list->type == KSType::Array) {
                auto& arr = list->getArray();
                switch (arr.type) {
                case KSType::Int:
                {
                    auto vec = list->getStdVector<KSInt>();
                    for (auto&& in : vec) {
                        *varr = KSValue(in);
                        for (auto&& exp : get<KSForeach>(expression).subexpressions) {
                            if (needsToReturn(exp, returnVal, i)) {
                                break;
                            }
                        }
                    }
                }
                break;
                case KSType::Float:
                {
                    auto vec = list->getStdVector<KSFloat>();
                    for (auto&& in : vec) {
                        *varr = KSValue(in);
                        for (auto&& exp : get<KSForeach>(expression).subexpressions) {
                            if (needsToReturn(exp, returnVal, i)) {
                                break;
                            }
                        }
                    }
                }
                break;
                case KSType::Vec3:
                {
                    auto vec = list->getStdVector<vec3>();
                    for (auto&& in : vec) {
                        *varr = KSValue(in);
                        for (auto&& exp : get<KSForeach>(expression).subexpressions) {
                            if (needsToReturn(exp, returnVal, i)) {
                                break;
                            }
                        }
                    }
                }
                break;
                case KSType::String:
                {
                    auto vec = list->getStdVector<string>();
                    for (auto&& in : vec) {
                        *varr = KSValue(in);
                        for (auto&& exp : get<KSForeach>(expression).subexpressions) {
                            if (needsToReturn(exp, returnVal, i)) {
                                break;
                            }
                        }
                    }
                }
                break;
                default:
                    break;
                }
            }
            i->closeCurrentScope();
            if (returnVal) {
                return make_shared<KSExpression>(returnVal, KSExpressionType::Return);
            } else {
                return make_shared<KSExpression>(make_shared<KSValue>(), KSExpressionType::Value);
            }
        }
        break;
        case KSExpressionType::IfElse:
        {
            KSValueRef returnVal = nullptr;
            for (auto& express : get<KSIfElse>(expression)) {
                if (!express.testExpression || i->getValue(express.testExpression)->getBool()) {
                    i->currentScope = i->newScope("ifelse");
                    for (auto exp : express.subexpressions) {
                        if (needsToReturn(exp, returnVal, i)) {
                            break;
                        }
                    }
                    i->closeCurrentScope();
                    break;
                }
            }
            if (returnVal) {
                return make_shared<KSExpression>(returnVal, KSExpressionType::Return);
            } else {
                return make_shared<KSExpression>(make_shared<KSValue>(), KSExpressionType::Value);
            }
        }
        break;
        default:
            break;
        }
        return make_shared<KSExpression>(get<KSValueRef>(expression), KSExpressionType::Value);
    }

    // evaluate an expression from tokens
    KSValueRef KataScriptInterpreter::getValue(const vector<string>& strings) {
        return get<KSValueRef>(getExpression(strings)->consolidated(this)->expression);
    }

    // evaluate an expression from expressionRef
    KSValueRef KataScriptInterpreter::getValue(KSExpressionRef exp) {
        // copy the expression so that we don't lose it when we consolidate
        return get<KSValueRef>(exp->consolidated(this)->expression);
    }

    // since the 'else' block in  an if/elfe is technically in a different scope
    // ifelse espressions are not closed immediately and instead left dangling
    // until the next expression is anything other than an 'else' or the else is unconditional
    void KataScriptInterpreter::closeDanglingIfExpression() {
        if (currentExpression && currentExpression->type == KSExpressionType::IfElse) {
            if (currentExpression->parent) {
                currentExpression = currentExpression->parent;
            } else {
                getValue(currentExpression);
                currentExpression = nullptr;
            }
        }
    }

    bool KataScriptInterpreter::closeCurrentExpression() {
        if (currentExpression) {
            if (currentExpression->type != KSExpressionType::IfElse) {
                if (currentExpression->parent) {
                    currentExpression = currentExpression->parent;
                } else {
                    if (currentExpression->type != KSExpressionType::FunctionDef) {
                        getValue(currentExpression);
                    }
                    currentExpression = nullptr;
                }
                return true;
            }
        }
        return false;
    }
}
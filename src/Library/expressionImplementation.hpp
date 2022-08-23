#pragma once

namespace KataScript {
    bool Expression::needsToReturn(ExpressionRef exp, ValueRef& returnVal, KataScriptInterpreter* i, ScopeRef scope) const {
        if (exp->type == ExpressionType::Return) {
            returnVal = i->getValue(exp, scope);
            return true;
        } else {
            auto result = exp->consolidated(i, scope);
            if (result->type == ExpressionType::Return) {
                returnVal = get<ValueRef>(result->expression);
                return true;
            }
        }
        return false;
    }

    // evaluate an expression
    ExpressionRef Expression::consolidated(KataScriptInterpreter* i, ScopeRef scope) const {
        switch (type) {
        case ExpressionType::DefineVar:
        {
            auto& def = get<DefineVar>(expression);
            auto& varr = scope->variables[def.name];
            if (def.defineExpression) {
                auto val = i->getValue(def.defineExpression, scope);
                varr = make_shared<Value>(val->value, val->type);
            } else {
                varr = make_shared<Value>();
            }
            return make_shared<Expression>(varr, ExpressionType::Value);
        }
        break;
        case ExpressionType::ResolveVar: {
            auto name = get<ResolveVar>(expression).name;
            auto var = i->resolveVariable(name, scope);
            return make_shared<Expression>(var, ExpressionType::Value);
            break;
        }
        case ExpressionType::ResolveFuncVar: {
            auto name = get<ResolveFuncVar>(expression).name;
            auto var = i->resolveVariable(name, scope);
            if (var->type == Type::Null) throw Exception("Cannot call non existant function "s + name + " at line: " + std::to_string(i->currentLine));
            return make_shared<Expression>(var, ExpressionType::Value);
        }
            break;
        case ExpressionType::Return:
            return make_shared<Expression>(i->getValue(get<Return>(expression).expression, scope), ExpressionType::Value);
            break;
        case ExpressionType::FunctionCall:
        {
            List args;
            auto& funcExpr = get<FunctionExpression>(expression);
            for (auto&& sub : funcExpr.subexpressions) {
                args.push_back(get<ValueRef>(sub->consolidated(i, scope)->expression));
            }
            if (args.size() && args[0]->type == Type::Function
                && funcExpr.function->type == Type::Function
                && funcExpr.function->getFunction() == i->applyFunctionLocation) {
                auto funcToUse = args[0]->getFunction();
                args.erase(args.begin());
                return make_shared<Expression>(i->callFunction(funcToUse, scope, args), ExpressionType::Value);
            }
            if (funcExpr.function->type == Type::Null) {
                if (args.size() && args[0]->type == Type::Function) {
                    auto funcToUse = args[0]->getFunction();
                    args.erase(args.begin());
                    return make_shared<Expression>(i->callFunction(funcToUse, scope, args), ExpressionType::Value);
                } else if (args.size()) {
                    return make_shared<Expression>(args[0], ExpressionType::Value);
                } else {
                    throw Exception("Unable to call non-existant function");
                }
            }
            return make_shared<Expression>(i->callFunction(funcExpr.function->getFunction(), scope, args), ExpressionType::Value);
        }
        break;
        case ExpressionType::Loop:
        {
            scope = i->newScope("loop", scope);
            auto& loopexp = get<Loop>(expression);
            if (loopexp.initExpression) {
                i->getValue(loopexp.initExpression, scope);
            }
            ValueRef returnVal = nullptr;
            while (returnVal == nullptr && i->getValue(loopexp.testExpression, scope)->getBool()) {
                for (auto&& exp : loopexp.subexpressions) {
                    if (needsToReturn(exp, returnVal, i, scope)) {
                        break;
                    }
                }
                if (returnVal == nullptr && loopexp.iterateExpression) {
                    i->getValue(loopexp.iterateExpression, scope);
                }
            }
            i->closeScope(scope);
            if (returnVal) {
                return make_shared<Expression>(returnVal, ExpressionType::Return);
            } else {
                return make_shared<Expression>(make_shared<Value>(), ExpressionType::Value);
            }
        }
        break;
        case ExpressionType::ForEach:
        {
            scope = i->newScope("loop", scope);
            auto varr = i->resolveVariable(get<Foreach>(expression).iterateName, scope);
            auto list = get<ValueRef>(get<Foreach>(expression).listExpression->consolidated(i, scope)->expression);
            ValueRef returnVal = nullptr;
            if (list->type == Type::List) {
                for (auto&& in : list->getList()) {
                    *varr = *in;
                    for (auto&& exp : get<Foreach>(expression).subexpressions) {
                        if (needsToReturn(exp, returnVal, i, scope)) {
                            break;
                        }
                    }
                }
            } else if (list->type == Type::Array) {
                auto& arr = list->getArray();
                switch (arr.type) {
                case Type::Int:
                {
                    auto vec = list->getStdVector<Int>();
                    for (auto&& in : vec) {
                        *varr = Value(in);
                        for (auto&& exp : get<Foreach>(expression).subexpressions) {
                            if (needsToReturn(exp, returnVal, i, scope)) {
                                break;
                            }
                        }
                    }
                }
                break;
                case Type::Float:
                {
                    auto vec = list->getStdVector<Float>();
                    for (auto&& in : vec) {
                        *varr = Value(in);
                        for (auto&& exp : get<Foreach>(expression).subexpressions) {
                            if (needsToReturn(exp, returnVal, i, scope)) {
                                break;
                            }
                        }
                    }
                }
                break;
                case Type::Vec3:
                {
                    auto vec = list->getStdVector<vec3>();
                    for (auto&& in : vec) {
                        *varr = Value(in);
                        for (auto&& exp : get<Foreach>(expression).subexpressions) {
                            if (needsToReturn(exp, returnVal, i, scope)) {
                                break;
                            }
                        }
                    }
                }
                break;
                case Type::String:
                {
                    auto vec = list->getStdVector<string>();
                    for (auto&& in : vec) {
                        *varr = Value(in);
                        for (auto&& exp : get<Foreach>(expression).subexpressions) {
                            if (needsToReturn(exp, returnVal, i, scope)) {
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
            i->closeScope(scope);
            if (returnVal) {
                return make_shared<Expression>(returnVal, ExpressionType::Return);
            } else {
                return make_shared<Expression>(make_shared<Value>(), ExpressionType::Value);
            }
        }
        break;
        case ExpressionType::IfElse:
        {
            ValueRef returnVal = nullptr;
            for (auto& express : get<IfElse>(expression)) {
                if (!express.testExpression || i->getValue(express.testExpression, scope)->getBool()) {
                    scope = i->newScope("ifelse", scope);
                    for (auto exp : express.subexpressions) {
                        if (needsToReturn(exp, returnVal, i, scope)) {
                            break;
                        }
                    }
                    i->closeScope(scope);
                    break;
                }
            }
            if (returnVal) {
                return make_shared<Expression>(returnVal, ExpressionType::Return);
            } else {
                return make_shared<Expression>(make_shared<Value>(), ExpressionType::Value);
            }
        }
        break;
        default:
            break;
        }
        return make_shared<Expression>(get<ValueRef>(expression), ExpressionType::Value);
    }

    // evaluate an expression from tokens
    ValueRef KataScriptInterpreter::getValue(const vector<string_view>& strings, ScopeRef scope) {
        return get<ValueRef>(getExpression(strings, scope)->consolidated(this, scope)->expression);
    }

    // evaluate an expression from expressionRef
    ValueRef KataScriptInterpreter::getValue(ExpressionRef exp, ScopeRef scope) {
        // copy the expression so that we don't lose it when we consolidate
        return get<ValueRef>(exp->consolidated(this, scope)->expression);
    }

    // since the 'else' block in  an if/elfe is technically in a different scope
    // ifelse espressions are not closed immediately and instead left dangling
    // until the next expression is anything other than an 'else' or the else is unconditional
    void KataScriptInterpreter::closeDanglingIfExpression() {
        if (currentExpression && currentExpression->type == ExpressionType::IfElse) {
            if (currentExpression->parent) {
                currentExpression = currentExpression->parent;
            } else {
                getValue(currentExpression, parseScope);
                currentExpression = nullptr;
            }
        }
    }

    bool KataScriptInterpreter::closeCurrentExpression() {
        if (currentExpression) {
            if (currentExpression->type != ExpressionType::IfElse) {
                if (currentExpression->parent) {
                    currentExpression = currentExpression->parent;
                } else {
                    if (currentExpression->type != ExpressionType::FunctionDef) {
                        getValue(currentExpression, parseScope);
                    }
                    currentExpression = nullptr;
                }
                return true;
            }
        }
        return false;
    }
}
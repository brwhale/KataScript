#pragma once

namespace KataScript {
    bool KataScriptInterpreter::needsToReturn(ExpressionRef exp, ValueRef& returnVal, ScopeRef scope, ClassRef classs) {
        if (exp->type == ExpressionType::Return) {
            returnVal = getValue(exp, scope, classs);
            return true;
        } else {
            auto result = consolidated(exp, scope, classs);
            if (result->type == ExpressionType::Return) {
                returnVal = get<ValueRef>(result->expression);
                return true;
            }
        }
        return false;
    }

    // walk the tree depth first and replace any function expressions 
    // with a value expression of their results
    ExpressionRef KataScriptInterpreter::consolidated(ExpressionRef exp, ScopeRef scope, ClassRef classs) {
        switch (exp->type) {
        case ExpressionType::DefineVar:
        {
            auto& def = get<DefineVar>(exp->expression);
            auto& varr = scope->variables[def.name];
            if (def.defineExpression) {
                auto val = getValue(def.defineExpression, scope, classs);
                varr = make_shared<Value>(val->value, val->type);
            } else {
                varr = make_shared<Value>();
            }
            return make_shared<Expression>(varr, ExpressionType::Value);
        }
        break;
        case ExpressionType::ResolveVar: {
            auto name = get<ResolveVar>(exp->expression).name;
            auto var = resolveVariable(name, scope);
            return make_shared<Expression>(var, ExpressionType::Value);
            break;
        }
        case ExpressionType::ResolveFuncVar: {
            auto name = get<ResolveFuncVar>(exp->expression).name;
            auto var = resolveVariable(name, scope);
            if (var->type == Type::Null) throw Exception("Cannot call non existant function "s + name + " at line: " + std::to_string(currentLine));
            return make_shared<Expression>(var, ExpressionType::Value);
        }
            break;
        case ExpressionType::ResolveClassVar: {
            auto name = get<ResolveClassVar>(exp->expression).name;
            auto var = resolveVariable(name, classs, scope);
            return make_shared<Expression>(var, ExpressionType::Value);
        }
            break;
        case ExpressionType::Return:
            return make_shared<Expression>(getValue(get<Return>(exp->expression).expression, scope, classs), ExpressionType::Value);
            break;
        case ExpressionType::FunctionCall:
        {
            List args;
            auto& funcExpr = get<FunctionExpression>(exp->expression);
            
            for (auto&& sub : funcExpr.subexpressions) {
                args.push_back(getValue(sub, scope, classs));
            }
            if (args.size() && args[0]->type == Type::Function
                && funcExpr.function->type == Type::Function
                && funcExpr.function->getFunction() == applyFunctionLocation) {
                auto funcToUse = args[0]->getFunction();
                args.erase(args.begin());
                return make_shared<Expression>(callFunction(funcToUse, scope, args, classs), ExpressionType::Value);
            }
            if (funcExpr.function->type == Type::Null) {
                if (args.size() && args[0]->type == Type::Function) {
                    auto funcToUse = args[0]->getFunction();
                    args.erase(args.begin());
                    return make_shared<Expression>(callFunction(funcToUse, scope, args, classs), ExpressionType::Value);
                } else if (args.size()) {
                    return make_shared<Expression>(args[0], ExpressionType::Value);
                } else {
                    throw Exception("Unable to call non-existant function");
                }
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
            ValueRef returnVal = nullptr;
            while (returnVal == nullptr && getValue(loopexp.testExpression, scope, classs)->getBool()) {
                for (auto&& sub : loopexp.subexpressions) {
                    if (needsToReturn(sub, returnVal, scope, classs)) {
                        break;
                    }
                }
                if (returnVal == nullptr && loopexp.iterateExpression) {
                    getValue(loopexp.iterateExpression, scope, classs);
                }
            }
            closeScope(scope);
            if (returnVal) {
                return make_shared<Expression>(returnVal, ExpressionType::Return);
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
            ValueRef returnVal = nullptr;
            if (list->type == Type::List) {
                for (auto&& in : list->getList()) {
                    *varr = *in;
                    for (auto&& sub : get<Foreach>(exp->expression).subexpressions) {
                        if (needsToReturn(sub, returnVal, scope, classs)) {
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
                        for (auto&& sub : get<Foreach>(exp->expression).subexpressions) {
                            if (needsToReturn(sub, returnVal, scope, classs)) {
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
                        for (auto&& sub : get<Foreach>(exp->expression).subexpressions) {
                            if (needsToReturn(sub, returnVal, scope, classs)) {
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
                        for (auto&& sub : get<Foreach>(exp->expression).subexpressions) {
                            if (needsToReturn(sub, returnVal, scope, classs)) {
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
                        for (auto&& sub : get<Foreach>(exp->expression).subexpressions) {
                            if (needsToReturn(sub, returnVal, scope, classs)) {
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
            closeScope(scope);
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
            for (auto& express : get<IfElse>(exp->expression)) {
                if (!express.testExpression || getValue(express.testExpression, scope, classs)->getBool()) {
                    scope = newScope("ifelse", scope);
                    for (auto sub : express.subexpressions) {
                        if (needsToReturn(sub, returnVal, scope, classs)) {
                            break;
                        }
                    }
                    closeScope(scope);
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
        return make_shared<Expression>(get<ValueRef>(exp->expression), ExpressionType::Value);
    }

    // evaluate an expression from tokens
    ValueRef KataScriptInterpreter::getValue(const vector<string_view>& strings, ScopeRef scope, ClassRef classs) {
        return getValue(getExpression(strings, scope, classs), scope, classs);
    }

    // evaluate an expression from expressionRef
    ValueRef KataScriptInterpreter::getValue(ExpressionRef exp, ScopeRef scope, ClassRef classs) {
        // copy the expression so that we don't lose it when we consolidate
        return get<ValueRef>(consolidated(exp, scope, classs)->expression);
    }

    // since the 'else' block in  an if/elfe is technically in a different scope
    // ifelse espressions are not closed immediately and instead left dangling
    // until the next expression is anything other than an 'else' or the else is unconditional
    void KataScriptInterpreter::closeDanglingIfExpression() {
        if (currentExpression && currentExpression->type == ExpressionType::IfElse) {
            if (currentExpression->parent) {
                currentExpression = currentExpression->parent;
            } else {
                getValue(currentExpression, parseScope, nullptr);
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
#pragma once
#include "KataScript.hpp"
namespace KataScript {
    KSScopeRef KataScriptInterpreter::newModule(const string& name, const unordered_map<string, KSLambda>& functions) {
        auto oldScope = currentScope;
        modules.push_back(make_shared<KSScope>(name, nullptr));
        currentScope = modules.back();

        for (auto& funcPair : functions) {
            newFunction(funcPair.first, funcPair.second);
        }

        currentScope = oldScope;
        return modules.back();
    }

    void KataScriptInterpreter::createStandardLibrary() {
		// register compiled functions and standard library:
        newModule("StandardLib"s, {

        // math operators
            {"=", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("=");
                }
                if (args.size() == 1) {
                    return args[0];
                }
                *args[0] = *args[1];
                return args[0];
                }},

            {"+", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("+");
                }
                if (args.size() == 1) {
                    return args[0];
                }
                return make_shared<KSValue>(*args[0] + *args[1]);
                }},

            {"-", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("-");
                }
                if (args.size() == 1) {
                    auto zero = KSValue(KSInt(0));
                    upconvert(*args[0], zero);
                    return make_shared<KSValue>(zero - *args[0]);
                }
                return make_shared<KSValue>(*args[0] - *args[1]);
                }},

            {"*", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("*");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>();
                }
                return make_shared<KSValue>(*args[0] * *args[1]);
                }},

            {"/", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("/");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>();
                }
                return make_shared<KSValue>(*args[0] / *args[1]);
                }},

            {"%", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("%");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>();
                }
                return make_shared<KSValue>(*args[0] % *args[1]);
                }},

            {"==", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("==");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(*args[0] == *args[1]));
                }},

            {"!=", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("!=");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(*args[0] != *args[1]));
                }},

            {"||", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("||");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(1));
                }
                return make_shared<KSValue>((KSInt)(*args[0] || *args[1]));
                }},

            {"&&", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("&&");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(*args[0] && *args[1]));
                }},

            {"++", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                auto t = KSValue(KSInt(1));
                *args[0] = *args[0] + t;
                return args[0];
                }},

            {"--", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                auto t = KSValue(KSInt(1));
                *args[0] = *args[0] - t;
                return args[0];
                }},

            {"+=", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                if (args.size() == 1) {
                    return args[0];
                }
                *args[0] += *args[1];
                return args[0];
                }},

            {"-=", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                if (args.size() == 1) {
                    return args[0];
                }
                *args[0] -= *args[1];
                return args[0];
                }},

            {"*=", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                if (args.size() == 1) {
                    return args[0];
                }
                *args[0] *= *args[1];
                return args[0];
                }},

            {"/=", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                if (args.size() == 1) {
                    return args[0];
                }
                *args[0] /= *args[1];
                return args[0];
                }},

            {">", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable(">");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(*args[0] > *args[1]));
                }},

            {"<", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("<");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(*args[0] < *args[1]));
                }},

            {">=", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable(">=");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(*args[0] >= *args[1]));
                }},

            {"<=", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("<=");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(*args[0] <= *args[1]));
                }},

            {"!", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(!args[0]->getBool()));
                }},

        // aliases
            {"identity", [](KSList args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                return args[0];
                }},

            {"copy", [](KSList args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                if (args[0]->type == KSType::Class) {
                    return make_shared<KSValue>(make_shared<KSClass>(*args[0]->getClass()), args[0]->type);
                }
                return make_shared<KSValue>(args[0]->value, args[0]->type);
                }},

            {"listindex", [](KSList args) {
                if (args.size() > 0) {
                    if (args.size() == 1) {
                        return args[0];
                    }

                    auto var = args[0];
                    if (args[1]->type != KSType::Int) {
                        var->upconvert(KSType::Dictionary);
                    }

                    switch (var->type) {
                    case KSType::Array:
                    {
                        auto ival = args[1]->getInt();
                        auto& arr = var->getArray();
                        if (ival < 0 || ival >= (KSInt)arr.size()) {
                            throw KSException("Out of bounds array access index "s + std::to_string(ival) + ", array length " + std::to_string(arr.size()));
                        } else {
                            switch (arr.type) {
                            case KSType::Int:
                                return make_shared<KSValue>(get<vector<KSInt>>(arr.value)[ival]);
                                break;
                            case KSType::Float:
                                return make_shared<KSValue>(get<vector<KSFloat>>(arr.value)[ival]);
                                break;
                            case KSType::Vec3:
                                return make_shared<KSValue>(get<vector<vec3>>(arr.value)[ival]);
                                break;
                            case KSType::String:
                                return make_shared<KSValue>(get<vector<string>>(arr.value)[ival]);
                                break;
                            default:
                                throw KSException("Attempting to access array of illegal type");
                                break;
                            }
                        }
                    }
                    break;
                    default:
                        var = make_shared<KSValue>(var->value, var->type);
                        var->upconvert(KSType::List);
                        [[fallthrough]];
                    case KSType::List:
                    {
                        auto ival = args[1]->getInt();

                        auto& list = var->getList();
                        if (ival < 0 || ival >= (KSInt)list.size()) {
                            throw KSException("Out of bounds list access index "s + std::to_string(ival) + ", list length " + std::to_string(list.size()));
                        } else {
                            return list[ival];
                        }
                    }
                    break;
                    case KSType::Class:
                    {
                        auto strval = args[1]->getString();
                        auto& struc = var->getClass();
                        auto iter = struc->variables.find(strval);
                        if (iter == struc->variables.end()) {
                            throw KSException("Class `"s + struc->name + "` does not contain member `" + strval + "`");
                        } else {
                            return iter->second;
                        }
                    }
                    break;
                    case KSType::Dictionary:
                    {
                        auto& dict = var->getDictionary();
                        auto& ref = dict[args[1]->getHash()];
                        if (ref == nullptr) {
                            ref = make_shared<KSValue>();
                        }
                        return ref;
                    }
                    break;
                    }
                }
                return make_shared<KSValue>();
                }},

            {"applyfunction", [this](KSList args) {
                if (args.size() < 2 || args[1]->type != KSType::Class) {
                    auto func = args[0]->type == KSType::Function ? args[0] : resolveVariable(args[0]->getString());
                    auto list = KSList();
                    for (size_t i = 1; i < args.size(); ++i) {
                        list.push_back(args[i]);
                    }
                    return callFunction(func->getFunction(), list);
                }
                KSFunctionRef func = nullptr;
                auto list = KSList();
                if (args[0]->type == KSType::Function) {
                    func = args[0]->getFunction();
                    list.push_back(args[1]);
                } else {
                    auto& strval = args[0]->getString();
                    auto& struc = args[1]->getClass();
                    if (args.size() > 2) {
                        if (struc->functionScope) {
                            auto iter = struc->functionScope->functions.find(strval);
                            if (iter != struc->functionScope->functions.end()) {
                                func = iter->second;
                            }
                        }
                        if (func == nullptr) {
                            throw KSException("Class `"s + struc->name + "` does not contain member function `" + strval + "`");
                        }
                    } else {
                        auto iter = struc->variables.find(strval);
                        if (iter == struc->variables.end()) {
                            // look at global class def if we don't have it locally
                            auto scopeIter = globalScope->scopes.find(struc->name);
                            if (scopeIter != globalScope->scopes.end()) {
                                iter = scopeIter->second->variables.find(strval);
                                if (iter == scopeIter->second->variables.end()) {
                                    throw KSException("Class `"s + struc->name + "` does not contain member function `" + strval + "`");
                                }
                            } else {
                                throw KSException("Class `"s + struc->name + "` does not contain member function `" + strval + "`");
                            }
    
                        }
                        func = iter->second->getFunction();
                    }
                }

                for (size_t i = 2; i < args.size(); ++i) {
                    list.push_back(args[i]);
                }
                auto tempClass = currentClass;
                currentClass = args[1]->getClass();
                auto res = callFunction(func, list);
                currentClass = tempClass;
                return res;
                }},

        // casting
            {"bool", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSInt(0));
                }
                auto val = *args[0];
                val.hardconvert(KSType::Int);
                val.value = (KSInt)args[0]->getBool();
                return make_shared<KSValue>(val);
                }},

            {"int", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSInt(0));
                }
                auto val = *args[0];
                val.hardconvert(KSType::Int);
                return make_shared<KSValue>(val);
                }},

            {"float", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSFloat(0.0));
                }
                auto val = *args[0];
                val.hardconvert(KSType::Float);
                return make_shared<KSValue>(val);
                }},

            {"vec3", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(vec3());
                }
                if (args.size() < 3) {
                    auto val = *args[0];
                    val.hardconvert(KSType::Float);
                    return make_shared<KSValue>(vec3((float)val.getFloat()));
                }
                auto x = *args[0];
                x.hardconvert(KSType::Float);
                auto y = *args[1];
                y.hardconvert(KSType::Float);
                auto z = *args[2];
                z.hardconvert(KSType::Float);
                return make_shared<KSValue>(vec3((float)x.getFloat(), (float)y.getFloat(), (float)z.getFloat()));
                }},

            {"string", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(""s);
                }
                auto val = *args[0];
                val.hardconvert(KSType::String);
                return make_shared<KSValue>(val);
                }},

            {"array", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSArray());
                }
                auto list = make_shared<KSValue>(args);
                list->hardconvert(KSType::Array);
                return list;
                }},

            {"list", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSList());
                }
                return make_shared<KSValue>(args);
                }},

            {"dictionary", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSDictionary());
                }
                if (args.size() == 1) {
                    auto val = *args[0];
                    val.hardconvert(KSType::Dictionary);
                    return make_shared<KSValue>(val);
                }
                auto dict = make_shared<KSValue>(KSDictionary());
                for (auto&& arg : args) {
                    auto val = *arg;
                    val.hardconvert(KSType::Dictionary);
                    dict->getDictionary().merge(val.getDictionary());
                }
                return dict;
                }},

            {"toarray", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSArray());
                }
                auto val = *args[0];
                val.hardconvert(KSType::Array);
                return make_shared<KSValue>(val);  
                }},

            {"tolist", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSList());
                }
                auto val = *args[0];
                val.hardconvert(KSType::List);
                return make_shared<KSValue>(val);
                }},

        // overal stdlib
            {"typeof", [](KSList args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                return make_shared<KSValue>(getTypeName(args[0]->type));
                }},

            {"sqrt", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                auto val = *args[0];
                val.hardconvert(KSType::Float);
                return make_shared<KSValue>(sqrt(val.getFloat()));
                }},

            {"sin", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                auto val = *args[0];
                val.hardconvert(KSType::Float);
                return make_shared<KSValue>(sin(val.getFloat()));
                }},

            {"cos", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                auto val = *args[0];
                val.hardconvert(KSType::Float);
                return make_shared<KSValue>(cos(val.getFloat()));
                }},

            {"tan", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                auto val = *args[0];
                val.hardconvert(KSType::Float);
                return make_shared<KSValue>(tan(val.getFloat()));
                }},

            {"pow", [](const KSList& args) {
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSFloat(0));
                }
                auto val = *args[0];
                val.hardconvert(KSType::Float);
                auto val2 = *args[1];
                val2.hardconvert(KSType::Float);
                return make_shared<KSValue>(pow(val.getFloat(), val2.getFloat()));
                }},

            {"abs", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                switch (args[0]->type) {
                case KSType::Int:
                    return make_shared<KSValue>(KSInt(abs(args[0]->getInt())));
                    break;
                case KSType::Float:
                    return make_shared<KSValue>(KSFloat(fabs(args[0]->getFloat())));
                    break;
                default:
                    return make_shared<KSValue>();
                    break;
                }                
                }},

            {"min", [](const KSList& args) {
                if (args.size() < 2) {
                    return make_shared<KSValue>();
                }
                auto val = *args[0];
                auto val2 = *args[1];
                upconvertThrowOnNonNumberToNumberCompare(val, val2);
                if (val > val2) {
                    return make_shared<KSValue>(val2.value, val2.type);
                }
                return make_shared<KSValue>(val.value, val.type);
                }},

            {"max", [](const KSList& args) {
                if (args.size() < 2) {
                    return make_shared<KSValue>();
                }
                auto val = *args[0];
                auto val2 = *args[1];
                upconvertThrowOnNonNumberToNumberCompare(val, val2);
                if (val < val2) {
                    return make_shared<KSValue>(val2.value, val2.type);
                }
                return make_shared<KSValue>(val.value, val.type);
                }},

            {"swap", [](const KSList& args) {
                if (args.size() < 2) {
                    return make_shared<KSValue>();
                }
                auto v = *args[0];
                *args[0] = *args[1];
                *args[1] = v;

                return make_shared<KSValue>();
                }},

            {"print", [](const KSList& args) {
                for (auto&& arg : args) {
                    printf("%s", arg->getPrintString().c_str());
                }
                printf("\n");
                return make_shared<KSValue>();
                }},

            {"getline", [](const KSList& args) {
                string s;
                // blocking calls are fine
                getline(std::cin, s);
                if (args.size() > 0) {
                    args[0]->value = s;
                    args[0]->type = KSType::String;
                }
                return make_shared<KSValue>(s);
                }},

            {"map", [this](const KSList& args) {
                if (args.size() < 2 || args[1]->type != KSType::Function) {
                    return make_shared<KSValue>();
                }
                auto ret = make_shared<KSValue>(KSList());
                auto& retList = ret->getList();
                auto func = args[1]->getFunction();

                if (args[0]->type == KSType::Array) {
                    auto val = *args[0];
                    val.upconvert(KSType::List);
                    for (auto&& v : val.getList()) {
                        retList.push_back(callFunction(func, { v }));
                    }
                    return ret;
                }

                for (auto&& v : args[0]->getList()) {
                    retList.push_back(callFunction(func, { v }));
                }
                return ret;

                }},

            {"fold", [this](const KSList& args) {
                if (args.size() < 3 || args[1]->type != KSType::Function) {
                    return make_shared<KSValue>();
                }

                auto func = args[1]->getFunction();
                auto iter = args[2];

                if (args[0]->type == KSType::Array) {
                    auto val = *args[0];
                    val.upconvert(KSType::List);
                    for (auto&& v : val.getList()) {
                        iter = callFunction(func, { iter, v });
                    }
                    return iter;
                }

                for (auto&& v : args[0]->getList()) {
                    iter = callFunction(func, { iter, v });
                }
                return iter;
                }},

            {"clock", [](const KSList&) {
                return make_shared<KSValue>(KSInt(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
                }},

            {"getduration", [](const KSList& args) {
                if (args.size() == 2 && args[0]->type == KSType::Int && args[1]->type == KSType::Int) {
                    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::time_point(std::chrono::nanoseconds(args[1]->getInt())) -
                        std::chrono::high_resolution_clock::time_point(std::chrono::nanoseconds(args[0]->getInt()));
                    return make_shared<KSValue>(KSFloat(duration.count()));
                }
                return make_shared<KSValue>();
                }},

            {"timesince", [](const KSList& args) {
                if (args.size() == 1 && args[0]->type == KSType::Int) {
                    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - 
                        std::chrono::high_resolution_clock::time_point(std::chrono::nanoseconds(args[0]->getInt()));
                    return make_shared<KSValue>(KSFloat(duration.count()));
                }
                return make_shared<KSValue>();
                }},

        // collection functions
            {"length", [](const KSList& args) {
                if (args.size() == 0 || (int)args[0]->type < (int)KSType::String) {
                    return make_shared<KSValue>(KSInt(0));
                }
                if (args[0]->type == KSType::String) {
                    return make_shared<KSValue>((KSInt)args[0]->getString().size());
                }
                if (args[0]->type == KSType::Array) {
                    return make_shared<KSValue>((KSInt)args[0]->getArray().size());
                }
                return make_shared<KSValue>((KSInt)args[0]->getList().size());
                }},

            {"find", [](const KSList& args) {
                if (args.size() < 2 || (int)args[0]->type < (int)KSType::Array) {
                    return make_shared<KSValue>();
                }
                if (args[0]->type == KSType::Array) {
                    if (args[1]->type == args[0]->getArray().type) {
                        switch (args[0]->getArray().type) {
                        case KSType::Int:
                        {
                            auto& arry = args[0]->getStdVector<KSInt>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getInt());
                            if (iter == arry.end()) {
                                return make_shared<KSValue>();
                            }
                            return make_shared<KSValue>((KSInt)(iter - arry.begin()));
                        }
                        break;
                        case KSType::Float:
                        {
                            auto& arry = args[0]->getStdVector<KSFloat>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getFloat());
                            if (iter == arry.end()) {
                                return make_shared<KSValue>();
                            }
                            return make_shared<KSValue>((KSInt)(iter - arry.begin()));
                        }
                        break;
                        case KSType::Vec3:
                        {
                            auto& arry = args[0]->getStdVector<vec3>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getVec3());
                            if (iter == arry.end()) {
                                return make_shared<KSValue>();
                            }
                            return make_shared<KSValue>((KSInt)(iter - arry.begin()));
                        }
                        break;
                        case KSType::String:
                        {
                            auto& arry = args[0]->getStdVector<string>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getString());
                            if (iter == arry.end()) {
                                return make_shared<KSValue>();
                            }
                            return make_shared<KSValue>((KSInt)(iter - arry.begin()));
                        }
                        break;
                        case KSType::Function:
                        {
                            auto& arry = args[0]->getStdVector<KSFunctionRef>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getFunction());
                            if (iter == arry.end()) {
                                return make_shared<KSValue>();
                            }
                            return make_shared<KSValue>((KSInt)(iter - arry.begin()));
                        }
                        break;
                        default:
                            break;
                        }
                    }
                    return make_shared<KSValue>();
                }
                auto& list = args[0]->getList();
                for (size_t i = 0; i < list.size(); ++i) {
                    if (*list[i] == *args[1]) {
                        return make_shared<KSValue>((KSInt)i);
                    }
                }
                return make_shared<KSValue>();
                }},

            {"erase", [](const KSList& args) {
                if (args.size() < 2 || (int)args[0]->type < (int)KSType::Array || args[1]->type != KSType::Int) {
                    return make_shared<KSValue>();
                }
                
                if (args[0]->type == KSType::Array) {
                    switch (args[0]->getArray().type) {
                    case KSType::Int:
                        args[0]->getStdVector<KSInt>().erase(args[0]->getStdVector<KSInt>().begin() + args[1]->getInt());
                        break;
                    case KSType::Float:
                        args[0]->getStdVector<KSFloat>().erase(args[0]->getStdVector<KSFloat>().begin() + args[1]->getInt());
                        break;
                    case KSType::Vec3:
                        args[0]->getStdVector<vec3>().erase(args[0]->getStdVector<vec3>().begin() + args[1]->getInt());
                        break;
                    case KSType::String:
                        args[0]->getStdVector<string>().erase(args[0]->getStdVector<string>().begin() + args[1]->getInt());
                        break;
                    case KSType::Function:
                        args[0]->getStdVector<KSFunctionRef>().erase(args[0]->getStdVector<KSFunctionRef>().begin() + args[1]->getInt());
                        break;
                    default:
                        break;
                    }
                } else {
                    args[0]->getList().erase(args[0]->getList().begin() + args[1]->getInt());
                }
                return make_shared<KSValue>();
                }},

            {"pushback", [](const KSList& args) {
                if (args.size() < 2 || (int)args[0]->type < (int)KSType::Array) {
                    return make_shared<KSValue>();
                }

                if (args[0]->type == KSType::Array) {
                    if (args[0]->getArray().type == args[1]->type) {
                        switch (args[0]->getArray().type) {
                        case KSType::Int:
                            args[0]->getStdVector<KSInt>().push_back(args[1]->getInt());
                            break;
                        case KSType::Float:
                            args[0]->getStdVector<KSFloat>().push_back(args[1]->getFloat());
                            break;
                        case KSType::Vec3:
                            args[0]->getStdVector<vec3>().push_back(args[1]->getVec3());
                            break;
                        case KSType::String:
                            args[0]->getStdVector<string>().push_back(args[1]->getString());
                            break;
                        case KSType::Function:
                            args[0]->getStdVector<KSFunctionRef>().push_back(args[1]->getFunction());
                            break;
                        default:
                            break;
                        }
                    }
                } else {
                    args[0]->getList().push_back(args[1]);
                }
                return make_shared<KSValue>();
                }},

            {"popback", [](const KSList& args) {
                if (args.size() < 1 || (int)args[0]->type < (int)KSType::Array) {
                    return make_shared<KSValue>();
                }
                if (args[0]->type == KSType::Array) {
                    args[0]->getArray().pop_back();
                } else {
                    args[0]->getList().pop_back();
                }
                return make_shared<KSValue>();
                }},

            {"popfront", [](const KSList& args) {
                if (args.size() < 1 || (int)args[0]->type < (int)KSType::Array) {
                    return make_shared<KSValue>();
                }
                if (args[0]->type == KSType::Array) {
                    switch (args[0]->getArray().type) {
                    case KSType::Int:
                    {
                        auto& arry = args[0]->getStdVector<KSInt>();
                        arry.erase(arry.begin());
                    }
                    break;
                    case KSType::Float:
                    {
                        auto& arry = args[0]->getStdVector<KSFloat>();
                        arry.erase(arry.begin());
                    }
                    break;
                    case KSType::Vec3:
                    {
                        auto& arry = args[0]->getStdVector<vec3>();
                        arry.erase(arry.begin());
                    }
                    break;
                    case KSType::Function:
                    {
                        auto& arry = args[0]->getStdVector<KSFunctionRef>();
                        arry.erase(arry.begin());
                    }
                    break;
                    case KSType::String:
                    {
                        auto& arry = args[0]->getStdVector<string>();
                        arry.erase(arry.begin());
                    }
                    break;
                    
                    default:
                        break;
                    }
                    return args[0];
                } else {
                    auto& list = args[0]->getList();
                    list.erase(list.begin());
                }
                return make_shared<KSValue>();
                }},

            {"front", [](const KSList& args) {
                if (args.size() < 1 || (int)args[0]->type < (int)KSType::Array) {
                    return make_shared<KSValue>();
                }
                if (args[0]->type == KSType::Array) {
                    switch (args[0]->getArray().type) {
                    case KSType::Int:
                        return make_shared<KSValue>(args[0]->getStdVector<KSInt>().front());
                    case KSType::Float:
                        return make_shared<KSValue>(args[0]->getStdVector<KSFloat>().front());
                    case KSType::Vec3:
                        return make_shared<KSValue>(args[0]->getStdVector<vec3>().front());
                    case KSType::Function:
                        return make_shared<KSValue>(args[0]->getStdVector<KSFunctionRef>().front());
                    case KSType::String:
                        return make_shared<KSValue>(args[0]->getStdVector<string>().front());
                    default:
                        break;
                    }
                    return make_shared<KSValue>();
                } else {
                    return args[0]->getList().front();
                }
                }},

            {"back", [](const KSList& args) {
                if (args.size() < 1 || (int)args[0]->type < (int)KSType::Array) {
                    return make_shared<KSValue>();
                }
                if (args[0]->type == KSType::Array) {
                    switch (args[0]->getArray().type) {
                    case KSType::Int:
                        return make_shared<KSValue>(args[0]->getStdVector<KSInt>().back());
                    case KSType::Float:
                        return make_shared<KSValue>(args[0]->getStdVector<KSFloat>().back());
                    case KSType::Vec3:
                        return make_shared<KSValue>(args[0]->getStdVector<vec3>().back());
                    case KSType::Function:
                        return make_shared<KSValue>(args[0]->getStdVector<KSFunctionRef>().back());
                    case KSType::String:
                        return make_shared<KSValue>(args[0]->getStdVector<string>().back());
                    default:
                        break;
                    }
                    return make_shared<KSValue>();
                } else {
                    return args[0]->getList().back();
                }
                }},

            {"reverse", [](const KSList& args) {                
                if (args.size() < 1 || (int)args[0]->type < (int)KSType::String) {
                    return make_shared<KSValue>();
                }
                auto copy = make_shared<KSValue>(args[0]->value, args[0]->type);

                if (args[0]->type == KSType::String) {
                    auto& str = copy->getString();
                    std::reverse(str.begin(), str.end());
                    return copy;
                } else if (args[0]->type == KSType::Array) { 
                    switch (copy->getArray().type) {
                    case KSType::Int:
                    {
                        auto& vl = copy->getStdVector<KSInt>();
                        std::reverse(vl.begin(), vl.end());
                        return copy;
                    }
                        break;
                    case KSType::Float:
                    {
                        auto& vl = copy->getStdVector<KSFloat>();
                        std::reverse(vl.begin(), vl.end());
                        return copy;
                    }
                        break;
                    case KSType::Vec3:
                    {
                        auto& vl = copy->getStdVector<vec3>();
                        std::reverse(vl.begin(), vl.end());
                        return copy;
                    }
                        break;
                    case KSType::String:
                    {
                        auto& vl = copy->getStdVector<string>();
                        std::reverse(vl.begin(), vl.end());
                        return copy;
                    }
                        break;
                    case KSType::Function:
                    {
                        auto& vl = copy->getStdVector<KSFunctionRef>();
                        std::reverse(vl.begin(), vl.end());
                        return copy;
                    }
                        break;
                    default:
                        break;
                    }
                } else if (args[0]->type == KSType::List) {
                    auto& vl = copy->getList();
                    std::reverse(vl.begin(), vl.end());
                    return copy;
                }
                return make_shared<KSValue>();
                }},

            {"range", [](const KSList& args) {
                if (args.size() == 2 && args[0]->type == args[1]->type) {
                    if (args[0]->type == KSType::Int) {
                        auto ret = make_shared<KSValue>(KSArray(vector<KSInt>{}));
                        auto& arry = ret->getStdVector<KSInt>();
                        auto a = args[0]->getInt();
                        auto b = args[1]->getInt();
                        if (b > a) {
                            arry.reserve(b - a);
                            for (KSInt i = a; i <= b; i++) {
                                arry.push_back(i);
                            }
                        } else {
                            arry.reserve(a - b);
                            for (KSInt i = a; i >= b; i--) {
                                arry.push_back(i);
                            }
                        }
                        return ret;
                    } else if (args[0]->type == KSType::Float) {
                        auto ret = make_shared<KSValue>(KSArray(vector<KSFloat>{}));
                        auto& arry = ret->getStdVector<KSFloat>();
                        KSFloat a = args[0]->getFloat();
                        KSFloat b = args[1]->getFloat();
                        if (b > a) {
                            arry.reserve((KSInt)(b - a));
                            for (KSFloat i = a; i <= b; i++) {
                                arry.push_back(i);
                            }
                        } else {
                            arry.reserve((KSInt)(a - b));
                            for (KSFloat i = a; i >= b; i--) {
                                arry.push_back(i);
                            }
                        }
                        return ret;
                    }                    
                }
                if (args.size() < 3 || (int)args[0]->type < (int)KSType::String) {
                    return make_shared<KSValue>();
                }
                auto indexA = *args[1];
                indexA.hardconvert(KSType::Int);
                auto indexB = *args[2];
                indexB.hardconvert(KSType::Int);
                auto intdexA = indexA.getInt();
                auto intdexB = indexB.getInt();

                if (args[0]->type == KSType::String) {
                    return make_shared<KSValue>(args[0]->getString().substr(intdexA, intdexB));
                } else if (args[0]->type == KSType::Array) {
                    if (args[0]->getArray().type == args[1]->type) {
                        switch (args[0]->getArray().type) {
                        case KSType::Int:
                            return make_shared<KSValue>(KSArray(vector<KSInt>(args[0]->getStdVector<KSInt>().begin() + intdexA, args[0]->getStdVector<KSInt>().begin() + intdexB)));
                            break;
                        case KSType::Float:
                            return make_shared<KSValue>(KSArray(vector<KSFloat>(args[0]->getStdVector<KSFloat>().begin() + intdexA, args[0]->getStdVector<KSFloat>().begin() + intdexB)));
                            break;
                        case KSType::Vec3:
                            return make_shared<KSValue>(KSArray(vector<vec3>(args[0]->getStdVector<vec3>().begin() + intdexA, args[0]->getStdVector<vec3>().begin() + intdexB)));
                            break;
                        case KSType::String:
                            return make_shared<KSValue>(KSArray(vector<string>(args[0]->getStdVector<string>().begin() + intdexA, args[0]->getStdVector<string>().begin() + intdexB)));
                            break;
                        case KSType::Function:
                            return make_shared<KSValue>(KSArray(vector<KSFunctionRef>(args[0]->getStdVector<KSFunctionRef>().begin() + intdexA, args[0]->getStdVector<KSFunctionRef>().begin() + intdexB)));
                            break;
                        default:
                            break;
                        }
                    }
                } else {
                    return make_shared<KSValue>(KSList(args[0]->getList().begin() + intdexA, args[0]->getList().begin() + intdexB));
                }
                return make_shared<KSValue>();
                }},

            {"replace", [](const KSList& args) {
                if (args.size() < 3 || args[0]->type != KSType::String || args[1]->type != KSType::String || args[2]->type != KSType::String) {
                    return make_shared<KSValue>();
                }

                string& input = args[0]->getString(); 
                const string& lookfor = args[1]->getString();
                const string& replacewith = args[2]->getString();
                size_t pos = 0;
                size_t lpos = 0;
                while ((pos = input.find(lookfor, lpos)) != string::npos) {
                    input.replace(pos, lookfor.size(), replacewith);
                    lpos = pos + replacewith.size();
                }
                    
                return make_shared<KSValue>(input);
                }},

            {"startswith", [](const KSList& args) {
                if (args.size() < 2 || args[0]->type != KSType::String || args[1]->type != KSType::String) {
                    return make_shared<KSValue>();
                }
                return make_shared<KSValue>(KSInt(startswith(args[0]->getString(), args[1]->getString())));
                }},

            {"endswith", [](const KSList& args) {
                if (args.size() < 2 || args[0]->type != KSType::String || args[1]->type != KSType::String) {
                    return make_shared<KSValue>();
                }
                return make_shared<KSValue>(KSInt(endswith(args[0]->getString(), args[1]->getString())));
                }},

            {"contains", [](const KSList& args) {
                if (args.size() < 2 || (int)args[0]->type < (int)KSType::Array) {
                    return make_shared<KSValue>(KSInt(0));
                }
                if (args[0]->type == KSType::Array) {
                    auto item = *args[1];
                    switch (args[0]->getArray().type) {
                    case KSType::Int:
                        item.hardconvert(KSType::Int);
                        return make_shared<KSValue>((KSInt)contains(args[0]->getStdVector<KSInt>(), item.getInt()));
                    case KSType::Float:
                        item.hardconvert(KSType::Float);
                        return make_shared<KSValue>((KSInt)contains(args[0]->getStdVector<KSFloat>(), item.getFloat()));
                    case KSType::Vec3:
                        item.hardconvert(KSType::Vec3);
                        return make_shared<KSValue>((KSInt)contains(args[0]->getStdVector<vec3>(), item.getVec3()));
                    case KSType::String:
                        item.hardconvert(KSType::String);
                        return make_shared<KSValue>((KSInt)contains(args[0]->getStdVector<string>(), item.getString()));
                    default:
                        break;
                    }
                    return make_shared<KSValue>(KSInt(0));
                }
                auto& list = args[0]->getList();
                for (size_t i = 0; i < list.size(); ++i) {
                    if (*list[i] == *args[1]) {
                        return make_shared<KSValue>(KSInt(1));
                    }
                }
                return make_shared<KSValue>(KSInt(0));
                }},

            {"split", [](const KSList& args) {
                if (args.size() > 0 && args[0]->type == KSType::String) {
                    if (args.size() == 1) {
                        vector<string> chars;
                        for (auto c : args[0]->getString()) {
                            chars.push_back(""s + c);
                        }
                        return make_shared<KSValue>(KSArray(chars));
                    }
                    return make_shared<KSValue>(KSArray(split(args[0]->getString(), args[1]->getPrintString())));
                }
                return make_shared<KSValue>();
                }},

            {"sort", [](const KSList& args) {
                if (args.size() < 1 || (int)args[0]->type < (int)KSType::Array) {
                    return make_shared<KSValue>();
                }
                if (args[0]->type == KSType::Array) {
                    switch (args[0]->getArray().type) {
                    case KSType::Int:
                    {
                        auto& arry = args[0]->getStdVector<KSInt>();
                        std::sort(arry.begin(), arry.end());
                    }
                    break;
                    case KSType::Float:
                    {
                        auto& arry = args[0]->getStdVector<KSFloat>();
                        std::sort(arry.begin(), arry.end());
                    }
                    break;
                    case KSType::String:
                    {
                        auto& arry = args[0]->getStdVector<string>();
                        std::sort(arry.begin(), arry.end());
                    }
                    break;
                    case KSType::Vec3:
                    {
                        auto& arry = args[0]->getStdVector<vec3>();
                        std::sort(arry.begin(), arry.end(), [](const vec3& a, const vec3& b) { return a.x < b.x; });
                    }
                    break;
                    default:
                        break;
                    }
                    return args[0];
                }
                auto& list = args[0]->getList();
                std::sort(list.begin(), list.end(), [](const KSValueRef& a, const KSValueRef& b) { return *a < *b; });
                return args[0];
                }},
        });

        applyFunctionLocation = resolveFunction("applyfunction", modules.back());
        applyFunctionVarLocation = resolveVariable("applyfunction", modules.back());
        listIndexFunctionVarLocation = resolveVariable("listindex", modules.back());
	}
}
#pragma once
#include "KataScript.hpp"
namespace KataScript {
    ScopeRef KataScriptInterpreter::newModule(const string& name, ModulePrivilegeFlags flags, const unordered_map<string, Lambda>& functions) {
        auto& modSource = flags ? optionalModules : modules;
        modSource.emplace_back(flags, make_shared<Scope>(name, this));
        auto scope = modSource.back().scope;

        for (auto& funcPair : functions) {
            newFunction(funcPair.first, scope, funcPair.second);
        }

        return modSource.back().scope;
    }

    Module* KataScriptInterpreter::getOptionalModule(const string& name) {
        auto iter = std::find_if(optionalModules.begin(), optionalModules.end(), [&name](const auto& mod) {return mod.scope->name == name; });
        if (iter != optionalModules.end()) {
            return &*iter;
        }
        return nullptr;
    }

    void KataScriptInterpreter::createStandardLibrary() {
		// register compiled functions and standard library:
        newModule("StandardLib"s, 0, {
        // math operators
            {"=", [this](const List& args) {
                if (args.size() == 0) {
                    return resolveVariable("=");
                }
                if (args.size() == 1) {
                    return args[0];
                }
                
                if (args[0]->getType() == Type::ArrayMember) {
                    args[0]->getArrayMember().setValue(args[1]);
                    return args[0]->getArrayMember().getValue();
                } else {
                    *(args[0]) = *(args[1]);
                    return args[0];
                }
                
                }},

            {"+", [this](const List& args) {
                if (args.size() == 0) {
                    return resolveVariable("+");
                }
                if (args.size() == 1) {
                    return args[0];
                }
                return make_shared<Value>(*args[0] + *args[1]);
                }},

            {"-", [this](const List& args) {
                if (args.size() == 0) {
                    return resolveVariable("-");
                }
                if (args.size() == 1) {
                    auto zero = Value(Int(0));
                    upconvert(*args[0], zero);
                    return make_shared<Value>(zero - *args[0]);
                }
                return make_shared<Value>(*args[0] - *args[1]);
                }},

            {"*", [this](const List& args) {
                if (args.size() == 0) {
                    return resolveVariable("*");
                }
                if (args.size() < 2) {
                    return makeNull();
                }
                return make_shared<Value>(*args[0] * *args[1]);
                }},

            {"/", [this](const List& args) {
                if (args.size() == 0) {
                    return resolveVariable("/");
                }
                if (args.size() < 2) {
                    return makeNull();
                }
                return make_shared<Value>(*args[0] / *args[1]);
                }},

            {"%", [this](const List& args) {
                if (args.size() == 0) {
                    return resolveVariable("%");
                }
                if (args.size() < 2) {
                    return makeNull();
                }
                return make_shared<Value>(*args[0] % *args[1]);
                }},

            {"==", [this](const List& args) {
                if (args.size() == 0) {
                    return resolveVariable("==");
                }
                if (args.size() < 2) {
                    return make_shared<Value>(Int(0));
                }
                return make_shared<Value>((Int)(*args[0] == *args[1]));
                }},

            {"!=", [this](const List& args) {
                if (args.size() == 0) {
                    return resolveVariable("!=");
                }
                if (args.size() < 2) {
                    return make_shared<Value>(Int(0));
                }
                return make_shared<Value>((Int)(*args[0] != *args[1]));
                }},

            {"||", [this](const List& args) {
                if (args.size() == 0) {
                    return resolveVariable("||");
                }
                if (args.size() < 2) {
                    return make_shared<Value>(Int(1));
                }
                return make_shared<Value>((Int)(*args[0] || *args[1]));
                }},

            {"&&", [this](const List& args) {
                if (args.size() == 0) {
                    return resolveVariable("&&");
                }
                if (args.size() < 2) {
                    return make_shared<Value>(Int(0));
                }
                return make_shared<Value>((Int)(*args[0] && *args[1]));
                }},

            {"++", [](const List& args) {
                if (args.size() == 0) {
                    return makeNull();
                }
                auto i = args.size() - 1;
                if (i) {
                    // prefix
                    *args[i] += Value(Int(1));
                    return args[i];
                } else {
                    // postfix
                    auto val = make_shared<Value>(args[i]->value);
                    *args[i] += Value(Int(1));
                    return val;
                }
                }},

            {"--", [](const List& args) {
                if (args.size() == 0) {
                    return makeNull();
                }
                auto i = args.size() - 1;
                if (i) {
                    // prefix
                    *args[i] -= Value(Int(1));
                    return args[i];
                } else {
                    // postfix
                    auto val = make_shared<Value>(args[i]->value);
                    *args[i] -= Value(Int(1));
                    return val;
                }
                }},

            {"+=", [](const List& args) {
                if (args.size() == 0) {
                    return makeNull();
                }
                if (args.size() == 1) {
                    return args[0];
                }
                *args[0] += *args[1];
                return args[0];
                }},

            {"-=", [](const List& args) {
                if (args.size() == 0) {
                    return makeNull();
                }
                if (args.size() == 1) {
                    return args[0];
                }
                *args[0] -= *args[1];
                return args[0];
                }},

            {"*=", [](const List& args) {
                if (args.size() == 0) {
                    return makeNull();
                }
                if (args.size() == 1) {
                    return args[0];
                }
                *args[0] *= *args[1];
                return args[0];
                }},

            {"/=", [](const List& args) {
                if (args.size() == 0) {
                    return makeNull();
                }
                if (args.size() == 1) {
                    return args[0];
                }
                *args[0] /= *args[1];
                return args[0];
                }},

            {">", [this](const List& args) {
                if (args.size() == 0) {
                    return resolveVariable(">");
                }
                if (args.size() < 2) {
                    return make_shared<Value>(Int(0));
                }
                return make_shared<Value>((Int)(*args[0] > *args[1]));
                }},

            {"<", [this](const List& args) {
                if (args.size() == 0) {
                    return resolveVariable("<");
                }
                if (args.size() < 2) {
                    return make_shared<Value>(Int(0));
                }
                return make_shared<Value>((Int)(*args[0] < *args[1]));
                }},

            {">=", [this](const List& args) {
                if (args.size() == 0) {
                    return resolveVariable(">=");
                }
                if (args.size() < 2) {
                    return make_shared<Value>(Int(0));
                }
                return make_shared<Value>((Int)(*args[0] >= *args[1]));
                }},

            {"<=", [this](const List& args) {
                if (args.size() == 0) {
                    return resolveVariable("<=");
                }
                if (args.size() < 2) {
                    return make_shared<Value>(Int(0));
                }
                return make_shared<Value>((Int)(*args[0] <= *args[1]));
                }},

            {"!", [](const List& args) {
                if (args.size() == 0) {
                    return make_shared<Value>(Int(0));
                }
                if (args.size() == 1) {
                    if (args[0]->getType() != Type::Int) return makeNull();
                    auto val = Int(1);
                    for (auto i = Int(1); i <= args[0]->getInt(); ++i) {
                        val *= i;
                    }
                    return make_shared<Value>(val);
                }
                if (args.size() == 2) {
                    return make_shared<Value>((Int)(!args[1]->getBool()));
                }
                return makeNull();
                }},

        // aliases
            {"identity", [](const List& args) {
                if (args.size() == 0) {
                    return makeNull();
                }
                return args[0];
                }},

            {"copy", [](const List& args) {
                if (args.size() == 0) {
                    return makeNull();
                }
                if (args[0]->getType() == Type::Class) {
                    return make_shared<Value>(make_shared<Class>(*args[0]->getClass()));
                }
                return make_shared<Value>(args[0]->value);
                }},

            {"listindex", [](const List& args) {
                if (args.size() > 0) {
                    if (args.size() == 1) {
                        return args[0];
                    }

                    auto var = args[0];
                    if (args[1]->getType() != Type::Int) {
                        var->upconvert(Type::Dictionary);
                    }

                    switch (var->getType()) {
                    case Type::Array:
                        return make_shared<Value>(ArrayMember{ var, args[1]->getInt() });
                    default:
                        var = make_shared<Value>(var->value);
                        var->upconvert(Type::List);
                        [[fallthrough]];
                    case Type::List:
                    {
                        auto ival = args[1]->getInt();

                        auto& list = var->getList();
                        if (ival < 0 || ival >= (Int)list.size()) {
                            throw Exception("Out of bounds list access index "s + std::to_string(ival) + ", list length " + std::to_string(list.size()));
                        } else {
                            return list[ival];
                        }
                    }
                    break;
                    case Type::Class:
                    {
                        auto strval = args[1]->getString();
                        auto& struc = var->getClass();
                        auto iter = struc->variables.find(strval);
                        if (iter == struc->variables.end()) {
                            throw Exception("Class `"s + struc->name + "` does not contain member `" + strval + "`");
                        } else {
                            return iter->second;
                        }
                    }
                    break;
                    case Type::Dictionary:
                    {
                        auto& dict = var->getDictionary();
                        auto& ref = (*dict)[args[1]->getHash()];
                        if (ref == nullptr) {
                            ref = makeNull();
                        }
                        return ref;
                    }
                    break;
                    }
                }
                return makeNull();
                }},
        // casting
            {"bool", [](const List& args) {
                if (args.size() == 0) {
                    return make_shared<Value>(Int(0));
                }
                auto val = *args[0];
                val.hardconvert(Type::Int);
                val.value = (Int)args[0]->getBool();
                return make_shared<Value>(val);
                }},

            {"int", [](const List& args) {
                if (args.size() == 0) {
                    return make_shared<Value>(Int(0));
                }
                auto val = *args[0];
                val.hardconvert(Type::Int);
                return make_shared<Value>(val);
                }},

            {"float", [](const List& args) {
                if (args.size() == 0) {
                    return make_shared<Value>(Float(0.0));
                }
                auto val = *args[0];
                val.hardconvert(Type::Float);
                return make_shared<Value>(val);
                }},

            {"vec3", [](const List& args) {
                if (args.size() == 0) {
                    return make_shared<Value>(vec3());
                }
                if (args.size() < 3) {
                    auto val = *args[0];
                    val.hardconvert(Type::Float);
                    return make_shared<Value>(vec3((float)val.getFloat()));
                }
                auto x = *args[0];
                x.hardconvert(Type::Float);
                auto y = *args[1];
                y.hardconvert(Type::Float);
                auto z = *args[2];
                z.hardconvert(Type::Float);
                return make_shared<Value>(vec3((float)x.getFloat(), (float)y.getFloat(), (float)z.getFloat()));
                }},

            {"string", [](const List& args) {
                if (args.size() == 0) {
                    return make_shared<Value>(""s);
                }
                auto val = *args[0];
                val.hardconvert(Type::String);
                return make_shared<Value>(val);
                }},

            {"array", [](const List& args) {
                if (args.size() == 0) {
                    return make_shared<Value>(Array());
                }
                auto list = make_shared<Value>(args);
                list->hardconvert(Type::Array);
                return list;
                }},

            {"list", [](const List& args) {
                if (args.size() == 0) {
                    return make_shared<Value>(List());
                }
                return make_shared<Value>(args);
                }},

            {"dictionary", [](const List& args) {
                if (args.size() == 0) {
                    return make_shared<Value>(make_shared<Dictionary>());
                }
                if (args.size() == 1) {
                    auto val = *args[0];
                    val.hardconvert(Type::Dictionary);
                    return make_shared<Value>(val);
                }
                auto dict = make_shared<Value>(make_shared<Dictionary>());
                for (auto&& arg : args) {
                    auto val = *arg;
                    val.hardconvert(Type::Dictionary);
                    dict->getDictionary()->merge(*val.getDictionary());
                }
                return dict;
                }},

            {"toarray", [](const List& args) {
                if (args.size() == 0) {
                    return make_shared<Value>(Array());
                }
                auto val = *args[0];
                val.hardconvert(Type::Array);
                return make_shared<Value>(val);  
                }},

            {"tolist", [](const List& args) {
                if (args.size() == 0) {
                    return make_shared<Value>(List());
                }
                auto val = *args[0];
                val.hardconvert(Type::List);
                return make_shared<Value>(val);
                }},

        // overal stdlib
            {"typeof", [](const List& args) {
                if (args.size() == 0) {
                    return makeNull();
                }
                auto mainType = args[0]->getType();
                if (mainType == Type::Array) {
                    return make_shared<Value>(getTypeName(mainType) + "<"s + getTypeName(args[0]->getArray().getType()) + ">"s);
                } else if (mainType == Type::Class) {
                    return make_shared<Value>(args[0]->getClass()->name);
                }
                return make_shared<Value>(getTypeName(mainType));
                }},

            {"sqrt", [](const List& args) {
                if (args.size() == 0) {
                    return makeNull();
                }
                auto val = *args[0];
                val.hardconvert(Type::Float);
                return make_shared<Value>(sqrt(val.getFloat()));
                }},

            {"sin", [](const List& args) {
                if (args.size() == 0) {
                    return makeNull();
                }
                auto val = *args[0];
                val.hardconvert(Type::Float);
                return make_shared<Value>(sin(val.getFloat()));
                }},

            {"cos", [](const List& args) {
                if (args.size() == 0) {
                    return makeNull();
                }
                auto val = *args[0];
                val.hardconvert(Type::Float);
                return make_shared<Value>(cos(val.getFloat()));
                }},

            {"tan", [](const List& args) {
                if (args.size() == 0) {
                    return makeNull();
                }
                auto val = *args[0];
                val.hardconvert(Type::Float);
                return make_shared<Value>(tan(val.getFloat()));
                }},

            {"pow", [](const List& args) {
                if (args.size() < 2) {
                    return make_shared<Value>(Float(0));
                }
                auto val = *args[0];
                val.hardconvert(Type::Float);
                auto val2 = *args[1];
                val2.hardconvert(Type::Float);
                return make_shared<Value>(pow(val.getFloat(), val2.getFloat()));
                }},

            {"abs", [](const List& args) {
                if (args.size() == 0) {
                    return makeNull();
                }
                switch (args[0]->getType()) {
                case Type::Int:
                    return make_shared<Value>(Int(abs(args[0]->getInt())));
                    break;
                case Type::Float:
                    return make_shared<Value>(Float(fabs(args[0]->getFloat())));
                    break;
                default:
                    return makeNull();
                    break;
                }                
                }},

            {"min", [](const List& args) {
                if (args.size() < 2) {
                    return makeNull();
                }
                auto val = *args[0];
                auto val2 = *args[1];
                upconvertThrowOnNonNumberToNumberCompare(val, val2);
                if (val > val2) {
                    return make_shared<Value>(val2.value);
                }
                return make_shared<Value>(val.value);
                }},

            {"max", [](const List& args) {
                if (args.size() < 2) {
                    return makeNull();
                }
                auto val = *args[0];
                auto val2 = *args[1];
                upconvertThrowOnNonNumberToNumberCompare(val, val2);
                if (val < val2) {
                    return make_shared<Value>(val2.value);
                }
                return make_shared<Value>(val.value);
                }},

            {"swap", [](const List& args) {
                if (args.size() < 2) {
                    return makeNull();
                }
                auto v = *args[0];
                *args[0] = *args[1];
                *args[1] = v;

                return makeNull();
                }},

            {"print", [](const List& args) {
                for (auto&& arg : args) {
                    printf("%s", arg->getPrintString().c_str());
                }
                printf("\n");
                return makeNull();
                }},

            {"getline", [](const List& args) {
                string s;
                // blocking calls are fine
                getline(std::cin, s);
                if (args.size() > 0) {
                    args[0]->value = s;
                }
                return make_shared<Value>(s);
                }},

            {"map", [this](const List& args) {
                if (args.size() < 2 || args[1]->getType() != Type::Function) {
                    return makeNull();
                }
                auto ret = make_shared<Value>(List());
                auto& retList = ret->getList();
                auto func = args[1]->getFunction();
                List* list;
                std::unique_ptr<Value> val;
                List curryArgs = { makeNull() };
                curryArgs.insert(curryArgs.end(), std::next(std::next(args.begin())), args.end());

                if (args[0]->getType() == Type::Array) {
                    val.reset(new Value());
                    *val = *args[0];
                    val->upconvert(Type::List);
                    list = &val->getList();
                } else {
                    list = &args[0]->getList();
                }

                retList.reserve(list->size());

                for (auto&& v : *list) {
                    curryArgs[0] = v;
                    retList.push_back(callFunction(func, curryArgs));
                }
                return ret;

                }},

            {"fold", [this](const List& args) {
                if (args.size() < 3 || args[1]->getType() != Type::Function) {
                    return makeNull();
                }

                auto func = args[1]->getFunction();
                auto iter = args[2];

                if (args[0]->getType() == Type::Array) {
                    auto val = *args[0];
                    val.upconvert(Type::List);
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

            {"clock", [](const List&) {
                return make_shared<Value>(Int(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
                }},

            {"getduration", [](const List& args) {
                if (args.size() == 2 && args[0]->getType() == Type::Int && args[1]->getType() == Type::Int) {
                    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::time_point(std::chrono::nanoseconds(args[1]->getInt())) -
                        std::chrono::high_resolution_clock::time_point(std::chrono::nanoseconds(args[0]->getInt()));
                    return make_shared<Value>(Float(duration.count()));
                }
                return makeNull();
                }},

            {"timesince", [](const List& args) {
                if (args.size() == 1 && args[0]->getType() == Type::Int) {
                    std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - 
                        std::chrono::high_resolution_clock::time_point(std::chrono::nanoseconds(args[0]->getInt()));
                    return make_shared<Value>(Float(duration.count()));
                }
                return makeNull();
                }},

        // collection functions
            {"length", [](const List& args) {
                if (args.size() == 0 || (int)args[0]->getType() < (int)Type::String) {
                    return make_shared<Value>(Int(0));
                }
                if (args[0]->getType() == Type::String) {
                    return make_shared<Value>((Int)args[0]->getString().size());
                } else
                if (args[0]->getType() == Type::Array) {
                    return make_shared<Value>((Int)args[0]->getArray().size());
                } else
                if (args[0]->getType() == Type::List) {
                    return make_shared<Value>((Int)args[0]->getList().size());
                } else
                if (args[0]->getType() == Type::List) {
                    return make_shared<Value>((Int)args[0]->getList().size());
                } else
                if (args[0]->getType() == Type::Dictionary) {
                    return make_shared<Value>((Int)args[0]->getDictionary()->size());
                }

                return makeNull();
                }},

            {"find", [](const List& args) {
                if (args.size() < 2 || (int)args[0]->getType() < (int)Type::Array) {
                    return makeNull();
                }
                if (args[0]->getType() == Type::Array) {
                    if (args[1]->getType() == args[0]->getArray().getType()) {
                        switch (args[0]->getArray().getType()) {
                        case Type::Int:
                        {
                            auto& arry = args[0]->getStdVector<Int>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getInt());
                            if (iter == arry.end()) {
                                return makeNull();
                            }
                            return make_shared<Value>((Int)(iter - arry.begin()));
                        }
                        break;
                        case Type::Float:
                        {
                            auto& arry = args[0]->getStdVector<Float>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getFloat());
                            if (iter == arry.end()) {
                                return makeNull();
                            }
                            return make_shared<Value>((Int)(iter - arry.begin()));
                        }
                        break;
                        case Type::Vec3:
                        {
                            auto& arry = args[0]->getStdVector<vec3>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getVec3());
                            if (iter == arry.end()) {
                                return makeNull();
                            }
                            return make_shared<Value>((Int)(iter - arry.begin()));
                        }
                        break;
                        case Type::String:
                        {
                            auto& arry = args[0]->getStdVector<string>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getString());
                            if (iter == arry.end()) {
                                return makeNull();
                            }
                            return make_shared<Value>((Int)(iter - arry.begin()));
                        }
                        break;
                        case Type::Function:
                        {
                            auto& arry = args[0]->getStdVector<FunctionRef>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getFunction());
                            if (iter == arry.end()) {
                                return makeNull();
                            }
                            return make_shared<Value>((Int)(iter - arry.begin()));
                        }
                        break;
                        default:
                            break;
                        }
                    }
                    return makeNull();
                }
                auto& list = args[0]->getList();
                for (size_t i = 0; i < list.size(); ++i) {
                    if (*list[i] == *args[1]) {
                        return make_shared<Value>((Int)i);
                    }
                }
                return makeNull();
                }},

            {"erase", [](const List& args) {
                if (args.size() < 2 || (int)args[0]->getType() < (int)Type::Array) {
                    return makeNull();
                }
                
                if (args[0]->getType() == Type::Array) {
                    if (args[1]->getType() != Type::Int) {
                        return makeNull();
                    }
                    switch (args[0]->getArray().getType()) {
                    case Type::Int:
                        args[0]->getStdVector<Int>().erase(args[0]->getStdVector<Int>().begin() + args[1]->getInt());
                        break;
                    case Type::Float:
                        args[0]->getStdVector<Float>().erase(args[0]->getStdVector<Float>().begin() + args[1]->getInt());
                        break;
                    case Type::Vec3:
                        args[0]->getStdVector<vec3>().erase(args[0]->getStdVector<vec3>().begin() + args[1]->getInt());
                        break;
                    case Type::String:
                        args[0]->getStdVector<string>().erase(args[0]->getStdVector<string>().begin() + args[1]->getInt());
                        break;
                    case Type::Function:
                        args[0]->getStdVector<FunctionRef>().erase(args[0]->getStdVector<FunctionRef>().begin() + args[1]->getInt());
                        break;
                    default:
                        break;
                    }
                } else if (args[0]->getType() == Type::List) {
                    if (args[1]->getType() != Type::Int) {
                        return makeNull();
                    }
                    args[0]->getList().erase(args[0]->getList().begin() + args[1]->getInt());
                } else if (args[0]->getType() == Type::Dictionary) {
                    args[0]->getDictionary()->erase(args[1]->getHash());
                }
                return makeNull();
                }},

            {"pushback", [](const List& args) {
                if (args.size() < 2 || (int)args[0]->getType() < (int)Type::Array) {
                    return makeNull();
                }

                if (args[0]->getType() == Type::Array) {
                    if (args[0]->getArray().getType() == args[1]->getType()) {
                        switch (args[0]->getArray().getType()) {
                        case Type::Int:
                            args[0]->getStdVector<Int>().push_back(args[1]->getInt());
                            break;
                        case Type::Float:
                            args[0]->getStdVector<Float>().push_back(args[1]->getFloat());
                            break;
                        case Type::Vec3:
                            args[0]->getStdVector<vec3>().push_back(args[1]->getVec3());
                            break;
                        case Type::String:
                            args[0]->getStdVector<string>().push_back(args[1]->getString());
                            break;
                        case Type::Function:
                            args[0]->getStdVector<FunctionRef>().push_back(args[1]->getFunction());
                            break;
                        default:
                            break;
                        }
                    }
                } else {
                    args[0]->getList().push_back(args[1]);
                }
                return makeNull();
                }},

            {"popback", [](const List& args) {
                if (args.size() < 1 || (int)args[0]->getType() < (int)Type::Array) {
                    return makeNull();
                }
                if (args[0]->getType() == Type::Array) {
                    args[0]->getArray().pop_back();
                } else {
                    args[0]->getList().pop_back();
                }
                return makeNull();
                }},

            {"popfront", [](const List& args) {
                if (args.size() < 1 || (int)args[0]->getType() < (int)Type::Array) {
                    return makeNull();
                }
                if (args[0]->getType() == Type::Array) {
                    switch (args[0]->getArray().getType()) {
                    case Type::Int:
                    {
                        auto& arry = args[0]->getStdVector<Int>();
                        arry.erase(arry.begin());
                    }
                    break;
                    case Type::Float:
                    {
                        auto& arry = args[0]->getStdVector<Float>();
                        arry.erase(arry.begin());
                    }
                    break;
                    case Type::Vec3:
                    {
                        auto& arry = args[0]->getStdVector<vec3>();
                        arry.erase(arry.begin());
                    }
                    break;
                    case Type::Function:
                    {
                        auto& arry = args[0]->getStdVector<FunctionRef>();
                        arry.erase(arry.begin());
                    }
                    break;
                    case Type::String:
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
                return makeNull();
                }},

            {"front", [](const List& args) {
                if (args.size() < 1 || (int)args[0]->getType() < (int)Type::Array) {
                    return makeNull();
                }
                if (args[0]->getType() == Type::Array) {
                    switch (args[0]->getArray().getType()) {
                    case Type::Int:
                        return make_shared<Value>(args[0]->getStdVector<Int>().front());
                    case Type::Float:
                        return make_shared<Value>(args[0]->getStdVector<Float>().front());
                    case Type::Vec3:
                        return make_shared<Value>(args[0]->getStdVector<vec3>().front());
                    case Type::Function:
                        return make_shared<Value>(args[0]->getStdVector<FunctionRef>().front());
                    case Type::String:
                        return make_shared<Value>(args[0]->getStdVector<string>().front());
                    default:
                        break;
                    }
                    return makeNull();
                } else {
                    return args[0]->getList().front();
                }
                }},

            {"back", [](const List& args) {
                if (args.size() < 1 || (int)args[0]->getType() < (int)Type::Array) {
                    return makeNull();
                }
                if (args[0]->getType() == Type::Array) {
                    switch (args[0]->getArray().getType()) {
                    case Type::Int:
                        return make_shared<Value>(args[0]->getStdVector<Int>().back());
                    case Type::Float:
                        return make_shared<Value>(args[0]->getStdVector<Float>().back());
                    case Type::Vec3:
                        return make_shared<Value>(args[0]->getStdVector<vec3>().back());
                    case Type::Function:
                        return make_shared<Value>(args[0]->getStdVector<FunctionRef>().back());
                    case Type::String:
                        return make_shared<Value>(args[0]->getStdVector<string>().back());
                    default:
                        break;
                    }
                    return makeNull();
                } else {
                    return args[0]->getList().back();
                }
                }},

            {"reverse", [](const List& args) {                
                if (args.size() < 1 || (int)args[0]->getType() < (int)Type::String) {
                    return makeNull();
                }
                auto copy = make_shared<Value>(args[0]->value);

                if (args[0]->getType() == Type::String) {
                    auto& str = copy->getString();
                    std::reverse(str.begin(), str.end());
                    return copy;
                } else if (args[0]->getType() == Type::Array) { 
                    switch (copy->getArray().getType()) {
                    case Type::Int:
                    {
                        auto& vl = copy->getStdVector<Int>();
                        std::reverse(vl.begin(), vl.end());
                        return copy;
                    }
                        break;
                    case Type::Float:
                    {
                        auto& vl = copy->getStdVector<Float>();
                        std::reverse(vl.begin(), vl.end());
                        return copy;
                    }
                        break;
                    case Type::Vec3:
                    {
                        auto& vl = copy->getStdVector<vec3>();
                        std::reverse(vl.begin(), vl.end());
                        return copy;
                    }
                        break;
                    case Type::String:
                    {
                        auto& vl = copy->getStdVector<string>();
                        std::reverse(vl.begin(), vl.end());
                        return copy;
                    }
                        break;
                    case Type::Function:
                    {
                        auto& vl = copy->getStdVector<FunctionRef>();
                        std::reverse(vl.begin(), vl.end());
                        return copy;
                    }
                        break;
                    default:
                        break;
                    }
                } else if (args[0]->getType() == Type::List) {
                    auto& vl = copy->getList();
                    std::reverse(vl.begin(), vl.end());
                    return copy;
                }
                return makeNull();
                }},

            {"range", [](const List& args) {
                if (args.size() == 2 && args[0]->getType() == args[1]->getType()) {
                    if (args[0]->getType() == Type::Int) {
                        auto ret = make_shared<Value>(Array(vector<Int>{}));
                        auto& arry = ret->getStdVector<Int>();
                        auto a = args[0]->getInt();
                        auto b = args[1]->getInt();
                        if (b > a) {
                            arry.reserve(b - a);
                            for (Int i = a; i <= b; i++) {
                                arry.push_back(i);
                            }
                        } else {
                            arry.reserve(a - b);
                            for (Int i = a; i >= b; i--) {
                                arry.push_back(i);
                            }
                        }
                        return ret;
                    } else if (args[0]->getType() == Type::Float) {
                        auto ret = make_shared<Value>(Array(vector<Float>{}));
                        auto& arry = ret->getStdVector<Float>();
                        Float a = args[0]->getFloat();
                        Float b = args[1]->getFloat();
                        if (b > a) {
                            arry.reserve((Int)(b - a));
                            for (Float i = a; i <= b; i++) {
                                arry.push_back(i);
                            }
                        } else {
                            arry.reserve((Int)(a - b));
                            for (Float i = a; i >= b; i--) {
                                arry.push_back(i);
                            }
                        }
                        return ret;
                    }                    
                }
                if (args.size() < 3 || (int)args[0]->getType() < (int)Type::String) {
                    return makeNull();
                }
                auto indexA = *args[1];
                indexA.hardconvert(Type::Int);
                auto indexB = *args[2];
                indexB.hardconvert(Type::Int);
                auto intdexA = indexA.getInt();
                auto intdexB = indexB.getInt();

                if (args[0]->getType() == Type::String) {
                    return make_shared<Value>(args[0]->getString().substr(intdexA, intdexB + 1 - intdexA));
                } else if (args[0]->getType() == Type::Array) {
                    if (args[0]->getArray().getType() == args[1]->getType()) {
                        switch (args[0]->getArray().getType()) {
                        case Type::Int:
                            return make_shared<Value>(Array(vector<Int>(args[0]->getStdVector<Int>().begin() + intdexA, args[0]->getStdVector<Int>().begin() + intdexB)));
                            break;
                        case Type::Float:
                            return make_shared<Value>(Array(vector<Float>(args[0]->getStdVector<Float>().begin() + intdexA, args[0]->getStdVector<Float>().begin() + intdexB)));
                            break;
                        case Type::Vec3:
                            return make_shared<Value>(Array(vector<vec3>(args[0]->getStdVector<vec3>().begin() + intdexA, args[0]->getStdVector<vec3>().begin() + intdexB)));
                            break;
                        case Type::String:
                            return make_shared<Value>(Array(vector<string>(args[0]->getStdVector<string>().begin() + intdexA, args[0]->getStdVector<string>().begin() + intdexB)));
                            break;
                        case Type::Function:
                            return make_shared<Value>(Array(vector<FunctionRef>(args[0]->getStdVector<FunctionRef>().begin() + intdexA, args[0]->getStdVector<FunctionRef>().begin() + intdexB)));
                            break;
                        default:
                            break;
                        }
                    }
                } else {
                    return make_shared<Value>(List(args[0]->getList().begin() + intdexA, args[0]->getList().begin() + intdexB));
                }
                return makeNull();
                }},

            {"replace", [](const List& args) {
                if (args.size() < 3 || args[0]->getType() != Type::String || args[1]->getType() != Type::String || args[2]->getType() != Type::String) {
                    return makeNull();
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
                    
                return make_shared<Value>(input);
                }},

            {"startswith", [](const List& args) {
                if (args.size() < 2 || args[0]->getType() != Type::String || args[1]->getType() != Type::String) {
                    return makeNull();
                }
                return make_shared<Value>(Int(startswith(args[0]->getString(), args[1]->getString())));
                }},

            {"endswith", [](const List& args) {
                if (args.size() < 2 || args[0]->getType() != Type::String || args[1]->getType() != Type::String) {
                    return makeNull();
                }
                return make_shared<Value>(Int(endswith(args[0]->getString(), args[1]->getString())));
                }},

            {"contains", [](const List& args) {
                if (args.size() < 2 || (int)args[0]->getType() < (int)Type::Array) {
                    return make_shared<Value>(Int(0));
                }
                if (args[0]->getType() == Type::Array) {
                    auto item = *args[1];
                    switch (args[0]->getArray().getType()) {
                    case Type::Int:
                        item.hardconvert(Type::Int);
                        return make_shared<Value>((Int)contains(args[0]->getStdVector<Int>(), item.getInt()));
                    case Type::Float:
                        item.hardconvert(Type::Float);
                        return make_shared<Value>((Int)contains(args[0]->getStdVector<Float>(), item.getFloat()));
                    case Type::Vec3:
                        item.hardconvert(Type::Vec3);
                        return make_shared<Value>((Int)contains(args[0]->getStdVector<vec3>(), item.getVec3()));
                    case Type::String:
                        item.hardconvert(Type::String);
                        return make_shared<Value>((Int)contains(args[0]->getStdVector<string>(), item.getString()));
                    default:
                        break;
                    }
                    return make_shared<Value>(Int(0));
                } else if (args[0]->getType() == Type::List) {
                    auto& list = args[0]->getList();
                    for (size_t i = 0; i < list.size(); ++i) {
                        if (*list[i] == *args[1]) {
                            return make_shared<Value>(Int(1));
                        }
                    }
                } else if (args[0]->getType() == Type::Dictionary) {
                    return make_shared<Value>(Int(args[0]->getDictionary()->contains(args[1]->getHash())));
                }
                return make_shared<Value>(Int(0));
                }},

            {"split", [](const List& args) {
                if (args.size() > 0 && args[0]->getType() == Type::String) {
                    if (args.size() == 1 || (args[1]->getType() == Type::String && args[1]->getString().size() == 0)) {
                        vector<string> chars;
                        for (auto c : args[0]->getString()) {
                            chars.push_back(string(1,c));
                        }
                        return make_shared<Value>(Array(chars));
                    }
                    return make_shared<Value>(Array(split(args[0]->getString(), args[1]->getPrintString())));
                }
                return makeNull();
                }},

            {"sort", [](const List& args) {
                if (args.size() < 1 || (int)args[0]->getType() < (int)Type::Array) {
                    return makeNull();
                }
                if (args[0]->getType() == Type::Array) {
                    switch (args[0]->getArray().getType()) {
                    case Type::Int:
                    {
                        auto& arry = args[0]->getStdVector<Int>();
                        std::sort(arry.begin(), arry.end());
                    }
                    break;
                    case Type::Float:
                    {
                        auto& arry = args[0]->getStdVector<Float>();
                        std::sort(arry.begin(), arry.end());
                    }
                    break;
                    case Type::String:
                    {
                        auto& arry = args[0]->getStdVector<string>();
                        std::sort(arry.begin(), arry.end());
                    }
                    break;
                    case Type::Vec3:
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
                std::sort(list.begin(), list.end(), [](const ValueRef& a, const ValueRef& b) { return *a < *b; });
                return args[0];
                }},
            { "applyfunction", [this](const List& args) {
                if (args.size() < 2 || args[1]->getType() != Type::Class) {
                    auto func = args[0]->getType() == Type::Function ? args[0] : args[0]->getType() == Type::String ? resolveVariable(args[0]->getString()) : throw Exception("Cannot call non existant function: null");
                    auto list = List();
                    for (size_t i = 1; i < args.size(); ++i) {
                        list.push_back(args[i]);
                    }
                    return callFunction(func->getFunction(), list);
                }
                return makeNull();
                }},
        });

        listIndexFunctionVarLocation = resolveVariable("listindex", modules.back().scope);
        identityFunctionVarLocation = resolveVariable("identity", modules.back().scope);
        setFunctionVarLocation = resolveVariable("=", modules.back().scope);
	}
}

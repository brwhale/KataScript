#pragma once

#include "types.hpp"
#include "value.hpp"

namespace KataScript {
    template <typename T>
    vector<T>& KSArray::getStdVector() {
        return get<vector<T>>(value);
    }

    template <typename T>
    vector<T>& KSValue::getStdVector() {
        return get<KSArray>(value).getStdVector<T>();
    }

    KSClass::KSClass(const KSClass& o) : name(o.name), functionScope(o.functionScope) {
        for (auto&& v : o.variables) {
            variables[v.first] = make_shared<KSValue>(v.second->value, v.second->type);
        }
    }

    KSClass::KSClass(const KSScopeRef& o) : name(o->name), functionScope(o) {
        for (auto&& v : o->variables) {
            variables[v.first] = make_shared<KSValue>(v.second->value, v.second->type);
        }
    }

    size_t KSValue::getHash() {
        size_t hash = 0;
        switch (type) {
        default: break;
        case KSType::Int:
            hash = (size_t)getInt();
            break;
        case KSType::Float:
            hash = std::hash<KSFloat>{}(getFloat());
            break;
        case KSType::Vec3:
            hash = std::hash<float>{}(getVec3().x) ^ std::hash<float>{}(getVec3().y) ^ std::hash<float>{}(getVec3().z);
            break;
        case KSType::Function:
            hash = std::hash<size_t>{}((size_t)getFunction().get());
            break;
        case KSType::String:
            hash = std::hash<string>{}(getString());
            break;
        }
        return hash ^ typeHashBits(type);
    }

    // convert this value up to the newType
    void KSValue::upconvert(KSType newType) {
        if (newType > type) {
            switch (newType) {
            default:
                throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                    getTypeName(type).c_str(), getTypeName(newType).c_str()));
                break;
            case KSType::Int:
                value = KSInt(0);
                break;
            case KSType::Float:
                switch (type) {
                default:
                    throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::Null:
                    value = 0.f;
                    break;
                case KSType::Int:
                    value = (KSFloat)getInt();
                    break;
                }
                break;

            case KSType::Vec3:
                switch (type) {
                default:
                    throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::Null:
                    value = vec3();
                    break;
                case KSType::Int:
                    value = vec3((float)getInt());
                    break;
                case KSType::Float:
                    value = vec3((float)getFloat());
                    break;
                }
                break;
            case KSType::String:
                switch (type) {
                default:
                    throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::Null:
                    value = "null"s;
                    break;
                case KSType::Int:
                    value = stringformat("%lld", getInt());
                    break;
                case KSType::Float:
                    value = stringformat("%f", getFloat());
                    break;
                case KSType::Vec3:
                {
                    auto& vec = getVec3();
                    value = stringformat("%f, %f, %f", vec.x, vec.y, vec.z);
                    break;
                }
                case KSType::Function:
                    value = getFunction()->name;
                    break;
                }
                break;
            case KSType::Array:
                switch (type) {
                default:
                    throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::Null:
                    value = KSArray();
                    getArray().push_back(KSInt(0));
                    break;
                case KSType::Int:
                    value = KSArray(vector<KSInt>{ getInt() });
                    break;
                case KSType::Float:
                    value = KSArray(vector<KSFloat>{ getFloat() });
                    break;
                case KSType::Vec3:
                    value = KSArray(vector<vec3>{ getVec3() });
                    break;
                case KSType::String:
                {
                    auto str = getString();
                    value = KSArray(vector<string>{ });
                    auto& arry = getStdVector<string>();
                    for (auto&& ch : str) {
                        arry.push_back(""s + ch);
                    }
                }
                break;
                }
                break;
            case KSType::List:
                switch (type) {
                default:
                    throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::Null:
                case KSType::Int:
                case KSType::Float:
                case KSType::Vec3:
                    value = KSList({ make_shared<KSValue>(value, type) });
                    break;
                case KSType::String:
                {
                    auto str = getString();
                    value = KSList();
                    auto& list = getList();
                    for (auto&& ch : str) {
                        list.push_back(make_shared<KSValue>(""s + ch));
                    }
                }
                break;
                case KSType::Array:
                    KSArray arr = getArray();
                    value = KSList();
                    auto& list = getList();
                    switch (arr.type) {
                    case KSType::Int:
                        for (auto&& item : get<vector<KSInt>>(arr.value)) {
                            list.push_back(make_shared<KSValue>(item));
                        }
                        break;
                    case KSType::Float:
                        for (auto&& item : get<vector<KSFloat>>(arr.value)) {
                            list.push_back(make_shared<KSValue>(item));
                        }
                        break;
                    case KSType::Vec3:
                        for (auto&& item : get<vector<vec3>>(arr.value)) {
                            list.push_back(make_shared<KSValue>(item));
                        }
                        break;
                    case KSType::String:
                        for (auto&& item : get<vector<string>>(arr.value)) {
                            list.push_back(make_shared<KSValue>(item));
                        }
                        break;
                    default:
                        throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                            getTypeName(type).c_str(), getTypeName(newType).c_str()));
                        break;
                    }
                    break;
                }
                break;
            case KSType::Dictionary:
                switch (type) {
                default:
                    throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::Null:
                case KSType::Int:
                case KSType::Float:
                case KSType::Vec3:
                case KSType::String:
                    value = KSDictionary();
                    break;
                case KSType::Array:
                {
                    KSArray arr = getArray();
                    value = KSDictionary();
                    auto hashbits = typeHashBits(KSType::Int);
                    auto& dict = getDictionary();
                    size_t index = 0;
                    switch (arr.type) {
                    case KSType::Int:
                        for (auto&& item : get<vector<KSInt>>(arr.value)) {
                            dict[index++ ^ hashbits] = make_shared<KSValue>(item);
                        }
                        break;
                    case KSType::Float:
                        for (auto&& item : get<vector<KSFloat>>(arr.value)) {
                            dict[index++ ^ hashbits] = make_shared<KSValue>(item);
                        }
                        break;
                    case KSType::Vec3:
                        for (auto&& item : get<vector<vec3>>(arr.value)) {
                            dict[index++ ^ hashbits] = make_shared<KSValue>(item);
                        }
                        break;
                    case KSType::String:
                        for (auto&& item : get<vector<string>>(arr.value)) {
                            dict[index++ ^ hashbits] = make_shared<KSValue>(item);
                        }
                        break;
                    default:
                        throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                            getTypeName(type).c_str(), getTypeName(newType).c_str()));
                        break;
                    }
                }
                break;
                case KSType::List:
                {
                    auto hashbits = typeHashBits(KSType::Int);
                    KSList list = getList();
                    value = KSDictionary();
                    auto& dict = getDictionary();
                    size_t index = 0;
                    for (auto&& item : list) {
                        dict[index++ ^ hashbits] = item;
                    }
                }
                break;
                }
                break;
            }
            type = newType;
        }
    }

    // convert this value to the newType even if it's a downcast
    void KSValue::hardconvert(KSType newType) {
        if (newType >= type) {
            upconvert(newType);
        } else {
            switch (newType) {
            default:
                throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                    getTypeName(type).c_str(), getTypeName(newType).c_str()));
                break;
            case KSType::Null:
                value = KSInt(0);
                break;
            case KSType::Int:
                switch (type) {
                default:
                    break;
                case KSType::Float:
                    value = (KSInt)getFloat();
                    break;
                case KSType::String:
                    value = (KSInt)fromChars(getString());
                    break;
                case KSType::Array:
                    value = (KSInt)getArray().size();
                    break;
                case KSType::List:
                    value = (KSInt)getList().size();
                    break;
                }
                break;
            case KSType::Float:
                switch (type) {
                default:
                    throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::String:
                    value = (KSFloat)fromChars(getString());
                    break;
                case KSType::Array:
                    value = (KSFloat)getArray().size();
                    break;
                case KSType::List:
                    value = (KSFloat)getList().size();
                    break;
                }
                break;
            case KSType::String:
                switch (type) {
                default:
                    throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::Array:
                {
                    string newval;
                    auto& arr = getArray();
                    switch (arr.type) {
                    case KSType::Int:
                        for (auto&& item : get<vector<KSInt>>(arr.value)) {
                            newval += KSValue(item).getPrintString() + ", ";
                        }
                        break;
                    case KSType::Float:
                        for (auto&& item : get<vector<KSFloat>>(arr.value)) {
                            newval += KSValue(item).getPrintString() + ", ";
                        }
                        break;
                    case KSType::Vec3:
                        for (auto&& item : get<vector<vec3>>(arr.value)) {
                            newval += KSValue(item).getPrintString() + ", ";
                        }
                        break;
                    case KSType::String:
                        for (auto&& item : get<vector<string>>(arr.value)) {
                            newval += KSValue(item).getPrintString() + ", ";
                        }
                        break;
                    default:
                        throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                            getTypeName(type).c_str(), getTypeName(newType).c_str()));
                        break;
                    }
                    if (arr.size()) {
                        newval.pop_back();
                        newval.pop_back();
                    }
                    value = newval;
                }
                break;
                case KSType::List:
                {
                    string newval;
                    auto& list = getList();
                    for (auto val : list) {
                        newval += val->getPrintString() + ", ";
                    }
                    if (newval.size()) {
                        newval.pop_back();
                        newval.pop_back();
                    }
                    value = newval;
                }
                break;
                case KSType::Dictionary:
                {
                    string newval;
                    auto& dict = getDictionary();
                    for (auto&& val : dict) {
                        newval += stringformat("`%u: %s`, ", val.first, val.second->getPrintString().c_str());
                    }
                    if (newval.size()) {
                        newval.pop_back();
                        newval.pop_back();
                    }
                    value = newval;
                }
                break;
                case KSType::Class:
                {
                    auto& strct = getClass();
                    string newval = strct->name + ":\n"s;
                    for (auto&& val : strct->variables) {
                        newval += stringformat("`%s: %s`\n", val.first.c_str(), val.second->getPrintString().c_str());
                    }
                    value = newval;
                }
                break;
                }
                break;
            case KSType::Array:
            {
                switch (type) {
                default:
                    throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::Dictionary:
                {
                    KSArray arr;
                    auto dict = getDictionary();
                    auto listType = dict.begin()->second->type;
                    switch (listType) {
                    case KSType::Int:
                        arr = KSArray(vector<KSInt>{});
                        for (auto&& item : dict) {
                            if (item.second->type == listType) {
                                arr.push_back(item.second->getInt());
                            }
                        }
                        break;
                    case KSType::Float:
                        arr = KSArray(vector<KSFloat>{});
                        for (auto&& item : dict) {
                            if (item.second->type == listType) {
                                arr.push_back(item.second->getFloat());
                            }
                        }
                        break;
                    case KSType::Vec3:
                        arr = KSArray(vector<vec3>{});
                        for (auto&& item : dict) {
                            if (item.second->type == listType) {
                                arr.push_back(item.second->getVec3());
                            }
                        }
                        break;
                    case KSType::Function:
                        arr = KSArray(vector<KSFunctionRef>{});
                        for (auto&& item : dict) {
                            if (item.second->type == listType) {
                                arr.push_back(item.second->getFunction());
                            }
                        }
                        break;
                    case KSType::String:
                        arr = KSArray(vector<string>{});
                        for (auto&& item : dict) {
                            if (item.second->type == listType) {
                                arr.push_back(item.second->getString());
                            }
                        }
                        break;
                    default:
                        throw KSException("Array cannot contain collections");
                        break;
                    }
                    value = arr;
                }
                break;
                case KSType::List:
                {
                    auto list = getList();
                    auto listType = list[0]->type;
                    KSArray arr;
                    switch (listType) {
                    case KSType::Null:
                        arr = KSArray(vector<KSInt>{});
                        break;
                    case KSType::Int:
                        arr = KSArray(vector<KSInt>{});
                        for (auto&& item : list) {
                            if (item->type == listType) {
                                arr.push_back(item->getInt());
                            }
                        }
                        break;
                    case KSType::Float:
                        arr = KSArray(vector<KSFloat>{});
                        for (auto&& item : list) {
                            if (item->type == listType) {
                                arr.push_back(item->getFloat());
                            }
                        }
                        break;
                    case KSType::Vec3:
                        arr = KSArray(vector<vec3>{});
                        for (auto&& item : list) {
                            if (item->type == listType) {
                                arr.push_back(item->getVec3());
                            }
                        }
                        break;
                    case KSType::Function:
                        arr = KSArray(vector<KSFunctionRef>{});
                        for (auto&& item : list) {
                            if (item->type == listType) {
                                arr.push_back(item->getFunction());
                            }
                        }
                        break;
                    case KSType::String:
                        arr = KSArray(vector<string>{});
                        for (auto&& item : list) {
                            if (item->type == listType) {
                                arr.push_back(item->getString());
                            }
                        }
                        break;
                    default:
                        throw KSException("Array cannot contain collections");
                        break;
                    }
                    value = arr;
                }
                break;
                }
                break;
            }
            break;
            case KSType::List:
            {
                switch (type) {
                default:
                    throw KSException(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::Dictionary:
                {
                    KSList list;
                    for (auto&& item : getDictionary()) {
                        list.push_back(item.second);
                    }
                    value = list;
                }
                break;
                case KSType::Class:
                    value = KSList({ make_shared<KSValue>(value, type) });
                    break;
                }
            }
            break;
            case KSType::Dictionary:
            {
                KSDictionary dict;
                for (auto&& item : getClass()->variables) {
                    dict[std::hash<string>()(item.first) ^ typeHashBits(KSType::String)] = item.second;
                }
            }
            }

        }
        type = newType;
    }
}
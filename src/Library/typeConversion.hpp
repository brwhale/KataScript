#pragma once

#include "types.hpp"
#include "value.hpp"

namespace KataScript {
    template <typename T>
    vector<T>& Array::getStdVector() {
        return get<vector<T>>(value);
    }

    template <typename T>
    vector<T>& Value::getStdVector() {
        return get<Array>(value).getStdVector<T>();
    }

    void ArrayMember::setValue(const ValueRef& val) {
        auto& arr = arrayRef->getArray();
        val->hardconvert(arr.getType());
        switch (arr.getType()) {
        case Type::Int:
            get<vector<Int>>(arr.value)[index] = val->getInt();
            break;
        case Type::Float:
            get<vector<Float>>(arr.value)[index] = val->getFloat();
            break;
        case Type::Vec3:
            get<vector<vec3>>(arr.value)[index] = val->getVec3();
            break;
        case Type::Function:
            get<vector<FunctionRef>>(arr.value)[index] = val->getFunction();
            break;
        case Type::UserPointer:
            get<vector<UserPointer>>(arr.value)[index] = val->getPointer();
            break;
        case Type::String:
            get<vector<string>>(arr.value)[index] = val->getString();
            break;
        default:
            throw Exception("Attempting to set array of illegal type");
            break;
        }
    }

    ValueRef ArrayMember::getValue() const {
        auto& arr = arrayRef->getArray();
        if (index < 0 || index >= (Int)arr.size()) {
            throw Exception("Out of bounds array access index "s + std::to_string(index) + ", array length " + std::to_string(arr.size()));
        }
        switch (arr.getType()) {
        case Type::Int:
            return make_shared<Value>(get<vector<Int>>(arr.value)[index]);
            break;
        case Type::Float:
            return make_shared<Value>(get<vector<Float>>(arr.value)[index]);
            break;
        case Type::Vec3:
            return make_shared<Value>(get<vector<vec3>>(arr.value)[index]);
            break;
        case Type::Function:
            return make_shared<Value>(get<vector<FunctionRef>>(arr.value)[index]);
            break;
        case Type::UserPointer:
            return make_shared<Value>(get<vector<UserPointer>>(arr.value)[index]);
            break;
        case Type::String:
            return make_shared<Value>(get<vector<string>>(arr.value)[index]);
            break;
        default:
            throw Exception("Attempting to access array of illegal type");
            break;
        }
    }

    Class::Class(const Class& o) : name(o.name), functionScope(o.functionScope) {
        for (auto&& v : o.variables) {
            variables[v.first] = make_shared<Value>(v.second->value);
        }
    }

    Class::Class(const ScopeRef& o) : name(o->name), functionScope(o) {
        for (auto&& v : o->variables) {
            variables[v.first] = make_shared<Value>(v.second->value);
        }
    }

    Class::~Class() {
        auto iter = functionScope->functions.find("~"s + name);
        if (iter != functionScope->functions.end()) {
            functionScope->host->callFunction(iter->second, functionScope, {}, this);
        }
    }

    size_t Value::getHash() {
        size_t hash = 0;
        switch (getType()) {
        default: break;
        case Type::Int:
            hash = (size_t)getInt();
            break;
        case Type::Float:
            hash = std::hash<Float>{}(getFloat());
            break;
        case Type::Vec3:
            hash = std::hash<float>{}(getVec3().x) ^ std::hash<float>{}(getVec3().y) ^ std::hash<float>{}(getVec3().z);
            break;
        case Type::Function:
            hash = std::hash<size_t>{}((size_t)getFunction().get());
            break;
        case Type::String:
            hash = std::hash<string>{}(getString());
            break;
        }
        return hash ^ typeHashBits(getType());
    }

    // convert this value up to the newType
    void Value::upconvert(Type newType) {
        if (newType > getType()) {
            switch (newType) {
            default:
                throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                break;
            case Type::Int:
                value = Int(0);
                break;
            case Type::Float:
                switch (getType()) {
                default:
                    throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                    break;
                case Type::Null:
                    value = 0.f;
                    break;
                case Type::Int:
                    value = (Float)getInt();
                    break;
                }
                break;

            case Type::Vec3:
                switch (getType()) {
                default:
                    throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                    break;
                case Type::Null:
                    value = vec3();
                    break;
                case Type::Int:
                    value = vec3((float)getInt());
                    break;
                case Type::Float:
                    value = vec3((float)getFloat());
                    break;
                }
                break;
            case Type::String:
                switch (getType()) {
                default:
                    throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                    break;
                case Type::Null:
                    value = "null"s;
                    break;
                case Type::Int:
                    value = std::to_string(getInt());
                    break;
                case Type::Float:
                    value = std::to_string(getFloat());
                    break;
                case Type::Vec3:
                {
                    auto& vec = getVec3();
                    value = std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z);
                    break;
                }
                case Type::Function:
                    value = getFunction()->name;
                    break;
                }
                break;
            case Type::Array:
                switch (getType()) {
                default:
                    throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                    break;
                case Type::Null:
                    value = Array();
                    getArray().push_back(Int(0));
                    break;
                case Type::Int:
                    value = Array(vector<Int>{ getInt() });
                    break;
                case Type::Float:
                    value = Array(vector<Float>{ getFloat() });
                    break;
                case Type::Vec3:
                    value = Array(vector<vec3>{ getVec3() });
                    break;
                case Type::String:
                {
                    auto str = getString();
                    value = Array(vector<string>{ });
                    auto& arry = getStdVector<string>();
                    for (auto&& ch : str) {
                        arry.push_back(""s + ch);
                    }
                }
                break;
                }
                break;
            case Type::List:
                switch (getType()) {
                default:
                    throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                    break;
                case Type::Null:
                case Type::Int:
                case Type::Float:
                case Type::Vec3:
                    value = List({ make_shared<Value>(value) });
                    break;
                case Type::String:
                {
                    auto str = getString();
                    value = List();
                    auto& list = getList();
                    for (auto&& ch : str) {
                        list.push_back(make_shared<Value>(""s + ch));
                    }
                }
                break;
                case Type::Array:
                    Array arr = getArray();
                    value = List();
                    auto& list = getList();
                    switch (arr.getType()) {
                    case Type::Int:
                        for (auto&& item : get<vector<Int>>(arr.value)) {
                            list.push_back(make_shared<Value>(item));
                        }
                        break;
                    case Type::Float:
                        for (auto&& item : get<vector<Float>>(arr.value)) {
                            list.push_back(make_shared<Value>(item));
                        }
                        break;
                    case Type::Vec3:
                        for (auto&& item : get<vector<vec3>>(arr.value)) {
                            list.push_back(make_shared<Value>(item));
                        }
                        break;
                    case Type::String:
                        for (auto&& item : get<vector<string>>(arr.value)) {
                            list.push_back(make_shared<Value>(item));
                        }
                        break;
                    default:
                        throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                        break;
                    }
                    break;
                }
                break;
            case Type::Dictionary:
                switch (getType()) {
                default:
                    throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                    break;
                case Type::Null:
                case Type::Int:
                case Type::Float:
                case Type::Vec3:
                case Type::String:
                    value = make_shared<Dictionary>();
                    break;
                case Type::Array:
                {
                    Array arr = getArray();
                    value = make_shared<Dictionary>();
                    auto hashbits = typeHashBits(Type::Int);
                    auto& dict = getDictionary();
                    size_t index = 0;
                    switch (arr.getType()) {
                    case Type::Int:
                        for (auto&& item : get<vector<Int>>(arr.value)) {
                            (*dict)[index++ ^ hashbits] = make_shared<Value>(item);
                        }
                        break;
                    case Type::Float:
                        for (auto&& item : get<vector<Float>>(arr.value)) {
                            (*dict)[index++ ^ hashbits] = make_shared<Value>(item);
                        }
                        break;
                    case Type::Vec3:
                        for (auto&& item : get<vector<vec3>>(arr.value)) {
                            (*dict)[index++ ^ hashbits] = make_shared<Value>(item);
                        }
                        break;
                    case Type::String:
                        for (auto&& item : get<vector<string>>(arr.value)) {
                            (*dict)[index++ ^ hashbits] = make_shared<Value>(item);
                        }
                        break;
                    default:
                        throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                        break;
                    }
                }
                break;
                case Type::List:
                {
                    auto hashbits = typeHashBits(Type::Int);
                    List list = getList();
                    value = make_shared<Dictionary>();
                    auto& dict = getDictionary();
                    size_t index = 0;
                    for (auto&& item : list) {
                        (*dict)[index++ ^ hashbits] = item;
                    }
                }
                break;
                }
                break;
            }
        }
    }

    // convert this value to the newType even if it's a downcast
    void Value::hardconvert(Type newType) {
        if (newType >= getType()) {
            upconvert(newType);
        } else {
            switch (newType) {
            default:
                throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                break;
            case Type::Null:
                value = Int(0);
                break;
            case Type::Int:
                switch (getType()) {
                default:
                    break;
                case Type::Float:
                    value = (Int)getFloat();
                    break;
                case Type::String: {
                    auto [val, valid] = fromChars(getString());
                    if (valid) {
                        value = (Int)val;
                    } else {
                        value = Null();
                    }
                }
                    break;
                case Type::Array:
                    value = (Int)getArray().size();
                    break;
                case Type::List:
                    value = (Int)getList().size();
                    break;
                }
                break;
            case Type::Float:
                switch (getType()) {
                default:
                    throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                    break;
                case Type::String: {
                    auto [val, valid] = fromChars(getString());
                    if (valid) {
                        value = (Float)val;
                    } else {
                        value = Null();
                    }
                }
                    break;
                case Type::Array:
                    value = (Float)getArray().size();
                    break;
                case Type::List:
                    value = (Float)getList().size();
                    break;
                }
                break;
            case Type::String:
                switch (getType()) {
                default:
                    throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                    break;
                case Type::Array:
                {
                    string newval;
                    auto& arr = getArray();
                    switch (arr.getType()) {
                    case Type::Int:
                        for (auto&& item : get<vector<Int>>(arr.value)) {
                            newval += Value(item).getPrintString() + ", ";
                        }
                        break;
                    case Type::Float:
                        for (auto&& item : get<vector<Float>>(arr.value)) {
                            newval += Value(item).getPrintString() + ", ";
                        }
                        break;
                    case Type::Vec3:
                        for (auto&& item : get<vector<vec3>>(arr.value)) {
                            newval += Value(item).getPrintString() + ", ";
                        }
                        break;
                    case Type::String:
                        for (auto&& item : get<vector<string>>(arr.value)) {
                            newval += Value(item).getPrintString() + ", ";
                        }
                        break;
                    default:
                        throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                        break;
                    }
                    if (arr.size()) {
                        newval.pop_back();
                        newval.pop_back();
                    }
                    value = newval;
                }
                break;
                case Type::ArrayMember:
                    value = getArrayMember().getValue()->getPrintString();
                    break;
                case Type::List:
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
                case Type::Dictionary:
                {
                    string newval;
                    auto& dict = getDictionary();
                    for (auto&& val : *dict) {
                        newval += "`"s + std::to_string(val.first)  + ": " + val.second->getPrintString() + "`, ";
                    }
                    if (newval.size()) {
                        newval.pop_back();
                        newval.pop_back();
                    }
                    value = newval;
                }
                break;
                case Type::Class:
                {
                    auto& strct = getClass();
                    string newval = strct->name + ":\n"s;
                    for (auto&& val : strct->variables) {
                        newval += "`"s + val.first + ": " + val.second->getPrintString() + "`\n";
                    }
                    value = newval;
                }
                break;
                }
                break;
            case Type::Array:
            {
                switch (getType()) {
                default:
                    throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                    break;
                case Type::Dictionary:
                {
                    Array arr;
                    auto dict = getDictionary();
                    auto listType = dict->begin()->second->getType();
                    switch (listType) {
                    case Type::Int:
                        arr = Array(vector<Int>{});
                        for (auto&& item : *dict) {
                            if (item.second->getType() == listType) {
                                arr.push_back(item.second->getInt());
                            }
                        }
                        break;
                    case Type::Float:
                        arr = Array(vector<Float>{});
                        for (auto&& item : *dict) {
                            if (item.second->getType() == listType) {
                                arr.push_back(item.second->getFloat());
                            }
                        }
                        break;
                    case Type::Vec3:
                        arr = Array(vector<vec3>{});
                        for (auto&& item : *dict) {
                            if (item.second->getType() == listType) {
                                arr.push_back(item.second->getVec3());
                            }
                        }
                        break;
                    case Type::Function:
                        arr = Array(vector<FunctionRef>{});
                        for (auto&& item : *dict) {
                            if (item.second->getType() == listType) {
                                arr.push_back(item.second->getFunction());
                            }
                        }
                        break;
                    case Type::String:
                        arr = Array(vector<string>{});
                        for (auto&& item : *dict) {
                            if (item.second->getType() == listType) {
                                arr.push_back(item.second->getString());
                            }
                        }
                        break;
                    default:
                        throw Exception("Array cannot contain collections");
                        break;
                    }
                    value = arr;
                }
                break;
                case Type::List:
                {
                    auto list = getList();
                    auto listType = list[0]->getType();
                    Array arr;
                    switch (listType) {
                    case Type::Null:
                        arr = Array(vector<Int>{});
                        break;
                    case Type::Int:
                        arr = Array(vector<Int>{});
                        for (auto&& item : list) {
                            if (item->getType() == listType) {
                                arr.push_back(item->getInt());
                            }
                        }
                        break;
                    case Type::Float:
                        arr = Array(vector<Float>{});
                        for (auto&& item : list) {
                            if (item->getType() == listType) {
                                arr.push_back(item->getFloat());
                            }
                        }
                        break;
                    case Type::Vec3:
                        arr = Array(vector<vec3>{});
                        for (auto&& item : list) {
                            if (item->getType() == listType) {
                                arr.push_back(item->getVec3());
                            }
                        }
                        break;
                    case Type::Function:
                        arr = Array(vector<FunctionRef>{});
                        for (auto&& item : list) {
                            if (item->getType() == listType) {
                                arr.push_back(item->getFunction());
                            }
                        }
                        break;
                    case Type::String:
                        arr = Array(vector<string>{});
                        for (auto&& item : list) {
                            if (item->getType() == listType) {
                                arr.push_back(item->getString());
                            }
                        }
                        break;
                    default:
                        throw Exception("Array cannot contain collections");
                        break;
                    }
                    value = arr;
                }
                break;
                }
                break;
            }
            break;
            case Type::List:
            {
                switch (getType()) {
                default:
                    throw Exception("Conversion not defined for types `"s + getTypeName(getType()) + "` to `" + getTypeName(newType) + "`");
                    break;
                case Type::Dictionary:
                {
                    List list;
                    for (auto&& item : *getDictionary()) {
                        list.push_back(item.second);
                    }
                    value = list;
                }
                break;
                case Type::Class:
                    value = List({ make_shared<Value>(value) });
                    break;
                }
            }
            break;
            case Type::Dictionary:
            {
                Dictionary dict;
                for (auto&& item : getClass()->variables) {
                    dict[std::hash<string>()(item.first) ^ typeHashBits(Type::String)] = item.second;
                }
            }
            }
        }
    }
}
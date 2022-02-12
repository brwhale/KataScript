// copyright Garrett Skelton 2020
// MIT license
#pragma once

#include <cstdarg>
#include <iostream>
#include <cmath>
#include <chrono>
#include <fstream>

#include "stringUtils.hpp"
#include "types.hpp"
#include "expressions.hpp"

namespace KataScript {
	struct KSScope {
		// this is the main storage object for all functions and variables
		string name;
		KSScopeRef parent;
        bool classScope = false;
		unordered_map<string, KSValueRef> variables;
		unordered_map<string, KSScopeRef> scopes; 
		unordered_map<string, KSFunctionRef> functions;
		KSScope(const string& name_, KSScopeRef scope) : name(name_), parent(scope) {}
        KSScope(const KSScope& o) : name(o.name), parent(o.parent), scopes(o.scopes), functions(o.functions) {
            for (auto&& v : o.variables) {
                variables[v.first] = make_shared<KSValue>(v.second->value, v.second->type);
            }
        }
	};

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
		KSFunctionRef& newFunction(const string& name, const vector<string>& argNames, const vector<KSExpressionRef>& body);
		KSValueRef getValue(KSExpressionRef expr);
		void newScope(const string& name);
        KSScopeRef resolveScope(const string& name);
		void closeCurrentScope();
		bool closeCurrentExpression();
		KSValueRef callFunction(const string& name, const KSList& args);
		KSValueRef callFunction(const KSFunctionRef fnc, const KSList& args);
	public:
		KSFunctionRef& newFunction(const string& name, const KSLambda& lam);		
		template <typename ... Ts>
		KSValueRef callFunction(const KSFunctionRef fnc, Ts...args) {
			KSList argsList = { make_shared<KSValue>(args)... };
			return callFunction(fnc, argsList);
		}
		KSValueRef& resolveVariable(const string& name, KSScopeRef = nullptr);
		KSFunctionRef resolveFunction(const string& name);
		bool readLine(const string& text);
		bool evaluate(const string& script);
        bool evaluateFile(const string& path);
		void clearState();
		KataScriptInterpreter();
	};

#ifdef KATASCRIPT_IMPL
	// implementations
	template <typename T>
	vector<T>& KSValue::getStdVector() {
		return get<vector<T>>(get<KSArray>(value).value);
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
                    value = KSList({make_shared<KSValue>(value, type)});
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

	// tokenizer special characters
	const string WhitespaceChars = " \t\n"s;
	const string GrammarChars = " \t\n,.(){}[];+-/*%<>=!&|\""s;
	const string MultiCharTokenStartChars = "+-/*<>=!&|"s;
	const string NumericChars = "0123456789."s;
    const string NumericStartChars = "0123456789.-"s;

	vector<string> KSTokenize(const string& input) {
		bool exitFromComment = false;
		vector<string> ret;
		if (input.empty()) return ret;
		size_t pos = 0;
		size_t lpos = 0;
		while ((pos = input.find_first_of(GrammarChars, lpos)) != string::npos) {
			size_t len = pos - lpos;
            if (input[pos] == '.' && pos + 1 < input.size() && contains(NumericChars, input[pos+1])) {
                pos = input.find_first_of(GrammarChars, pos + 1);
                ret.push_back(input.substr(lpos, pos - lpos));
                lpos = pos;
                continue;
            }
			if (len) {
				ret.push_back(input.substr(lpos, pos - lpos));
				lpos = pos;
			} else {
				if (input[pos] == '\"' && pos > 0 && input[pos-1] != '\\') {
					auto testpos = lpos + 1;
					auto unescaped = "\""s;
					bool loop = true;
					while (loop) {						
						pos = input.find('\"', testpos);
						if (pos == string::npos) {
							throw KSException(stringformat("Quote mismatch at %s", 
                                input.substr(lpos, input.size() - lpos).c_str()));
						}
						loop = (input[pos - 1] == '\\');
						unescaped += input.substr(testpos, ++pos - testpos);
						testpos = pos;
						
						if (loop) {
							unescaped.pop_back();
							unescaped.pop_back();
							unescaped += '\"';
						}
					}
					ret.push_back(replaceWhitespaceLiterals(unescaped));
					lpos = pos;
					continue;
				}
			}
            if (input[pos] == '-' && contains(NumericChars, input[pos + 1]) 
                && (ret.size() == 0 || contains(MultiCharTokenStartChars, ret.back().back()))) {
                pos = input.find_first_of(GrammarChars, pos + 1);
                if (input[pos] == '.' && pos + 1 < input.size() && contains(NumericChars, input[pos + 1])) {
                    pos = input.find_first_of(GrammarChars, pos + 1);
                }
                ret.push_back(input.substr(lpos, pos - lpos));
                lpos = pos;
            } else if (!contains(WhitespaceChars, input[pos])) {
				auto stride = 1;
				if (contains(MultiCharTokenStartChars, input[pos]) && contains(MultiCharTokenStartChars, input[pos + 1])) {
					if (input[pos] == '/' && input[pos + 1] == '/') {
						exitFromComment = true;
						break;
					}
					++stride;
				}
				ret.push_back(input.substr(lpos, stride));
				lpos += stride;
			} else {
				++lpos;
			}
		}
		if (!exitFromComment && lpos < input.length()) {
			ret.push_back(input.substr(lpos, input.length()));
		}
		return ret;
	}

	// functions for figuring out the type of token

	bool isStringLiteral(const string& test) {
		return (test.size() > 1 && test[0] == '"');
	}

	bool isFloatLiteral(const string& test) {
		return (test.size() > 1 && contains(test, '.'));
	}

	bool isVarOrFuncToken(const string& test) {
		return (test.size() > 0 && !contains(NumericStartChars, test[0]));
	}

	bool isMathOperator(const string& test) {
		if (test.size() == 1) {
			return contains("+-*/%<>=!"s, test[0]);
		}
		if (test.size() == 2) {
			return contains("=+-&|"s, test[1]) && contains("<>=!+-/*&|"s, test[0]);
		}
		return false;
	}

    bool isMemberCall(const string& test) {
        if (test.size() == 1) {
            return '.' == test[0];
        }
        return false;
    }

	// scope control lets you have object lifetimes
	void KataScriptInterpreter::newScope(const string& name) {
		// if the scope exists we just use it as is
		auto iter = currentScope->scopes.find(name);
		if (iter != currentScope->scopes.end()) {
			currentScope = iter->second;
		} else {
			auto parent = currentScope;
			auto& newScope = currentScope->scopes[name];
			newScope = make_shared<KSScope>(name, parent);
			currentScope = newScope;
		}
	}

    KSScopeRef KataScriptInterpreter::resolveScope(const string& name) {
        // if the scope exists we just use it as is
        auto scope = currentScope;
        unordered_map<string, KSScopeRef>::iterator iter;
        while (scope->scopes.end() == (iter = scope->scopes.find(name)) && scope->parent) {
            scope = scope->parent;
        } 
        if (scope->scopes.end() != iter) {
            return iter->second;
        }
        throw KSException("Cannot resolve non-existant scope");
    }

	void KataScriptInterpreter::closeCurrentScope() {
		if (currentScope->parent) {
            if (currentScope->classScope) {
                currentScope = currentScope->parent;
            } else {
                auto name = currentScope->name;
                currentScope->functions.clear();
                currentScope->variables.clear();
                currentScope->scopes.clear();
                currentScope = currentScope->parent;
                currentScope->scopes.erase(name);
            }
		}
	}

	// call function by name
	KSValueRef KataScriptInterpreter::callFunction(const string& name, const KSList& args) {
		return callFunction(resolveFunction(name), args);
	}

    void each(KSExpressionRef collection, function<void(KSExpressionRef)> func) {
        func(collection);
        for (auto&& ex : *collection) {
            each(ex, func);            
        }
    }

	// call function by reference
	KSValueRef KataScriptInterpreter::callFunction(const KSFunctionRef fnc, const KSList& args) {
		if (fnc->subexpressions.size()) {
            auto oldscope = currentScope;
            // get function scope
            newScope(fnc->name);
			auto limit = min(args.size(), fnc->argNames.size());
            vector<string> newVars;
			for (size_t i = 0; i < limit; ++i) {
                auto& ref = currentScope->variables[fnc->argNames[i]];
                if (ref == nullptr) {
                    newVars.push_back(fnc->argNames[i]);
                }
                ref = args[i];
			}
        
			KSValueRef returnVal = nullptr;
			for (auto&& sub : fnc->subexpressions) {
				if (sub->type == KSExpressionType::Return) {
					returnVal = getValue(sub);
					break;
				} else{
					auto result = sub->consolidated(this);
                    if (result->type == KSExpressionType::Return) {
                        returnVal = get<KSValueRef>(result->expression);
                        break;
                    }
				}
			}

            if (fnc->type == KSFunctionType::CONSTRUCTOR) {
                for (auto&& vr : newVars) {
                    currentScope->variables.erase(vr);
                }
                returnVal = make_shared<KSValue>(make_shared<KSClass>(currentScope));
            }

            closeCurrentScope();
            currentScope = oldscope;
			return returnVal ? returnVal : make_shared<KSValue>();
		} else if (fnc->lambda) {
			return fnc->lambda(args);
        } else {
            //empty func
            return make_shared<KSValue>();
        }
	}

	KSFunctionRef& KataScriptInterpreter::newFunction(
        const string& name,
        const vector<string>& argNames,
        const vector<KSExpressionRef>& body
    ) {
        bool isConstructor = currentScope->classScope && currentScope->name == name && currentScope->parent;
        auto fnScope = isConstructor ? currentScope->parent : currentScope;
		auto& ref = fnScope->functions[name];
		ref = make_shared<KSFunction>(name, argNames, body);
        ref->type = isConstructor 
            ? KSFunctionType::CONSTRUCTOR 
            : currentScope->classScope
                ? KSFunctionType::MEMBER 
                : KSFunctionType::FREE;
		auto& funcvar = resolveVariable(name, fnScope);
		funcvar->type = KSType::Function;
		funcvar->value = ref;
		return ref;
	}

	KSFunctionRef& KataScriptInterpreter::newFunction(const string& name, const KSLambda& lam) {
		auto& ref = currentScope->functions[name];
		ref = make_shared<KSFunction>(name, lam);
		auto& funcvar = resolveVariable(name);
		funcvar->type = KSType::Function;
		funcvar->value = ref;
		return ref;
	}

	KSFunctionRef& KataScriptInterpreter::newLibraryFunction(
        const string& name,
        const KSLambda& lam,
        KSScopeRef scope) {
		auto oldScope = currentScope;
		currentScope = scope;
		auto& ref = newFunction(name, lam);
		currentScope = oldScope;
		return ref;
	}

	// name resolution for variables
	KSValueRef& KataScriptInterpreter::resolveVariable(const string& name, KSScopeRef scope) {
        if (currentClass) {
            auto iter = currentClass->variables.find(name);
            if (iter != currentClass->variables.end()) {
                return iter->second;
            }
        }
		if (!scope) {
			scope = currentScope;
		}
		auto initialScope = scope;
		while (scope) {            
			auto iter = scope->variables.find(name);
			if (iter != scope->variables.end()) {
				return iter->second;
			} else {
                scope = scope->parent;
			}
		}
		if (!scope) {
			for (auto m : modules) {
				auto iter = m->variables.find(name);
				if (iter != m->variables.end()) {
					return iter->second;
				}
			}
		}
		auto& varr = initialScope->variables[name];
		varr = make_shared<KSValue>();
		return varr;
	}

	// name lookup for callfunction api method
	KSFunctionRef KataScriptInterpreter::resolveFunction(const string& name) {
		auto scope = currentScope;
		while (scope) {
			auto iter = scope->functions.find(name);
			if (iter != scope->functions.end()) {
				return iter->second;
			} else {
				scope = scope->parent;
			}
		}
		return make_shared<KSFunction>(name);
	}

	// recursively build an expression tree from a list of tokens
	KSExpressionRef KataScriptInterpreter::getExpression(const vector<string>& strings) {
		KSExpressionRef root = nullptr;
		size_t i = 0;
		while (i < strings.size()) {
			if (isMathOperator(strings[i])) {
				auto prev = root;
				root = make_shared<KSExpression>(resolveVariable(strings[i], modules[0]));
				auto curr = prev;
				if (curr) {
					// find operations of lesser precedence
					if (curr->type == KSExpressionType::FunctionCall) {
						auto curfunc = get<KSFunctionExpression>(curr->expression).function->getFunction();
						auto newfunc = get<KSFunctionExpression>(root->expression).function->getFunction();
						if (curfunc && (int)curfunc->opPrecedence < (int)newfunc->opPrecedence) {
							while (get<KSFunctionExpression>(curr->expression).subexpressions.back()->type == KSExpressionType::FunctionCall) {
								curfunc = get<KSFunctionExpression>(get<KSFunctionExpression>(curr->expression).subexpressions.back()->expression).function->getFunction();
								if (curfunc && (int)curfunc->opPrecedence < (int)newfunc->opPrecedence) {
									curr = get<KSFunctionExpression>(curr->expression).subexpressions.back();
								} else {
									break;
								}
							}
							// swap values around to correct the otherwise incorect order of operations
                            get<KSFunctionExpression>(root->expression).subexpressions.push_back(get<KSFunctionExpression>(curr->expression).subexpressions.back());
                            get<KSFunctionExpression>(curr->expression).subexpressions.pop_back();
							vector<string> minisub = { strings[++i] };
							auto j = i + 1;
							string checkstr;
							if (strings[i] == "[" || strings[i] == "(") {
								checkstr = strings[i];
							} 
							if (checkstr.size() == 0 && (strings.size() > j && (strings[j] == "[" || strings[j] == "("))) {
								checkstr = strings[++i];
								minisub.push_back(strings[i]);
							}
							if (checkstr.size()) {
								auto endstr = checkstr == "[" ? "]"s : ")"s;
								int nestLayers = 1;
								while (nestLayers > 0 && ++i < strings.size()) {
									if (strings[i] == endstr) {
										--nestLayers;
									} else if (strings[i] == checkstr) {
										++nestLayers;		
									} 
									minisub.push_back(strings[i]);
                                    if (nestLayers == 0) {
                                        if (i+1 < strings.size() && (strings[i + 1] == "[" || strings[i + 1] == "(")) {
                                            ++nestLayers;
                                            checkstr = strings[++i];
                                            endstr = checkstr == "[" ? "]"s : ")"s;
                                        }
                                    }
								}
							}
                            get<KSFunctionExpression>(root->expression).subexpressions.push_back(getExpression(move(minisub)));
                            get<KSFunctionExpression>(curr->expression).subexpressions.push_back(root);
							root = prev;
						} else {
                            get<KSFunctionExpression>(root->expression).subexpressions.push_back(curr);
						}
					} else {
                        get<KSFunctionExpression>(root->expression).subexpressions.push_back(curr);
					}
				}
			} else if (isStringLiteral(strings[i])) {
				// trim quotation marks
				auto val = strings[i].substr(1, strings[i].size() - 2);
                auto newExpr = make_shared<KSExpression>(make_shared<KSValue>(val), KSExpressionType::Value);
				if (root) {
                    get<KSFunctionExpression>(root->expression).subexpressions.push_back(newExpr);
				} else {
					root = newExpr;
				}
            } else if (strings[i] == "(" || strings[i] == "[" || isVarOrFuncToken(strings[i])) {
                if (strings[i] == "(" || i + 2 < strings.size() && strings[i + 1] == "(") {
                    // function
                    KSExpressionRef cur = nullptr;
                    if (strings[i] == "(") {
                        if (root) {
                            if (root->type == KSExpressionType::FunctionCall
                                && (get<KSFunctionExpression>(root->expression).function->getFunction()->opPrecedence == KSOperatorPrecedence::func)) {
                                cur = make_shared<KSExpression>(make_shared<KSValue>());
                                get<KSFunctionExpression>(cur->expression).subexpressions.push_back(root);
                                root = cur;
                            } else {
                                get<KSFunctionExpression>(root->expression).subexpressions.push_back(make_shared<KSExpression>(resolveVariable("identity", modules[0])));
                                cur = get<KSFunctionExpression>(root->expression).subexpressions.back();
                            }
                        } else {
                            root = make_shared<KSExpression>(resolveVariable("identity", modules[0]));
                            cur = root;
                        }
                    } else {
                        auto funccall = make_shared<KSExpression>(resolveVariable("applyfunction", modules[0]));
                        if (root) {
                            get<KSFunctionExpression>(root->expression).subexpressions.push_back(funccall);
                            cur = get<KSFunctionExpression>(root->expression).subexpressions.back();
                        } else {
                            root = funccall;
                            cur = root;
                        }                        
                        get<KSFunctionExpression>(cur->expression).subexpressions.push_back(make_shared<KSExpression>(KSResolveVar(strings[i])));
                        ++i;
                    }
                    vector<string> minisub;
                    int nestLayers = 1;
                    while (nestLayers > 0 && ++i < strings.size()) {
                        if (nestLayers == 1 && strings[i] == ",") {
                            if (minisub.size()) {
                                get<KSFunctionExpression>(cur->expression).subexpressions.push_back(getExpression(move(minisub)));
                                minisub.clear();
                            }
                        } else if (strings[i] == ")" || strings[i] == "]") {
                            if (--nestLayers > 0) {
                                minisub.push_back(strings[i]);
                            } else {
                                if (minisub.size()) {
                                    get<KSFunctionExpression>(cur->expression).subexpressions.push_back(getExpression(move(minisub)));
                                    minisub.clear();
                                }
                            }
                        } else if (strings[i] == "(" || strings[i] == "[" || !(strings[i].size() == 1 && contains("+-*%/"s, strings[i][0])) && i + 2 < strings.size() && strings[i + 1] == "(") {
                            ++nestLayers;
                            if (strings[i] == "(") {
                                minisub.push_back(strings[i]);
                            } else {
                                minisub.push_back(strings[i]);
                                ++i;
                                if (strings[i] == ")" || strings[i] == "]") {
                                    --nestLayers;
                                }
                                if (nestLayers > 0) {
                                    minisub.push_back(strings[i]);
                                }
                            }
                        } else {
                            minisub.push_back(strings[i]);
                        }
                    }
                    
                } else if (strings[i] == "[" || i + 2 < strings.size() && strings[i + 1] == "[") {
                    // list
                    bool indexOfIndex = i > 0 && (strings[i - 1] == "]" || strings[i - 1] == ")" || strings[i - 1].back() == '\"') || (i > 1 && strings[i-2] == ".");
                    KSExpressionRef cur = nullptr;
                    if (!indexOfIndex && strings[i] == "[") {
                        // list literal / collection literal
                        if (root) {
                            get<KSFunctionExpression>(root->expression).subexpressions.push_back(
                                make_shared<KSExpression>(make_shared<KSValue>(KSList()), KSExpressionType::Value));
                            cur = get<KSFunctionExpression>(root->expression).subexpressions.back();
                        } else {
                            root = make_shared<KSExpression>(make_shared<KSValue>(KSList()), KSExpressionType::Value);
                            cur = root;
                        }
                        vector<string> minisub;
                        int nestLayers = 1;
                        while (nestLayers > 0 && ++i < strings.size()) {
                            if (nestLayers == 1 && strings[i] == ",") {
                                if (minisub.size()) {
                                    auto val = *getValue(move(minisub));
                                    get<KSValueRef>(cur->expression)->getList().push_back(make_shared<KSValue>(val.value, val.type));
                                    minisub.clear();
                                }
                            } else if (strings[i] == "]" || strings[i] == ")") {
                                if (--nestLayers > 0) {
                                    minisub.push_back(strings[i]);
                                } else {
                                    if (minisub.size()) {
                                        auto val = *getValue(move(minisub));
                                        get<KSValueRef>(cur->expression)->getList().push_back(make_shared<KSValue>(val.value, val.type));
                                        minisub.clear();
                                    }
                                }
                            } else if (strings[i] == "[" || strings[i] == "(") {
                                ++nestLayers;
                                minisub.push_back(strings[i]);
                            } else {
                                minisub.push_back(strings[i]);
                            }
                        }
                        auto& list = get<KSValueRef>(cur->expression)->getList();
                        if (list.size()) {
                            bool canBeArray = true;
                            auto type = list[0]->type;
                            for (auto& val : list) {
                                if (val->type == KSType::Null || val->type != type || (int)val->type >= (int)KSType::Array) {
                                    canBeArray = false;
                                    break;
                                }
                            }
                            if (canBeArray) {
                                get<KSValueRef>(cur->expression)->hardconvert(KSType::Array);
                            }
                        }
                    } else {
                        // list access
                        auto indexexpr = make_shared<KSExpression>(resolveVariable("listindex", modules[0]));
                        if (indexOfIndex) {
                            cur = root;
                            auto parent = root;
                            while (cur->type == KSExpressionType::FunctionCall && get<KSFunctionExpression>(cur->expression).function->getFunction()->opPrecedence != KSOperatorPrecedence::func) {
                                parent = cur;
                                cur = get<KSFunctionExpression>(cur->expression).subexpressions.back();
                            }
                            get<KSFunctionExpression>(indexexpr->expression).subexpressions.push_back(cur);
                            if (cur == root) {
                                root = indexexpr;
                                cur = indexexpr;
                            } else {
                                get<KSFunctionExpression>(parent->expression).subexpressions.pop_back();
                                get<KSFunctionExpression>(parent->expression).subexpressions.push_back(indexexpr);
                                cur = get<KSFunctionExpression>(parent->expression).subexpressions.back();
                            }
                        } else {
                            if (root) {
                                get<KSFunctionExpression>(root->expression).subexpressions.push_back(indexexpr);
                                cur = get<KSFunctionExpression>(root->expression).subexpressions.back();
                            } else {
                                root = indexexpr;
                                cur = root;
                            }
                            get<KSFunctionExpression>(cur->expression).subexpressions.push_back(make_shared<KSExpression>(KSResolveVar(strings[i])));
                            ++i;
                        }

                        vector<string> minisub;
                        int nestLayers = 1;
                        while (nestLayers > 0 && ++i < strings.size()) {
                            if (strings[i] == "]") {
                                if (--nestLayers > 0) {
                                    minisub.push_back(strings[i]);
                                } else {
                                    if (minisub.size()) {
                                        get<KSFunctionExpression>(cur->expression).subexpressions.push_back(getExpression(move(minisub)));
                                        minisub.clear();
                                    }
                                }
                            } else if (strings[i] == "[") {
                                ++nestLayers;
                                minisub.push_back(strings[i]);
                            } else {
                                minisub.push_back(strings[i]);
                            }
                        }
                    }
                } else {
                    // variable
                    KSExpressionRef newExpr;
                    if (strings[i] == "true") {
                        newExpr = make_shared<KSExpression>(make_shared<KSValue>(KSInt(1)), KSExpressionType::Value);
                    } else if (strings[i] == "false") {
                        newExpr = make_shared<KSExpression>(make_shared<KSValue>(KSInt(0)), KSExpressionType::Value);
                    } else if (strings[i] == "null") {
                        newExpr = make_shared<KSExpression>(make_shared<KSValue>(), KSExpressionType::Value);
                    } else {
                        newExpr = make_shared<KSExpression>(KSResolveVar(strings[i]));
                    }

                    if (root) {
                        if (root->type == KSExpressionType::ResolveVar) {
                            throw KSException(stringformat("Syntax Error: unexpected series of values at %s, possible missing `,`", strings[i].c_str()));
                        }
                        get<KSFunctionExpression>(root->expression).subexpressions.push_back(newExpr);
                    } else {
                        root = newExpr;
                    }
                }
            } else if (isMemberCall(strings[i])) {
                // member var
                bool isfunc = strings.size() > i + 2 && strings[i + 2] == "("s;
                auto memberExpr = make_shared<KSExpression>(resolveVariable(isfunc?"applyfunction"s:"listindex"s, modules[0]));
                auto& membExpr = get<KSFunctionExpression>(memberExpr->expression);
                KSExpressionRef argsInsert;
                if (root->type == KSExpressionType::FunctionCall && get<KSFunctionExpression>(root->expression).subexpressions.size()) {
                    auto& rootExpr = get<KSFunctionExpression>(root->expression);
                    if (isfunc) {
                        membExpr.subexpressions.push_back(make_shared<KSExpression>(make_shared<KSValue>(strings[++i]), KSExpressionType::Value));
                        membExpr.subexpressions.push_back(rootExpr.subexpressions.back());
                    } else {
                        membExpr.subexpressions.push_back(rootExpr.subexpressions.back());
                        membExpr.subexpressions.push_back(make_shared<KSExpression>(make_shared<KSValue>(strings[++i]), KSExpressionType::Value));
                    }
                    rootExpr.subexpressions.pop_back();
                    rootExpr.subexpressions.push_back(memberExpr);
                    argsInsert = rootExpr.subexpressions.back();
                } else {   
                    if (isfunc) {
                        membExpr.subexpressions.push_back(make_shared<KSExpression>(make_shared<KSValue>(strings[++i]), KSExpressionType::Value));
                        membExpr.subexpressions.push_back(root);
                    } else {
                        membExpr.subexpressions.push_back(root);
                        membExpr.subexpressions.push_back(make_shared<KSExpression>(make_shared<KSValue>(strings[++i]), KSExpressionType::Value));
                    }                    
                    root = memberExpr;
                    argsInsert = root;
                }
                
                if (isfunc) {
                    bool addedArgs = false;
                    auto previ = i;
                    ++i;
                    vector<string> minisub;
                    int nestLayers = 1;
                    while (nestLayers > 0 && ++i < strings.size()) {
                        if (nestLayers == 1 && strings[i] == ",") {
                            if (minisub.size()) {
                                get<KSFunctionExpression>(argsInsert->expression).subexpressions.push_back(getExpression(move(minisub)));
                                minisub.clear();
                                addedArgs = true;
                            }
                        } else if (strings[i] == ")") {
                            if (--nestLayers > 0) {
                                minisub.push_back(strings[i]);
                            } else {
                                if (minisub.size()) {
                                    get<KSFunctionExpression>(argsInsert->expression).subexpressions.push_back(getExpression(move(minisub)));
                                    minisub.clear();
                                    addedArgs = true;
                                }
                            }
                        } else if ( strings[i] == "(") {
                            ++nestLayers;
                            minisub.push_back(strings[i]);
                        } else {
                            minisub.push_back(strings[i]);
                        }
                    }
                    if (!addedArgs) {
                        i = previ;
                    }
                }
			} else {
				// number
				auto val = fromChars(strings[i]);
                bool isFloat = contains(strings[i], '.');
                auto newExpr = make_shared<KSExpression>(KSValueRef(isFloat ? new KSValue((KSFloat)val) : new KSValue((KSInt)val)), KSExpressionType::Value);
				if (root) {
                    get<KSFunctionExpression>(root->expression).subexpressions.push_back(newExpr);
				} else {
					root = newExpr;
				}
			}
			++i;
		}

		return root;
	}

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
            i->newScope("loop");
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
            i->newScope("loop");
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
                    i->newScope("ifelse");
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

	// general purpose clear to reset state machine for next statement
	void KataScriptInterpreter::clearParseStacks() {
		parseState = KSParseState::beginExpression;
		parseStrings.clear();
		outerNestLayer = 0;
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

	// parse one token at a time, uses the state machine
	void KataScriptInterpreter::parse(const string& token) {
        auto tempState = parseState;
		switch (parseState) {
		case KSParseState::beginExpression:
		{
			bool wasElse = false;
			bool closeScope = false;
			bool closedExpr = false;
            bool isEndCurlBracket = false;
			if (token == "func") {
				parseState = KSParseState::defineFunc;
			} else if (token == "var") {
                parseState = KSParseState::defineVar;
            } else if (token == "for" || token == "while") {
				parseState = KSParseState::loopCall;
				if (currentExpression) {
					auto newexpr = make_shared<KSExpression>(KSLoop(), currentExpression);
					currentExpression->push_back(newexpr);
					currentExpression = newexpr;
				} else {
					currentExpression = make_shared<KSExpression>(KSLoop());
				}
			} else if (token == "foreach") {
				parseState = KSParseState::forEach;
				if (currentExpression) {
					auto newexpr = make_shared<KSExpression>(KSForeach(), currentExpression);
					currentExpression->push_back(newexpr);
					currentExpression = newexpr;
				} else {
					currentExpression = make_shared<KSExpression>(KSForeach());
				}
			} else if (token == "if") {
				parseState = KSParseState::ifCall;
				if (currentExpression) {
					auto newexpr = make_shared<KSExpression>(KSIfElse(), currentExpression);
					currentExpression->push_back(newexpr);
					currentExpression = newexpr;
				} else {
					currentExpression = make_shared<KSExpression>(KSIfElse());
				}
            } else if (token == "else") {
                parseState = KSParseState::expectIfEnd;
                wasElse = true;
            } else if (token == "class") {
                parseState = KSParseState::defineClass;
			} else if (token == "{") {
                newScope("anon"s);
				clearParseStacks();
			} else if (token == "}") {
                wasElse = !currentExpression || currentExpression->type != KSExpressionType::IfElse;
                bool wasFreefunc = !(currentExpression && currentExpression->type == KSExpressionType::FunctionDef 
                    && get<KSFunctionExpression>(currentExpression->expression).function->getFunction()->type != KSFunctionType::FREE);
				closedExpr = closeCurrentExpression();
                if (wasFreefunc) {
                    closeCurrentScope();
                }
				closeScope = true;
                isEndCurlBracket = true;
			} else if (token == "return") {
				parseState = KSParseState::returnLine;
            } else if (token == "import") {
                parseState = KSParseState::importModule;
            } else if (token == ";") {
				clearParseStacks();
			} else {
				parseState = KSParseState::readLine;
				parseStrings.push_back(token);
			}
			if (!closedExpr && (closeScope && lastStatementClosedScope || (!lastStatementWasElse && !wasElse && lastTokenEndCurlBraket))) {
                bool wasIfExpr = currentExpression && currentExpression->type == KSExpressionType::IfElse;
				closeDanglingIfExpression();
                if (closeScope && wasIfExpr && currentExpression->type != KSExpressionType::IfElse) {
                    closeCurrentExpression();
                    closeScope = false;
                }
			}
            lastStatementClosedScope = closeScope;
            lastTokenEndCurlBraket = isEndCurlBracket;
            lastStatementWasElse = wasElse;
		}
		break;
		case KSParseState::loopCall:
			if (token == ")") {
				if (--outerNestLayer <= 0) {
					vector<vector<string>> exprs = {};
					exprs.push_back({});
					for (auto&& str : parseStrings) {
						if (str == ";") {
							exprs.push_back({});
						} else {
							exprs.back().push_back(move(str));
						}
					}
                    auto& loop = get<KSLoop>(currentExpression->expression);
					switch (exprs.size()) {
					case 1:
                        loop.testExpression = getExpression(move(exprs[0]));
						break;
					case 2:
						loop.testExpression = getExpression(move(exprs[0]));
						loop.iterateExpression = getExpression(move(exprs[1]));
						break;
					case 3: 
                    {
                        auto name = exprs[0].front();
                        exprs[0].erase(exprs[0].begin(), exprs[0].begin()+1);
                        loop.initExpression = make_shared<KSExpression>(KSDefineVar(name, getExpression(move(exprs[0]))));
                        loop.testExpression = getExpression(move(exprs[1]));
                        loop.iterateExpression = getExpression(move(exprs[2]));
                    }
						break;
					default:
						break;
					}

					clearParseStacks();
					outerNestLayer = 0;
				} else {
					parseStrings.push_back(token);
				}
			} else if (token == "(") {
				if (++outerNestLayer > 1) {
					parseStrings.push_back(token);
				}
			} else {
				parseStrings.push_back(token);
			}
			break;
		case KSParseState::forEach:
			if (token == ")") {
				if (--outerNestLayer <= 0) {
					vector<vector<string>> exprs = {};
					exprs.push_back({});
					for (auto&& str : parseStrings) {
						if (str == ";") {
							exprs.push_back({});
						} else {
							exprs.back().push_back(move(str));
						}
					}
					if (exprs.size() != 2) {
						clearParseStacks();
						throw KSException(stringformat("Syntax error, `foreach` requires 2 statements, %i statements supplied instead", (int)exprs.size()));
					}

					resolveVariable(exprs[0][0]);
					get<KSForeach>(currentExpression->expression).iterateName = exprs[0][0];
					get<KSForeach>(currentExpression->expression).listExpression = getExpression(exprs[1]);

					clearParseStacks();
					outerNestLayer = 0;
				} else {
					parseStrings.push_back(token);
				}
			} else if (token == "(") {
				if (++outerNestLayer > 1) {
					parseStrings.push_back(token);
				}
			} else {
				parseStrings.push_back(token);
			}
			break;
		case KSParseState::ifCall:			
			if (token == ")") {
				if (--outerNestLayer <= 0) {
					currentExpression->push_back(KSIf());
                    get<KSIfElse>(currentExpression->expression).back().testExpression = getExpression(move(parseStrings));
					clearParseStacks();
				} else {
					parseStrings.push_back(token);
				}
			} else if (token == "(") {
				if (++outerNestLayer > 1) {
					parseStrings.push_back(token);
				}
			} else {
				parseStrings.push_back(token);
			}
			break;
		case KSParseState::readLine:
            if (token == ";") {
                auto line = move(parseStrings);
                clearParseStacks();
                // we clear before evaluating lines so any exceptions can clear the offending code
				if (!currentExpression) {
					getValue(line);
				} else {
					currentExpression->push_back(getExpression(line));
				}
                
			} else {
				parseStrings.push_back(token);
			}
			break;
		case KSParseState::returnLine:
			if (token == ";") {
				if (currentExpression) {
					currentExpression->push_back(make_shared<KSExpression>(KSReturn(getExpression(move(parseStrings)))));
				}
				clearParseStacks();				
			} else {
				parseStrings.push_back(token);
			}
			break;
		case KSParseState::expectIfEnd:
			if (token == "if") {
				clearParseStacks();
				parseState = KSParseState::ifCall;
			} else if (token == "{") {
				newScope("ifelse"s);
				currentExpression->push_back(KSIf());
				clearParseStacks();
			} else {
				clearParseStacks();
				throw KSException(stringformat("Malformed Syntax: Incorrect token `%s` following `else` keyword",
					token.c_str()));
			}
			break;
        case KSParseState::defineVar:
            if (token == ";") {
                if (parseStrings.size() == 0) {
                    throw KSException("Malformed Syntax: `var` keyword must be followed by user supplied name");
                }
                auto name = parseStrings.front();
                KSExpressionRef defineExpr = nullptr;
                if (parseStrings.size() > 2) {
                    parseStrings.erase(parseStrings.begin());
                    parseStrings.erase(parseStrings.begin());
                    defineExpr = getExpression(move(parseStrings));
                }
                if (currentExpression) {
                    currentExpression->push_back(make_shared<KSExpression>(KSDefineVar(name, defineExpr)));
                } else {
                    getValue(make_shared<KSExpression>(KSDefineVar(name, defineExpr)));
                }
                clearParseStacks();
            } else {
                parseStrings.push_back(token);
            }
            break;
        case KSParseState::defineClass:
            newScope(token);
            currentScope->classScope = true;
            parseState = KSParseState::classArgs;
            parseStrings.clear();
            break;
        case KSParseState::classArgs:
            if (token == ",") {
                if (parseStrings.size()) {
                    auto otherscope = resolveScope(parseStrings.back());
                    currentScope->variables.insert(otherscope->variables.begin(), otherscope->variables.end());
                    currentScope->functions.insert(otherscope->functions.begin(), otherscope->functions.end());
                    parseStrings.clear();
                }
            } else if (token == "{") {
                if (parseStrings.size()) {
                    auto otherscope = resolveScope(parseStrings.back());
                    currentScope->variables.insert(otherscope->variables.begin(), otherscope->variables.end());
                    currentScope->functions.insert(otherscope->functions.begin(), otherscope->functions.end());
                }
                clearParseStacks();
            } else {
                parseStrings.push_back(token);
            }
            break;
        case KSParseState::defineFunc:
            parseStrings.push_back(token);
            parseState = KSParseState::funcArgs;
            break;
        case KSParseState::importModule:
            clearParseStacks();
            if (token.size() > 2 && token.front() == '\"' && token.back() == '\"') {
                // import file       
                evaluateFile(token.substr(1, token.size() - 2));
                clearParseStacks();
            } else {
                // import module
                // TODO
            }           
            break;
		case KSParseState::funcArgs:
			if (token == "(" || token == ",") {
				// eat these tokens
            } else if (token == ")") {
                auto fncName = move(parseStrings.front());
                parseStrings.erase(parseStrings.begin());
                auto& newfunc = newFunction(fncName, parseStrings, {});
                if (currentExpression) {
                    auto newexpr = make_shared<KSExpression>(newfunc, currentExpression);
                    currentExpression->push_back(newexpr);
                    currentExpression = newexpr;
                } else {
                    currentExpression = make_shared<KSExpression>(newfunc, nullptr);
                }
            } else if (token == "{") {
				clearParseStacks();
			} else {
				parseStrings.push_back(token);
			}
			break;
		default:
			break;
		}

        prevState = tempState;
	}

	bool KataScriptInterpreter::readLine(const string& text) {
        ++currentLine;
        bool didExcept = false;
		try {
			for (auto& token : KSTokenize(text)) {
				parse(move(token));
			}
		} catch (KSException e) {
#if defined __EMSCRIPTEN__
            callFunction(resolveFunction("print"), stringformat("Error at line %llu: %s\n", currentLine, e.wh.c_str()));
#else
            printf("Error at line %llu: %s\n", currentLine, e.wh.c_str());
#endif		
            clearParseStacks();
            currentScope = globalScope;
            currentClass = nullptr;
            currentExpression = nullptr;
            didExcept = true;
		} catch (std::exception e) {
#if defined __EMSCRIPTEN__
            callFunction(resolveFunction("print"), stringformat("Error at line %llu: %s\n", currentLine, e.what()));
#else
            printf("Error at line %llu: %s\n", currentLine, e.what());
#endif		
            clearParseStacks();
            currentScope = globalScope;
            currentClass = nullptr;
            currentExpression = nullptr;
            didExcept = true;
        }
        return didExcept;
	}

	bool KataScriptInterpreter::evaluate(const string& script) {
		for (auto& line : split(script, '\n')) {
            if (readLine(move(line))) {
                return true;
            }
		}
        // close any dangling if-expressions that may exist
        return readLine(";"s);
	}

    bool KataScriptInterpreter::evaluateFile(const string& path) {
        string s;
        auto file = std::ifstream(path);
        if (file) {
            file.seekg(0, std::ios::end);
            s.reserve(file.tellg());
            file.seekg(0, std::ios::beg);
            // bash kata scripts have a header line we need to skip
            if (endswith(path, ".sh")) {
                getline(file, s);
            }
            s.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            return evaluate(s);
        } else {
            printf("file: %s not found\n", path.c_str());
            return 1;
        }
    }

	void KataScriptInterpreter::clearState() {
		clearParseStacks();
		globalScope = make_shared<KSScope>("global", nullptr);
		currentScope = globalScope;
		currentExpression = nullptr;
	}

	#include "modules.hpp"
#endif
}

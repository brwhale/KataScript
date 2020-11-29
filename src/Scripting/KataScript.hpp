// copyright Garrett Skelton 2020
// MIT license
#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <variant>
#include <functional>
#include <unordered_map>
#include <cstdarg>
#include <charconv>
#include <iostream>
#include <memory>
#include <algorithm>

namespace KataScript {
	using std::min;
	using std::max;
	using std::string;
	using std::vector;
	using std::variant;
	using std::function;
	using std::unordered_map;
	using std::shared_ptr;
	using std::make_shared;
	using namespace std::string_literals;

	inline double fromChars(const string& token) {
		double x;
		std::from_chars(token.data(), token.data() + token.size(), x);
		return x;
	}

	inline bool fromChars(const string& token, double& x) {
		return std::from_chars(token.data(), token.data() + token.size(), x).ec == std::errc();
	}

	template<typename T, typename C>
	inline bool contains(const C& container, const T& element) {
		return find(container.begin(), container.end(), element) != container.end();
	}

#pragma warning( push )
#pragma warning( disable : 4996)
	// TODO: replace this with std format once that happens
	inline string stringformat(const char* format, ...) {
		va_list args;
		va_start(args, format);
		std::unique_ptr<char[]> buf(new char[1000]);
		vsprintf(buf.get(), format, args);
		return string(buf.get());
	}
#pragma warning( pop )

	// our basic Type flag
	enum class KSType : uint8_t {
		// these are capital, otherwise they'd conflict with the c++ types of the same names
		NONE = 0, // void
		INT,
		FLOAT,
		STRING
	};

	// our basic Object/Value type
	struct KSValue {
		variant<int, float, string> value;
		KSType type;

		KSValue() : type(KSType::NONE) {}
		KSValue(int a) : type(KSType::INT), value(a) {}
		KSValue(float a) : type(KSType::FLOAT), value(a) {}
		KSValue(string a) : type(KSType::STRING), value(a) {}
		~KSValue() {};

		int getInt() {
			return get<int>(value);
		}

		float getFloat() {
			return get<float>(value);
		}

		string getString() {
			return get<string>(value);
		}

		bool getTruthiness() {
			// non zero or "true" are true
			bool truthiness = false;
			switch (type) {
			case KSType::INT:
				truthiness = getInt() != 0;
				break;
			case KSType::FLOAT:
				truthiness = getFloat() != 0;
				break;
			case KSType::STRING:
				truthiness = getString() == "true";
				break;
			default:
				break;
			}
			return truthiness;
		}

		void upconvert(KSType newType) {
			if (type == KSType::NONE) {
				type = KSType::INT;
				value = 0;
			}
			if (newType > type) {
				if (type == KSType::INT) {
					if (newType == KSType::FLOAT) {
						value = (float)getInt();
					} else {
						value = stringformat("%i", getInt());
					}
				} else {
					value = stringformat("%f", getFloat());
				}
				type = newType;
			}
		}

		void hardconvert(KSType newType) {
			if (newType >= type) {
				upconvert(newType);
			} else {				
				switch (newType) {
				case KSType::NONE:
					value = 0;
					break;
				case KSType::INT:
					if (type == KSType::STRING) {
						value = (int)fromChars(getString());
					} else {
						value = (int)getFloat();
					}
					break;
				case KSType::FLOAT:
					if (type == KSType::STRING){
						value = (float)fromChars(getString());
					} else {
						value = 0.f;
					}
					break;
				default:
					break;
				}
				type = newType;
			}
		}
	};

	using KSValueRef = shared_ptr<KSValue>;

	// functions for working with KSValues

	// makes both values have matching types
	inline void upconvert(KSValue& a, KSValue& b) {
		if (a.type != b.type) {
			if (a.type < b.type) {
				a.upconvert(b.type);
			} else {
				b.upconvert(a.type);
			}
		}
	}

	// math operators
	inline KSValue operator + (KSValue& a, KSValue& b) {
		upconvert(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() + b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() + b.getFloat() };
			break;
		case KSType::STRING:
			return KSValue{ a.getString() + b.getString() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator - (KSValue& a, KSValue& b) {
		upconvert(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() - b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() - b.getFloat() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator * (KSValue& a, KSValue& b) {
		upconvert(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() * b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() * b.getFloat() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator / (KSValue& a, KSValue& b) {
		upconvert(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() / b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() / b.getFloat() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator % (KSValue& a, KSValue& b) {
		upconvert(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() % b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ fmod(a.getFloat(), b.getFloat()) };
			break;
		default:
			break;
		}
		return a;
	}

	// comparison operators

	inline KSValue operator == (KSValue& a, KSValue& b) {
		upconvert(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() == b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() == b.getFloat() };
			break;
		case KSType::STRING:
			return KSValue{ a.getString() == b.getString() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator != (KSValue& a, KSValue& b) {
		upconvert(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() != b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() != b.getFloat() };
			break;
		case KSType::STRING:
			return KSValue{ a.getString() != b.getString() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator < (KSValue& a, KSValue& b) {
		upconvert(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() < b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() < b.getFloat() };
			break;
		case KSType::STRING:
			return KSValue{ a.getString() < b.getString() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator > (KSValue& a, KSValue& b) {
		upconvert(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() > b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() > b.getFloat() };
			break;
		case KSType::STRING:
			return KSValue{ a.getString() > b.getString() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator <= (KSValue& a, KSValue& b) {
		upconvert(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() <= b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() <= b.getFloat() };
			break;
		case KSType::STRING:
			return KSValue{ a.getString() <= b.getString() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator >= (KSValue& a, KSValue& b) {
		upconvert(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() >= b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() >= b.getFloat() };
			break;
		case KSType::STRING:
			return KSValue{ a.getString() >= b.getString() };
			break;
		default:
			break;
		}
		return a;
	}

	// operator precedence for pemdas

	enum class KSOperatorPrecedence : int {
		assign = 0,
		compare,
		addsub,
		muldiv,
		incdec,
		func
	};

	// KSLambda is a "native function" it's how you wrap c++ code for use inside KataScript
	using KSFunctionArgs = vector<KSValueRef>;
	using KSLambda = function<KSValueRef(KSFunctionArgs)>;

	// our basic function type
	struct KSFunction {
		string name;
		KSOperatorPrecedence opPrecedence;
		vector<string> argNames;

		// to calculate a result
		// either we have a KataScript body
		vector<string> body;
		// or a KSLambda 
		KSLambda lambda;

		KSOperatorPrecedence getPrecedence() {
			if (name.size() > 2) {
				return KSOperatorPrecedence::func;
			}
			if (contains("!<>"s, name[0]) || name == "==") {
				return KSOperatorPrecedence::compare;
			}
			if (contains(name, '=')) {
				return KSOperatorPrecedence::assign;
			}
			if (contains("/*%"s, name[0])) {
				return KSOperatorPrecedence::muldiv;
			}
			if (contains("-+"s, name[0])) {
				return KSOperatorPrecedence::addsub;
			}
			return KSOperatorPrecedence::func;
		}

		KSFunction(const string& name_, const KSLambda& l) : lambda(l), name(name_), opPrecedence(getPrecedence()) {}
		// when using a KataScript function body, the operator precedence will always be "func" level (aka the highest)
		KSFunction(const string& name_, const vector<string>& argNames_, const vector<string>& body_)
			: name(name_), body(body_), argNames(argNames_), opPrecedence(KSOperatorPrecedence::func) {}
		// default constructor makes a function with no args that returns void
		KSFunction(const string& name) : KSFunction(name, [](KSFunctionArgs) { return make_shared<KSValue>(); }) {}
		KSFunction() : KSFunction("anon") {}

	};
	using KSFunctionRef = shared_ptr<KSFunction>;

	// forward declare so we can cross refernce types
	// KSExpression is a 'generic' expression
	struct KSExpression;
	using KSExpressionRef = shared_ptr<KSExpression>;
	// describes an expression tree with a function at the root
	struct KSFunctionExpression {
		KSFunctionRef func;
		vector<KSExpressionRef> subexpressions;

		KSFunctionExpression(const KSFunctionExpression& o) = delete;
		KSFunctionExpression(KSFunctionRef fnc) : func(fnc) {}

		void clear() {
			subexpressions.clear();
		}

		~KSFunctionExpression() {
			clear();
		}
	};

	// forward declare so we can use the parser to process functions
	class KataScriptInterpereter;
	// describes a 'generic' expression tree, with either a value or function at the root
	struct KSExpression {
		// either we have a value, or a function expression which then has sub-expressions
		bool hasValue = false;
		KSFunctionExpression expr;
		KSValueRef value;

		KSExpression(KSFunctionRef fnc) : hasValue(false), expr(fnc), value(nullptr) {}
		KSExpression(KSValueRef val) : hasValue(true), value(val), expr(nullptr) {}

		// walk the tree depth first and replace any function expressions 
		// with a value expression of their results
		void consolidate(KataScriptInterpereter* i);
	};

	struct KSLoop {
		vector<string> testExpression;
		vector<string> iterateExpression;
		vector<string> contents;
	};

	struct KSScope {
		// this is the main storage object for all functions and variables
		string name;
		unordered_map<string, KSValueRef> variables;
		unordered_map<string, KSScope> scopes;
		unordered_map<string, KSFunctionRef> functions;
		KSScope* parent = nullptr;
	};

	// state enum for state machine for token by token parsing
	enum class KSParseState : uint8_t {
		beginExpression,
		defineFunc,
		funcArgs,
		callFunc,
		expectSemicolon,
		readLine,
		returnLine,
		ifCall,
		expectScopeEnd,
		expectIfEnd,
		checkElse,
		loopCall,
		loopRead,
		readFunction,
	};

	// finally we have our interpereter
	class KataScriptInterpereter {
		KSScope globalScope;
		KSScope* currentScope = &globalScope;

		vector<KSParseState> parseStack = { KSParseState::beginExpression };
		vector<string> parseStrings;
		vector<char> parseOperations;
		vector<KSType> parseTypes;
		int outerNestLayer = 0;
		int anonScopeCount = 0;

		vector<KSLoop> loopStack;
		KSValueRef returnVal;

		void newScope(const string& name);
		void closeCurrentScope();

		
		KSFunctionRef resolveFunction(const string& name);
		KSExpressionRef getExpression(const vector<string>& strings);
		KSValueRef GetValue(const vector<string>& strings);
		void clearParseStacks();
		void parse(const string& token);
	public:
		void newFunction(const string& name, const vector<string>& argNames, const vector<string>& body);
		void newFunction(const string& name, const KSLambda& lam);
		KSValueRef callFunction(const string& name, const KSFunctionArgs& args);
		KSValueRef callFunction(const KSFunctionRef fnc, const KSFunctionArgs& args);
		KSValueRef resolveVariable(const string& name);

		bool active = false;
		void readLine(const string& text);
		KataScriptInterpereter();
	};

	// implementations

	// tokenizer special characters
	const string WhitespaceChars = " \t\n"s;
	const string GrammarChars = " \t\n,(){};+-/*%<>=!\""s;
	const string MultiCharTokenStartChars = "+-/*<>=!"s;
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
			if (len) {
				ret.push_back(input.substr(lpos, pos - lpos));
				lpos = pos;
			} else {
				if (input[pos] == '\"') {
					pos = input.find('\"', lpos + 1) + 1;
					ret.push_back(input.substr(lpos, pos - lpos));
					lpos = pos;
					continue;
				}
			}
			if (input[pos] == '-' && contains(NumericChars, input[pos + 1])) {
				pos = input.find_first_of(GrammarChars, pos + 1);
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
			return contains("=+-"s, test[1]) && contains("<>=!+-"s, test[0]);
		}
		return false;
	}

	// scope control lets you have object lifetimes
	void KataScriptInterpereter::newScope(const string& name) {
		// if the scope exists we just use it as is
		auto parent = currentScope;
		auto newscope = &parent->scopes[name];
		currentScope = newscope;
		currentScope->parent = parent;
		currentScope->name = name;
	}

	void KataScriptInterpereter::closeCurrentScope() {
		if (currentScope->parent) {
			auto name = currentScope->name;
			currentScope->functions.clear();
			currentScope->variables.clear();
			currentScope->scopes.clear();
			currentScope = currentScope->parent;
			currentScope->scopes.erase(name);
		}
	}

	// call function by name
	KSValueRef KataScriptInterpereter::callFunction(const string& name, const KSFunctionArgs& args) {
		return callFunction(resolveFunction(name), args);
	}

	// call function by reference
	KSValueRef KataScriptInterpereter::callFunction(const KSFunctionRef fnc, const KSFunctionArgs& args) {
		if (fnc->body.size()) {
			newScope(fnc->name);
			int limit = (int)min(args.size(), fnc->argNames.size());
			for (int i = 0; i < limit; ++i) {
				*resolveVariable(fnc->argNames[i]) = *args[i];
			}
			returnVal = nullptr;
			for (auto&& token : fnc->body) {
				parse(token);

				if (returnVal) {
					break;
				}
			}
			closeCurrentScope();
			return returnVal ? returnVal : make_shared<KSValue>();
		} else {
			return fnc->lambda(args);
		}
	}

	void KataScriptInterpereter::newFunction(const string& name, const vector<string>& argNames, const vector<string>& body) {
		currentScope->functions[name] = make_shared<KSFunction>(name, argNames, body);
	}

	void KataScriptInterpereter::newFunction(const string& name, const KSLambda& lam) {
		currentScope->functions[name] = make_shared<KSFunction>(name, lam);
	}

	// name resolution for variables
	KSValueRef KataScriptInterpereter::resolveVariable(const string& name) {
		auto scope = currentScope;
		while (scope) {
			auto iter = scope->variables.find(name);
			if (iter != scope->variables.end()) {
				return iter->second;
			} else {
				scope = scope->parent;
			}
		}
		auto& varr = currentScope->variables[name];
		varr = KSValueRef(new KSValue());
		return varr;
	}

	// name resolution for functions
	KSFunctionRef KataScriptInterpereter::resolveFunction(const string& name) {
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
	KSExpressionRef KataScriptInterpereter::getExpression(const vector<string>& strings) {
		KSExpressionRef root = nullptr;
		size_t i = 0;
		while (i < strings.size()) {
			if (isMathOperator(strings[i])) {
				auto prev = root;
				root = make_shared<KSExpression>(resolveFunction(strings[i]));
				auto curr = prev;
				if (curr) {
					if (!curr->hasValue && (int)curr->expr.func->opPrecedence < (int)root->expr.func->opPrecedence) {
						while (!curr->expr.subexpressions.back()->hasValue) {
							if ((int)curr->expr.subexpressions.back()->expr.func->opPrecedence < (int)root->expr.func->opPrecedence) {
								curr = curr->expr.subexpressions.back();
							} else {
								break;
							}
						}

						root->expr.subexpressions.push_back(curr->expr.subexpressions.back());
						curr->expr.subexpressions.pop_back();
						root->expr.subexpressions.push_back(getExpression({ strings[++i] }));
						curr->expr.subexpressions.push_back(root);
						root = prev;
					} else {
						root->expr.subexpressions.push_back(curr);
					}
				}
			} else if (isStringLiteral(strings[i])) {
				auto val = strings[i].substr(1, strings[i].size() - 2);
				auto newExpr = make_shared<KSExpression>(KSValueRef(new KSValue(val)));
				if (root) {
					root->expr.subexpressions.push_back(newExpr);
				} else {
					root = newExpr;
				}
			} else if (strings[i] == "(" || isVarOrFuncToken(strings[i])) {
				if (strings[i] == "(" || i + 2 < strings.size() && strings[i + 1] == "(") {
					// function
					KSExpressionRef cur = nullptr;
					if (strings[i] == "(") {
						if (root) {
							root->expr.subexpressions.push_back(make_shared<KSExpression>(resolveFunction("identity")));
							cur = root->expr.subexpressions.back();
						} else {
							root = make_shared<KSExpression>(resolveFunction("identity"));
							cur = root;
						}
					} else {
						if (root) {
							root->expr.subexpressions.push_back(make_shared<KSExpression>(resolveFunction(strings[i])));
							cur = root->expr.subexpressions.back();
						} else {
							root = make_shared<KSExpression>(resolveFunction(strings[i]));
							cur = root;
						}
						++i;
					}
					vector<string> minisub;
					int nestLayers = 1;
					while (nestLayers > 0 && ++i < strings.size()) {
						if (nestLayers == 1 && strings[i] == ",") {
							if (minisub.size()) {
								cur->expr.subexpressions.push_back(getExpression(minisub));
								minisub.clear();
							}
						} else if (strings[i] == ")") {
							if (--nestLayers > 0) {
								minisub.push_back(strings[i]);
							} else {
								if (minisub.size()) {
									cur->expr.subexpressions.push_back(getExpression(minisub));
									minisub.clear();
								}
							}
						} else if (strings[i] == "(" || !(strings[i].size() == 1 && contains("+-*/"s, strings[i][0])) && i + 2 < strings.size() && strings[i + 1] == "(") {
							++nestLayers;
							if (strings[i] == "(") {
								minisub.push_back(strings[i]);
							} else {
								minisub.push_back(strings[i]);
								minisub.push_back(strings[++i]);
							}
						} else {
							minisub.push_back(strings[i]);
						}
					}
				} else {
					// variable
					auto newExpr = make_shared<KSExpression>(resolveVariable(strings[i]));
					if (root) {
						root->expr.subexpressions.push_back(newExpr);
					} else {
						root = newExpr;
					}
				}
			} else {
				// number
				auto val = fromChars(strings[i]);
				auto newExpr = make_shared<KSExpression>(KSValueRef(contains(strings[i], '.') ? new KSValue((float)val) : new KSValue((int)val)));
				if (root) {
					root->expr.subexpressions.push_back(newExpr);
				} else {
					root = newExpr;
				}
			}
			++i;
		}

		return root;
	}

	// evaluate an expression
	void KSExpression::consolidate(KataScriptInterpereter* i) {
		if (!hasValue) {
			KSFunctionArgs args;
			for (auto&& sub : expr.subexpressions) {
				sub->consolidate(i);
				args.push_back(sub->value);
			}
			hasValue = true;
			value = i->callFunction(expr.func, args);
			expr.clear();
		}
	}

	// evaluate an expression from tokens
	KSValueRef KataScriptInterpereter::GetValue(const vector<string>& strings) {
		auto expr = getExpression(strings);
		expr->consolidate(this);
		return expr->value;
	}

	// general purpose clear to reset state machine for next statement
	void KataScriptInterpereter::clearParseStacks() {
		parseStack.clear();
		parseStack.push_back(KSParseState::beginExpression);
		parseStrings.clear();
		parseOperations.clear();
		parseTypes.clear();
		outerNestLayer = 0;
	}

	// parse one token at a time, uses the state machine
	void KataScriptInterpereter::parse(const string& token) {
		switch (parseStack.back()) {
		case KSParseState::checkElse:
			if (token == "else") {
				break;
			}
			[[fallthrough]];
		case KSParseState::beginExpression:
		{
			if (token == "func") {
				parseStack.push_back(KSParseState::defineFunc);
			} else if (token == "for" || token == "while") {
				parseStack.push_back(KSParseState::loopCall);
				loopStack.push_back(KSLoop());
				newScope(stringformat("loop~%i", (int)loopStack.size()));
			} else if (token == "if") {
				parseStack.push_back(KSParseState::ifCall);
			} else if (token == "else") {
				parseStack.push_back(KSParseState::expectIfEnd);
			} else if (token == "{") {
				newScope(stringformat("anon~%i", anonScopeCount++));
				clearParseStacks();
			} else if (token == "}") {
				closeCurrentScope();
			} else if (token == "return") {
				parseStack.push_back(KSParseState::returnLine);
			} else if (token == "quit") {
				clearParseStacks();
				active = false;
			} else if (token == ";") {
				clearParseStacks();
			} else {
				parseStack.push_back(KSParseState::readLine);
				parseStrings.push_back(token);
			}
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
							exprs.back().push_back(str);
						}
					}
					switch (exprs.size()) {
					case 1:
						loopStack.back().testExpression = exprs[0];
						break;
					case 2:
						loopStack.back().testExpression = exprs[0];
						loopStack.back().iterateExpression = exprs[1];
						break;
					case 3:
						GetValue(exprs[0]);
						loopStack.back().testExpression = exprs[1];
						loopStack.back().iterateExpression = exprs[2];
						break;
					default:
						break;
					}

					clearParseStacks();
					parseStack.push_back(KSParseState::loopRead);
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
		case KSParseState::loopRead:
			if (token == "}") {
				if (--outerNestLayer <= 0) {
					clearParseStacks();
					bool looping = true;
					while (looping) {
						looping = GetValue(loopStack.back().testExpression)->getTruthiness();

						if (looping) {
							for (auto&& token : loopStack.back().contents) {
								parse(token);
							}
							if (loopStack.back().iterateExpression.size()) {
								GetValue(loopStack.back().iterateExpression);
							}
						} else {
							clearParseStacks();
							closeCurrentScope();
							loopStack.pop_back();
						}
					}
				} else {
					loopStack.back().contents.push_back(token);
				}
			} else if (token == "{") {
				if (++outerNestLayer > 1) {
					loopStack.back().contents.push_back(token);
				}
			} else {
				loopStack.back().contents.push_back(token);
			}
			break;
		case KSParseState::ifCall:
			parseStrings.push_back(token);
			if (token == ")") {
				if (--outerNestLayer <= 0) {
					auto value = GetValue(parseStrings);
					clearParseStacks();
					if (!value->getTruthiness()) {
						parseStack.push_back(KSParseState::expectScopeEnd);
						outerNestLayer = 0;
					}
				}
			} else if (token == "(") {
				++outerNestLayer;
			}
			break;
		case KSParseState::readLine:
			if (token == ";") {
				auto line = std::move(parseStrings);
				clearParseStacks();
				GetValue(line);
			} else {
				parseStrings.push_back(token);
			}
			break;
		case KSParseState::returnLine:
			if (token == ";") {
				auto line = std::move(parseStrings);
				clearParseStacks();
				returnVal = GetValue(line);
			} else {
				parseStrings.push_back(token);
			}
			break;
		case KSParseState::expectSemicolon:
			if (token == ";") {
				clearParseStacks();
			}
			break;
		case KSParseState::expectScopeEnd:
			if (token == "}" && --outerNestLayer <= 0) {
				clearParseStacks();
				parseStack.push_back(KSParseState::checkElse);
			} else if (token == "{") {
				++outerNestLayer;
			}
			break;
		case KSParseState::expectIfEnd:
			if (token == "}" && --outerNestLayer <= 0) {
				clearParseStacks();
			} else if (token == "{") {
				++outerNestLayer;
			}
			break;
		case KSParseState::defineFunc:
			parseStrings.push_back(token);
			parseStack.push_back(KSParseState::funcArgs);
			break;
		case KSParseState::funcArgs:
			if (token == "(" || token == ",") {
				// eat these tokens
			} else if (token == ")") {
				// creat function scope
				auto fncName = parseStrings.front();
				parseStrings.erase(parseStrings.begin());
				// get func body
				newFunction(fncName, parseStrings, {});

				parseStrings.clear();
				parseStrings.push_back(fncName);

				parseStack.push_back(KSParseState::readFunction);
				outerNestLayer = 0;
			} else {
				parseStrings.push_back(token);
			}
			break;
		case KSParseState::callFunc:
			parseStrings.push_back(token);
			if (token == ")") {
				if (--outerNestLayer < 0) {
					GetValue(parseStrings);
					parseStack.push_back(KSParseState::expectSemicolon);
				}
			} else if (token == "(") {
				++outerNestLayer;
			}
			break;
		case KSParseState::readFunction:
			if (token == "}") {
				if (--outerNestLayer <= 0) {
					auto fncName = parseStrings.front();
					parseStrings.erase(parseStrings.begin());
					resolveFunction(fncName)->body = parseStrings;
					clearParseStacks();
				} else {
					parseStrings.push_back(token);
				}
			} else if (token == "{") {
				if (++outerNestLayer > 1) {
					parseStrings.push_back(token);
				}
			} else {
				parseStrings.push_back(token);
			}
			break;

		default:
			break;
		}
	}

	void KataScriptInterpereter::readLine(const string& text) {
		for (auto&& token : KSTokenize(text)) {
			parse(token);
		}
	}

	KataScriptInterpereter::KataScriptInterpereter() {
		// register compiled functions and standard library:
		newFunction("identity", [](KSFunctionArgs args) {
			return args[0];
			});

		newFunction("sqrt", [](KSFunctionArgs args) {
			args[0]->hardconvert(KSType::FLOAT);
			args[0]->value = sqrtf(get<float>(args[0]->value));
			return args[0];
			});

		newFunction("print", [](KSFunctionArgs args) {
			auto t = *args[0];
			t.upconvert(KSType::STRING);
			printf("%s\n", get<string>(t.value).c_str());
			return KSValueRef(new KSValue());
			});

		newFunction("=", [](KSFunctionArgs args) {
			if (args.size() == 1) {
				return args[0];
			}
			upconvert(*args[0], *args[1]);
			*args[0] = *args[1];
			return args[0];
			});

		newFunction("+", [](KSFunctionArgs args) {
			if (args.size() == 1) {
				return args[0];
			}
			upconvert(*args[0], *args[1]);
			return KSValueRef(new KSValue(*args[0] + *args[1]));
			});

		newFunction("-", [](KSFunctionArgs args) {
			if (args.size() == 1) {
				auto zero = KSValue(0);
				upconvert(*args[0], zero);
				return KSValueRef(new KSValue(zero - *args[0]));
			}
			upconvert(*args[0], *args[1]);
			return KSValueRef(new KSValue(*args[0] - *args[1]));
			});

		newFunction("*", [](KSFunctionArgs args) {
			upconvert(*args[0], *args[1]);
			return KSValueRef(new KSValue(*args[0] * *args[1]));
			});

		newFunction("/", [](KSFunctionArgs args) {
			upconvert(*args[0], *args[1]);
			return KSValueRef(new KSValue(*args[0] / *args[1]));
			});

		newFunction("%", [](KSFunctionArgs args) {
			upconvert(*args[0], *args[1]);
			return KSValueRef(new KSValue(*args[0] % *args[1]));
			});

		newFunction("==", [](KSFunctionArgs args) {
			upconvert(*args[0], *args[1]);
			return KSValueRef(new KSValue(*args[0] == *args[1]));
			});

		newFunction("!=", [](KSFunctionArgs args) {
			upconvert(*args[0], *args[1]);
			return KSValueRef(new KSValue(*args[0] != *args[1]));
			});

		newFunction("++", [](KSFunctionArgs args) {
			auto t = KSValue(1);
			upconvert(*args[0], t);
			*args[0] = *args[0] + t;
			return args[0];
			});

		newFunction("--", [](KSFunctionArgs args) {
			auto t = KSValue(1);
			upconvert(*args[0], t);
			*args[0] = *args[0] - t;
			return args[0];
			});

		newFunction("+=", [](KSFunctionArgs args) {
			upconvert(*args[0], *args[1]);
			*args[0] = *args[0] + *args[1];
			return args[0];
			});

		newFunction("-=", [](KSFunctionArgs args) {
			upconvert(*args[0], *args[1]);
			*args[0] = *args[0] - *args[1];
			return args[0];
			});

		newFunction(">", [](KSFunctionArgs args) {
			upconvert(*args[0], *args[1]);
			return KSValueRef(new KSValue(*args[0] > * args[1]));
			});

		newFunction("<", [](KSFunctionArgs args) {
			upconvert(*args[0], *args[1]);
			return KSValueRef(new KSValue(*args[0] < *args[1]));
			});

		newFunction(">=", [](KSFunctionArgs args) {
			upconvert(*args[0], *args[1]);
			return KSValueRef(new KSValue(*args[0] >= *args[1]));
			});

		newFunction("<=", [](KSFunctionArgs args) {
			upconvert(*args[0], *args[1]);
			return KSValueRef(new KSValue(*args[0] <= *args[1]));
			});

		newFunction("int", [](KSFunctionArgs args) {
			args[0]->hardconvert(KSType::INT);
			return args[0];
			});

		newFunction("float", [](KSFunctionArgs args) {
			args[0]->hardconvert(KSType::FLOAT);
			return args[0];
			});

		newFunction("string", [](KSFunctionArgs args) {
			args[0]->hardconvert(KSType::STRING);
			return args[0];
			});
	}
}
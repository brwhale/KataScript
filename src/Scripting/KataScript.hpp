// copyright Garrett Skelton 2020
// MIT license
#pragma once

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
	~KSValue() {
	};

	bool getTruthiness() {
		// non zero or "true" are true
		bool truthiness = false; 
		switch (type) {
		case KSType::INT:
			truthiness = get<int>(value) != 0;
			break;
		case KSType::FLOAT:
			truthiness = get<float>(value) != 0;
			break;
		case KSType::STRING:
			truthiness = get<string>(value) == "true";
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
					value = (float)get<int>(value);
				} else {
					value = stringformat("%i",get<int>(value));
				}
			} else {
				value = stringformat("%f", get<float>(value));
			}
			type = newType;
		}
	}

	void hardconvert(KSType newType) {
		if (newType >= type) {
			upconvert(newType);
		} else {
			type = newType;
			switch (type) {
			case KSType::NONE:
				break;
			case KSType::INT:
				value = 1;
				break;
			case KSType::FLOAT:
				value = 1.f;
				break;
			case KSType::STRING:
				value = "";
				break;
			default:
				break;
			}
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
		return KSValue{ get<int>(a.value) + get<int>(b.value) };
		break;
	case KSType::FLOAT:
		return KSValue{ get<float>(a.value) + get<float>(b.value) };
		break;
	case KSType::STRING:
		return KSValue{ get<string>(a.value) + get<string>(b.value) };
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
		return KSValue{ get<int>(a.value) - get<int>(b.value) };
		break;
	case KSType::FLOAT:
		return KSValue{ get<float>(a.value) - get<float>(b.value) };
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
		return KSValue{ get<int>(a.value) * get<int>(b.value) };
		break;
	case KSType::FLOAT:
		return KSValue{ get<float>(a.value) * get<float>(b.value) };
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
		return KSValue{ get<int>(a.value) / get<int>(b.value) };
		break;
	case KSType::FLOAT:
		return KSValue{ get<float>(a.value) / get<float>(b.value) };
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
		return KSValue{ get<int>(a.value) % get<int>(b.value) };
		break;
	case KSType::FLOAT:
		return KSValue{ fmod(get<float>(a.value), get<float>(b.value)) };
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
		return KSValue{ get<int>(a.value) == get<int>(b.value) };
		break;
	case KSType::FLOAT:
		return KSValue{ get<float>(a.value) == get<float>(b.value) };
		break;
	case KSType::STRING:
		return KSValue{ get<string>(a.value) == get<string>(b.value) };
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
		return KSValue{ get<int>(a.value) != get<int>(b.value) };
		break;
	case KSType::FLOAT:
		return KSValue{ get<float>(a.value) != get<float>(b.value) };
		break;
	case KSType::STRING:
		return KSValue{ get<string>(a.value) != get<string>(b.value) };
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
		return KSValue{ get<int>(a.value) < get<int>(b.value) };
		break;
	case KSType::FLOAT:
		return KSValue{ get<float>(a.value) < get<float>(b.value) };
		break;
	case KSType::STRING:
		return KSValue{ get<string>(a.value) < get<string>(b.value) };
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
		return KSValue{ get<int>(a.value) > get<int>(b.value) };
		break;
	case KSType::FLOAT:
		return KSValue{ get<float>(a.value) > get<float>(b.value) };
		break;
	case KSType::STRING:
		return KSValue{ get<string>(a.value) > get<string>(b.value) };
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
		return KSValue{ get<int>(a.value) <= get<int>(b.value) };
		break;
	case KSType::FLOAT:
		return KSValue{ get<float>(a.value) <= get<float>(b.value) };
		break;
	case KSType::STRING:
		return KSValue{ get<string>(a.value) <= get<string>(b.value) };
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
		return KSValue{ get<int>(a.value) >= get<int>(b.value) };
		break;
	case KSType::FLOAT:
		return KSValue{ get<float>(a.value) >= get<float>(b.value) };
		break;
	case KSType::STRING:
		return KSValue{ get<string>(a.value) >= get<string>(b.value) };
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
using KSLambda = function<KSValueRef(vector<KSValueRef>)>;

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
	
	KSFunction(const string& name_, const KSLambda& l) : lambda(l), name(name_), opPrecedence(getPrecedence()) { }
	// when using a KataScript function body, the operator precedence will always be "func" level (aka the highest)
	KSFunction(const string& name_, const vector<string>& argNames_, const vector<string>& body_) 
		: name(name_), body(body_), argNames(argNames_), opPrecedence(KSOperatorPrecedence::func) {}
	// default constructor makes a function with no args that returns void
	KSFunction(const string& name) : KSFunction(name, [](vector<KSValueRef>) { return make_shared<KSValue>(); }) {}
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

	KSValueRef resolveVariable(const string& name);
	KSFunctionRef resolveFunction(const string& name);
	KSExpressionRef getExpression(const vector<string>& strings);
	KSValueRef GetValue(const vector<string>& strings);	
	void clearParseStacks();
	void parse(const string& token);
public:
	void newFunction(const string& name, const vector<string>& argNames, const vector<string>& body);
	void newFunction(const string& name, const KSLambda& lam);
	KSValueRef callFunction(const string& name, const vector<KSValueRef>& args);
	KSValueRef callFunction(const KSFunctionRef fnc, const vector<KSValueRef>& args);

	bool active = false;
	void readLine(const string& text);
	KataScriptInterpereter();
};

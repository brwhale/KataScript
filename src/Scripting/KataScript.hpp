// copyright Garrett Skelton 2020
// MIT license
#pragma once
#include <vector>
#include <string>
#include <variant>
#include <functional>
#include <map>
#include <unordered_map>
#include <cstdarg>
#include <charconv>
#include <iostream>
#include <memory>
#include <algorithm>
#include <exception>
#include <cmath>

namespace KataScript {
	using std::min;
	using std::max;
	using std::string;
	using std::vector;
	using std::variant;
	using std::function;
	using std::unordered_map;
	using std::map;
	using std::shared_ptr;
	using std::make_shared;
	using namespace std::string_literals;

	inline double fromChars(const string& token) {
		double x;
#ifdef _MSC_VER
		std::from_chars(token.data(), token.data() + token.size(), x);
#else
		x = std::stod(token);
#endif
		return x;
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
		FUNCTION,
		STRING,
		LIST
	};

	inline string getTypeName(KSType t) {
		switch (t) {
		case KSType::NONE:
			return "NONE";
			break;
		case KSType::INT:
			return "INT";
			break;
		case KSType::FLOAT:
			return "FLOAT";
			break;
		case KSType::FUNCTION:
			return "FUNCTION";
			break;
		case KSType::STRING:
			return "STRING";
			break;
		case KSType::LIST:
			return "LIST";
			break;
		default:
			return "UNKNOWN";
			break;
		}
	}

	// forward declare so lists can contain lists
	struct KSValue;
	using KSValueRef = shared_ptr<KSValue>;
	using KSList = vector<KSValueRef>;
	// forward declare function for functions as values
	struct KSFunction;
	using KSFunctionRef = shared_ptr<KSFunction>;
	using KSValueVariant = variant<int, float, KSFunctionRef, string, KSList>;
	// our basic Object/Value type
	struct KSValue {
		KSValueVariant value;
		KSType type;

		KSValue() : type(KSType::NONE) {}
		KSValue(int a) : type(KSType::INT), value(a) {}
		KSValue(float a) : type(KSType::FLOAT), value(a) {}
		KSValue(string a) : type(KSType::STRING), value(a) {}
		KSValue(KSFunctionRef a) : type(KSType::FUNCTION), value(a) {}
		KSValue(KSList a) : type(KSType::LIST), value(a) {}
		KSValue(KSValueVariant a, KSType t) : type(t), value(a) {}
		~KSValue() {};

		string getPrintString() {
			auto t = *this;
			t.hardconvert(KSType::STRING);
			return get<string>(t.value);
		}

		int& getInt() {
			return get<int>(value);
		}

		float& getFloat() {
			return get<float>(value);
		}

		KSFunctionRef& getFunction() {
			return get<KSFunctionRef>(value);
		}

		string& getString() {
			return get<string>(value);
		}

		KSList& getList() {
			return get<KSList>(value);
		}

		bool getBool() {
			// non zero or "true" are true
			bool truthiness = false;
			switch (type) {
			case KSType::INT:
				truthiness = getInt();
				break;
			case KSType::FLOAT:
				truthiness = getFloat() != 0;
				break;
			case KSType::STRING:
				truthiness = getString() == "true";
				break;
			case KSType::LIST:
				truthiness = getList().size() > 0;
				break;
			default:
				break;
			}
			return truthiness;
		}

		void upconvert(KSType newType) {
			if (type == KSType::FUNCTION || newType == KSType::FUNCTION) {
				throw std::runtime_error("cannot convert functions");
			}
			if (newType > type) {
				switch (newType) {
				default:
					break;
				case KSType::INT:
					value = 0;
					break;
				case KSType::FLOAT:
					switch (type) {
					default:
						break;
					case KSType::NONE:
						value = 0.f;
						break;
					case KSType::INT:
						value = (float)getInt();
						break;
					}
					break;
				case KSType::STRING:
					switch (type) {
					default:
						break;
					case KSType::NONE:
						value = "null"s;
						break;
					case KSType::INT:
						value = stringformat("%i", getInt());
						break;
					case KSType::FLOAT:
						value = stringformat("%f", getFloat());
						break;
					}
					break;
				case KSType::LIST:
					switch (type) {
					default:
						break;
					case KSType::NONE:
					case KSType::INT:
					case KSType::FLOAT:
						value = KSList({ make_shared<KSValue>(value, type) });
						break;
					case KSType::STRING:
					{
						auto str = getPrintString();
						value = KSList({ });
						auto& list = getList();
						for (auto c : str) {
							list.push_back(make_shared<KSValue>(""s + c));
						}
					}
						break;
					}
					break;
				}
				type = newType;
			}
		}

		void hardconvert(KSType newType) {
			if (newType >= type) {
				upconvert(newType);
			} else {				
				switch (newType) {
				default:
					break;
				case KSType::NONE:
					value = 0;
					break;
				case KSType::INT:
					switch (type) {
					default:
						break;
					case KSType::FLOAT:
						value = (int)getFloat();
						break;
					case KSType::STRING:
						value = (int)fromChars(getString());
						break;
					case KSType::LIST:
						value = (int)getList().size();
						break;
					}
					break;
				case KSType::FLOAT:
					switch (type) {
					default:
						break;
					case KSType::STRING:
						value = (float)fromChars(getString());
						break;
					case KSType::LIST:
						value = (float)getList().size();
						break;
					}
					break;
				case KSType::STRING:
					switch (type) {
					default:
						break;
					case KSType::LIST:
					{
						string newval;
						auto& list = getList();
						for (auto val : list) {
							newval += val->getPrintString() + ", ";
						}
						if (list.size()) {
							newval.pop_back();
							newval.pop_back();
						}
						value = newval;
					}
						break;
					}				
					break;
				}
				type = newType;
			}
		}
	};

	// define cout operator for KSValues
	inline  std::ostream& operator<<(std::ostream& os, const vector<KSValueRef>& values) {
		for (auto val : values) {
			std::visit([&os](auto&& arg) { os << arg; }, val->value);
		}
		return os;
	}

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

	inline void upconvertThrowOnNonNumberToNumberCompare(KSValue& a, KSValue& b) {
		if (a.type != b.type) {
			if (max((int)a.type, (int)b.type) > (int)KSType::FLOAT) {
				throw std::runtime_error(stringformat("Bad comparison comparing `%s %s` to `%s %s`",
					getTypeName(a.type).c_str(), a.getPrintString().c_str(), getTypeName(b.type).c_str(), b.getPrintString().c_str()).c_str());
			}
			if (a.type < b.type) {
				a.upconvert(b.type);
			} else {
				b.upconvert(a.type);
			}
		}
	}

	// math operators
	inline KSValue operator + (KSValue a, KSValue b) {
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
		case KSType::LIST:
		{
			auto list = KSList(a.getList());
			auto& blist = b.getList();
			list.insert(list.end(), blist.begin(), blist.end());
			return KSValue{ list };
		}
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator - (KSValue a, KSValue b) {
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

	inline KSValue operator * (KSValue a, KSValue b) {
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

	inline KSValue operator / (KSValue a, KSValue b) {
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

	inline KSValue operator % (KSValue a, KSValue b) {
		upconvert(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() % b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ std::fmod(a.getFloat(), b.getFloat()) };
			break;
		default:
			break;
		}
		return a;
	}

	// comparison operators
	KSValue operator != (KSValue a, KSValue b);
	inline KSValue operator == (KSValue a, KSValue b) {
		upconvert(a, b); // allow 5 == "5" to be true
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
		case KSType::LIST:
		{
			auto& alist = a.getList();
			auto& blist = b.getList();
			if (alist.size() != blist.size()) {
				return KSValue{ 0 };
			}
			for (size_t i = 0; i < alist.size(); ++i) {
				if ((*alist[i] != *blist[i]).getBool()) {
					return KSValue{ 0 };
				}
			}
			return KSValue{ 1 };
		}
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator != (KSValue a, KSValue b) {
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
		case KSType::LIST:
			return KSValue{ !(a == b).getBool() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator || (KSValue& a, KSValue& b) {
		return a.getBool() || b.getBool();
	}

	inline KSValue operator && (KSValue& a, KSValue& b) {
		return a.getBool() && b.getBool();
	}

	inline KSValue operator < (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
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
		case KSType::LIST:
			return KSValue{ a.getList().size() < b.getList().size() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator > (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
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
		case KSType::LIST:
			return KSValue{ a.getList().size() > b.getList().size() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator <= (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
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
		case KSType::LIST:
			return KSValue{ a.getList().size() <= b.getList().size() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator >= (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
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
		case KSType::LIST:
			return KSValue{ a.getList().size() >= b.getList().size() };
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
	using KSLambda = function<KSValueRef(const KSList&)>;

	// forward declare so we can cross refernce types
	// KSExpression is a 'generic' expression
	struct KSExpression;
	using KSExpressionRef = shared_ptr<KSExpression>;
	struct KSScope;
	using KSScopeRef = shared_ptr<KSScope>;

	// our basic function type
	struct KSFunction {
		string name;
		KSOperatorPrecedence opPrecedence;
		vector<string> argNames;
		KSScopeRef scope;

		// to calculate a result
		// either we have a KataScript body
		vector<KSExpressionRef> subexpressions;
		// or a KSLambda 
		KSLambda lambda;

		KSOperatorPrecedence getPrecedence() {
			if (name.size() > 2) {
				return KSOperatorPrecedence::func;
			}
			if (contains("!<>|&"s, name[0]) || name == "==") {
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

		KSFunction(const string& name_, const KSLambda& l, const KSScopeRef& s) : scope(s), lambda(l), name(name_), opPrecedence(getPrecedence()) {}
		// when using a KataScript function body, the operator precedence will always be "func" level (aka the highest)
		KSFunction(const string& name_, const vector<string>& argNames_, const vector<KSExpressionRef>& body_, const KSScopeRef& s) 
			: scope(s), name(name_), subexpressions(body_), argNames(argNames_), opPrecedence(KSOperatorPrecedence::func) {}
		// default constructor makes a function with no args that returns void
		KSFunction(const string& name, const KSScopeRef& s) : KSFunction(name, [](KSList) { return make_shared<KSValue>(); }, s) {}
		KSFunction() : KSFunction("anon", nullptr) {}

	};

	// describes an expression tree with a function at the root
	struct KSFunctionExpression {
		KSValueRef function;
		vector<KSExpressionRef> subexpressions;

		KSFunctionExpression(const KSFunctionExpression& o) {
			function = o.function;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<KSExpression>(*sub));
			}
		}
		KSFunctionExpression(KSFunctionRef fnc) : function(new KSValue(fnc)) {}
		KSFunctionExpression(KSValueRef fncvalue) : function(fncvalue) {}
		KSFunctionExpression() {}

		void clear() {
			subexpressions.clear();
		}

		~KSFunctionExpression() {
			clear();
		}
	};

	struct KSReturn {
		KSExpressionRef expression;

		KSReturn(const KSReturn& o) {
			expression = o.expression ? make_shared<KSExpression>(*o.expression) : nullptr;
		}
		KSReturn(KSExpressionRef e) : expression(e) {}
		KSReturn() {}
	};

	struct KSIf {
		KSExpressionRef testExpression;
		vector<KSExpressionRef> subexpressions;

		KSIf(const KSIf& o) {
			testExpression = o.testExpression ? make_shared<KSExpression>(*o.testExpression) : nullptr;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<KSExpression>(*sub));
			}
		}
		KSIf() {}
	};

	using KSIfElse = vector<KSIf>;

	struct KSLoop {
		KSExpressionRef initExpression;
		KSExpressionRef testExpression;
		KSExpressionRef iterateExpression;
		vector<KSExpressionRef> subexpressions;

		KSLoop(const KSLoop& o) {
			initExpression = o.initExpression ? make_shared<KSExpression>(*o.initExpression) : nullptr;
			testExpression = o.testExpression ? make_shared<KSExpression>(*o.testExpression) : nullptr;
			iterateExpression = o.iterateExpression ? make_shared<KSExpression>(*o.iterateExpression) : nullptr;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<KSExpression>(*sub));
			}
		}
		KSLoop() {}
	};

	struct KSForeach {
		KSExpressionRef list;
		string iterateName;
		vector<KSExpressionRef> subexpressions;

		KSForeach(const KSForeach& o) {
			list = o.list;
			iterateName = o.iterateName;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<KSExpression>(*sub));
			}
		}
		KSForeach() {}
	};

	enum class KSExpressionType : uint8_t {
		NONE,
		VALUE,
		FUNCTIONDEF,
		FUNCTIONCALL,
		RETURN,
		LOOP,
		FOREACH,
		IFELSE
	};

	// forward declare so we can use the parser to process functions
	class KataScriptInterpreter;
	// describes a 'generic' expression tree, with either a value or function at the root
	struct KSExpression {
		// either we have a value, or a function expression which then has sub-expressions
		KSExpressionType type;
		KSFunctionExpression expr;
		KSLoop loop;
		KSForeach foreach;
		KSIfElse ifelse;
		KSValueRef value;
		KSReturn returnExpression;
		KSExpressionRef parent = nullptr;

		KSExpression(KSValueRef val) : type(KSExpressionType::FUNCTIONCALL), expr(val), parent(nullptr) {}
		KSExpression(KSFunctionRef fnc, KSExpressionRef par) : type(KSExpressionType::FUNCTIONDEF), expr(fnc), parent(par) {}
		KSExpression(KSValueRef val, KSExpressionRef par) : type(KSExpressionType::VALUE), value(val), parent(par) {}
		KSExpression(KSForeach val, KSExpressionRef par = nullptr) : type(KSExpressionType::FOREACH), foreach(val), parent(par) {}
		KSExpression(KSLoop val, KSExpressionRef par = nullptr) : type(KSExpressionType::LOOP), loop(val), parent(par) {}
		KSExpression(KSIfElse val, KSExpressionRef par = nullptr) : type(KSExpressionType::IFELSE), ifelse(val), parent(par) {}
		KSExpression(KSReturn val, KSExpressionRef par = nullptr) : type(KSExpressionType::RETURN), returnExpression(val), parent(par) {}

		bool isCompletedIfElse() {
			return (ifelse.size() > 1 &&
				ifelse.back().testExpression &&
				ifelse.back().testExpression->type == KSExpressionType::VALUE &&
				ifelse.back().testExpression->value->getBool()
				);
		}

		KSExpressionRef back() {
			switch (type) {
			case KSExpressionType::FUNCTIONDEF:
				return expr.function->getFunction()->subexpressions.back();
				break;
			case KSExpressionType::FUNCTIONCALL:
				return expr.subexpressions.back();
				break;
			case KSExpressionType::LOOP:
				return loop.subexpressions.back();
				break;
			case KSExpressionType::FOREACH:
				return foreach.subexpressions.back();
				break;
			case KSExpressionType::IFELSE:
				return ifelse.back().subexpressions.back();
				break;
			default:
				break;
			}
			return nullptr;
		}

		void push_back(KSExpressionRef ref) {
			switch (type) {
			case KSExpressionType::FUNCTIONCALL:
				expr.subexpressions.push_back(ref);
				break;
			case KSExpressionType::FUNCTIONDEF:
				expr.function->getFunction()->subexpressions.push_back(ref);
				break;
			case KSExpressionType::LOOP:
				loop.subexpressions.push_back(ref);
				break;
			case KSExpressionType::FOREACH:
				foreach.subexpressions.push_back(ref);
				break;
			case KSExpressionType::IFELSE:
				ifelse.back().subexpressions.push_back(ref);
				break;
			default:
				break;
			}
		}

		void push_back(const KSIf& ref) {
			switch (type) {
			case KSExpressionType::IFELSE:
				ifelse.push_back(ref);
				break;
			default:
				break;
			}
		}

		// walk the tree depth first and replace any function expressions 
		// with a value expression of their results
		void consolidate(KataScriptInterpreter* i);
	};

	struct KSScope {
		// this is the main storage object for all functions and variables
		string name;
		KSScopeRef parent;
		bool keepAlive;
		unordered_map<string, KSValueRef> variables;
		unordered_map<string, KSScopeRef> scopes; 
		unordered_map<string, KSFunctionRef> functions;
		KSScope(const string& name_, KSScopeRef scope, bool persist = false) : name(name_), parent(scope), keepAlive(persist) {}
	};

	// state enum for state machine for token by token parsing
	enum class KSParseState : uint8_t {
		beginExpression,
		readLine,
		defineFunc,
		funcArgs,
		returnLine,		
		ifCall,
		expectIfEnd,
		loopCall,
		forEach,
	};

	// finally we have our interpereter
	class KataScriptInterpreter {
		KSScopeRef globalScope = make_shared<KSScope>("global", nullptr);
		KSScopeRef currentScope = globalScope;

		KSExpressionRef currentExpression;

		vector<KSParseState> parseStack = { KSParseState::beginExpression };
		vector<string> parseStrings;
		vector<char> parseOperations;
		vector<KSType> parseTypes;
		int outerNestLayer = 0;
		
		KSFunctionRef resolveFunction(const string& name);
		KSExpressionRef getExpression(const vector<string>& strings);
		KSValueRef getValue(const vector<string>& strings);
		
		void clearParseStacks();
		void closeDanglingIfExpression();
		void parse(const string& token);
	public:
		// just public for access within this file
		KSFunctionRef& newFunction(const string& name, const vector<string>& argNames, const vector<KSExpressionRef>& body);
		KSValueRef getValue(KSExpressionRef expr);
		void newScope(const string& name, bool keepAlive = false);
		void closeCurrentScope();
		bool closeCurrentExpression();
		// actual public API
		KSFunctionRef& newFunction(const string& name, const KSLambda& lam);
		KSValueRef callFunction(const string& name, const KSList& args);
		KSValueRef callFunction(const KSFunctionRef fnc, const KSList& args);
		KSValueRef& resolveVariable(const string& name, KSScopeRef = nullptr);
		void readLine(const string& text);
		void evaluate(const string& script);
		KataScriptInterpreter();
	};

#ifdef KATASCRIPT_IMPL
	// implementations

	// tokenizer special characters
	const string WhitespaceChars = " \t\n"s;
	const string GrammarChars = " \t\n,(){}[];+-/*%<>=!&|\""s;
	const string MultiCharTokenStartChars = "+-/*<>=!&|"s;
	const string NumericChars = "0123456789."s;
	const string NumericStartChars = "0123456789.-"s;

	inline vector<string> split(const string& input, char delimiter) {
		vector<string> ret;
		if (input.empty()) return ret;
		size_t pos = 0;
		size_t lpos = 0;
		while ((pos = input.find(delimiter, lpos)) != string::npos) {
			ret.push_back(input.substr(lpos, pos - lpos));
			lpos = pos + 1;
		}
		ret.push_back(input.substr(lpos, input.length()));
		return ret;
	}

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
				if (input[pos] == '\"' && pos > 0 && input[pos-1] != '\\') {
					auto testpos = lpos + 1;
					auto unescaped = "\""s;
					bool loop = true;
					while (loop) {						
						pos = input.find('\"', testpos);
						if (pos == string::npos) {
							throw std::runtime_error(stringformat("Quote mismatch at %s", input.substr(lpos, input.size() - lpos).c_str()).c_str());
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
					ret.push_back(unescaped);
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
			return contains("=+-&|"s, test[1]) && contains("<>=!+-&|"s, test[0]);
		}
		return false;
	}

	// scope control lets you have object lifetimes
	void KataScriptInterpreter::newScope(const string& name, bool keepAlive) {
		// if the scope exists we just use it as is
		auto iter = currentScope->scopes.find(name);
		if (iter != currentScope->scopes.end()) {
			currentScope = iter->second;
		} else {
			auto parent = currentScope;
			auto& newScope = currentScope->scopes[name];
			newScope = make_shared<KSScope>(name, parent, keepAlive);
			currentScope = newScope;
		}
	}

	void KataScriptInterpreter::closeCurrentScope() {
		if (currentScope->parent) {
			if (currentScope->keepAlive) {
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

	// call function by reference
	KSValueRef KataScriptInterpreter::callFunction(const KSFunctionRef fnc, const KSList& args) {
		if (fnc->subexpressions.size()) {
			newScope(fnc->name);
			int limit = (int)min(args.size(), fnc->argNames.size());
			for (int i = 0; i < limit; ++i) {
				*resolveVariable(fnc->argNames[i], fnc->scope) = *args[i];
			}
			KSValueRef returnVal = nullptr;
			for (auto&& sub : fnc->subexpressions) {
				if (sub->type == KSExpressionType::RETURN) {
					returnVal = getValue(sub);
					break;
				} else{
					getValue(sub);
				}
			}
			closeCurrentScope();
			return returnVal ? returnVal : make_shared<KSValue>();
		} else {
			return fnc->lambda(args);
		}
	}

	KSFunctionRef& KataScriptInterpreter::newFunction(const string& name, const vector<string>& argNames, const vector<KSExpressionRef>& body) {
		auto& ref = currentScope->functions[name];
		auto parentScope = currentScope;
		newScope(name, true);
		for (auto&& arg : argNames) {
			currentScope->variables[arg] = make_shared<KSValue>();
		}
		ref = make_shared<KSFunction>(name, argNames, body, currentScope);
		auto& funcvar = resolveVariable(name, parentScope);
		funcvar->type = KSType::FUNCTION;
		funcvar->value = ref;
		return ref;
	}

	KSFunctionRef& KataScriptInterpreter::newFunction(const string& name, const KSLambda& lam) {
		auto& ref = currentScope->functions[name];
		auto parentScope = currentScope;
		newScope(name, true);
		closeCurrentScope();
		ref = make_shared<KSFunction>(name, lam, currentScope);
		auto& funcvar = resolveVariable(name, parentScope);
		funcvar->type = KSType::FUNCTION;
		funcvar->value = ref;
		return ref;
	}

	// name resolution for variables
	KSValueRef& KataScriptInterpreter::resolveVariable(const string& name, KSScopeRef scope) {
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
		auto& varr = initialScope->variables[name];
		varr = make_shared<KSValue>();
		return varr;
	}

	// name resolution for functions
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
		return make_shared<KSFunction>(name, currentScope->scopes[name]);
	}

	// recursively build an expression tree from a list of tokens
	KSExpressionRef KataScriptInterpreter::getExpression(const vector<string>& strings) {
		KSExpressionRef root = nullptr;
		size_t i = 0;
		while (i < strings.size()) {
			if (isMathOperator(strings[i])) {
				auto prev = root;
				root = make_shared<KSExpression>(resolveVariable(strings[i]));
				auto curr = prev;
				if (curr) {
					// find operations of lesser precedence
					if (curr->type == KSExpressionType::FUNCTIONCALL) {
						auto curfunc = curr->expr.function->getFunction();
						auto newfunc = root->expr.function->getFunction();
						if (curfunc && (int)curfunc->opPrecedence < (int)newfunc->opPrecedence) {
							while (curr->expr.subexpressions.back()->type == KSExpressionType::FUNCTIONCALL) {
								auto curfunc = curr->expr.subexpressions.back()->value->getFunction();
								if (curfunc && (int)curfunc->opPrecedence < (int)newfunc->opPrecedence) {
									curr = curr->expr.subexpressions.back();
								} else {
									break;
								}
							}
							// swap values around to correct the otherwise incorect order of operations
							root->expr.subexpressions.push_back(curr->expr.subexpressions.back());
							curr->expr.subexpressions.pop_back();
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
									minisub.push_back(move(strings[i]));
								}
							}
							root->expr.subexpressions.push_back(getExpression(minisub));
							curr->expr.subexpressions.push_back(root);
							root = prev;
						} else {
							root->expr.subexpressions.push_back(curr);
						}
					} else {
						root->expr.subexpressions.push_back(curr);
					}
				}
			} else if (isStringLiteral(strings[i])) {
				// trim quotation marks
				auto val = strings[i].substr(1, strings[i].size() - 2);
				auto newExpr = make_shared<KSExpression>(make_shared<KSValue>(val), nullptr);
				if (root) {
					root->expr.subexpressions.push_back(newExpr);
				} else {
					root = newExpr;
				}
			} else if (strings[i] == "(" || strings[i] == "[" || isVarOrFuncToken(strings[i])) {
				if (strings[i] == "(" || i + 2 < strings.size() && strings[i + 1] == "(") {
					// function
					KSExpressionRef cur = nullptr;
					if (strings[i] == "(") {
						if (root) {
							if (root->type == KSExpressionType::FUNCTIONCALL 
								&& (root->expr.function->getFunction()->opPrecedence == KSOperatorPrecedence::func)) {

								cur = make_shared<KSExpression>(make_shared<KSValue>());
								cur->expr.subexpressions.push_back(root);
								root = cur;
							} else {
								root->expr.subexpressions.push_back(make_shared<KSExpression>(resolveVariable("identity")));
								cur = root->expr.subexpressions.back();
							}
						} else {
							root = make_shared<KSExpression>(resolveVariable("identity"));
							cur = root;
						}
					} else {
						auto var = resolveVariable(strings[i]);
						if (var->type != KSType::NONE && var->type != KSType::FUNCTION) {
							throw std::runtime_error(stringformat("Attempted to call non-function value as function: `%s %s`", getTypeName(var->type).c_str(), var->getPrintString().c_str()).c_str());
						}
						if (root) {
							root->expr.subexpressions.push_back(make_shared<KSExpression>(var));
							cur = root->expr.subexpressions.back();
						} else {
							root = make_shared<KSExpression>(var);
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
								minisub.push_back(move(strings[i]));
							} else {
								if (minisub.size()) {
									cur->expr.subexpressions.push_back(getExpression(minisub));
									minisub.clear();
								}
							}
						} else if (strings[i] == "(" || !(strings[i].size() == 1 && contains("+-*/"s, strings[i][0])) && i + 2 < strings.size() && strings[i + 1] == "(") {
							++nestLayers;
							if (strings[i] == "(") {
								minisub.push_back(move(strings[i]));
							} else {
								minisub.push_back(move(strings[i]));
								minisub.push_back(move(strings[++i]));
							}
						} else {
							minisub.push_back(move(strings[i]));
						}
					}
				} else if (strings[i] == "[" || i + 2 < strings.size() && strings[i + 1] == "[") {
					// list
					bool indexOfIndex = i > 0 && (strings[i - 1] == "]" || strings[i - 1].back() == '\"');
					KSExpressionRef cur = nullptr;
					if (!indexOfIndex && strings[i] == "[") {
						// list literal
						if (root) {
							root->expr.subexpressions.push_back(make_shared<KSExpression>(make_shared<KSValue>(KSList()), nullptr));
							cur = root->expr.subexpressions.back();
						} else {
							root = make_shared<KSExpression>(make_shared<KSValue>(KSList()), nullptr);
							cur = root;
						}
						vector<string> minisub;
						int nestLayers = 1;
						while (nestLayers > 0 && ++i < strings.size()) {
							if (nestLayers == 1 && strings[i] == ",") {
								if (minisub.size()) {
									cur->value->getList().push_back(getValue(minisub));
									minisub.clear();
								}
							} else if (strings[i] == "]") {
								if (--nestLayers > 0) {
									minisub.push_back(move(strings[i]));
								} else {
									if (minisub.size()) {
										cur->value->getList().push_back(getValue(minisub));
										minisub.clear();
									}
								}
							} else if (strings[i] == "[") {
								++nestLayers;
								minisub.push_back(move(strings[i]));
							} else {
								minisub.push_back(move(strings[i]));
							}
						} 
					} else {
						// list access
						if (indexOfIndex) {
							cur = make_shared<KSExpression>(resolveVariable("listindex"));
							cur->expr.subexpressions.push_back(root);
							root = cur;
						} else {
							if (root) {
								root->expr.subexpressions.push_back(make_shared<KSExpression>(resolveVariable("listindex")));
								cur = root->expr.subexpressions.back();
							} else {
								root = make_shared<KSExpression>(resolveVariable("listindex"));
								cur = root;
							}
							auto var = resolveVariable(strings[i]);
							if (var->type != KSType::LIST) {
								var = make_shared<KSValue>(var->value, var->type);
								var->upconvert(KSType::LIST);
							}
							cur->expr.subexpressions.push_back(make_shared<KSExpression>(var, nullptr));
							++i;
						}
						
						vector<string> minisub;
						int nestLayers = 1;
						while (nestLayers > 0 && ++i < strings.size()) {
							if (strings[i] == "]") {
								if (--nestLayers > 0) {
									minisub.push_back(move(strings[i]));
								} else {
									if (minisub.size()) {
										cur->expr.subexpressions.push_back(getExpression(minisub));
										minisub.clear();
									}
								}
							} else if (strings[i] == "[") {
								++nestLayers;
								minisub.push_back(move(strings[i]));
							} else {
								minisub.push_back(move(strings[i]));
							}
						}
					}
					
				} else {
					// variable
					KSExpressionRef newExpr;
					if (strings[i] == "true") {
						newExpr = make_shared<KSExpression>(make_shared<KSValue>(1), nullptr);
					} else if (strings[i] == "false") {
						newExpr = make_shared<KSExpression>(make_shared<KSValue>(0), nullptr);
					} else { 
						newExpr = make_shared<KSExpression>(resolveVariable(strings[i]), nullptr);
					}
					if (root) {
						root->expr.subexpressions.push_back(newExpr);
					} else {
						root = newExpr;
					}
				}
			} else {
				// number
				auto val = fromChars(strings[i]);
				auto newExpr = make_shared<KSExpression>(KSValueRef(contains(strings[i], '.') ? new KSValue((float)val) : new KSValue((int)val)), nullptr);
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
	void KSExpression::consolidate(KataScriptInterpreter* i) {
		switch (type) {
		case KSExpressionType::RETURN:
			value = i->getValue(returnExpression.expression);
			type = KSExpressionType::VALUE;
			break;
		case KSExpressionType::FUNCTIONCALL:
		{
			KSList args;
			for (auto&& sub : expr.subexpressions) {
				sub->consolidate(i);
				args.push_back(sub->value);
			}
			if (expr.function->type == KSType::NONE) {
				if (args.size() && args[0]->type == KSType::FUNCTION) {
					expr.function = args[0];
					args.erase(args.begin());
				} else {
					throw std::runtime_error("Unable to call non-existant function");
				}
			}
			value = i->callFunction(expr.function->getFunction(), args);
			type = KSExpressionType::VALUE;
			expr.clear();
		}
			break;
		case KSExpressionType::LOOP:
		{
			i->newScope("loop");
			if (loop.initExpression) {
				i->getValue(loop.initExpression);
			}
			while (i->getValue(loop.testExpression)->getBool()) {				
				for (auto&& expr : loop.subexpressions) {
					i->getValue(expr);
				}
				if (loop.iterateExpression) {
					i->getValue(loop.iterateExpression);
				}
			}
			value = 0;
			type = KSExpressionType::VALUE;
			i->closeCurrentScope();
		}
			break;
		case KSExpressionType::FOREACH:
		{
			i->newScope("loop");
			auto& var = i->resolveVariable(foreach.iterateName);
			auto list = *foreach.list;
			list.consolidate(i);
			for (auto&& in : list.value->getList()) {
				*var = *in;
				for (auto&& expr : foreach.subexpressions) {
					i->getValue(expr);
				}
			}
			value = 0;
			type = KSExpressionType::VALUE;
			i->closeCurrentScope();
		}
		break;
		case KSExpressionType::IFELSE:
		{
			for (auto& express : ifelse) {
				if (!express.testExpression || i->getValue(express.testExpression)->getBool()) {
					i->newScope("ifelse");
					for (auto sub : express.subexpressions) {
						i->getValue(sub);
					}
					i->closeCurrentScope();
					break;
				}
			}
		}
			break;
		default:
			break;
		}
	}

	// evaluate an expression from tokens
	KSValueRef KataScriptInterpreter::getValue(const vector<string>& strings) {
		auto expr = getExpression(strings);
		expr->consolidate(this);
		return expr->value;
	}

	// evaluate an expression from expressionRef
	KSValueRef KataScriptInterpreter::getValue(KSExpressionRef exp) {
		// copy the expression so that we don't lose it when we consolidate
		auto expr = *exp;
		expr.consolidate(this);
		return expr.value;
	}

	// general purpose clear to reset state machine for next statement
	void KataScriptInterpreter::clearParseStacks() {
		parseStack.clear();
		parseStack.push_back(KSParseState::beginExpression);
		parseStrings.clear();
		parseOperations.clear();
		parseTypes.clear();
		outerNestLayer = 0;
	}

	// since the 'else' block in  an if/elfe is technically in a different scope
	// ifelse espressions are not closed immediately and instead left dangling
	// until the next expression is anything other than an 'else' or the else is unconditional
	void KataScriptInterpreter::closeDanglingIfExpression() {
		if (currentExpression && currentExpression->type == KSExpressionType::IFELSE) {
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
			if (currentExpression->type != KSExpressionType::IFELSE || currentExpression->isCompletedIfElse()) {
				if (currentExpression->parent) {
					currentExpression = currentExpression->parent;
				} else {
					if (currentExpression->type != KSExpressionType::FUNCTIONDEF) {
						getValue(currentExpression);
					}
					currentExpression = nullptr;
				}
				return true;
			}
		}
		return false;
	}

	bool lastStatementClosed = false;
	// parse one token at a time, uses the state machine
	void KataScriptInterpreter::parse(const string& token) {
		switch (parseStack.back()) {
		case KSParseState::beginExpression:
		{
			bool wasElse = false;
			bool closeScope = false;
			bool closedExpr = false;
			if (token == "func") {
				parseStack.push_back(KSParseState::defineFunc);
			} else if (token == "for" || token == "while") {
				parseStack.push_back(KSParseState::loopCall);
				if (currentExpression) {
					auto newexpr = make_shared<KSExpression>(KSLoop(), currentExpression);
					currentExpression->push_back(newexpr);
					currentExpression = newexpr;
				} else {
					currentExpression = make_shared<KSExpression>(KSLoop());
				}
			} else if (token == "foreach") {
				parseStack.push_back(KSParseState::forEach);
				if (currentExpression) {
					auto newexpr = make_shared<KSExpression>(KSForeach(), currentExpression);
					currentExpression->push_back(newexpr);
					currentExpression = newexpr;
				} else {
					currentExpression = make_shared<KSExpression>(KSForeach());
				}
			} else if (token == "if") {
				parseStack.push_back(KSParseState::ifCall);
				if (currentExpression) {
					auto newexpr = make_shared<KSExpression>(KSIfElse(), currentExpression);
					currentExpression->push_back(newexpr);
					currentExpression = newexpr;
				} else {
					currentExpression = make_shared<KSExpression>(KSIfElse());
				}
				wasElse = true;
			} else if (token == "else") {
				parseStack.push_back(KSParseState::expectIfEnd);
				wasElse = true;
			} else if (token == "{") {
				bool isFunc = currentExpression && currentExpression->type == KSExpressionType::FUNCTIONDEF;
				if (!isFunc) {
					newScope("anon"s);
				}
				clearParseStacks();
			} else if (token == "}") {
				closedExpr = closeCurrentExpression();
				closeCurrentScope();
				closeScope = true;
				wasElse = true;
			} else if (token == "return") {
				parseStack.push_back(KSParseState::returnLine);
			} else if (token == ";") {
				clearParseStacks();
			} else {
				parseStack.push_back(KSParseState::readLine);
				parseStrings.push_back(token);
			}
			if (!closedExpr && 
				((closeScope && (lastStatementClosed || currentExpression && currentExpression->isCompletedIfElse())) 
					|| (!wasElse && lastStatementClosed))) {
				closeDanglingIfExpression();
			}
			lastStatementClosed = closeScope;
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
					switch (exprs.size()) {
					case 1:
						currentExpression->loop.testExpression = getExpression(move(exprs[0]));
						break;
					case 2:
						currentExpression->loop.testExpression = getExpression(move(exprs[0]));
						currentExpression->loop.iterateExpression = getExpression(move(exprs[1]));
						break;
					case 3:
						currentExpression->loop.initExpression = getExpression(move(exprs[0]));
						currentExpression->loop.testExpression = getExpression(move(exprs[1]));
						currentExpression->loop.iterateExpression = getExpression(move(exprs[2]));
						break;
					default:
						break;
					}

					clearParseStacks();
					outerNestLayer = 0;
				} else {
					parseStrings.push_back(move(token));
				}
			} else if (token == "(") {
				if (++outerNestLayer > 1) {
					parseStrings.push_back(move(token));
				}
			} else {
				parseStrings.push_back(move(token));
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
						throw std::runtime_error(stringformat("Syntax error, `foreach` requires 2 statements, %i statements supplied instead", (int)exprs.size()).c_str());
					}

					resolveVariable(exprs[0][0]);
					currentExpression->foreach.iterateName = exprs[0][0];
					currentExpression->foreach.list = getExpression(exprs[1]);

					clearParseStacks();
					outerNestLayer = 0;
				} else {
					parseStrings.push_back(move(token));
				}
			} else if (token == "(") {
				if (++outerNestLayer > 1) {
					parseStrings.push_back(move(token));
				}
			} else {
				parseStrings.push_back(move(token));
			}
			break;
		case KSParseState::ifCall:			
			if (token == ")") {
				if (--outerNestLayer <= 0) {
					currentExpression->push_back(KSIf());
					currentExpression->ifelse.back().testExpression = getExpression(parseStrings);
					clearParseStacks();
				} else {
					parseStrings.push_back(move(token));
				}
			} else if (token == "(") {
				if (++outerNestLayer > 1) {
					parseStrings.push_back(move(token));
				}
			} else {
				parseStrings.push_back(move(token));
			}
			break;
		case KSParseState::readLine:
			if (token == "{") {
				newScope("anon"s);
			} else if (token == ";") {
				auto line = move(parseStrings);
				clearParseStacks();
				if (!currentExpression) {
					getValue(line);
				} else {
					currentExpression->push_back(getExpression(line));
				}
			} else {
				parseStrings.push_back(move(token));
			}
			break;
		case KSParseState::returnLine:
			if (token == ";") {
				if (currentExpression) {
					currentExpression->push_back(make_shared<KSExpression>(KSReturn(getExpression(move(parseStrings)))));
				}
				clearParseStacks();				
			} else {
				parseStrings.push_back(move(token));
			}
			break;
		case KSParseState::expectIfEnd:
			if (token == "if") {
				clearParseStacks();
				parseStack.push_back(KSParseState::ifCall);
			} else if (token == "{") {
				newScope("anon"s);
				currentExpression->push_back(KSIf());
				currentExpression->ifelse.back().testExpression = make_shared<KSExpression>(make_shared<KSValue>(true), nullptr);
				clearParseStacks();
			} else {
				throw std::runtime_error(stringformat("Malformed Syntax: Incorrect token `%s` following `else` keyword",
					token.c_str()).c_str());
			}
			break;
		case KSParseState::defineFunc:
			parseStrings.push_back(move(token));
			parseStack.push_back(KSParseState::funcArgs);
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

				clearParseStacks();
			} else {
				parseStrings.push_back(move(token));
			}
			break;
		default:
			break;
		}
	}

	void KataScriptInterpreter::readLine(const string& text) {
		try {
			for (auto&& token : KSTokenize(text)) {
				parse(token);
			}
		} catch (std::exception e) {
			printf("Error: %s\n", e.what());
		}
	}

	void KataScriptInterpreter::evaluate(const string& script) {
		for (auto&& line : split(script, '\n')) {
			readLine(line);
		}
	}

	KataScriptInterpreter::KataScriptInterpreter() {
		// register compiled functions and standard library:
		newFunction("identity", [](KSList args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			return args[0];
			});

		newFunction("listindex", [](KSList args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			if (args.size() == 1) {
				return args[0];
			}
			args[1]->hardconvert(KSType::INT);
			auto ival = args[1]->getInt();

			auto var = args[0];
			if (var->type != KSType::LIST) {
				var = make_shared<KSValue>(var->value, var->type);
				var->upconvert(KSType::LIST);
			}

			auto& list = var->getList();
			if (ival < 0 || ival >= list.size()) {
				throw std::runtime_error(stringformat("Out of bounds list access index %i, list length %i",
					ival, list.size()).c_str());
			} else {
				return list[ival];
			}
			});

		newFunction("sqrt", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>(0.f);
			}
			args[0]->hardconvert(KSType::FLOAT);
			args[0]->value = sqrtf(get<float>(args[0]->value));
			return args[0];
			});

		newFunction("print", [](const KSList& args) {
			for (auto&& arg : args) {
				printf("%s", arg->getPrintString().c_str());
			}
			printf("\n");
			return make_shared<KSValue>();
			});

		newFunction("getline", [](const KSList& args) {
			string s;
			// blocking calls are fine
			getline(std::cin, s);
			return make_shared<KSValue>(s);
			});

		newFunction("=", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			if (args.size() == 1) {
				return args[0];
			}
			*args[0] = *args[1];
			return args[0];
			});

		newFunction("+", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			if (args.size() == 1) {
				return args[0];
			}
			return make_shared<KSValue>(*args[0] + *args[1]);
			});

		newFunction("-", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			if (args.size() == 1) {
				auto zero = KSValue(0);
				upconvert(*args[0], zero);
				return make_shared<KSValue>(zero - *args[0]);
			}
			return make_shared<KSValue>(*args[0] - *args[1]);
			});

		newFunction("*", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>();
			}
			return make_shared<KSValue>(*args[0] * *args[1]);
			});

		newFunction("/", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>();
			}
			return make_shared<KSValue>(*args[0] / *args[1]);
			});

		newFunction("%", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>();
			}
			return make_shared<KSValue>(*args[0] % *args[1]);
			});

		newFunction("==", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(*args[0] == *args[1]);
			});

		newFunction("!=", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(*args[0] != *args[1]);
			});

		newFunction("||", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(1);
			}
			return make_shared<KSValue>(*args[0] || *args[1]);
			});

		newFunction("&&", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(*args[0] && *args[1]);
			});

		newFunction("++", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			auto t = KSValue(1);
			*args[0] = *args[0] + t;
			return args[0];
			});

		newFunction("--", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			auto t = KSValue(1);
			*args[0] = *args[0] - t;
			return args[0];
			});

		newFunction("+=", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			if (args.size() == 1) {
				return args[0];
			}
			*args[0] = *args[0] + *args[1];
			return args[0];
			});

		newFunction("-=", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			if (args.size() == 1) {
				return args[0];
			}
			*args[0] = *args[0] - *args[1];
			return args[0];
			});

		newFunction(">", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(*args[0] > *args[1]);
			});

		newFunction("<", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(*args[0] < *args[1]);
			});

		newFunction(">=", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(*args[0] >= *args[1]);
			});

		newFunction("<=", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(*args[0] <= *args[1]);
			});

		newFunction("!", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(!args[0]->getBool());
			});

		newFunction("length", [](const KSList& args) {
			if (args.size() == 0 || (int)args[0]->type < (int)KSType::STRING) {
				return make_shared<KSValue>(0);
			}
			if (args[0]->type == KSType::STRING) {
				return make_shared<KSValue>((int)args[0]->getString().size());
			}
			return make_shared<KSValue>((int)args[0]->getList().size());
			});

		newFunction("bool", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>(0);
			}
			auto val = *args[0];
			val.hardconvert(KSType::INT);
			val.value = args[0]->getBool();
			return make_shared<KSValue>(val);
			});

		newFunction("int", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>(0);
			}
			auto val = *args[0];
			val.hardconvert(KSType::INT);
			return make_shared<KSValue>(val);
			});

		newFunction("float", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>(0.f);
			}
			auto val = *args[0];
			val.hardconvert(KSType::FLOAT);
			return make_shared<KSValue>(val);
			});

		newFunction("string", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>(""s);
			}
			auto val = *args[0];
			val.hardconvert(KSType::STRING);
			return make_shared<KSValue>(val);
			});

		newFunction("list", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>(""s);
			}
			auto val = *args[0];
			val.hardconvert(KSType::LIST);
			return make_shared<KSValue>(val);
			});
	}
#endif
}
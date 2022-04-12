#pragma once
#include "types.hpp"

namespace KataScript {
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
        KSExpressionRef listExpression;
		string iterateName;
		vector<KSExpressionRef> subexpressions;

		KSForeach(const KSForeach& o) {
            listExpression = o.listExpression ? make_shared<KSExpression>(*o.listExpression) : nullptr;
			iterateName = o.iterateName;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<KSExpression>(*sub));
			}
		}
		KSForeach() {}
	};

    struct KSResolveVar {
        string name;

        KSResolveVar(const KSResolveVar& o) {
            name = o.name;
        }
        KSResolveVar() {}
        KSResolveVar(const string& n) : name(n) {}
    };

    struct KSDefineVar {
        string name;
        KSExpressionRef defineExpression;

        KSDefineVar(const KSDefineVar& o) {
            name = o.name;
            defineExpression = o.defineExpression ? make_shared<KSExpression>(*o.defineExpression) : nullptr;
        }
        KSDefineVar() {}
        KSDefineVar(const string& n) : name(n) {}
        KSDefineVar(const string& n, KSExpressionRef defExpr) : name(n), defineExpression(defExpr) {}
    };

	enum class KSExpressionType : uint8_t {
        Value,
        ResolveVar,
        DefineVar,
		FunctionDef,
        FunctionCall,
		Return,
		Loop,
		ForEach,
		IfElse
	};

    using KSExpressionVariant = 
        variant<
        KSValueRef,
        KSResolveVar, 
        KSDefineVar, 
        KSFunctionExpression,
        KSReturn, 
        KSLoop, 
        KSForeach,
        KSIfElse
        >;

	// forward declare so we can use the parser to process functions
	class KataScriptInterpreter;
	// describes a 'generic' expression tree, with either a value or function at the root
	struct KSExpression {
		// either we have a value, or a function expression which then has sub-expressions
        KSExpressionVariant expression;
        KSExpressionType type;
		KSExpressionRef parent = nullptr;

		KSExpression(KSValueRef val) 
            : type(KSExpressionType::FunctionCall), expression(KSFunctionExpression(val)), parent(nullptr) {}
		KSExpression(KSFunctionRef val, KSExpressionRef par) 
            : type(KSExpressionType::FunctionDef), expression(KSFunctionExpression(val)), parent(par) {}
		KSExpression(KSValueRef val, KSExpressionRef par) 
            : type(KSExpressionType::Value), expression(val), parent(par) {}
		KSExpression(KSForeach val, KSExpressionRef par = nullptr) 
            : type(KSExpressionType::ForEach), expression(val), parent(par) {}
		KSExpression(KSLoop val, KSExpressionRef par = nullptr) 
            : type(KSExpressionType::Loop), expression(val), parent(par) {}
		KSExpression(KSIfElse val, KSExpressionRef par = nullptr) 
            : type(KSExpressionType::IfElse), expression(val), parent(par) {}
		KSExpression(KSReturn val, KSExpressionRef par = nullptr) 
            : type(KSExpressionType::Return), expression(val), parent(par) {}
        KSExpression(KSResolveVar val, KSExpressionRef par = nullptr) 
            : type(KSExpressionType::ResolveVar), expression(val), parent(par) {}
        KSExpression(KSDefineVar val, KSExpressionRef par = nullptr) 
            : type(KSExpressionType::DefineVar), expression(val), parent(par) {}

        KSExpression(KSExpressionVariant val, KSExpressionType ty)
            : type(ty), expression(val) {}

		KSExpressionRef back() {
			switch (type) {
			case KSExpressionType::FunctionDef:
				return get<KSFunctionExpression>(expression).function->getFunction()->subexpressions.back();
				break;
			case KSExpressionType::FunctionCall:
				return get<KSFunctionExpression>(expression).subexpressions.back();
				break;
			case KSExpressionType::Loop:
				return get<KSLoop>(expression).subexpressions.back();
				break;
			case KSExpressionType::ForEach:
				return get<KSForeach>(expression).subexpressions.back();
				break;
			case KSExpressionType::IfElse:
				return get<KSIfElse>(expression).back().subexpressions.back();
				break;
			default:
				break;
			}
			return nullptr;
		}

        auto begin() {
            switch (type) {
            case KSExpressionType::FunctionCall:
                return get<KSFunctionExpression>(expression).subexpressions.begin();
                break;
            case KSExpressionType::FunctionDef:
                return get<KSFunctionExpression>(expression).function->getFunction()->subexpressions.begin();
                break;
            case KSExpressionType::Loop:
                return get<KSLoop>(expression).subexpressions.begin();
                break;
            case KSExpressionType::ForEach:
                return get<KSForeach>(expression).subexpressions.begin();
                break;
            case KSExpressionType::IfElse:
                return get<KSIfElse>(expression).back().subexpressions.begin();
                break;
            default:
                return vector<KSExpressionRef>::iterator();
                break;
            }
        }

        auto end() {
            switch (type) {
            case KSExpressionType::FunctionCall:
                return get<KSFunctionExpression>(expression).subexpressions.end();
                break;
            case KSExpressionType::FunctionDef:
                return get<KSFunctionExpression>(expression).function->getFunction()->subexpressions.end();
                break;
            case KSExpressionType::Loop:
                return get<KSLoop>(expression).subexpressions.end();
                break;
            case KSExpressionType::ForEach:
                return get<KSForeach>(expression).subexpressions.end();
                break;
            case KSExpressionType::IfElse:
                return get<KSIfElse>(expression).back().subexpressions.end();
                break;
            default:
                return vector<KSExpressionRef>::iterator();
                break;
            }
        }

		void push_back(KSExpressionRef ref) {
			switch (type) {
			case KSExpressionType::FunctionCall:
                get<KSFunctionExpression>(expression).subexpressions.push_back(ref);
				break;
			case KSExpressionType::FunctionDef:
                get<KSFunctionExpression>(expression).function->getFunction()->subexpressions.push_back(ref);
				break;
			case KSExpressionType::Loop:
                get<KSLoop>(expression).subexpressions.push_back(ref);
				break;
			case KSExpressionType::ForEach:
                get<KSForeach>(expression).subexpressions.push_back(ref);
				break;
			case KSExpressionType::IfElse:
                get<KSIfElse>(expression).back().subexpressions.push_back(ref);
				break;
			default:
				break;
			}
		}

		void push_back(const KSIf& ref) {
			switch (type) {
			case KSExpressionType::IfElse:
                get<KSIfElse>(expression).push_back(ref);
				break;
			default:
				break;
			}
		}

        bool needsToReturn(KSExpressionRef expr, KSValueRef& returnVal, KataScriptInterpreter* i) const;

        // walk the tree depth first and replace any function expressions 
		// with a value expression of their results
        KSExpressionRef consolidated(KataScriptInterpreter* i) const;
	};
}
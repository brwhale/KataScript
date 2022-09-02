#pragma once
#include "types.hpp"

namespace KataScript {
    // describes an expression tree with a function at the root
	struct FunctionExpression {
		ValueRef function;
		vector<ExpressionRef> subexpressions;

		FunctionExpression(const FunctionExpression& o) {
			function = o.function;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<Expression>(*sub));
			}
		}
		FunctionExpression(FunctionRef fnc) : function(new Value(fnc)) {}
		FunctionExpression(ValueRef fncvalue) : function(fncvalue) {}
		FunctionExpression() {}

		void clear() {
			subexpressions.clear();
		}

		~FunctionExpression() {
			clear();
		}
	};

    struct MemberVariable {
        ExpressionRef object;
		string name;

		MemberVariable(const MemberVariable& o) {
            object = o.object;
			name = o.name;
		}
		MemberVariable(ExpressionRef ob, const string& name_) : object(ob), name(name_) {}
		MemberVariable() {}
	};

    struct MemberFunctionCall {
        ExpressionRef object;
		string functionName;
		vector<ExpressionRef> subexpressions;

		MemberFunctionCall(const MemberFunctionCall& o) {
            object = o.object;
			functionName = o.functionName;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<Expression>(*sub));
			}
		}
		MemberFunctionCall(ExpressionRef ob, const string& fncvalue, const vector<ExpressionRef>& sub) 
            : object(ob), functionName(fncvalue), subexpressions(sub) {}
		MemberFunctionCall() {}

		void clear() {
			subexpressions.clear();
		}

		~MemberFunctionCall() {
			clear();
		}
	};

	struct Return {
		ExpressionRef expression;

		Return(const Return& o) {
			expression = o.expression ? make_shared<Expression>(*o.expression) : nullptr;
		}
		Return(ExpressionRef e) : expression(e) {}
		Return() {}
	};

	struct If {
		ExpressionRef testExpression;
		vector<ExpressionRef> subexpressions;

		If(const If& o) {
			testExpression = o.testExpression ? make_shared<Expression>(*o.testExpression) : nullptr;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<Expression>(*sub));
			}
		}
		If() {}
	};

	using IfElse = vector<If>;

	struct Loop {
		ExpressionRef initExpression;
		ExpressionRef testExpression;
		ExpressionRef iterateExpression;
		vector<ExpressionRef> subexpressions;

		Loop(const Loop& o) {
			initExpression = o.initExpression ? make_shared<Expression>(*o.initExpression) : nullptr;
			testExpression = o.testExpression ? make_shared<Expression>(*o.testExpression) : nullptr;
			iterateExpression = o.iterateExpression ? make_shared<Expression>(*o.iterateExpression) : nullptr;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<Expression>(*sub));
			}
		}
		Loop() {}
	};

	struct Foreach {
        ExpressionRef listExpression;
		string iterateName;
		vector<ExpressionRef> subexpressions;

		Foreach(const Foreach& o) {
            listExpression = o.listExpression ? make_shared<Expression>(*o.listExpression) : nullptr;
			iterateName = o.iterateName;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<Expression>(*sub));
			}
		}
		Foreach() {}
	};

    struct ResolveVar {
        string name;

        ResolveVar(const ResolveVar& o) {
            name = o.name;
        }
        ResolveVar() {}
        ResolveVar(const string& n) : name(n) {}
    };

    struct ResolveFuncVar {
        string name;

        ResolveFuncVar(const ResolveFuncVar& o) {
            name = o.name;
        }
        ResolveFuncVar() {}
        ResolveFuncVar(const string& n) : name(n) {}
    };

    struct ResolveClassVar {
        string name;

        ResolveClassVar(const ResolveClassVar& o) {
            name = o.name;
        }
        ResolveClassVar() {}
        ResolveClassVar(const string& n) : name(n) {}
    };

    struct DefineVar {
        string name;
        ExpressionRef defineExpression;

        DefineVar(const DefineVar& o) {
            name = o.name;
            defineExpression = o.defineExpression ? make_shared<Expression>(*o.defineExpression) : nullptr;
        }
        DefineVar() {}
        DefineVar(const string& n) : name(n) {}
        DefineVar(const string& n, ExpressionRef defExpr) : name(n), defineExpression(defExpr) {}
    };

	enum class ExpressionType : uint8_t {
        Value,
        ResolveClassVar,
        ResolveFuncVar,
        ResolveVar,
        DefineVar,
		FunctionDef,
        FunctionCall,
        MemberFunctionCall,
        MemberVariable,
		Return,
		Loop,
		ForEach,
		IfElse
	};

    using ExpressionVariant = 
        variant<
        ValueRef,
        ResolveClassVar,
        ResolveFuncVar,
        ResolveVar, 
        DefineVar, 
        FunctionExpression,
        MemberFunctionCall,
        MemberVariable,
        Return, 
        Loop, 
        Foreach,
        IfElse
        >;

	// forward declare so we can use the parser to process functions
	class KataScriptInterpreter;
	// describes a 'generic' expression tree, with either a value or function at the root
	struct Expression {
		// either we have a value, or a function expression which then has sub-expressions
        ExpressionVariant expression;
        ExpressionType type;
		ExpressionRef parent = nullptr;

		Expression(ValueRef val) 
            : type(ExpressionType::FunctionCall), expression(FunctionExpression(val)), parent(nullptr) {}
        
        Expression(ExpressionRef obj, const string& name) 
            : type(ExpressionType::MemberVariable), expression(MemberVariable(obj, name)), parent(nullptr) {}
        Expression(ExpressionRef obj, const string& name, const vector<ExpressionRef> subs) 
            : type(ExpressionType::MemberFunctionCall), expression(MemberFunctionCall(obj, name, subs)), parent(nullptr) {}
		Expression(FunctionRef val, ExpressionRef par) 
            : type(ExpressionType::FunctionDef), expression(FunctionExpression(val)), parent(par) {}
		Expression(ValueRef val, ExpressionRef par) 
            : type(ExpressionType::Value), expression(val), parent(par) {}
		Expression(Foreach val, ExpressionRef par = nullptr) 
            : type(ExpressionType::ForEach), expression(val), parent(par) {}
		Expression(Loop val, ExpressionRef par = nullptr) 
            : type(ExpressionType::Loop), expression(val), parent(par) {}
		Expression(IfElse val, ExpressionRef par = nullptr) 
            : type(ExpressionType::IfElse), expression(val), parent(par) {}
		Expression(Return val, ExpressionRef par = nullptr) 
            : type(ExpressionType::Return), expression(val), parent(par) {}
        Expression(ResolveClassVar val, ExpressionRef par = nullptr)
            : type(ExpressionType::ResolveClassVar), expression(val), parent(par) {}
        Expression(ResolveFuncVar val, ExpressionRef par = nullptr)
            : type(ExpressionType::ResolveFuncVar), expression(val), parent(par) {}
        Expression(ResolveVar val, ExpressionRef par = nullptr) 
            : type(ExpressionType::ResolveVar), expression(val), parent(par) {}
        Expression(DefineVar val, ExpressionRef par = nullptr) 
            : type(ExpressionType::DefineVar), expression(val), parent(par) {}

        Expression(ExpressionVariant val, ExpressionType ty)
            : type(ty), expression(val) {}

		ExpressionRef back() {
			switch (type) {
			case ExpressionType::FunctionDef:
				return get<vector<ExpressionRef>>(get<FunctionExpression>(expression).function->getFunction()->body).back();
				break;
			case ExpressionType::FunctionCall:
				return get<FunctionExpression>(expression).subexpressions.back();
				break;
			case ExpressionType::Loop:
				return get<Loop>(expression).subexpressions.back();
				break;
			case ExpressionType::ForEach:
				return get<Foreach>(expression).subexpressions.back();
				break;
			case ExpressionType::IfElse:
				return get<IfElse>(expression).back().subexpressions.back();
				break;
			default:
				break;
			}
			return nullptr;
		}

        auto begin() {
            switch (type) {
            case ExpressionType::FunctionCall:
                return get<FunctionExpression>(expression).subexpressions.begin();
                break;
            case ExpressionType::FunctionDef:
                return get<vector<ExpressionRef>>(get<FunctionExpression>(expression).function->getFunction()->body).begin();
                break;
            case ExpressionType::Loop:
                return get<Loop>(expression).subexpressions.begin();
                break;
            case ExpressionType::ForEach:
                return get<Foreach>(expression).subexpressions.begin();
                break;
            case ExpressionType::IfElse:
                return get<IfElse>(expression).back().subexpressions.begin();
                break;
            default:
                return vector<ExpressionRef>::iterator();
                break;
            }
        }

        auto end() {
            switch (type) {
            case ExpressionType::FunctionCall:
                return get<FunctionExpression>(expression).subexpressions.end();
                break;
            case ExpressionType::FunctionDef:
                return get<vector<ExpressionRef>>(get<FunctionExpression>(expression).function->getFunction()->body).end();
                break;
            case ExpressionType::Loop:
                return get<Loop>(expression).subexpressions.end();
                break;
            case ExpressionType::ForEach:
                return get<Foreach>(expression).subexpressions.end();
                break;
            case ExpressionType::IfElse:
                return get<IfElse>(expression).back().subexpressions.end();
                break;
            default:
                return vector<ExpressionRef>::iterator();
                break;
            }
        }

		void push_back(ExpressionRef ref) {
			switch (type) {
			case ExpressionType::FunctionCall:
                get<FunctionExpression>(expression).subexpressions.push_back(ref);
				break;
			case ExpressionType::FunctionDef:
                get<vector<ExpressionRef>>(get<FunctionExpression>(expression).function->getFunction()->body).push_back(ref);
				break;
			case ExpressionType::Loop:
                get<Loop>(expression).subexpressions.push_back(ref);
				break;
			case ExpressionType::ForEach:
                get<Foreach>(expression).subexpressions.push_back(ref);
				break;
			case ExpressionType::IfElse:
                get<IfElse>(expression).back().subexpressions.push_back(ref);
				break;
			default:
				break;
			}
		}

		void push_back(const If& ref) {
			switch (type) {
			case ExpressionType::IfElse:
                get<IfElse>(expression).push_back(ref);
				break;
			default:
				break;
			}
		}
	};
}
// copyright Garrett Skelton 2020
// MIT license
#include "Main.h"

// tokenizer special characters
const string WhitespaceChars = " \t\n"s;
const string GrammarChars = " \t\n,(){};+-/*%<>=!\""s;
const string MultiCharTokenStartChars = "+-/*<>=!"s;
const string NumericChars = "0123456789."s;
const string NumericStartChars = "0123456789.-"s;

vector<string> KSTokenize(const string& input) {
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
				++stride;
			}
			ret.push_back(input.substr(lpos, stride));
			lpos += stride;
		} else {
			++lpos;
		}
	}
	if (lpos < input.length()) {
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
KSValueRef KataScriptInterpereter::callFunction(const string& name, const vector<KSValueRef>& args) {
	return callFunction(resolveFunction(name), args);
}

// call function by reference
KSValueRef KataScriptInterpereter::callFunction(const KSFunctionRef fnc, const vector<KSValueRef>& args) {
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
					root->expr.subexpressions.push_back(getExpression({strings[++i]}));
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
			if (strings[i] == "(" || i+2 < strings.size() && strings[i + 1] == "(") {
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
					} else if (strings[i] == "(" || !(strings[i].size() == 1 && contains("+-*/"s, strings[i][0])) && i+2 < strings.size() && strings[i + 1] == "(") {
						++nestLayers;
						if (strings[i] == "(") {
							minisub.push_back( strings[i]);
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
		vector<KSValueRef> args;
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
	print("> " + text);

	for (auto&& token : KSTokenize(text)) {
		parse(token);
	}
}

KataScriptInterpereter::KataScriptInterpereter() {
	// register compiled functions and standard library:
	newFunction("identity", [](vector<KSValueRef> args) {
		return args[0];
	});

	newFunction("sqrt", [](vector<KSValueRef> args) {
		args[0]->hardconvert(KSType::FLOAT);
		args[0]->value = sqrtf(get<float>(args[0]->value));
		return args[0];
	});

	newFunction("print", [](vector<KSValueRef> args) {
		auto t = *args[0];
		t.upconvert(KSType::STRING);
		print(get<string>(t.value));
		return KSValueRef(new KSValue());
	});

	newFunction("=", [](vector<KSValueRef> args) {
		if (args.size() == 1) {
			return args[0];
		}
		upconvert(*args[0], *args[1]);
		*args[0] = *args[1];
		return args[0];
	});

	newFunction("+", [](vector<KSValueRef> args) {
		if (args.size() == 1) {
			return args[0];
		}
		upconvert(*args[0], *args[1]);
		return KSValueRef(new KSValue(*args[0] + *args[1]));
	});

	newFunction("-", [](vector<KSValueRef> args) {
		if (args.size() == 1) {
			auto zero = KSValue(0);
			upconvert(*args[0], zero);
			return KSValueRef(new KSValue(zero - *args[0]));
		}
		upconvert(*args[0], *args[1]);
		return KSValueRef(new KSValue(*args[0] - *args[1]));
	});

	newFunction("*", [](vector<KSValueRef> args) {
		upconvert(*args[0], *args[1]);
		return KSValueRef(new KSValue(*args[0] * *args[1]));
	});

	newFunction("/", [](vector<KSValueRef> args) {
		upconvert(*args[0], *args[1]);
		return KSValueRef(new KSValue(*args[0] / *args[1]));
	});

	newFunction("%", [](vector<KSValueRef> args) {
		upconvert(*args[0], *args[1]);
		return KSValueRef(new KSValue(*args[0] % *args[1]));
	});

	newFunction("==", [](vector<KSValueRef> args) {
		upconvert(*args[0], *args[1]);
		return KSValueRef(new KSValue(*args[0] == *args[1]));
	});

	newFunction("!=", [](vector<KSValueRef> args) {
		upconvert(*args[0], *args[1]);
		return KSValueRef(new KSValue(*args[0] != *args[1]));
	});

	newFunction("++", [](vector<KSValueRef> args) {
		auto t = KSValue(1);
		upconvert(*args[0], t);
		*args[0] = *args[0] + t;
		return args[0];
	});

	newFunction("--", [](vector<KSValueRef> args) {
		auto t = KSValue(1);
		upconvert(*args[0], t);
		*args[0] = *args[0] - t;
		return args[0];
	});

	newFunction("+=", [](vector<KSValueRef> args) {
		upconvert(*args[0], *args[1]);
		*args[0] = *args[0] + *args[1];
		return args[0];
	});

	newFunction("-=", [](vector<KSValueRef> args) {
		upconvert(*args[0], *args[1]);
		*args[0] = *args[0] - *args[1];
		return args[0];
	});

	newFunction(">", [](vector<KSValueRef> args) {
		upconvert(*args[0], *args[1]);
		return KSValueRef(new KSValue(*args[0] > *args[1]));
	});

	newFunction("<", [](vector<KSValueRef> args) {
		upconvert(*args[0], *args[1]);
		return KSValueRef(new KSValue(*args[0] < *args[1]));
	});

	newFunction(">=", [](vector<KSValueRef> args) {
		upconvert(*args[0], *args[1]);
		return KSValueRef(new KSValue(*args[0] >= *args[1]));
	});

	newFunction("<=", [](vector<KSValueRef> args) {
		upconvert(*args[0], *args[1]);
		return KSValueRef(new KSValue(*args[0] <= *args[1]));
	});
}

#pragma once

namespace KataScript {
    // tokenizer special characters
    const string WhitespaceChars = " \t\n"s;
    const string GrammarChars = " \t\n,.(){}[];+-/*%<>=!&|\""s;
    const string MultiCharTokenStartChars = "+-/*<>=!&|"s;
    const string NumericChars = "0123456789."s;
    const string NumericStartChars = "0123456789."s;
    const string DisallowedIdentifierStartChars = "0123456789.- \t\n,.(){}[];+-/*%<>=!&|\""s;

    vector<string_view> ViewTokenize(string_view input) {
        vector<string_view> ret;
        if (input.empty()) return ret;
        bool exitFromComment = false;

        size_t pos = 0;
        size_t lpos = 0;
        while ((pos = input.find_first_of(GrammarChars, lpos)) != string::npos) {
            size_t len = pos - lpos;
            // differentiate between decimals and dot syntax for function calls
            if (input[pos] == '.' && pos + 1 < input.size() && contains(NumericChars, input[pos + 1])) {
                pos = input.find_first_of(GrammarChars, pos + 1);
                ret.push_back(input.substr(lpos, pos - lpos));
                lpos = pos;
                continue;
            }
            if (len) {
                ret.push_back(input.substr(lpos, pos - lpos));
                lpos = pos;
            } else {
                // handle strings and escaped strings
                if (input[pos] == '\"' && pos > 0 && input[pos - 1] != '\\') {
                    auto originalPos = pos;
                    auto testpos = lpos + 1;
                    bool loop = true;
                    while (loop) {
                        pos = input.find('\"', testpos);
                        if (pos == string::npos) {
                            throw Exception("Quote mismatch at "s + string(input.substr(lpos, input.size() - lpos)));
                        }
                        loop = (input[pos - 1] == '\\');
                        testpos = ++pos;
                    }

                    ret.push_back(input.substr(originalPos, pos - originalPos));
                    lpos = pos;
                    continue;
                }
            }
            // special case for negative numbers
            if (input[pos] == '-' && contains(NumericChars, input[pos + 1])
                && (ret.size() == 0 || contains(MultiCharTokenStartChars, ret.back().back()))) {
                pos = input.find_first_of(GrammarChars, pos + 1);
                if (input[pos] == '.' && pos + 1 < input.size() && contains(NumericChars, input[pos + 1])) {
                    pos = input.find_first_of(GrammarChars, pos + 1);
                }
                ret.push_back(input.substr(lpos, pos - lpos));
                lpos = pos;
            } else if (!contains(WhitespaceChars, input[pos])) {
                // process multicharacter special tokens like ++, //, -=, etc
                auto stride = 1;
                if (contains(MultiCharTokenStartChars, input[pos]) && pos + 1 < input.size() && contains(MultiCharTokenStartChars, input[pos + 1])) {
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

    bool isStringLiteral(string_view test) {
        return (test.size() > 1 && test[0] == '\"');
    }

    bool isFloatLiteral(string_view test) {
        return (test.size() > 1 && contains(test, '.'));
    }

    bool isVarOrFuncToken(string_view test) {
        return (test.size() > 0 && !contains(DisallowedIdentifierStartChars, test[0]));
    }

    bool isNumeric(string_view test) {
        if (test.size() > 1 && test[0] == '-') contains(NumericStartChars, test[1]);
        return (test.size() > 0 && contains(NumericStartChars, test[0]));
    }

    bool isMathOperator(string_view test) {
        if (test.size() == 1) {
            return contains("+-*/%<>=!"s, test[0]);
        }
        if (test.size() == 2) {
            return contains("=+-&|"s, test[1]) && contains(MultiCharTokenStartChars, test[0]);
        }
        return false;
    }

    bool isUnaryMathOperator(string_view test) {
        if (test.size() == 1) {
            return '!' == test[0];
        }
        if (test.size() == 2) {
            return contains("+-"s, test[1]) && test[1] == test[0];
        }
        return false;
    }

    bool isMemberCall(string_view test) {
        if (test.size() == 1) {
            return '.' == test[0];
        }
        return false;
    }

    bool isOpeningBracketOrParen(string_view test) {
        return (test.size() == 1 && (test[0] == '[' || test[0] == '('));
    }

    bool isClosingBracketOrParen(string_view test) {
        return (test.size() == 1 && (test[0] == ']' || test[0] == ')'));
    }

    bool needsUnaryPlacementFix(const vector<string_view>& strings, size_t i) {
        return (isUnaryMathOperator(strings[i]) && (i == 0 || !(isClosingBracketOrParen(strings[i - 1]) || isVarOrFuncToken(strings[i - 1]) || isNumeric(strings[i - 1]))));
    }

    bool checkPrecedence(OperatorPrecedence curr, OperatorPrecedence neww) {
        return (int)curr < (int)neww || (neww == curr && neww == OperatorPrecedence::incdec);
    }

    // recursively build an expression tree from a list of tokens
    ExpressionRef KataScriptInterpreter::getExpression(const vector<string_view>& strings, ScopeRef scope) {
        ExpressionRef root = nullptr;
        size_t i = 0;
        while (i < strings.size()) {
            if (isMathOperator(strings[i])) {
                auto prev = root;
                root = make_shared<Expression>(resolveVariable(string(strings[i]), modules[0].scope));
                auto curr = prev;
                if (curr) {
                    // find operations of lesser precedence
                    if (curr->type == ExpressionType::FunctionCall) {
                        auto& rootExpression = get<FunctionExpression>(root->expression);
                        auto curfunc = get<FunctionExpression>(curr->expression).function->getFunction();
                        auto newfunc = rootExpression.function->getFunction();
                        if (curfunc && checkPrecedence(curfunc->opPrecedence, newfunc->opPrecedence)) {
                            while (get<FunctionExpression>(curr->expression).subexpressions.back()->type == ExpressionType::FunctionCall) {
                                curfunc = get<FunctionExpression>(get<FunctionExpression>(curr->expression).subexpressions.back()->expression).function->getFunction();
                                if (curfunc && checkPrecedence(curfunc->opPrecedence, newfunc->opPrecedence)) {
                                    curr = get<FunctionExpression>(curr->expression).subexpressions.back();
                                } else {
                                    break;
                                }
                            }
                            auto& currExpression = get<FunctionExpression>(curr->expression);
                            // swap values around to correct the otherwise incorect order of operations (except unary)
                            if (needsUnaryPlacementFix(strings, i)) {
                                rootExpression.subexpressions.insert(rootExpression.subexpressions.begin(), make_shared<Expression>(make_shared<Value>(), root));
                            } else {
                                rootExpression.subexpressions.push_back(currExpression.subexpressions.back());
                                currExpression.subexpressions.pop_back();
                            }
                            
                            // gather any subexpressions from list literals/indexing or function call args
                            if (i + 1 < strings.size() && !isMathOperator(strings[i+1])) {
                                vector<string_view> minisub = { strings[++i] };
                                // list literal or parenthesis expression
                                char checkstr = 0;
                                if (isOpeningBracketOrParen(strings[i])) {
                                    checkstr = strings[i][0];
                                }
                                // list index or function call
                                else if (strings.size() > i + 1 && isOpeningBracketOrParen(strings[i + 1])) {
                                    ++i;
                                    checkstr = strings[i][0];
                                    minisub.push_back(strings[i]);
                                }
                                // gather tokens until the end of the sub expression
                                if (checkstr != 0) {
                                    auto endstr = checkstr == '[' ? ']' : ')';
                                    int nestLayers = 1;
                                    while (nestLayers > 0 && ++i < strings.size()) {
                                        if (strings[i].size() == 1) {
                                            if (strings[i][0] == endstr) {
                                                --nestLayers;
                                            }
                                            else if (strings[i][0] == checkstr) {
                                                ++nestLayers;
                                            }
                                        }
                                        minisub.push_back(strings[i]);
                                        if (nestLayers == 0) {
                                            if (i + 1 < strings.size() && isOpeningBracketOrParen(strings[i + 1])) {
                                                ++nestLayers;
                                                checkstr = strings[++i][0];
                                                endstr = checkstr == '[' ? ']' : ')';
                                            }
                                        }
                                    }
                                }
                                rootExpression.subexpressions.push_back(getExpression(move(minisub), scope));
                            }
                            currExpression.subexpressions.push_back(root);
                            root = prev;
                        } else {
                            rootExpression.subexpressions.push_back(curr);
                        }
                    } else {
                        get<FunctionExpression>(root->expression).subexpressions.push_back(curr);
                    }
                } else {
                    if (needsUnaryPlacementFix(strings, i)) {
                        auto& rootExpression = get<FunctionExpression>(root->expression);
                        rootExpression.subexpressions.insert(rootExpression.subexpressions.begin(), make_shared<Expression>(make_shared<Value>(), root));
                    }
                }
            } else if (isStringLiteral(strings[i])) {
                // trim quotation marks
                auto val = string(strings[i].substr(1, strings[i].size() - 2));
                replaceWhitespaceLiterals(val);
                auto newExpr = make_shared<Expression>(make_shared<Value>(val), ExpressionType::Value);
                if (root) {
                    get<FunctionExpression>(root->expression).subexpressions.push_back(newExpr);
                } else {
                    root = newExpr;
                }
            } else if (strings[i] == "(" || strings[i] == "[" || isVarOrFuncToken(strings[i])) {
                if (strings[i] == "(" || i + 2 < strings.size() && strings[i + 1] == "(") {
                    // function
                    ExpressionRef cur = nullptr;
                    if (strings[i] == "(") {
                        if (root) {
                            if (root->type == ExpressionType::FunctionCall
                                && (get<FunctionExpression>(root->expression).function->getFunction()->opPrecedence == OperatorPrecedence::func)) {
                                cur = make_shared<Expression>(make_shared<Value>());
                                get<FunctionExpression>(cur->expression).subexpressions.push_back(root);
                                root = cur;
                            } else {
                                get<FunctionExpression>(root->expression).subexpressions.push_back(make_shared<Expression>(identityFunctionVarLocation));
                                cur = get<FunctionExpression>(root->expression).subexpressions.back();
                            }
                        } else {
                            root = make_shared<Expression>(identityFunctionVarLocation);
                            cur = root;
                        }
                    } else {
                        auto funccall = make_shared<Expression>(applyFunctionVarLocation);
                        if (root) {
                            get<FunctionExpression>(root->expression).subexpressions.push_back(funccall);
                            cur = get<FunctionExpression>(root->expression).subexpressions.back();
                        } else {
                            root = funccall;
                            cur = root;
                        }
                        get<FunctionExpression>(cur->expression).subexpressions.push_back(make_shared<Expression>(ResolveFuncVar(string(strings[i]))));
                        ++i;
                    }
                    vector<string_view> minisub;
                    int nestLayers = 1;
                    while (nestLayers > 0 && ++i < strings.size()) {
                        if (nestLayers == 1 && strings[i] == ",") {
                            if (minisub.size()) {
                                get<FunctionExpression>(cur->expression).subexpressions.push_back(getExpression(move(minisub), scope));
                                minisub.clear();
                            }
                        } else if (isClosingBracketOrParen(strings[i])) {
                            if (--nestLayers > 0) {
                                minisub.push_back(strings[i]);
                            } else {
                                if (minisub.size()) {
                                    get<FunctionExpression>(cur->expression).subexpressions.push_back(getExpression(move(minisub), scope));
                                    minisub.clear();
                                }
                            }
                        } else if (isOpeningBracketOrParen(strings[i]) || !(strings[i].size() == 1 && contains("+-*%/"s, strings[i][0])) && i + 2 < strings.size() && strings[i + 1] == "(") {
                            ++nestLayers;
                            if (strings[i] == "(") {
                                minisub.push_back(strings[i]);
                            } else {
                                minisub.push_back(strings[i]);
                                ++i;
                                if (isClosingBracketOrParen(strings[i])) {
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
                    bool indexOfIndex = i > 0 && (isClosingBracketOrParen(strings[i-1]) || strings[i - 1].back() == '\"') || (i > 1 && strings[i - 2] == ".");
                    ExpressionRef cur = nullptr;
                    if (!indexOfIndex && strings[i] == "[") {
                        // list literal / collection literal
                        if (root) {
                            get<FunctionExpression>(root->expression).subexpressions.push_back(
                                make_shared<Expression>(make_shared<Value>(List()), ExpressionType::Value));
                            cur = get<FunctionExpression>(root->expression).subexpressions.back();
                        } else {
                            root = make_shared<Expression>(make_shared<Value>(List()), ExpressionType::Value);
                            cur = root;
                        }
                        vector<string_view> minisub;
                        int nestLayers = 1;
                        while (nestLayers > 0 && ++i < strings.size()) {
                            if (nestLayers == 1 && strings[i] == ",") {
                                if (minisub.size()) {
                                    auto val = *getValue(move(minisub), scope);
                                    get<ValueRef>(cur->expression)->getList().push_back(make_shared<Value>(val.value, val.type));
                                    minisub.clear();
                                }
                            } else if (isClosingBracketOrParen(strings[i])) {
                                if (--nestLayers > 0) {
                                    minisub.push_back(strings[i]);
                                } else {
                                    if (minisub.size()) {
                                        auto val = *getValue(move(minisub), scope);
                                        get<ValueRef>(cur->expression)->getList().push_back(make_shared<Value>(val.value, val.type));
                                        minisub.clear();
                                    }
                                }
                            } else if (isOpeningBracketOrParen(strings[i])) {
                                ++nestLayers;
                                minisub.push_back(strings[i]);
                            } else {
                                minisub.push_back(strings[i]);
                            }
                        }
                        auto& list = get<ValueRef>(cur->expression)->getList();
                        if (list.size()) {
                            bool canBeArray = true;
                            auto type = list[0]->type;
                            for (auto& val : list) {
                                if (val->type == Type::Null || val->type != type || (int)val->type >= (int)Type::Array) {
                                    canBeArray = false;
                                    break;
                                }
                            }
                            if (canBeArray) {
                                get<ValueRef>(cur->expression)->hardconvert(Type::Array);
                            }
                        }
                    } else {
                        // list access
                        auto indexexpr = make_shared<Expression>(listIndexFunctionVarLocation);
                        if (indexOfIndex) {
                            cur = root;
                            auto parent = root;
                            while (cur->type == ExpressionType::FunctionCall && get<FunctionExpression>(cur->expression).function->getFunction()->opPrecedence != OperatorPrecedence::func) {
                                parent = cur;
                                cur = get<FunctionExpression>(cur->expression).subexpressions.back();
                            }
                            get<FunctionExpression>(indexexpr->expression).subexpressions.push_back(cur);
                            if (cur == root) {
                                root = indexexpr;
                                cur = indexexpr;
                            } else {
                                get<FunctionExpression>(parent->expression).subexpressions.pop_back();
                                get<FunctionExpression>(parent->expression).subexpressions.push_back(indexexpr);
                                cur = get<FunctionExpression>(parent->expression).subexpressions.back();
                            }
                        } else {
                            if (root) {
                                get<FunctionExpression>(root->expression).subexpressions.push_back(indexexpr);
                                cur = get<FunctionExpression>(root->expression).subexpressions.back();
                            } else {
                                root = indexexpr;
                                cur = root;
                            }
                            get<FunctionExpression>(cur->expression).subexpressions.push_back(make_shared<Expression>(ResolveVar(string(strings[i]))));
                            ++i;
                        }

                        vector<string_view> minisub;
                        int nestLayers = 1;
                        while (nestLayers > 0 && ++i < strings.size()) {
                            if (strings[i] == "]") {
                                if (--nestLayers > 0) {
                                    minisub.push_back(strings[i]);
                                } else {
                                    if (minisub.size()) {
                                        get<FunctionExpression>(cur->expression).subexpressions.push_back(getExpression(move(minisub), scope));
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
                    ExpressionRef newExpr;
                    if (strings[i] == "true") {
                        newExpr = make_shared<Expression>(make_shared<Value>(Int(1)), ExpressionType::Value);
                    } else if (strings[i] == "false") {
                        newExpr = make_shared<Expression>(make_shared<Value>(Int(0)), ExpressionType::Value);
                    } else if (strings[i] == "null") {
                        newExpr = make_shared<Expression>(make_shared<Value>(), ExpressionType::Value);
                    } else {
                        newExpr = make_shared<Expression>(ResolveVar(string(strings[i])));
                    }

                    if (root) {
                        if (root->type == ExpressionType::ResolveVar) {
                            throw Exception("Syntax Error: unexpected series of values at "s + string(strings[i]) +", possible missing `,`");
                        }
                        get<FunctionExpression>(root->expression).subexpressions.push_back(newExpr);
                    } else {
                        root = newExpr;
                    }
                }
            } else if (isMemberCall(strings[i])) {
                // member var
                bool isfunc = strings.size() > i + 2 && strings[i + 2] == "("s;
                auto memberExpr = make_shared<Expression>(isfunc ? applyFunctionVarLocation : listIndexFunctionVarLocation);
                auto& membExpr = get<FunctionExpression>(memberExpr->expression);
                ExpressionRef argsInsert;
                if (root->type == ExpressionType::FunctionCall && get<FunctionExpression>(root->expression).subexpressions.size()) {
                    auto& rootExpr = get<FunctionExpression>(root->expression);
                    if (isfunc) {
                        membExpr.subexpressions.push_back(make_shared<Expression>(make_shared<Value>(string(strings[++i])), ExpressionType::Value));
                        membExpr.subexpressions.push_back(rootExpr.subexpressions.back());
                    } else {
                        membExpr.subexpressions.push_back(rootExpr.subexpressions.back());
                        membExpr.subexpressions.push_back(make_shared<Expression>(make_shared<Value>(string(strings[++i])), ExpressionType::Value));
                    }
                    rootExpr.subexpressions.pop_back();
                    rootExpr.subexpressions.push_back(memberExpr);
                    argsInsert = rootExpr.subexpressions.back();
                } else {
                    if (isfunc) {
                        membExpr.subexpressions.push_back(make_shared<Expression>(make_shared<Value>(string(strings[++i])), ExpressionType::Value));
                        membExpr.subexpressions.push_back(root);
                    } else {
                        membExpr.subexpressions.push_back(root);
                        membExpr.subexpressions.push_back(make_shared<Expression>(make_shared<Value>(string(strings[++i])), ExpressionType::Value));
                    }
                    root = memberExpr;
                    argsInsert = root;
                }

                if (isfunc) {
                    bool addedArgs = false;
                    auto previ = i;
                    ++i;
                    vector<string_view> minisub;
                    int nestLayers = 1;
                    while (nestLayers > 0 && ++i < strings.size()) {
                        if (nestLayers == 1 && strings[i] == ",") {
                            if (minisub.size()) {
                                get<FunctionExpression>(argsInsert->expression).subexpressions.push_back(getExpression(move(minisub), scope));
                                minisub.clear();
                                addedArgs = true;
                            }
                        } else if (strings[i] == ")") {
                            if (--nestLayers > 0) {
                                minisub.push_back(strings[i]);
                            } else {
                                if (minisub.size()) {
                                    get<FunctionExpression>(argsInsert->expression).subexpressions.push_back(getExpression(move(minisub), scope));
                                    minisub.clear();
                                    addedArgs = true;
                                }
                            }
                        } else if (strings[i] == "(") {
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
                auto newExpr = make_shared<Expression>(ValueRef(isFloat ? new Value((Float)val) : new Value((Int)val)), ExpressionType::Value);
                if (root) {
                    get<FunctionExpression>(root->expression).subexpressions.push_back(newExpr);
                } else {
                    root = newExpr;
                }
            }
            ++i;
        }

        return root;
    }

    // parse one token at a time, uses the state machine
    void KataScriptInterpreter::parse(string_view token) {
        auto tempState = parseState;
        switch (parseState) {
        case ParseState::beginExpression:
        {
            bool wasElse = false;
            bool closedScope = false;
            bool closedExpr = false;
            bool isEndCurlBracket = false;
            if (token == "fn" || token == "func" || token == "function") {
                parseState = ParseState::defineFunc;
            } else if (token == "var") {
                parseState = ParseState::defineVar;
            } else if (token == "for" || token == "while") {
                parseState = ParseState::loopCall;
                if (currentExpression) {
                    auto newexpr = make_shared<Expression>(Loop(), currentExpression);
                    currentExpression->push_back(newexpr);
                    currentExpression = newexpr;
                } else {
                    currentExpression = make_shared<Expression>(Loop());
                }
            } else if (token == "foreach") {
                parseState = ParseState::forEach;
                if (currentExpression) {
                    auto newexpr = make_shared<Expression>(Foreach(), currentExpression);
                    currentExpression->push_back(newexpr);
                    currentExpression = newexpr;
                } else {
                    currentExpression = make_shared<Expression>(Foreach());
                }
            } else if (token == "if") {
                parseState = ParseState::ifCall;
                if (currentExpression) {
                    auto newexpr = make_shared<Expression>(IfElse(), currentExpression);
                    currentExpression->push_back(newexpr);
                    currentExpression = newexpr;
                } else {
                    currentExpression = make_shared<Expression>(IfElse());
                }
            } else if (token == "else") {
                parseState = ParseState::expectIfEnd;
                wasElse = true;
            } else if (token == "class") {
                parseState = ParseState::defineClass;
            } else if (token == "{") {
                parseScope = newScope("__anon"s, parseScope);
                clearParseStacks();
            } else if (token == "}") {
                wasElse = !currentExpression || currentExpression->type != ExpressionType::IfElse;
                bool wasFreefunc = !currentExpression || (currentExpression->type == ExpressionType::FunctionDef
                    && get<FunctionExpression>(currentExpression->expression).function->getFunction()->type == FunctionType::FREE);
                closedExpr = closeCurrentExpression();
                if (!closedExpr && wasFreefunc || parseScope->name == "__anon") {
                    closeScope(parseScope);
                }
                closedScope = true;
                isEndCurlBracket = true;
            } else if (token == "return") {
                parseState = ParseState::returnLine;
            } else if (token == "import") {
                parseState = ParseState::importModule;
            } else if (token == ";") {
                clearParseStacks();
            } else {
                parseState = ParseState::readLine;
                parseStrings.push_back(token);
            }
            if (!closedExpr && (closedScope && lastStatementClosedScope || (!lastStatementWasElse && !wasElse && lastTokenEndCurlBraket))) {
                bool wasIfExpr = currentExpression && currentExpression->type == ExpressionType::IfElse;
                closeDanglingIfExpression();
                if (closedScope && wasIfExpr && currentExpression->type != ExpressionType::IfElse) {
                    closeCurrentExpression();
                    closedScope = false;
                }
            }
            lastStatementClosedScope = closedScope;
            lastTokenEndCurlBraket = isEndCurlBracket;
            lastStatementWasElse = wasElse;
        }
        break;
        case ParseState::loopCall:
            if (token == ")") {
                if (--outerNestLayer <= 0) {
                    vector<vector<string_view>> exprs = {};
                    exprs.push_back({});
                    for (auto&& str : parseStrings) {
                        if (str == ";") {
                            exprs.push_back({});
                        } else {
                            exprs.back().push_back(str);
                        }
                    }
                    auto& loop = get<Loop>(currentExpression->expression);
                    switch (exprs.size()) {
                    case 1:
                        loop.testExpression = getExpression(exprs[0], parseScope);
                        break;
                    case 2:
                        loop.testExpression = getExpression(exprs[0], parseScope);
                        loop.iterateExpression = getExpression(exprs[1], parseScope);
                        break;
                    case 3:
                    {
                        auto name = exprs[0].front();
                        exprs[0].erase(exprs[0].begin(), exprs[0].begin() + 2);
                        loop.initExpression = make_shared<Expression>(DefineVar(string(name), getExpression(exprs[0], parseScope)));
                        loop.testExpression = getExpression(exprs[1], parseScope);
                        loop.iterateExpression = getExpression(exprs[2], parseScope);
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
        case ParseState::forEach:
            if (token == ")") {
                if (--outerNestLayer <= 0) {
                    vector<vector<string_view>> exprs = {};
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
                        throw Exception("Syntax error, `foreach` requires 2 statements, "s + std::to_string(exprs.size()) + " statements supplied instead");
                    }

                    auto name = string(exprs[0][0]);
                    resolveVariable(name, parseScope);
                    get<Foreach>(currentExpression->expression).iterateName = move(name);
                    get<Foreach>(currentExpression->expression).listExpression = getExpression(exprs[1], parseScope);

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
        case ParseState::ifCall:
            if (token == ")") {
                if (--outerNestLayer <= 0) {
                    currentExpression->push_back(If());
                    get<IfElse>(currentExpression->expression).back().testExpression = getExpression(move(parseStrings), parseScope);
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
        case ParseState::readLine:
            if (token == ";") {
                auto line = move(parseStrings);
                clearParseStacks();
                // we clear before evaluating lines so any exceptions can clear the offending code
                if (!currentExpression) {
                    getValue(line, parseScope);
                } else {
                    currentExpression->push_back(getExpression(line, parseScope));
                }

            } else {
                parseStrings.push_back(token);
            }
            break;
        case ParseState::returnLine:
            if (token == ";") {
                if (currentExpression) {
                    currentExpression->push_back(make_shared<Expression>(Return(getExpression(move(parseStrings), parseScope))));
                }
                clearParseStacks();
            } else {
                parseStrings.push_back(token);
            }
            break;
        case ParseState::expectIfEnd:
            if (token == "if") {
                clearParseStacks();
                parseState = ParseState::ifCall;
            } else if (token == "{") {
                parseScope = newScope("ifelse"s, parseScope);
                currentExpression->push_back(If());
                clearParseStacks();
            } else {
                clearParseStacks();
                throw Exception("Malformed Syntax: Incorrect token `" + string(token) + "` following `else` keyword");
            }
            break;
        case ParseState::defineVar:
            if (token == ";") {
                if (parseStrings.size() == 0) {
                    throw Exception("Malformed Syntax: `var` keyword must be followed by user supplied name");
                }
                auto name = parseStrings.front();
                ExpressionRef defineExpr = nullptr;
                if (parseStrings.size() > 2) {
                    parseStrings.erase(parseStrings.begin());
                    parseStrings.erase(parseStrings.begin());
                    defineExpr = getExpression(move(parseStrings), parseScope);
                }
                if (currentExpression) {
                    currentExpression->push_back(make_shared<Expression>(DefineVar(string(name), defineExpr)));
                } else {
                    getValue(make_shared<Expression>(DefineVar(string(name), defineExpr)), parseScope);
                }
                clearParseStacks();
            } else {
                parseStrings.push_back(token);
            }
            break;
        case ParseState::defineClass:
            parseScope = newClassScope(string(token), parseScope);
            parseState = ParseState::classArgs;
            parseStrings.clear();
            break;
        case ParseState::classArgs:
            if (token == ",") {
                if (parseStrings.size()) {
                    auto otherscope = resolveScope(string(parseStrings.back()), parseScope);
                    parseScope->variables.insert(otherscope->variables.begin(), otherscope->variables.end());
                    parseScope->functions.insert(otherscope->functions.begin(), otherscope->functions.end());
                    parseStrings.clear();
                }
            } else if (token == "{") {
                if (parseStrings.size()) {
                    auto otherscope = resolveScope(string(parseStrings.back()), parseScope);
                    parseScope->variables.insert(otherscope->variables.begin(), otherscope->variables.end());
                    parseScope->functions.insert(otherscope->functions.begin(), otherscope->functions.end());
                }
                clearParseStacks();
            } else {
                parseStrings.push_back(token);
            }
            break;
        case ParseState::defineFunc:
            parseStrings.push_back(token);
            parseState = ParseState::funcArgs;
            break;
        case ParseState::importModule:
            clearParseStacks();
            if (token.size() > 2 && token.front() == '\"' && token.back() == '\"') {
                // import file       
                evaluateFile(string(token.substr(1, token.size() - 2)));
                clearParseStacks();
            } else {
                auto modName = string(token);
                auto iter = std::find_if(modules.begin(), modules.end(), [&modName](auto& mod) {return mod.scope->name == modName; });
                if (iter == modules.end()) {
                    auto newMod = getOptionalModule(modName);
                    if (newMod) {
                        if (shouldAllow(allowedModulePrivileges, newMod->requiredPermissions)) {
                            modules.emplace_back(newMod->requiredPermissions, newMod->scope);
                        } else {
                            throw Exception("Error: Cannot import restricted module: "s + modName);
                        }
                    }
                }
            }
            break;
        case ParseState::funcArgs:
            if (token == "(" || token == ",") {
                // eat these tokens
            } else if (token == ")") {
                auto fncName = move(parseStrings.front());
                parseStrings.erase(parseStrings.begin());
                vector<string> args;
                args.reserve(parseStrings.size());
                for (auto parseString : parseStrings) {
                    args.emplace_back(parseString);
                }
                auto newfunc = newFunction(string(fncName), parseScope, args, {});
                if (currentExpression) {
                    auto newexpr = make_shared<Expression>(newfunc, currentExpression);
                    currentExpression->push_back(newexpr);
                    currentExpression = newexpr;
                } else {
                    currentExpression = make_shared<Expression>(newfunc, nullptr);
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

    bool KataScriptInterpreter::readLine(string_view text) {
        ++currentLine;
        auto tokenCount = 0;
        auto tokens = ViewTokenize(text);
        bool didExcept = false;
        try {            
            for (auto& token : tokens) {
                parse(token);
                ++tokenCount;
            }
        } catch (Exception e) {
#if defined KATASCRIPT_DO_INTERNAL_PRINT
            callFunction(resolveFunction("print"), "Error at line "s + std::to_string(currentLine) + ", at: " + std::to_string(tokenCount) + string(tokens[tokenCount]) + ": " + e.wh + "\n");
#else
            printf("Error at line %llu at %i: %s : %s\n", currentLine, tokenCount, string(tokens[tokenCount]).c_str(), e.wh.c_str());
#endif		
            clearParseStacks();
            parseScope = globalScope;
            parseClass = nullptr;
            currentExpression = nullptr;
            didExcept = true;
        } catch (std::exception& e) {
#if defined KATASCRIPT_DO_INTERNAL_PRINT
            callFunction(resolveFunction("print"), "Error at line "s + std::to_string(currentLine)  + ", at: " + std::to_string(tokenCount) + string(tokens[tokenCount]) +  ": " + e.what()  + "\n");
#else
            printf("Error at line %llu at %i: %s : %s\n", currentLine, tokenCount, string(tokens[tokenCount]).c_str(), e.what());
#endif		
            clearParseStacks();
            parseScope = globalScope;
            parseClass = nullptr;
            currentExpression = nullptr;
            didExcept = true;
        }
        return didExcept;
    }

    bool KataScriptInterpreter::evaluate(string_view script) {
        for (auto& line : split(script, '\n')) {
            if (readLine(line)) {
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

    bool KataScriptInterpreter::readLine(string_view text, ScopeRef scope) {
        auto temp = parseScope;
        parseScope = scope;
        auto result = readLine(text);
        parseScope = temp;
        return result;
    }

    bool KataScriptInterpreter::evaluate(string_view script, ScopeRef scope) {
        auto temp = parseScope;
        parseScope = scope;
        auto result = evaluate(script);
        parseScope = temp;
        return result;
    }

    bool KataScriptInterpreter::evaluateFile(const string& path, ScopeRef scope) {
        auto temp = parseScope;
        parseScope = scope;
        auto result = evaluateFile(path);
        parseScope = temp;
        return result;
    }

    void KataScriptInterpreter::clearState() {
        clearParseStacks();
        globalScope = make_shared<Scope>("global", nullptr);
        parseScope = globalScope;
        currentExpression = nullptr;
        if (modules.size() > 1) {
            modules.erase(modules.begin() + 1, modules.end());
        }
    }

    // general purpose clear to reset state machine for next statement
    void KataScriptInterpreter::clearParseStacks() {
        parseState = ParseState::beginExpression;
        parseStrings.clear();
        outerNestLayer = 0;
    }
}
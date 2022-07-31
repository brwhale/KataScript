#pragma once

namespace KataScript {
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
                    replaceWhitespaceLiterals(unescaped);
                    ret.push_back(unescaped);
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
                            // gather any subexpressions from list literals/indexing or function call args
                            vector<string> minisub = { strings[++i] };
                            // list literal or parenthesis expression
                            string checkstr;
                            if (strings[i] == "[" || strings[i] == "(") {
                                checkstr = strings[i];
                            }
                            // list index or function call
                            auto j = i + 1;
                            if (checkstr.size() == 0 && (strings.size() > j && (strings[j] == "[" || strings[j] == "("))) {
                                checkstr = strings[++i];
                                minisub.push_back(strings[i]);
                            }
                            // gather tokens until the end of the sub expression
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
                                        if (i + 1 < strings.size() && (strings[i + 1] == "[" || strings[i + 1] == "(")) {
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
                    bool indexOfIndex = i > 0 && (strings[i - 1] == "]" || strings[i - 1] == ")" || strings[i - 1].back() == '\"') || (i > 1 && strings[i - 2] == ".");
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
                auto memberExpr = make_shared<KSExpression>(resolveVariable(isfunc ? "applyfunction"s : "listindex"s, modules[0]));
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
                bool wasFreefunc = !currentExpression || (currentExpression->type == KSExpressionType::FunctionDef
                    && get<KSFunctionExpression>(currentExpression->expression).function->getFunction()->type == KSFunctionType::FREE);
                closedExpr = closeCurrentExpression();
                if (!closedExpr && wasFreefunc) {
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
                        exprs[0].erase(exprs[0].begin(), exprs[0].begin() + 1);
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
            newClassScope(token);
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
        } catch (std::exception& e) {
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

    bool KataScriptInterpreter::readLine(const string& text, KSScopeRef scope) {
        auto temp = currentScope;
        currentScope = scope;
        auto result = readLine(text);
        currentScope = temp;
        return result;
    }

    bool KataScriptInterpreter::evaluate(const string& script, KSScopeRef scope) {
        auto temp = currentScope;
        currentScope = scope;
        auto result = evaluate(script);
        currentScope = temp;
        return result;
    }

    bool KataScriptInterpreter::evaluateFile(const string& path, KSScopeRef scope) {
        auto temp = currentScope;
        currentScope = scope;
        auto result = evaluateFile(path);
        currentScope = temp;
        return result;
    }

    void KataScriptInterpreter::clearState() {
        clearParseStacks();
        globalScope = make_shared<KSScope>("global", nullptr);
        currentScope = globalScope;
        currentExpression = nullptr;
    }

    // general purpose clear to reset state machine for next statement
    void KataScriptInterpreter::clearParseStacks() {
        parseState = KSParseState::beginExpression;
        parseStrings.clear();
        outerNestLayer = 0;
    }
}
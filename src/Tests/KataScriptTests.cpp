#include "CppUnitTest.h"
#define KATASCRIPT_IMPL
#include "../../src/Library/KataScript.hpp"

using namespace std::string_literals;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			template<> static std::wstring ToString<KataScript::KSType>(const KataScript::KSType& t) {
				auto str = KataScript::getTypeName(t);
				return std::wstring(str.begin(), str.end());
			}

			template<> static std::wstring ToString<KataScript::KSOperatorPrecedence>(const KataScript::KSOperatorPrecedence& t) {
				return ToString((int)t);
			}
		}
	}
}

namespace KataScriptTests {
	TEST_CLASS(KataScriptTests) {
public:

	KataScript::KataScriptInterpreter interpreter;

	TEST_METHOD_INITIALIZE(initTest) {
		interpreter.clearState();
	}

	TEST_METHOD_CLEANUP(cleanTest) {
		interpreter.clearState();
	}

	TEST_METHOD(AssignNull) {
		interpreter.evaluate("i;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::NONE, value->type);
		Assert::AreEqual(0, value->getInt());
	}

	TEST_METHOD(AssignInt) {	
		interpreter.evaluate("i = 5;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(5, value->getInt());
	}

	TEST_METHOD(AssignFloat) {
		interpreter.evaluate("i = 50.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(50.0f, value->getFloat());
	}

	TEST_METHOD(AssignFunction) {
		interpreter.evaluate("i = print;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FUNCTION, value->type);
	}

	TEST_METHOD(AssignString) {
		interpreter.evaluate("i = \"fish tacos\";"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::STRING, value->type);
		Assert::AreEqual("fish tacos"s, value->getString());
	}

	TEST_METHOD(AssignList) {
		interpreter.evaluate("i = [1,2,3,4];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::LIST, value->type);
		Assert::AreEqual(4ull, value->getList().size());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[0]->type);
		Assert::AreEqual(1, value->getList()[0]->getInt());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[1]->type);
		Assert::AreEqual(2, value->getList()[1]->getInt());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[2]->type);
		Assert::AreEqual(3, value->getList()[2]->getInt());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[3]->type);
		Assert::AreEqual(4, value->getList()[3]->getInt());
	}

	TEST_METHOD(AssignTinyList) {
		interpreter.evaluate("i = [];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::LIST, value->type);
		Assert::AreEqual(0ull, value->getList().size());
	}

	TEST_METHOD(AssignChangeTypes) {
		interpreter.evaluate("i = [1];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::LIST, value->type);
		Assert::AreEqual(1ull, value->getList().size());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[0]->type);
		Assert::AreEqual(1, value->getList()[0]->getInt());

		interpreter.evaluate("i = \"fish tacos\";"s);
		Assert::AreEqual(KataScript::KSType::STRING, value->type);
		Assert::AreEqual("fish tacos"s, value->getString());

		interpreter.evaluate("i = 5;"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(5, value->getInt());

		interpreter.evaluate("i = 50.0;"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(50.0f, value->getFloat());

		interpreter.evaluate("i = print;"s);
		Assert::AreEqual(KataScript::KSType::FUNCTION, value->type);
	}

	TEST_METHOD(AddInts) {
		interpreter.evaluate("i = 2 + 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(5, value->getInt());
	}

	TEST_METHOD(AddFloats) {
		interpreter.evaluate("i = 2.4 + 3.4;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(5.8f, value->getFloat());
	}

	TEST_METHOD(AddStrings) {
		interpreter.evaluate("i = \"fish \" + \"tacos\";"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::STRING, value->type);
		Assert::AreEqual("fish tacos"s, value->getString());
	}

	TEST_METHOD(AddLists) {
		interpreter.evaluate("i = [1,2] + [3,4];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::LIST, value->type);
		Assert::AreEqual(4ull, value->getList().size());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[0]->type);
		Assert::AreEqual(1, value->getList()[0]->getInt());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[1]->type);
		Assert::AreEqual(2, value->getList()[1]->getInt());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[2]->type);
		Assert::AreEqual(3, value->getList()[2]->getInt());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[3]->type);
		Assert::AreEqual(4, value->getList()[3]->getInt());
	}

	TEST_METHOD(SubInts) {
		interpreter.evaluate("i = 2 - 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(-1, value->getInt());
	}

	TEST_METHOD(SubFloats) {
		interpreter.evaluate("i = 2.4 - 3.4;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(-1.0f, value->getFloat());
	}

	TEST_METHOD(MulInts) {
		interpreter.evaluate("i = 2 * 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(6, value->getInt());
	}

	TEST_METHOD(MulFloats) {
		interpreter.evaluate("i = 2.5 * 3.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(7.5f, value->getFloat());
	}

	TEST_METHOD(DivInts) {
		interpreter.evaluate("i = 18 / 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(6, value->getInt());
	}

	TEST_METHOD(DivFloats) {
		interpreter.evaluate("i = 1.5 / 3.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(0.5f, value->getFloat());
	}

	TEST_METHOD(IntConvertToFloat) {
		interpreter.evaluate("i = 1.5 / 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(0.5f, value->getFloat());
	}

	TEST_METHOD(OrderOfOperations) {
		interpreter.evaluate("i = 2*1+(2.0 + 3/2);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(5.0f, value->getFloat());
	}

	TEST_METHOD(NumbersDontConvertStrings) {
		interpreter.evaluate("i = 1.5 / \"3\";"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::NONE, value->type);
		Assert::AreEqual(0, value->getInt());
	}

	TEST_METHOD(FunctionsDontConvert) {
		interpreter.evaluate("i = print + 7;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::NONE, value->type);
		Assert::AreEqual(0, value->getInt());
	}

	TEST_METHOD(StringConcatInt) {
		interpreter.evaluate("i = \"hi\" + 7;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::STRING, value->type);
		Assert::AreEqual("hi7"s, value->getString());
	}

	TEST_METHOD(StringConcatFloat) {
		interpreter.evaluate("i = \"hi\" + 7.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::STRING, value->type);
		Assert::AreEqual("hi7.000000"s, value->getString());
	}

	TEST_METHOD(StringConcatSecond) {
		interpreter.evaluate("i = 7 + \"hi\" + 7;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::STRING, value->type);
		Assert::AreEqual("7hi7"s, value->getString());
	}

	TEST_METHOD(ListAccess) {
		interpreter.evaluate("j = [1,2,3,4]; i = j[2];"s);
		auto value = interpreter.resolveVariable("i"s);
	
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(3, value->getInt());
	}

	TEST_METHOD(ListAppend) {
		interpreter.evaluate("i = []; i += 7;"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::KSType::LIST, value->type);
		Assert::AreEqual(1ull, value->getList().size());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[0]->type);
		Assert::AreEqual(7, value->getList()[0]->getInt());
	}

	TEST_METHOD(FunctionCreate) {
		interpreter.evaluate("func i(a){return a;}"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::KSType::FUNCTION, value->type);
		Assert::AreEqual(1ull, value->getFunction()->argNames.size());
		Assert::AreEqual("a"s, value->getFunction()->argNames[0]);
		Assert::AreEqual("i"s, value->getFunction()->name);
		Assert::AreEqual(1ull, value->getFunction()->subexpressions.size());
		Assert::AreEqual(KataScript::KSOperatorPrecedence::func, value->getFunction()->opPrecedence);

		// make sure function args don't leak into outer scope
		auto nonval = interpreter.resolveVariable("a"s);
		Assert::AreEqual(KataScript::KSType::NONE, nonval->type);
		Assert::AreEqual(0, nonval->getInt());
	}

	TEST_METHOD(FunctionCall) {
		interpreter.evaluate("func j(a){return a;} i = j(999);"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(999, value->getInt());
	}

	TEST_METHOD(FunctionAssign) {
		interpreter.evaluate("func j(a){return a;} b = j; i = b(999);"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(999, value->getInt());
	}

	// todo add more tests

	};
}

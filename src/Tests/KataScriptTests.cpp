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

	// todo add more tests


	};
}

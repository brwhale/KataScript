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
			
			template<> static std::wstring ToString<KataScript::vec3>(const KataScript::vec3& t) {
				auto str = KataScript::KSValue(t).getPrintString();
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
		Assert::AreEqual(KataScript::KSInt(0), value->getInt());
	}

	TEST_METHOD(AssignInt) {	
		interpreter.evaluate("i = 5;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(5), value->getInt());
	}

	TEST_METHOD(AssignFloat) {
		interpreter.evaluate("i = 50.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(KataScript::KSFloat(50.0), value->getFloat());
	}

    TEST_METHOD(AssignIntNegative) {
        interpreter.evaluate("i = -5;"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::INT, value->type);
        Assert::AreEqual(KataScript::KSInt(-5), value->getInt());
    }

    TEST_METHOD(AssignFloatNegative) {
        interpreter.evaluate("i = -50.0;"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
        Assert::AreEqual(KataScript::KSFloat(-50.0), value->getFloat());
    }

	TEST_METHOD(AssignVec3) {
		interpreter.evaluate("i = vec3(1,3,5);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::VEC3, value->type);
		Assert::AreEqual(KataScript::vec3(1,3,5), value->getVec3());
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
		interpreter.evaluate("i = [1,2,3,4.0];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::LIST, value->type);
		Assert::AreEqual(4ull, value->getList().size());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[0]->type);
		Assert::AreEqual(KataScript::KSInt(1), value->getList()[0]->getInt());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[1]->type);
		Assert::AreEqual(KataScript::KSInt(2), value->getList()[1]->getInt());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[2]->type);
		Assert::AreEqual(KataScript::KSInt(3), value->getList()[2]->getInt());
		Assert::AreEqual(KataScript::KSType::FLOAT, value->getList()[3]->type);
		Assert::AreEqual(KataScript::KSFloat(4.0), value->getList()[3]->getFloat());
	}
	
	TEST_METHOD(AssignListOfVec3) {
		interpreter.evaluate("i = list(vec3(1,3,5), vec3(2,2,2), vec3(7,8,9));"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::LIST, value->type);
		Assert::AreEqual(3ull, value->getList().size());
		Assert::AreEqual(KataScript::vec3(1, 3, 5), value->getList()[0]->getVec3());
		Assert::AreEqual(KataScript::vec3(2, 2, 2), value->getList()[1]->getVec3()); 
		Assert::AreEqual(KataScript::vec3(7, 8, 9), value->getList()[2]->getVec3());
	}

	TEST_METHOD(AssignTinyList) {
		interpreter.evaluate("i = [];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::LIST, value->type);
		Assert::AreEqual(0ull, value->getList().size());
	}

	TEST_METHOD(AssignArray) {
		interpreter.evaluate("i = [1,2,3,4];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::ARRAY, value->type);
		Assert::AreEqual(4ull, value->getArray().size());
		Assert::AreEqual(KataScript::KSInt(1), value->getStdVector<KataScript::KSInt>()[0]);
		Assert::AreEqual(KataScript::KSInt(2), value->getStdVector<KataScript::KSInt>()[1]);
		Assert::AreEqual(KataScript::KSInt(3), value->getStdVector<KataScript::KSInt>()[2]);
		Assert::AreEqual(KataScript::KSInt(4), value->getStdVector<KataScript::KSInt>()[3]);
	}

	TEST_METHOD(AssignArrayOfVec3) {
		interpreter.evaluate("i = [vec3(1,3,5), vec3(2,2,2), vec3(7,8,9)];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::ARRAY, value->type);
		Assert::AreEqual(3ull, value->getArray().size());
		Assert::AreEqual(KataScript::vec3(1, 3, 5), value->getStdVector<KataScript::vec3>()[0]);
		Assert::AreEqual(KataScript::vec3(2, 2, 2), value->getStdVector<KataScript::vec3>()[1]);
		Assert::AreEqual(KataScript::vec3(7, 8, 9), value->getStdVector<KataScript::vec3>()[2]);
	}

    TEST_METHOD(AssignDictionary) {
        interpreter.evaluate("i = dictionary();"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::DICTIONARY, value->type);
        Assert::AreEqual(0ull, value->getDictionary().size());
    }

    TEST_METHOD(AssignDictionaryFromList) {
        interpreter.evaluate("i = []; i[\"winky\"] = \"pinky\";"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::DICTIONARY, value->type);
        Assert::AreEqual(1ull, value->getDictionary().size());
        Assert::AreEqual(KataScript::KSType::STRING, value->getDictionary().begin()->second->type);
        Assert::AreEqual("pinky"s, value->getDictionary().begin()->second->getString());
    }

    TEST_METHOD(DictionaryIndex) {
        interpreter.evaluate("j = []; j[\"winky\"] = \"pinky\"; i = j[\"winky\"];"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::STRING, value->type);
        Assert::AreEqual("pinky"s, value->getString());
    }

    TEST_METHOD(DictionaryIndexOfDictionaryIndex) {
        interpreter.evaluate("j = []; j[\"winky\"] = \"pinky\"; j[\"pinky\"] = \"tornado\"; i = j[j[\"winky\"]];"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::STRING, value->type);
        Assert::AreEqual("tornado"s, value->getString());
    }

	TEST_METHOD(AssignTinyArray) {
		interpreter.evaluate("i = array();"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::ARRAY, value->type);
		Assert::AreEqual(0ull, value->getArray().size());
	}

	TEST_METHOD(ArrayRejectWrongTypes) {
		interpreter.evaluate("i = array(1,2,3,4,5.0,\"bird\");"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::ARRAY, value->type);
		Assert::AreEqual(4ull, value->getArray().size());
		Assert::AreEqual(KataScript::KSInt(1), value->getStdVector<KataScript::KSInt>()[0]);
		Assert::AreEqual(KataScript::KSInt(2), value->getStdVector<KataScript::KSInt>()[1]);
		Assert::AreEqual(KataScript::KSInt(3), value->getStdVector<KataScript::KSInt>()[2]);
		Assert::AreEqual(KataScript::KSInt(4), value->getStdVector<KataScript::KSInt>()[3]);
	}

	TEST_METHOD(AssignChangeTypes) {
		interpreter.evaluate("i = list(1);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::LIST, value->type);
		Assert::AreEqual(1ull, value->getList().size());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[0]->type);
		Assert::AreEqual(KataScript::KSInt(1), value->getList()[0]->getInt());

		interpreter.evaluate("i = \"fish tacos\";"s);
		Assert::AreEqual(KataScript::KSType::STRING, value->type);
		Assert::AreEqual("fish tacos"s, value->getString());

		interpreter.evaluate("i = 5;"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(5), value->getInt());

		interpreter.evaluate("i = 50.0;"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(KataScript::KSFloat(50.0), value->getFloat());

		interpreter.evaluate("i = print;"s);
		Assert::AreEqual(KataScript::KSType::FUNCTION, value->type);
	}

	TEST_METHOD(AddInts) {
		interpreter.evaluate("i = 2 + 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(5), value->getInt());
	}

	TEST_METHOD(AddFloats) {
		interpreter.evaluate("i = 2.4 + 3.4;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(KataScript::KSFloat(5.8), value->getFloat());
	}

	TEST_METHOD(AddVec3s) {
		interpreter.evaluate("i = vec3(2.4) + vec3(3.4);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::VEC3, value->type);
		Assert::AreEqual(5.8f, value->getVec3().x);
	}

	TEST_METHOD(AddStrings) {
		interpreter.evaluate("i = \"fish \" + \"tacos\";"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::STRING, value->type);
		Assert::AreEqual("fish tacos"s, value->getString());
	}

	TEST_METHOD(AddArrays) {
		interpreter.evaluate("i = [1,2] + [3,4];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::ARRAY, value->type);
		Assert::AreEqual(4ull, value->getArray().size());
		Assert::AreEqual(KataScript::KSInt(1), value->getStdVector<KataScript::KSInt>()[0]);
		Assert::AreEqual(KataScript::KSInt(2), value->getStdVector<KataScript::KSInt>()[1]);
		Assert::AreEqual(KataScript::KSInt(3), value->getStdVector<KataScript::KSInt>()[2]);
		Assert::AreEqual(KataScript::KSInt(4), value->getStdVector<KataScript::KSInt>()[3]);
	}

	TEST_METHOD(AddLists) {
		interpreter.evaluate("i = [1,2.0] + [3,4.0];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::LIST, value->type);
		Assert::AreEqual(4ull, value->getList().size());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[0]->type);
		Assert::AreEqual(KataScript::KSInt(1), value->getList()[0]->getInt());
		Assert::AreEqual(KataScript::KSType::FLOAT, value->getList()[1]->type);
		Assert::AreEqual(KataScript::KSFloat(2.0), value->getList()[1]->getFloat());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[2]->type);
		Assert::AreEqual(KataScript::KSInt(3), value->getList()[2]->getInt());
		Assert::AreEqual(KataScript::KSType::FLOAT, value->getList()[3]->type);
		Assert::AreEqual(KataScript::KSFloat(4.0), value->getList()[3]->getFloat());
	}

    TEST_METHOD(AddDictionarys) {
        interpreter.evaluate("j[\"winky\"] = \"pinky\"; k[\"pinky\"] = \"tornado\"; l = j + k; i = l[l[\"winky\"]];"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::STRING, value->type);
        Assert::AreEqual("tornado"s, value->getString());
    }

	TEST_METHOD(SubInts) {
		interpreter.evaluate("i = 2 - 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(-1), value->getInt());
	}

    TEST_METHOD(SubIntsNegative) {
        interpreter.evaluate("i = 2 + -3;"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::INT, value->type);
        Assert::AreEqual(KataScript::KSInt(-1), value->getInt());
    }

	TEST_METHOD(SubFloats) {
		interpreter.evaluate("i = 2.4 - 3.4;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(KataScript::KSFloat(-1.0), value->getFloat());
	}

    TEST_METHOD(SubFloatsNegative) {
        interpreter.evaluate("i = 2.4 + -3.4;"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
        Assert::AreEqual(KataScript::KSFloat(-1.0), value->getFloat());
    }

	TEST_METHOD(MulInts) {
		interpreter.evaluate("i = 2 * 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(6), value->getInt());
	}

	TEST_METHOD(MulFloats) {
		interpreter.evaluate("i = 2.5 * 3.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(KataScript::KSFloat(7.5), value->getFloat());
	}

	TEST_METHOD(DivInts) {
		interpreter.evaluate("i = 18 / 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(6), value->getInt());
	}

	TEST_METHOD(DivFloats) {
		interpreter.evaluate("i = 1.5 / 3.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(KataScript::KSFloat(0.5), value->getFloat());
	}

	TEST_METHOD(AddEqualsInts) {
		interpreter.evaluate("i = 2; i += 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(5), value->getInt());
	}

	TEST_METHOD(AddEqualsFloats) {
		interpreter.evaluate("i = 2.4; i += 3.4;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(KataScript::KSFloat(5.8), value->getFloat());
	}

	TEST_METHOD(AddEqualsVec3s) {
		interpreter.evaluate("i = vec3(2.4); i += vec3(3.4);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::VEC3, value->type);
		Assert::AreEqual(5.8f, value->getVec3().x);
	}

	TEST_METHOD(AddEqualsStrings) {
		interpreter.evaluate("i = \"fish \"; i += \"tacos\";"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::STRING, value->type);
		Assert::AreEqual("fish tacos"s, value->getString());
	}

	TEST_METHOD(AddEqualsArrays) {
		interpreter.evaluate("i = array(1,2); i += array(3,4);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::ARRAY, value->type);
		Assert::AreEqual(4ull, value->getArray().size());
		Assert::AreEqual(KataScript::KSInt(1), value->getStdVector<KataScript::KSInt>()[0]);
		Assert::AreEqual(KataScript::KSInt(2), value->getStdVector<KataScript::KSInt>()[1]);
		Assert::AreEqual(KataScript::KSInt(3), value->getStdVector<KataScript::KSInt>()[2]);
		Assert::AreEqual(KataScript::KSInt(4), value->getStdVector<KataScript::KSInt>()[3]);
	}

	TEST_METHOD(AddEqualsLists) {
		interpreter.evaluate("i = [1,2.0]; i += [3,4.0];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::LIST, value->type);
		Assert::AreEqual(4ull, value->getList().size());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[0]->type);
		Assert::AreEqual(KataScript::KSInt(1), value->getList()[0]->getInt());
		Assert::AreEqual(KataScript::KSType::FLOAT, value->getList()[1]->type);
		Assert::AreEqual(KataScript::KSFloat(2.0), value->getList()[1]->getFloat());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[2]->type);
		Assert::AreEqual(KataScript::KSInt(3), value->getList()[2]->getInt());
		Assert::AreEqual(KataScript::KSType::FLOAT, value->getList()[3]->type);
		Assert::AreEqual(KataScript::KSFloat(4.0), value->getList()[3]->getFloat());
	}

	TEST_METHOD(SubEqualsInts) {
		interpreter.evaluate("i = 2; i -= 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(-1), value->getInt());
	}

	TEST_METHOD(SubEqualsFloats) {
		interpreter.evaluate("i = 2.4; i -= 3.4;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(KataScript::KSFloat(-1.0), value->getFloat());
	}

	TEST_METHOD(MulEqualsInts) {
		interpreter.evaluate("i = 2; i *= 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(6), value->getInt());
	}

	TEST_METHOD(MulEqualsFloats) {
		interpreter.evaluate("i = 2.5; i *= 3.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(KataScript::KSFloat(7.5), value->getFloat());
	}

	TEST_METHOD(DivEqualsInts) {
		interpreter.evaluate("i = 18; i /= 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(6), value->getInt());
	}

	TEST_METHOD(DivEqualsFloats) {
		interpreter.evaluate("i = 1.5; i /= 3.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(KataScript::KSFloat(0.5), value->getFloat());
	}

	TEST_METHOD(IntConvertToFloat) {
		interpreter.evaluate("i = 1.5 / 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(KataScript::KSFloat(0.5), value->getFloat());
	}

	TEST_METHOD(OrderOfOperations) {
		interpreter.evaluate("i = 2*1+(2.0 + 3/2);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(KataScript::KSFloat(5.0), value->getFloat());
	}

	TEST_METHOD(NumbersDontConvertStrings) {
		interpreter.evaluate("i = 1.5 / \"3\";"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::NONE, value->type);
		Assert::AreEqual(KataScript::KSInt(0), value->getInt());
	}

	TEST_METHOD(FunctionsDontConvert) {
		interpreter.evaluate("i = print + 7;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::NONE, value->type);
		Assert::AreEqual(KataScript::KSInt(0), value->getInt());
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

	TEST_METHOD(ArrayAccess) {
		interpreter.evaluate("j = array(1,2,3,4); i = j[2];"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(3), value->getInt());
	}

	TEST_METHOD(ArrayAppend) {
		interpreter.evaluate("i = array(0); i += 7;"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::KSType::ARRAY, value->type);
		Assert::AreEqual(2ull, value->getArray().size());
		Assert::AreEqual(KataScript::KSInt(7), value->getStdVector<KataScript::KSInt>()[1]);
	}

	TEST_METHOD(ListAccess) {
		interpreter.evaluate("j = [1,2,3,4]; i = j[2];"s);
		auto value = interpreter.resolveVariable("i"s);
	
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(3), value->getInt());
	}

	TEST_METHOD(ListAppend) {
		interpreter.evaluate("i = []; i += 7;"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::KSType::LIST, value->type);
		Assert::AreEqual(1ull, value->getList().size());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[0]->type);
		Assert::AreEqual(KataScript::KSInt(7), value->getList()[0]->getInt());
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
		Assert::AreEqual(KataScript::KSInt(0), nonval->getInt());
	}

	TEST_METHOD(FunctionCall) {
		interpreter.evaluate("func j(a){return a;} i = j(999);"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(999), value->getInt());
	}

	TEST_METHOD(FunctionResultForMath) {
		interpreter.evaluate("func j(){return 999;} i = 1 + j() - 2;"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(998), value->getInt());
	}

	TEST_METHOD(FunctionResultForLoopExpression) {
		interpreter.evaluate("func j(){return 4;} a = 0; for(i=0;i<=j();i++){a+=i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(10), value->getInt());
	}

	TEST_METHOD(LoopFor3State) {
		interpreter.evaluate("a = 0; for(i=0;i<=4;i++){a+=i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(10), value->getInt());
	}

	TEST_METHOD(LoopFor2State) {
		interpreter.evaluate("a = 0; i = 0; for(i<=4;i++){a+=i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(10), value->getInt());
	}

	TEST_METHOD(LoopFor1State) {
		interpreter.evaluate("a = 0; i = 0; for(i<=4){a+=i; ++i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(10), value->getInt());
	}

	TEST_METHOD(LoopWhile) {
		interpreter.evaluate("a = 0; i = 0; while(i<=4){a+=i; ++i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(10), value->getInt());
	}

	TEST_METHOD(LoopForEach) {
		interpreter.evaluate("a = 0; b = [1,2,3,4]; foreach(i; b){a+=i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(10), value->getInt());
	}

	TEST_METHOD(IfElseSimpleIfTrue) {
		interpreter.evaluate("a = 0; b = true; if(b){a+=10;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(10), value->getInt());
	}

	TEST_METHOD(IfElseSimpleIfFalse) {
		interpreter.evaluate("a = 10; b = false; if(b){a-=10;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(10), value->getInt());
	}

	TEST_METHOD(IfElseSimpleIfTrueElse) {
		interpreter.evaluate("a = 0; b = true; if(b){a+=10;}else{a=100;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(10), value->getInt());
	}

	TEST_METHOD(IfElseSimpleIfFalseElse) {
		interpreter.evaluate("a = 0; b = false; if(b){a-=10;}else{a=10;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(10), value->getInt());
	}

	TEST_METHOD(IfElseComplexIfTrueElse) {
		interpreter.evaluate("a = 0; b = true; if(b){a+=10;}else if(b){a=23;}else{a=100;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(10), value->getInt());
	}

	TEST_METHOD(IfElseComplexIfFalseElse) {
		interpreter.evaluate("a = 0; b = false; if(b){a-=10;}else if(b){a=23;}else{a=10;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(KataScript::KSInt(10), value->getInt());
	}

    TEST_METHOD(IfInsideNestedForLoops) {
        interpreter.evaluate("v = [1,2,3,4]; a=0; for(i=0;i<4;i++){for(j=i+1;j<4;j++){ if (i * j < 2){a += v[i] + v[j];}}}"s);

        auto value = interpreter.resolveVariable("a"s);

        Assert::AreEqual(KataScript::KSType::INT, value->type);
        Assert::AreEqual(KataScript::KSInt(12), value->getInt());
    }

    TEST_METHOD(IfElseInsideNestedForLoops) {
        interpreter.evaluate("v = [1,2,3,4]; a=0; for(i=0;i<4;i++){for(j=i+1;j<4;j++){ if (i * j < 2){a += v[i] + v[j];}else{a+=1;}}}"s);

        auto value = interpreter.resolveVariable("a"s);

        Assert::AreEqual(KataScript::KSType::INT, value->type);
        Assert::AreEqual(KataScript::KSInt(15), value->getInt());
    }

    TEST_METHOD(IndexFunctionResult) {
        interpreter.evaluate("a = split(split(\"1-3 a: absdf\", \" \")[0], \"-\")");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::ARRAY, value->type);
        Assert::AreEqual(2ull, value->getArray().size());
        Assert::AreEqual("1"s, value->getStdVector<std::string>()[0]);
        Assert::AreEqual("3"s, value->getStdVector<std::string>()[1]);
    }

    TEST_METHOD(IndexResultOfFunctionResult) {
        interpreter.evaluate("tokens = split(\"1-3 a: absdf\", \" \"); r = split(tokens[1], \":\")[0];");

        auto value = interpreter.resolveVariable("r"s);
        Assert::AreEqual(KataScript::KSType::STRING, value->type);
        Assert::AreEqual("a"s, value->getString());
    }

    TEST_METHOD(BooleanOrder) {
        interpreter.evaluate("a = 0 >= 1 && 0 <= 3;");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::INT, value->type);
        Assert::AreEqual(KataScript::KSInt(0), value->getInt());
    }

    TEST_METHOD(FunctionIfScoping) {
        interpreter.evaluate("func j(s) { if (s) { print(s); } a = 2; return a; }");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::NONE, value->type);
        Assert::AreEqual(KataScript::KSInt(0), value->getInt());
    }

    TEST_METHOD(FunctionNestedControlFlowReturn) {
        interpreter.evaluate("func f() { if (1) { for(1){return 5;} } } i = f();");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::INT, value->type);
        Assert::AreEqual(KataScript::KSInt(5), value->getInt());
    }

    TEST_METHOD(EmptyListLiteralinsideFunctionParse) {
        interpreter.evaluate("a = typeof([]);");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::STRING, value->type);
        Assert::AreEqual("LIST"s, value->getString());
    }

    TEST_METHOD(MapFunction) {
        interpreter.evaluate("i = [0,1,2,3]; func add1(a) {return a + 1; } i = map(i, add1);"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::LIST, value->type);
        Assert::AreEqual(4ull, value->getList().size());
        Assert::AreEqual(KataScript::KSInt(1), value->getList()[0]->getInt());
        Assert::AreEqual(KataScript::KSInt(2), value->getList()[1]->getInt());
        Assert::AreEqual(KataScript::KSInt(3), value->getList()[2]->getInt());
        Assert::AreEqual(KataScript::KSInt(4), value->getList()[3]->getInt());
    }

    TEST_METHOD(FoldString) {
        interpreter.evaluate("a = fold([1,2,3,4],+,\"\");");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::STRING, value->type);
        Assert::AreEqual("1234"s, value->getString());
    }

    TEST_METHOD(DotSyntaxMapFunction) {
        interpreter.evaluate("i = [0,1,2,3]; func add1(a) {return a + 1; } i = i.map(add1);"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::LIST, value->type);
        Assert::AreEqual(4ull, value->getList().size());
        Assert::AreEqual(KataScript::KSInt(1), value->getList()[0]->getInt());
        Assert::AreEqual(KataScript::KSInt(2), value->getList()[1]->getInt());
        Assert::AreEqual(KataScript::KSInt(3), value->getList()[2]->getInt());
        Assert::AreEqual(KataScript::KSInt(4), value->getList()[3]->getInt());
    }

    TEST_METHOD(DotSyntaxFoldString) {
        interpreter.evaluate("a = [1,2,3,4].fold(+,\"\");");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::STRING, value->type);
        Assert::AreEqual("1234"s, value->getString());
    }

    TEST_METHOD(FoldInt) {
        interpreter.evaluate("a = fold([1,2,3,4],+,0);");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::INT, value->type);
        Assert::AreEqual(KataScript::KSInt(10), value->getInt());
    }

    TEST_METHOD(ForEachOverALiteral) {
        interpreter.evaluate("i = []; foreach(a; [1,2,3,4]) { i += a; }"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::LIST, value->type);
        Assert::AreEqual(4ull, value->getList().size());
        Assert::AreEqual(KataScript::KSInt(1), value->getList()[0]->getInt());
        Assert::AreEqual(KataScript::KSInt(2), value->getList()[1]->getInt());
        Assert::AreEqual(KataScript::KSInt(3), value->getList()[2]->getInt());
        Assert::AreEqual(KataScript::KSInt(4), value->getList()[3]->getInt());
    }

    TEST_METHOD(RecursiveFunc) {
        interpreter.evaluate("i = []; func r(n) { if (n > 0) { i += n; r(--n); } } r(4);"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::LIST, value->type);
        Assert::AreEqual(4ull, value->getList().size());
        Assert::AreEqual(KataScript::KSInt(4), value->getList()[0]->getInt());
        Assert::AreEqual(KataScript::KSInt(3), value->getList()[1]->getInt());
        Assert::AreEqual(KataScript::KSInt(2), value->getList()[2]->getInt());
        Assert::AreEqual(KataScript::KSInt(1), value->getList()[3]->getInt());
    }

    TEST_METHOD(RecursiveList) {
        interpreter.evaluate("func r(n) { var l = []; if (n > 0) { l += n; if (n > 1){l += r(--n); }}  return l; } i = r(4);"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::LIST, value->type);
        Assert::AreEqual(4ull, value->getList().size());
        Assert::AreEqual(KataScript::KSInt(4), value->getList()[0]->getInt());
        Assert::AreEqual(KataScript::KSInt(3), value->getList()[1]->getInt());
        Assert::AreEqual(KataScript::KSInt(2), value->getList()[2]->getInt());
        Assert::AreEqual(KataScript::KSInt(1), value->getList()[3]->getInt());
    }

    TEST_METHOD(MinInts) {
        interpreter.evaluate("i = min(5,12);");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::INT, value->type);
        Assert::AreEqual(KataScript::KSInt(5), value->getInt());
    }

    TEST_METHOD(MaxInts) {
        interpreter.evaluate("i = max(5,-12);");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::INT, value->type);
        Assert::AreEqual(KataScript::KSInt(5), value->getInt());
    }

    TEST_METHOD(Swap) {
        interpreter.evaluate("i = 1; j = 5; swap(i,j);");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::INT, value->type);
        Assert::AreEqual(KataScript::KSInt(5), value->getInt());

        auto value2 = interpreter.resolveVariable("j"s);
        Assert::AreEqual(KataScript::KSType::INT, value2->type);
        Assert::AreEqual(KataScript::KSInt(1), value2->getInt());
    }

    TEST_METHOD(Pow) {
        interpreter.evaluate("i = pow(2,.5);");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
        Assert::AreEqual(KataScript::KSFloat(pow(2.0,.5)), value->getFloat());
    }

    TEST_METHOD(DictConvertToListKeepOldIndexes) {
        interpreter.evaluate("i = [0,1,2,3,4,5]; i[\"taco\"] = \"pizza\"; a = i[\"taco\"]; b = i[3];");

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::STRING, val->type);
        Assert::AreEqual("pizza"s, val->getString());

        auto value = interpreter.resolveVariable("b"s);
        Assert::AreEqual(KataScript::KSType::INT, value->type);
        Assert::AreEqual(KataScript::KSInt(3), value->getInt());
    }

    TEST_METHOD(ScopingViolationBugfix) {
        interpreter.evaluate("func f() { while(1) { print(1); } tacos = 5; i = 1; return i; }");

        auto val = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::KSType::NONE, val->type);
    }

    TEST_METHOD(DifferentTypesNeverEqual) {
        interpreter.evaluate("a = \"\" == []; b = 4.0 == 4; c = list() == array(); d = list() == null; e = null == []; f = 0 == null;");

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::INT, val->type);
        Assert::AreEqual(KataScript::KSInt(false), val->getInt());

        val = interpreter.resolveVariable("b"s);
        Assert::AreEqual(KataScript::KSType::INT, val->type);
        Assert::AreEqual(KataScript::KSInt(false), val->getInt());

        val = interpreter.resolveVariable("c"s);
        Assert::AreEqual(KataScript::KSType::INT, val->type);
        Assert::AreEqual(KataScript::KSInt(false), val->getInt());

        val = interpreter.resolveVariable("d"s);
        Assert::AreEqual(KataScript::KSType::INT, val->type);
        Assert::AreEqual(KataScript::KSInt(false), val->getInt());

        val = interpreter.resolveVariable("e"s);
        Assert::AreEqual(KataScript::KSType::INT, val->type);
        Assert::AreEqual(KataScript::KSInt(false), val->getInt());

        val = interpreter.resolveVariable("f"s);
        Assert::AreEqual(KataScript::KSType::INT, val->type);
        Assert::AreEqual(KataScript::KSInt(false), val->getInt());
    }

    TEST_METHOD(ConvertedTypesDoEqual) {
        interpreter.evaluate("a = \"\" == string([]); b = 4.0 == float(4); c = list(1,2,3) == tolist(array(1,2,3)); d = null == null;");

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::INT, val->type);
        Assert::AreEqual(KataScript::KSInt(true), val->getInt());

        val = interpreter.resolveVariable("b"s);
        Assert::AreEqual(KataScript::KSType::INT, val->type);
        Assert::AreEqual(KataScript::KSInt(true), val->getInt());

        val = interpreter.resolveVariable("c"s);
        Assert::AreEqual(KataScript::KSType::INT, val->type);
        Assert::AreEqual(KataScript::KSInt(true), val->getInt());

        val = interpreter.resolveVariable("d"s);
        Assert::AreEqual(KataScript::KSType::INT, val->type);
        Assert::AreEqual(KataScript::KSInt(true), val->getInt());
    }

    TEST_METHOD(MakeStruct) {
        interpreter.evaluate("struct xy { var x; var y; func xy(_x, _y) { x = _x; y = _y; } func add(_x, _y) { x += _x; y += _y; } func sqr() { return x * y; } } "s +
            "a = xy(4,5.0);"s);

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::STRUCT, val->type);
        Assert::AreEqual(KataScript::KSType::INT, val->getStruct().variables["x"]->type);
        Assert::AreEqual(KataScript::KSInt(4), val->getStruct().variables["x"]->getInt());
        Assert::AreEqual(KataScript::KSType::FLOAT, val->getStruct().variables["y"]->type);
        Assert::AreEqual(KataScript::KSFloat(5.0), val->getStruct().variables["y"]->getFloat());
    }

    TEST_METHOD(AccessStruct) {
        interpreter.evaluate("struct xy { var x; var y; func xy(_x, _y) { x = _x; y = _y; } func add(_x, _y) { x += _x; y += _y; } func sqr() { return x * y; } } "s +
            "a = xy(4,5.0); c = a.sqr(); e = a.x; f = a.y;"s);

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::STRUCT, val->type);
        Assert::AreEqual(KataScript::KSType::INT, val->getStruct().variables["x"]->type);
        Assert::AreEqual(KataScript::KSInt(4), val->getStruct().variables["x"]->getInt());
        Assert::AreEqual(KataScript::KSType::FLOAT, val->getStruct().variables["y"]->type);
        Assert::AreEqual(KataScript::KSFloat(5.0), val->getStruct().variables["y"]->getFloat());

        val = interpreter.resolveVariable("c"s);
        Assert::AreEqual(KataScript::KSType::FLOAT, val->type);
        Assert::AreEqual(KataScript::KSFloat(20.0), val->getFloat());

        val = interpreter.resolveVariable("e"s);
        Assert::AreEqual(KataScript::KSType::INT, val->type);
        Assert::AreEqual(KataScript::KSInt(4), val->getInt());

        val = interpreter.resolveVariable("f"s);
        Assert::AreEqual(KataScript::KSType::FLOAT, val->type);
        Assert::AreEqual(KataScript::KSFloat(5.0), val->getFloat());
    }

    TEST_METHOD(MutateStruct) {
        interpreter.evaluate("struct xy { var x; var y; func xy(_x, _y) { x = _x; y = _y; } func add(_x, _y) { x += _x; y += _y; } func sqr() { return x * y; } } "s +
            "a = xy(4,5.0); c = a.sqr(); a.add(4,5); d = a.sqr(); e = a.x; f = a.y;"s);

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::STRUCT, val->type);
        Assert::AreEqual(KataScript::KSType::INT, val->getStruct().variables["x"]->type);
        Assert::AreEqual(KataScript::KSInt(8), val->getStruct().variables["x"]->getInt());
        Assert::AreEqual(KataScript::KSType::FLOAT, val->getStruct().variables["y"]->type);
        Assert::AreEqual(KataScript::KSFloat(10.0), val->getStruct().variables["y"]->getFloat());

        val = interpreter.resolveVariable("c"s);
        Assert::AreEqual(KataScript::KSType::FLOAT, val->type);
        Assert::AreEqual(KataScript::KSFloat(20.0), val->getFloat());

        val = interpreter.resolveVariable("d"s);
        Assert::AreEqual(KataScript::KSType::FLOAT, val->type);
        Assert::AreEqual(KataScript::KSFloat(80.0), val->getFloat());

        val = interpreter.resolveVariable("e"s);
        Assert::AreEqual(KataScript::KSType::INT, val->type);
        Assert::AreEqual(KataScript::KSInt(8), val->getInt());

        val = interpreter.resolveVariable("f"s);
        Assert::AreEqual(KataScript::KSType::FLOAT, val->type);
        Assert::AreEqual(KataScript::KSFloat(10.0), val->getFloat());
    }

    TEST_METHOD(CopyMutateStructLeaveOriginalAlone) {
        interpreter.evaluate("struct xy { var x; var y; func xy(_x, _y) { x = _x; y = _y; } func add(_x, _y) { x += _x; y += _y; } func sqr() { return x * y; } } "s +
            "a = xy(4,5.0); b = copy(a); b.add(\"a\",\"b\"); c = a.sqr(); a.add(4,5); d = a.sqr(); e = a.x; f = a.y;"s);

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::STRUCT, val->type);
        Assert::AreEqual(KataScript::KSType::INT, val->getStruct().variables["x"]->type);
        Assert::AreEqual(KataScript::KSInt(8), val->getStruct().variables["x"]->getInt());
        Assert::AreEqual(KataScript::KSType::FLOAT, val->getStruct().variables["y"]->type);
        Assert::AreEqual(KataScript::KSFloat(10.0), val->getStruct().variables["y"]->getFloat());

        val = interpreter.resolveVariable("b"s);
        Assert::AreEqual(KataScript::KSType::STRUCT, val->type);
        Assert::AreEqual(KataScript::KSType::STRING, val->getStruct().variables["x"]->type);
        Assert::AreEqual("4a"s, val->getStruct().variables["x"]->getString());
        Assert::AreEqual(KataScript::KSType::STRING, val->getStruct().variables["y"]->type);
        Assert::AreEqual("5.000000b"s, val->getStruct().variables["y"]->getString());

        val = interpreter.resolveVariable("c"s);
        Assert::AreEqual(KataScript::KSType::FLOAT, val->type);
        Assert::AreEqual(KataScript::KSFloat(20.0), val->getFloat());

        val = interpreter.resolveVariable("d"s);
        Assert::AreEqual(KataScript::KSType::FLOAT, val->type);
        Assert::AreEqual(KataScript::KSFloat(80.0), val->getFloat());

        val = interpreter.resolveVariable("e"s);
        Assert::AreEqual(KataScript::KSType::INT, val->type);
        Assert::AreEqual(KataScript::KSInt(8), val->getInt());

        val = interpreter.resolveVariable("f"s);
        Assert::AreEqual(KataScript::KSType::FLOAT, val->type);
        Assert::AreEqual(KataScript::KSFloat(10.0), val->getFloat());
    }

    TEST_METHOD(SimpleFunctionApplyIdentity) {
        interpreter.evaluate("func test(t) {return t(\"hey\");} a = test(identity);"s);

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::STRING, val->type);
        Assert::AreEqual("hey"s, val->getString());
    }

    TEST_METHOD(UserPointers) {
        interpreter.evaluate("ptrs = []; func storeptr(ptr) { ptrs+=ptr; } func getlast() { return ptrs.back(); }");
        auto ptr = &interpreter;
        interpreter.callFunction(interpreter.resolveFunction("storeptr"), ptr);
        interpreter.evaluate("a = getlast();");

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::USERPOINTER, val->type);
        auto ptrfetch = val->getPointer();
        Assert::AreEqual((size_t)&interpreter, (size_t)ptrfetch);
    }
    
	// todo add more tests

	};
}

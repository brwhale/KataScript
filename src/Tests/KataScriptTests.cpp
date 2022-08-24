#include "CppUnitTest.h"
#define KATASCRIPT_IMPL
#include "../../src/Library/KataScript.hpp"

using namespace std::string_literals;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

KataScript::KataScriptInterpreter* interpRef;

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			template<> static std::wstring ToString<KataScript::Type>(const KataScript::Type& t) {
				auto str = KataScript::getTypeName(t);
				return std::wstring(str.begin(), str.end());
			}

			template<> static std::wstring ToString<KataScript::OperatorPrecedence>(const KataScript::OperatorPrecedence& t) {
				return ToString((int)t);
			}
			
			template<> static std::wstring ToString<KataScript::vec3>(const KataScript::vec3& t) {
				auto str = KataScript::Value(t).getPrintString();
				return std::wstring(str.begin(), str.end());
			}
		}
	}
}

namespace KataScriptTests {
	TEST_CLASS(KataScriptTests) {
public:

	KataScript::KataScriptInterpreter interpreter = KataScript::KataScriptInterpreter(KataScript::ModulePrivilege::allPrivilege);

	TEST_METHOD_INITIALIZE(initTest) {
        interpreter.clearState();
        interpRef = &interpreter;
	}

	TEST_METHOD_CLEANUP(cleanTest) {
        interpreter.clearState();
        interpreter = KataScript::KataScriptInterpreter(KataScript::ModulePrivilege::allPrivilege);
	}

	TEST_METHOD(AssignNull) {
		interpreter.evaluate("i;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Null, value->type);
		Assert::AreEqual(KataScript::Int(0), value->getInt());
	}

	TEST_METHOD(AssignInt) {	
		interpreter.evaluate("i = 5;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(5), value->getInt());
	}

	TEST_METHOD(AssignFloat) {
		interpreter.evaluate("i = 50.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Float, value->type);
		Assert::AreEqual(KataScript::Float(50.0), value->getFloat());
	}

    TEST_METHOD(AssignIntNegative) {
        interpreter.evaluate("i = -5;"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(-5), value->getInt());
    }

    TEST_METHOD(AssignFloatNegative) {
        interpreter.evaluate("i = -50.0;"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Float, value->type);
        Assert::AreEqual(KataScript::Float(-50.0), value->getFloat());
    }

	TEST_METHOD(AssignVec3) {
		interpreter.evaluate("i = vec3(1,3,5);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Vec3, value->type);
		Assert::AreEqual(KataScript::vec3(1,3,5), value->getVec3());
	}

	TEST_METHOD(AssignFunction) {
		interpreter.evaluate("i = print;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Function, value->type);
	}

	TEST_METHOD(AssignString) {
		interpreter.evaluate("i = \"fish tacos\";"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::String, value->type);
		Assert::AreEqual("fish tacos"s, value->getString());
	}

	TEST_METHOD(AssignList) {
		interpreter.evaluate("i = [1,2,3,4.0];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::List, value->type);
		Assert::AreEqual(4ull, value->getList().size());
		Assert::AreEqual(KataScript::Type::Int, value->getList()[0]->type);
		Assert::AreEqual(KataScript::Int(1), value->getList()[0]->getInt());
		Assert::AreEqual(KataScript::Type::Int, value->getList()[1]->type);
		Assert::AreEqual(KataScript::Int(2), value->getList()[1]->getInt());
		Assert::AreEqual(KataScript::Type::Int, value->getList()[2]->type);
		Assert::AreEqual(KataScript::Int(3), value->getList()[2]->getInt());
		Assert::AreEqual(KataScript::Type::Float, value->getList()[3]->type);
		Assert::AreEqual(KataScript::Float(4.0), value->getList()[3]->getFloat());
	}
	
	TEST_METHOD(AssignListOfVec3) {
		interpreter.evaluate("i = list(vec3(1,3,5), vec3(2,2,2), vec3(7,8,9));"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::List, value->type);
		Assert::AreEqual(3ull, value->getList().size());
		Assert::AreEqual(KataScript::vec3(1, 3, 5), value->getList()[0]->getVec3());
		Assert::AreEqual(KataScript::vec3(2, 2, 2), value->getList()[1]->getVec3()); 
		Assert::AreEqual(KataScript::vec3(7, 8, 9), value->getList()[2]->getVec3());
	}

	TEST_METHOD(AssignTinyList) {
		interpreter.evaluate("i = [];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::List, value->type);
		Assert::AreEqual(0ull, value->getList().size());
	}

	TEST_METHOD(AssignArray) {
		interpreter.evaluate("i = [1,2,3,4];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Array, value->type);
		Assert::AreEqual(4ull, value->getArray().size());
		Assert::AreEqual(KataScript::Int(1), value->getStdVector<KataScript::Int>()[0]);
		Assert::AreEqual(KataScript::Int(2), value->getStdVector<KataScript::Int>()[1]);
		Assert::AreEqual(KataScript::Int(3), value->getStdVector<KataScript::Int>()[2]);
		Assert::AreEqual(KataScript::Int(4), value->getStdVector<KataScript::Int>()[3]);
	}

	TEST_METHOD(AssignArrayOfVec3) {
		interpreter.evaluate("i = [vec3(1,3,5), vec3(2,2,2), vec3(7,8,9)];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Array, value->type);
		Assert::AreEqual(3ull, value->getArray().size());
		Assert::AreEqual(KataScript::vec3(1, 3, 5), value->getStdVector<KataScript::vec3>()[0]);
		Assert::AreEqual(KataScript::vec3(2, 2, 2), value->getStdVector<KataScript::vec3>()[1]);
		Assert::AreEqual(KataScript::vec3(7, 8, 9), value->getStdVector<KataScript::vec3>()[2]);
	}

    TEST_METHOD(AssignDictionary) {
        interpreter.evaluate("i = dictionary();"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Dictionary, value->type);
        Assert::AreEqual(0ull, value->getDictionary().size());
    }

    TEST_METHOD(AssignDictionaryFromList) {
        interpreter.evaluate("i = []; i[\"winky\"] = \"pinky\";"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Dictionary, value->type);
        Assert::AreEqual(1ull, value->getDictionary().size());
        Assert::AreEqual(KataScript::Type::String, value->getDictionary().begin()->second->type);
        Assert::AreEqual("pinky"s, value->getDictionary().begin()->second->getString());
    }

    TEST_METHOD(DictionaryIndex) {
        interpreter.evaluate("j = []; j[\"winky\"] = \"pinky\"; i = j[\"winky\"];"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("pinky"s, value->getString());
    }

    TEST_METHOD(DictionaryIndexOfDictionaryIndex) {
        interpreter.evaluate("j = []; j[\"winky\"] = \"pinky\"; j[\"pinky\"] = \"tornado\"; i = j[j[\"winky\"]];"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("tornado"s, value->getString());
    }

	TEST_METHOD(AssignTinyArray) {
		interpreter.evaluate("i = array();"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Array, value->type);
		Assert::AreEqual(0ull, value->getArray().size());
	}

	TEST_METHOD(ArrayRejectWrongTypes) {
		interpreter.evaluate("i = array(1,2,3,4,5.0,\"bird\");"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Array, value->type);
		Assert::AreEqual(4ull, value->getArray().size());
		Assert::AreEqual(KataScript::Int(1), value->getStdVector<KataScript::Int>()[0]);
		Assert::AreEqual(KataScript::Int(2), value->getStdVector<KataScript::Int>()[1]);
		Assert::AreEqual(KataScript::Int(3), value->getStdVector<KataScript::Int>()[2]);
		Assert::AreEqual(KataScript::Int(4), value->getStdVector<KataScript::Int>()[3]);
	}

	TEST_METHOD(AssignChangeTypes) {
		interpreter.evaluate("i = list(1);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::List, value->type);
		Assert::AreEqual(1ull, value->getList().size());
		Assert::AreEqual(KataScript::Type::Int, value->getList()[0]->type);
		Assert::AreEqual(KataScript::Int(1), value->getList()[0]->getInt());

		interpreter.evaluate("i = \"fish tacos\";"s);
		Assert::AreEqual(KataScript::Type::String, value->type);
		Assert::AreEqual("fish tacos"s, value->getString());

		interpreter.evaluate("i = 5;"s);
		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(5), value->getInt());

		interpreter.evaluate("i = 50.0;"s);
		Assert::AreEqual(KataScript::Type::Float, value->type);
		Assert::AreEqual(KataScript::Float(50.0), value->getFloat());

		interpreter.evaluate("i = print;"s);
		Assert::AreEqual(KataScript::Type::Function, value->type);
	}

	TEST_METHOD(AddInts) {
		interpreter.evaluate("i = 2 + 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(5), value->getInt());
	}

	TEST_METHOD(AddFloats) {
		interpreter.evaluate("i = 2.4 + 3.4;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Float, value->type);
		Assert::AreEqual(KataScript::Float(5.8), value->getFloat());
	}

	TEST_METHOD(AddVec3s) {
		interpreter.evaluate("i = vec3(2.4) + vec3(3.4);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Vec3, value->type);
		Assert::AreEqual(5.8f, value->getVec3().x);
	}

	TEST_METHOD(AddStrings) {
		interpreter.evaluate("i = \"fish \" + \"tacos\";"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::String, value->type);
		Assert::AreEqual("fish tacos"s, value->getString());
	}

	TEST_METHOD(AddArrays) {
		interpreter.evaluate("i = [1,2] + [3,4];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Array, value->type);
		Assert::AreEqual(4ull, value->getArray().size());
		Assert::AreEqual(KataScript::Int(1), value->getStdVector<KataScript::Int>()[0]);
		Assert::AreEqual(KataScript::Int(2), value->getStdVector<KataScript::Int>()[1]);
		Assert::AreEqual(KataScript::Int(3), value->getStdVector<KataScript::Int>()[2]);
		Assert::AreEqual(KataScript::Int(4), value->getStdVector<KataScript::Int>()[3]);
	}

	TEST_METHOD(AddLists) {
		interpreter.evaluate("i = [1,2.0] + [3,4.0];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::List, value->type);
		Assert::AreEqual(4ull, value->getList().size());
		Assert::AreEqual(KataScript::Type::Int, value->getList()[0]->type);
		Assert::AreEqual(KataScript::Int(1), value->getList()[0]->getInt());
		Assert::AreEqual(KataScript::Type::Float, value->getList()[1]->type);
		Assert::AreEqual(KataScript::Float(2.0), value->getList()[1]->getFloat());
		Assert::AreEqual(KataScript::Type::Int, value->getList()[2]->type);
		Assert::AreEqual(KataScript::Int(3), value->getList()[2]->getInt());
		Assert::AreEqual(KataScript::Type::Float, value->getList()[3]->type);
		Assert::AreEqual(KataScript::Float(4.0), value->getList()[3]->getFloat());
	}

    TEST_METHOD(AddDictionarys) {
        interpreter.evaluate("j[\"winky\"] = \"pinky\"; k[\"pinky\"] = \"tornado\"; l = j + k; i = l[l[\"winky\"]];"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("tornado"s, value->getString());
    }

	TEST_METHOD(SubInts) {
		interpreter.evaluate("i = 2 - 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(-1), value->getInt());
	}

    TEST_METHOD(SubIntsNegative) {
        interpreter.evaluate("i = 2 + -3;"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(-1), value->getInt());
    }

	TEST_METHOD(SubFloats) {
		interpreter.evaluate("i = 2.4 - 3.4;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Float, value->type);
		Assert::AreEqual(KataScript::Float(-1.0), value->getFloat());
	}

    TEST_METHOD(SubFloatsNegative) {
        interpreter.evaluate("i = 2.4 + -3.4;"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Float, value->type);
        Assert::AreEqual(KataScript::Float(-1.0), value->getFloat());
    }

	TEST_METHOD(MulInts) {
		interpreter.evaluate("i = 2 * 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(6), value->getInt());
	}

	TEST_METHOD(MulFloats) {
		interpreter.evaluate("i = 2.5 * 3.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Float, value->type);
		Assert::AreEqual(KataScript::Float(7.5), value->getFloat());
	}

	TEST_METHOD(DivInts) {
		interpreter.evaluate("i = 18 / 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(6), value->getInt());
	}

	TEST_METHOD(DivFloats) {
		interpreter.evaluate("i = 1.5 / 3.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Float, value->type);
		Assert::AreEqual(KataScript::Float(0.5), value->getFloat());
	}

	TEST_METHOD(AddEqualsInts) {
		interpreter.evaluate("i = 2; i += 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(5), value->getInt());
	}

	TEST_METHOD(AddEqualsFloats) {
		interpreter.evaluate("i = 2.4; i += 3.4;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Float, value->type);
		Assert::AreEqual(KataScript::Float(5.8), value->getFloat());
	}

	TEST_METHOD(AddEqualsVec3s) {
		interpreter.evaluate("i = vec3(2.4); i += vec3(3.4);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Vec3, value->type);
		Assert::AreEqual(5.8f, value->getVec3().x);
	}

	TEST_METHOD(AddEqualsStrings) {
		interpreter.evaluate("i = \"fish \"; i += \"tacos\";"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::String, value->type);
		Assert::AreEqual("fish tacos"s, value->getString());
	}

	TEST_METHOD(AddEqualsArrays) {
		interpreter.evaluate("i = array(1,2); i += array(3,4);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Array, value->type);
		Assert::AreEqual(4ull, value->getArray().size());
		Assert::AreEqual(KataScript::Int(1), value->getStdVector<KataScript::Int>()[0]);
		Assert::AreEqual(KataScript::Int(2), value->getStdVector<KataScript::Int>()[1]);
		Assert::AreEqual(KataScript::Int(3), value->getStdVector<KataScript::Int>()[2]);
		Assert::AreEqual(KataScript::Int(4), value->getStdVector<KataScript::Int>()[3]);
	}

	TEST_METHOD(AddEqualsLists) {
		interpreter.evaluate("i = [1,2.0]; i += [3,4.0];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::List, value->type);
		Assert::AreEqual(4ull, value->getList().size());
		Assert::AreEqual(KataScript::Type::Int, value->getList()[0]->type);
		Assert::AreEqual(KataScript::Int(1), value->getList()[0]->getInt());
		Assert::AreEqual(KataScript::Type::Float, value->getList()[1]->type);
		Assert::AreEqual(KataScript::Float(2.0), value->getList()[1]->getFloat());
		Assert::AreEqual(KataScript::Type::Int, value->getList()[2]->type);
		Assert::AreEqual(KataScript::Int(3), value->getList()[2]->getInt());
		Assert::AreEqual(KataScript::Type::Float, value->getList()[3]->type);
		Assert::AreEqual(KataScript::Float(4.0), value->getList()[3]->getFloat());
	}

	TEST_METHOD(SubEqualsInts) {
		interpreter.evaluate("i = 2; i -= 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(-1), value->getInt());
	}

	TEST_METHOD(SubEqualsFloats) {
		interpreter.evaluate("i = 2.4; i -= 3.4;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Float, value->type);
		Assert::AreEqual(KataScript::Float(-1.0), value->getFloat());
	}

	TEST_METHOD(MulEqualsInts) {
		interpreter.evaluate("i = 2; i *= 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(6), value->getInt());
	}

	TEST_METHOD(MulEqualsFloats) {
		interpreter.evaluate("i = 2.5; i *= 3.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Float, value->type);
		Assert::AreEqual(KataScript::Float(7.5), value->getFloat());
	}

	TEST_METHOD(DivEqualsInts) {
		interpreter.evaluate("i = 18; i /= 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(6), value->getInt());
	}

	TEST_METHOD(DivEqualsFloats) {
		interpreter.evaluate("i = 1.5; i /= 3.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Float, value->type);
		Assert::AreEqual(KataScript::Float(0.5), value->getFloat());
	}

	TEST_METHOD(IntConvertToFloat) {
		interpreter.evaluate("i = 1.5 / 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Float, value->type);
		Assert::AreEqual(KataScript::Float(0.5), value->getFloat());
	}

	TEST_METHOD(OrderOfOperations) {
		interpreter.evaluate("i = 2*1+(2.0 + 3/2);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Float, value->type);
		Assert::AreEqual(KataScript::Float(5.0), value->getFloat());
	}

	TEST_METHOD(NumbersDontConvertStrings) {
		interpreter.evaluate("i = 1.5 / \"3\";"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Null, value->type);
		Assert::AreEqual(KataScript::Int(0), value->getInt());
	}

	TEST_METHOD(FunctionsDontConvert) {
		interpreter.evaluate("i = print + 7;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::Null, value->type);
		Assert::AreEqual(KataScript::Int(0), value->getInt());
	}

	TEST_METHOD(StringConcatInt) {
		interpreter.evaluate("i = \"hi\" + 7;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::String, value->type);
		Assert::AreEqual("hi7"s, value->getString());
	}

	TEST_METHOD(StringConcatFloat) {
		interpreter.evaluate("i = \"hi\" + 7.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::String, value->type);
		Assert::AreEqual("hi7.000000"s, value->getString());
	}

	TEST_METHOD(StringConcatSecond) {
		interpreter.evaluate("i = 7 + \"hi\" + 7;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::Type::String, value->type);
		Assert::AreEqual("7hi7"s, value->getString());
	}

    TEST_METHOD(StringTruthy) {
        interpreter.evaluate("i = \"false\";"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual(true, value->getBool());
    }

    TEST_METHOD(StringFalsy) {
        interpreter.evaluate("i = \"\";"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual(false, value->getBool());
    }

    TEST_METHOD(StringFalsyConstructed) {
        interpreter.evaluate("i = string();"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual(false, value->getBool());
    }    

	TEST_METHOD(ArrayAccess) {
		interpreter.evaluate("j = array(1,2,3,4); i = j[2];"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(3), value->getInt());
	}

	TEST_METHOD(ArrayAppend) {
		interpreter.evaluate("i = array(0); i += 7;"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::Type::Array, value->type);
		Assert::AreEqual(2ull, value->getArray().size());
		Assert::AreEqual(KataScript::Int(7), value->getStdVector<KataScript::Int>()[1]);
	}

	TEST_METHOD(ListAccess) {
		interpreter.evaluate("j = [1,2,3,4]; i = j[2];"s);
		auto value = interpreter.resolveVariable("i"s);
	
		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(3), value->getInt());
	}

	TEST_METHOD(ListAppend) {
		interpreter.evaluate("i = []; i += 7;"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::Type::List, value->type);
		Assert::AreEqual(1ull, value->getList().size());
		Assert::AreEqual(KataScript::Type::Int, value->getList()[0]->type);
		Assert::AreEqual(KataScript::Int(7), value->getList()[0]->getInt());
	}

	TEST_METHOD(FunctionCreate) {
		interpreter.evaluate("func i(a){return a;}"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::Type::Function, value->type);
		Assert::AreEqual(1ull, value->getFunction()->argNames.size());
		Assert::AreEqual("a"s, value->getFunction()->argNames[0]);
		Assert::AreEqual("i"s, value->getFunction()->name);
		Assert::AreEqual(1ull, value->getFunction()->subexpressions.size());
		Assert::AreEqual(KataScript::OperatorPrecedence::func, value->getFunction()->opPrecedence);

		// make sure function args don't leak into outer scope
		auto nonval = interpreter.resolveVariable("a"s);
		Assert::AreEqual(KataScript::Type::Null, nonval->type);
		Assert::AreEqual(KataScript::Int(0), nonval->getInt());
	}

	TEST_METHOD(FunctionCall) {
		interpreter.evaluate("function j(a){return a;} i = j(999);"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(999), value->getInt());
	}

    TEST_METHOD(FuncCall) {
        interpreter.evaluate("func j(a){return a;} i = j(999);"s);
        auto value = interpreter.resolveVariable("i"s);

        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(999), value->getInt());
    }

    TEST_METHOD(FNCall) {
        interpreter.evaluate("fn j(a){return a;} i = j(999);"s);
        auto value = interpreter.resolveVariable("i"s);

        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(999), value->getInt());
    }

	TEST_METHOD(FunctionResultForMath) {
		interpreter.evaluate("func j(){return 999;} i = 1 + j() - 2;"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(998), value->getInt());
	}

	TEST_METHOD(FunctionResultForLoopExpression) {
		interpreter.evaluate("func j(){return 4;} a = 0; for(i=0;i<=j();i++){a+=i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(10), value->getInt());
	}

	TEST_METHOD(LoopFor3State) {
		interpreter.evaluate("a = 0; for(i=0;i<=4;i++){a+=i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(10), value->getInt());
	}

	TEST_METHOD(LoopFor2State) {
		interpreter.evaluate("a = 0; i = 0; for(i<=4;i++){a+=i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(10), value->getInt());
	}

	TEST_METHOD(LoopFor1State) {
		interpreter.evaluate("a = 0; i = 0; for(i<=4){a+=i; ++i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(10), value->getInt());
	}

	TEST_METHOD(LoopWhile) {
		interpreter.evaluate("a = 0; i = 0; while(i<=4){a+=i; ++i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(10), value->getInt());
	}

	TEST_METHOD(LoopForEach) {
		interpreter.evaluate("a = 0; b = [1,2,3,4]; foreach(i; b){a+=i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(10), value->getInt());
	}

	TEST_METHOD(IfElseSimpleIfTrue) {
		interpreter.evaluate("a = 0; b = true; if(b){a+=10;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(10), value->getInt());
	}

	TEST_METHOD(IfElseSimpleIfFalse) {
		interpreter.evaluate("a = 10; b = false; if(b){a-=10;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(10), value->getInt());
	}

	TEST_METHOD(IfElseSimpleIfTrueElse) {
		interpreter.evaluate("a = 0; b = true; if(b){a+=10;}else{a=100;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(10), value->getInt());
	}

	TEST_METHOD(IfElseSimpleIfFalseElse) {
		interpreter.evaluate("a = 0; b = false; if(b){a-=10;}else{a=10;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(10), value->getInt());
	}

	TEST_METHOD(IfElseComplexIfTrueElse) {
		interpreter.evaluate("a = 0; b = true; if(b){a+=10;}else if(b){a=23;}else{a=100;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(10), value->getInt());
	}

	TEST_METHOD(IfElseComplexIfFalseElse) {
		interpreter.evaluate("a = 0; b = false; if(b){a-=10;}else if(b){a=23;}else{a=10;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::Type::Int, value->type);
		Assert::AreEqual(KataScript::Int(10), value->getInt());
	}

    TEST_METHOD(IfInsideNestedForLoops) {
        interpreter.evaluate("v = [1,2,3,4]; a=0; for(i=0;i<4;i++){for(j=i+1;j<4;j++){ if (i * j < 2){a += v[i] + v[j];}}}"s);

        auto value = interpreter.resolveVariable("a"s);

        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(12), value->getInt());
    }

    TEST_METHOD(IfElseInsideNestedForLoops) {
        interpreter.evaluate("v = [1,2,3,4]; a=0; for(i=0;i<4;i++){for(j=i+1;j<4;j++){ if (i * j < 2){a += v[i] + v[j];}else{a+=1;}}}"s);

        auto value = interpreter.resolveVariable("a"s);

        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(15), value->getInt());
    }

    TEST_METHOD(IndexFunctionResult) {
        interpreter.evaluate("a = split(split(\"1-3 a: absdf\", \" \")[0], \"-\")");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::Array, value->type);
        Assert::AreEqual(2ull, value->getArray().size());
        Assert::AreEqual("1"s, value->getStdVector<std::string>()[0]);
        Assert::AreEqual("3"s, value->getStdVector<std::string>()[1]);
    }

    TEST_METHOD(IndexResultOfFunctionResult) {
        interpreter.evaluate("tokens = split(\"1-3 a: absdf\", \" \"); r = split(tokens[1], \":\")[0];");

        auto value = interpreter.resolveVariable("r"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("a"s, value->getString());
    }

    TEST_METHOD(BooleanOrder) {
        interpreter.evaluate("a = 0 >= 1 && 0 <= 3;");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(0), value->getInt());
    }

    TEST_METHOD(FunctionIfScoping) {
        interpreter.evaluate("func j(s) { if (s) { print(s); } a = 2; return a; }");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::Null, value->type);
        Assert::AreEqual(KataScript::Int(0), value->getInt());
    }

    TEST_METHOD(FunctionNestedControlFlowReturn) {
        interpreter.evaluate("func f() { if (1) { for(1){return 5;} } } i = f();");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(5), value->getInt());
    }

    TEST_METHOD(EmptyListLiteralinsideFunctionParse) {
        interpreter.evaluate("a = typeof([]);");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("list"s, value->getString());
    }

    TEST_METHOD(MapFunction) {
        interpreter.evaluate("i = [0,1,2,3]; func add1(a) {return a + 1; } i = map(i, add1);"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::List, value->type);
        Assert::AreEqual(4ull, value->getList().size());
        Assert::AreEqual(KataScript::Int(1), value->getList()[0]->getInt());
        Assert::AreEqual(KataScript::Int(2), value->getList()[1]->getInt());
        Assert::AreEqual(KataScript::Int(3), value->getList()[2]->getInt());
        Assert::AreEqual(KataScript::Int(4), value->getList()[3]->getInt());
    }

    TEST_METHOD(FoldString) {
        interpreter.evaluate("a = fold([1,2,3,4],+,\"\");");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("1234"s, value->getString());
    }

    TEST_METHOD(DotSyntaxMapFunction) {
        interpreter.evaluate("i = [0,1,2,3]; func add1(a) {return a + 1; } i = i.map(add1);"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::List, value->type);
        Assert::AreEqual(4ull, value->getList().size());
        Assert::AreEqual(KataScript::Int(1), value->getList()[0]->getInt());
        Assert::AreEqual(KataScript::Int(2), value->getList()[1]->getInt());
        Assert::AreEqual(KataScript::Int(3), value->getList()[2]->getInt());
        Assert::AreEqual(KataScript::Int(4), value->getList()[3]->getInt());
    }

    TEST_METHOD(DotSyntaxFoldString) {
        interpreter.evaluate("a = [1,2,3,4].fold(+,\"\");");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("1234"s, value->getString());
    }

    TEST_METHOD(FoldInt) {
        interpreter.evaluate("a = fold([1,2,3,4],+,0);");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(10), value->getInt());
    }

    TEST_METHOD(ForEachOverALiteral) {
        interpreter.evaluate("i = []; foreach(a; [1,2,3,4]) { i += a; }"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::List, value->type);
        Assert::AreEqual(4ull, value->getList().size());
        Assert::AreEqual(KataScript::Int(1), value->getList()[0]->getInt());
        Assert::AreEqual(KataScript::Int(2), value->getList()[1]->getInt());
        Assert::AreEqual(KataScript::Int(3), value->getList()[2]->getInt());
        Assert::AreEqual(KataScript::Int(4), value->getList()[3]->getInt());
    }

    TEST_METHOD(RecursiveFunc) {
        interpreter.evaluate("i = []; func r(n) { if (n > 0) { i += n; r(--n); } } r(4);"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::List, value->type);
        Assert::AreEqual(4ull, value->getList().size());
        Assert::AreEqual(KataScript::Int(4), value->getList()[0]->getInt());
        Assert::AreEqual(KataScript::Int(3), value->getList()[1]->getInt());
        Assert::AreEqual(KataScript::Int(2), value->getList()[2]->getInt());
        Assert::AreEqual(KataScript::Int(1), value->getList()[3]->getInt());
    }

    TEST_METHOD(RecursiveList) {
        interpreter.evaluate("func r(n) { var l = []; if (n > 0) { l += n; if (n > 1){l += r(--n); }}  return l; } i = r(4);"s);
        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::List, value->type);
        Assert::AreEqual(4ull, value->getList().size());
        Assert::AreEqual(KataScript::Int(4), value->getList()[0]->getInt());
        Assert::AreEqual(KataScript::Int(3), value->getList()[1]->getInt());
        Assert::AreEqual(KataScript::Int(2), value->getList()[2]->getInt());
        Assert::AreEqual(KataScript::Int(1), value->getList()[3]->getInt());
    }

    TEST_METHOD(MinInts) {
        interpreter.evaluate("i = min(5,12);");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(5), value->getInt());
    }

    TEST_METHOD(MaxInts) {
        interpreter.evaluate("i = max(5,-12);");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(5), value->getInt());
    }

    TEST_METHOD(Swap) {
        interpreter.evaluate("i = 1; j = 5; swap(i,j);");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(5), value->getInt());

        auto value2 = interpreter.resolveVariable("j"s);
        Assert::AreEqual(KataScript::Type::Int, value2->type);
        Assert::AreEqual(KataScript::Int(1), value2->getInt());
    }

    TEST_METHOD(Pow) {
        interpreter.evaluate("i = pow(2,.5);");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Float, value->type);
        Assert::AreEqual(KataScript::Float(pow(2.0,.5)), value->getFloat());
    }

    TEST_METHOD(Abs) {
        interpreter.evaluate("i = abs(-2); j = abs(-5.0);");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(2), value->getInt());

        value = interpreter.resolveVariable("j"s);
        Assert::AreEqual(KataScript::Type::Float, value->type);
        Assert::AreEqual(KataScript::Float(5.0), value->getFloat());
    }

    TEST_METHOD(DictConvertToListKeepOldIndexes) {
        interpreter.evaluate("i = [0,1,2,3,4,5]; i[\"taco\"] = \"pizza\"; a = i[\"taco\"]; b = i[3];");

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::String, val->type);
        Assert::AreEqual("pizza"s, val->getString());

        auto value = interpreter.resolveVariable("b"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(3), value->getInt());
    }

    TEST_METHOD(ScopingViolationBugfix) {
        interpreter.evaluate("func f() { while(1) { print(1); } tacos = 5; i = 1; return i; }");

        auto val = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Null, val->type);
    }

    TEST_METHOD(DifferentTypesNeverEqual) {
        interpreter.evaluate("a = \"\" == []; b = 4.0 == 4; c = list() == array(); d = list() == null; e = null == []; f = 0 == null;");

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(false), val->getInt());

        val = interpreter.resolveVariable("b"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(false), val->getInt());

        val = interpreter.resolveVariable("c"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(false), val->getInt());

        val = interpreter.resolveVariable("d"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(false), val->getInt());

        val = interpreter.resolveVariable("e"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(false), val->getInt());

        val = interpreter.resolveVariable("f"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(false), val->getInt());
    }

    TEST_METHOD(ConvertedTypesDoEqual) {
        interpreter.evaluate("a = \"\" == string([]); b = 4.0 == float(4); c = list(1,2,3) == tolist(array(1,2,3)); d = null == null;");

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(true), val->getInt());

        val = interpreter.resolveVariable("b"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(true), val->getInt());

        val = interpreter.resolveVariable("c"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(true), val->getInt());

        val = interpreter.resolveVariable("d"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(true), val->getInt());
    }

    TEST_METHOD(MakeClass) {
        interpreter.evaluate("class xy { var x; var y; func xy(_x, _y) { x = _x; y = _y; } func add(_x, _y) { x += _x; y += _y; } func sqr() { return x * y; } } "s +
            "a = xy(4,5.0);"s);

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::Class, val->type);
        Assert::AreEqual(KataScript::Type::Int, val->getClass()->variables["x"]->type);
        Assert::AreEqual(KataScript::Int(4), val->getClass()->variables["x"]->getInt());
        Assert::AreEqual(KataScript::Type::Float, val->getClass()->variables["y"]->type);
        Assert::AreEqual(KataScript::Float(5.0), val->getClass()->variables["y"]->getFloat());
    }

    TEST_METHOD(AccessClass) {
        interpreter.evaluate("class xy { var x; var y; func xy(_x, _y) { x = _x; y = _y; } func add(_x, _y) { x += _x; y += _y; } func sqr() { return x * y; } } "s +
            "a = xy(4,5.0); c = a.sqr(); e = a.x; f = a.y;"s);

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::Class, val->type);
        Assert::AreEqual(KataScript::Type::Int, val->getClass()->variables["x"]->type);
        Assert::AreEqual(KataScript::Int(4), val->getClass()->variables["x"]->getInt());
        Assert::AreEqual(KataScript::Type::Float, val->getClass()->variables["y"]->type);
        Assert::AreEqual(KataScript::Float(5.0), val->getClass()->variables["y"]->getFloat());

        val = interpreter.resolveVariable("c"s);
        Assert::AreEqual(KataScript::Type::Float, val->type);
        Assert::AreEqual(KataScript::Float(20.0), val->getFloat());

        val = interpreter.resolveVariable("e"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(4), val->getInt());

        val = interpreter.resolveVariable("f"s);
        Assert::AreEqual(KataScript::Type::Float, val->type);
        Assert::AreEqual(KataScript::Float(5.0), val->getFloat());
    }

    TEST_METHOD(MutateClass) {
        interpreter.evaluate("class xy { var x; var y; func xy(_x, _y) { x = _x; y = _y; } func add(_x, _y) { x += _x; y += _y; } func sqr() { return x * y; } } "s +
            "a = xy(4,5.0); c = a.sqr(); a.add(4,5); d = a.sqr(); e = a.x; f = a.y;"s);

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::Class, val->type);
        Assert::AreEqual(KataScript::Type::Int, val->getClass()->variables["x"]->type);
        Assert::AreEqual(KataScript::Int(8), val->getClass()->variables["x"]->getInt());
        Assert::AreEqual(KataScript::Type::Float, val->getClass()->variables["y"]->type);
        Assert::AreEqual(KataScript::Float(10.0), val->getClass()->variables["y"]->getFloat());

        val = interpreter.resolveVariable("c"s);
        Assert::AreEqual(KataScript::Type::Float, val->type);
        Assert::AreEqual(KataScript::Float(20.0), val->getFloat());

        val = interpreter.resolveVariable("d"s);
        Assert::AreEqual(KataScript::Type::Float, val->type);
        Assert::AreEqual(KataScript::Float(80.0), val->getFloat());

        val = interpreter.resolveVariable("e"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(8), val->getInt());

        val = interpreter.resolveVariable("f"s);
        Assert::AreEqual(KataScript::Type::Float, val->type);
        Assert::AreEqual(KataScript::Float(10.0), val->getFloat());
    }

    TEST_METHOD(CopyMutateClassLeaveOriginalAlone) {
        interpreter.evaluate("class xy { var x; var y; func xy(_x, _y) { x = _x; y = _y; } func add(_x, _y) { x += _x; y += _y; } func sqr() { return x * y; } } "s +
            "a = xy(4,5.0); b = copy(a); b.add(\"a\",\"b\"); c = a.sqr(); a.add(4,5); d = a.sqr(); e = a.x; f = a.y;"s);

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::Class, val->type);
        Assert::AreEqual(KataScript::Type::Int, val->getClass()->variables["x"]->type);
        Assert::AreEqual(KataScript::Int(8), val->getClass()->variables["x"]->getInt());
        Assert::AreEqual(KataScript::Type::Float, val->getClass()->variables["y"]->type);
        Assert::AreEqual(KataScript::Float(10.0), val->getClass()->variables["y"]->getFloat());

        val = interpreter.resolveVariable("b"s);
        Assert::AreEqual(KataScript::Type::Class, val->type);
        Assert::AreEqual(KataScript::Type::String, val->getClass()->variables["x"]->type);
        Assert::AreEqual("4a"s, val->getClass()->variables["x"]->getString());
        Assert::AreEqual(KataScript::Type::String, val->getClass()->variables["y"]->type);
        Assert::AreEqual("5.000000b"s, val->getClass()->variables["y"]->getString());

        val = interpreter.resolveVariable("c"s);
        Assert::AreEqual(KataScript::Type::Float, val->type);
        Assert::AreEqual(KataScript::Float(20.0), val->getFloat());

        val = interpreter.resolveVariable("d"s);
        Assert::AreEqual(KataScript::Type::Float, val->type);
        Assert::AreEqual(KataScript::Float(80.0), val->getFloat());

        val = interpreter.resolveVariable("e"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(8), val->getInt());

        val = interpreter.resolveVariable("f"s);
        Assert::AreEqual(KataScript::Type::Float, val->type);
        Assert::AreEqual(KataScript::Float(10.0), val->getFloat());
    }

    TEST_METHOD(SimpleFunctionApplyIdentity) {
        interpreter.evaluate("func test(t) {return t(\"hey\");} a = test(identity);"s);

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::String, val->type);
        Assert::AreEqual("hey"s, val->getString());
    }

    TEST_METHOD(UserPointers) {
        interpreter.evaluate("ptrs = []; func storeptr(ptr) { ptrs+=ptr; } func getlast() { return ptrs.back(); }");
        auto ptr = &interpreter;
        interpreter.callFunctionWithArgs(interpreter.resolveFunction("storeptr"), ptr);
        interpreter.evaluate("a = getlast();");

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::UserPointer, val->type);
        auto ptrfetch = val->getPointer();
        Assert::AreEqual((size_t)&interpreter, (size_t)ptrfetch);
    }

    TEST_METHOD(ClassAddFunctionAfterwards) {
        interpreter.evaluate("class wein { var x; func wein() { x = 2; } } w = wein(); a = w.x; class wein { func add(n) { x += n; } } w.add(5); b = w.x;");
        
        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(2), val->getInt());

        val = interpreter.resolveVariable("b"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(7), val->getInt());
    }

    TEST_METHOD(NestedClassFunctions) {
        interpreter.evaluate("class point { "s+
           " var _x = 0.0; var _y = 0.0;"+
        "func point(x, y) { _x = x;	_y = y; }"+
        "func show() {"+
           " return \"point:[\" + _x +\",\"+ _y + \"]\"; }"+
        "}"+
        "class Line {"+
         "   var points = point(10.5, 15.7);"+
          "  func Line(name, pos) { points = pos; }"+
          "  func display() { return points.show(); }"+
        "}"+
        "line = Line(\"test\",point(15.0,17.0));"+
         "   var out = line.display(); ");

        auto val = interpreter.resolveVariable("out"s);
        Assert::AreEqual(KataScript::Type::String, val->type);
        Assert::AreEqual("point:[15.000000,17.000000]"s, val->getString());
    }

    TEST_METHOD(NestedTwiceClassFunctions) {
        interpreter.evaluate(" class scalar {"s +
       " var v = 0.0;"+
       " func scalar(x) { v = x; }"+
       " func get() { return v; }"+
   " }"+
    "class point {"+
       " var _x = scalar(0.0); var _y = scalar(0.0);"+
       " func point(x, y) { _x = scalar(x);	_y = scalar(y); }"+
       " func show() { return \"point:[\" + _x.get() + \",\" + _y.get() + \"]\"; }"+
    "}"+
   " class Line {"+
        "var points = point(10.5, 15.7);"+
        "func Line(name, pos) { points = pos; }"+
        "func display() { return points.show(); }"+
    "}"+
    "line = Line(\"test\", point(15.0, 17.0));"+
    "var out = line.display();");

        auto val = interpreter.resolveVariable("out"s);
        Assert::AreEqual(KataScript::Type::String, val->type);
        Assert::AreEqual("point:[15.000000,17.000000]"s, val->getString());
    }

    TEST_METHOD(NestedClassLists) {
        interpreter.evaluate("class test {"s +
            "var val = list();" +
            "func test() {" +
            "  val = list(1, 2, 3, 4);" +
            " }  }" +
            "var aa = test();" +
            "var out = aa.val[1];");

        auto val = interpreter.resolveVariable("out"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(2), val->getInt());
    }

    TEST_METHOD(NestedTwiceClassLists) {
        interpreter.evaluate("class test {"s +
            "var val = list();" +
            "func test() {" +
            "  val = list(list(1, 4, 3),list(2, 3, 4));" +
            " }  }" +
            "var aa = test();" +
            "var out = aa.val[1][0];");

        auto val = interpreter.resolveVariable("out"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(2), val->getInt());
    }

    TEST_METHOD(ClassInherits) {
        interpreter.evaluate("class xx { var x; func xx(_x) { x = _x; } func add(_x, _y) { x += _x; y += _y; } }"s+
            "class yy { var y; func yy(_y) { y = _y; } func sqr() { return x * y; } } "s +
            "class xy -> xx, yy { func xy(_x, _y) { x = _x; y = _y; } }"s +
            "a = xy(4,5.0); b = copy(a); b.add(\"a\",\"b\"); c = a.sqr(); a.add(4,5); d = a.sqr(); e = a.x; f = a.y;"s);

        auto val = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::Type::Class, val->type);
        Assert::AreEqual(KataScript::Type::Int, val->getClass()->variables["x"]->type);
        Assert::AreEqual(KataScript::Int(8), val->getClass()->variables["x"]->getInt());
        Assert::AreEqual(KataScript::Type::Float, val->getClass()->variables["y"]->type);
        Assert::AreEqual(KataScript::Float(10.0), val->getClass()->variables["y"]->getFloat());

        val = interpreter.resolveVariable("b"s);
        Assert::AreEqual(KataScript::Type::Class, val->type);
        Assert::AreEqual(KataScript::Type::String, val->getClass()->variables["x"]->type);
        Assert::AreEqual("4a"s, val->getClass()->variables["x"]->getString());
        Assert::AreEqual(KataScript::Type::String, val->getClass()->variables["y"]->type);
        Assert::AreEqual("5.000000b"s, val->getClass()->variables["y"]->getString());

        val = interpreter.resolveVariable("c"s);
        Assert::AreEqual(KataScript::Type::Float, val->type);
        Assert::AreEqual(KataScript::Float(20.0), val->getFloat());

        val = interpreter.resolveVariable("d"s);
        Assert::AreEqual(KataScript::Type::Float, val->type);
        Assert::AreEqual(KataScript::Float(80.0), val->getFloat());

        val = interpreter.resolveVariable("e"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(8), val->getInt());

        val = interpreter.resolveVariable("f"s);
        Assert::AreEqual(KataScript::Type::Float, val->type);
        Assert::AreEqual(KataScript::Float(10.0), val->getFloat());
    }

    TEST_METHOD(AssignListReverse) {
        interpreter.evaluate("i = [4.0,3,2,1].reverse();"s);

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::List, value->type);
        Assert::AreEqual(4ull, value->getList().size());
        Assert::AreEqual(KataScript::Type::Int, value->getList()[0]->type);
        Assert::AreEqual(KataScript::Int(1), value->getList()[0]->getInt());
        Assert::AreEqual(KataScript::Type::Int, value->getList()[1]->type);
        Assert::AreEqual(KataScript::Int(2), value->getList()[1]->getInt());
        Assert::AreEqual(KataScript::Type::Int, value->getList()[2]->type);
        Assert::AreEqual(KataScript::Int(3), value->getList()[2]->getInt());
        Assert::AreEqual(KataScript::Type::Float, value->getList()[3]->type);
        Assert::AreEqual(KataScript::Float(4.0), value->getList()[3]->getFloat());
    }

    TEST_METHOD(AssignArrayReverse) {
        interpreter.evaluate("i = [4,3,2,1].reverse();"s);

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Array, value->type);
        Assert::AreEqual(4ull, value->getArray().size());
        Assert::AreEqual(KataScript::Int(1), value->getStdVector<KataScript::Int>()[0]);
        Assert::AreEqual(KataScript::Int(2), value->getStdVector<KataScript::Int>()[1]);
        Assert::AreEqual(KataScript::Int(3), value->getStdVector<KataScript::Int>()[2]);
        Assert::AreEqual(KataScript::Int(4), value->getStdVector<KataScript::Int>()[3]);
    }

    TEST_METHOD(AssignStringReverse) {
        interpreter.evaluate("i = \"socat hsif\".reverse();"s);

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("fish tacos"s, value->getString());
    }

    TEST_METHOD(AssignStringReplace) {
        interpreter.evaluate("i = \"ice tacos\".replace(\"ice\", \"fish\");"s);

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("fish tacos"s, value->getString());
    }

    TEST_METHOD(StartsWith) {
        interpreter.evaluate("i = \"fish tacos\".startswith(\"fish\"); j = \"fish tacos\".startswith(\"tacos\");"s);

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(true), value->getInt());

        value = interpreter.resolveVariable("j"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(false), value->getInt());
    }

    TEST_METHOD(EndsWith) {
        interpreter.evaluate("i = \"fish tacos\".endswith(\"fish\"); j = \"fish tacos\".endswith(\"tacos\");"s);

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(false), value->getInt());

        value = interpreter.resolveVariable("j"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(true), value->getInt());
    }

    TEST_METHOD(ImportFile) {
        interpreter.evaluate("import \"../../../samples/demo.ks\" i = typeof(a); j = a.x; k = a.y;");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("class"s, value->getString());

        value = interpreter.resolveVariable("j"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(2), value->getInt());

        value = interpreter.resolveVariable("k"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("KataScript interpreter sucessfully installed!"s, value->getString());
    }

    TEST_METHOD(ClassFromCPP) {
        auto interp = &interpreter;
        interpreter.newClass("beansClass", { {"color", std::make_shared<KataScript::Value>("white")} }, 
            [interp](KataScript::ClassRef classs, KataScript::ScopeRef scope, const KataScript::List& vars) {
            if (vars.size() > 0) {
                interp->resolveVariable("color", classs, scope) = vars[0];
            }
            return std::make_shared<KataScript::Value>(); 
            }, {
            {"changeColor", [interp](KataScript::ClassRef classs, KataScript::ScopeRef scope, const KataScript::List& vars) {
            if (vars.size() > 0) {
                interp->resolveVariable("color", scope) = vars[0];
            }
            return std::make_shared<KataScript::Value>();
            } },
        {"isRipe", [interp](KataScript::ClassRef classs, KataScript::ScopeRef scope, const KataScript::List&) {
            auto color = interp->resolveVariable("color", scope);
            if (color->type == KataScript::Type::String) { return std::make_shared<KataScript::Value>(color->getString() == "brown"); }
            return std::make_shared<KataScript::Value>(false);
            } },
            });

        interpreter.evaluate("i = beansClass(\"orange\");");
        interpreter.evaluate("j = i.color;");
        interpreter.evaluate("k = beansClass();");
        interpreter.evaluate("l = beansClass(); l.changeColor(\"brown\")");
        interpreter.evaluate("k2 = beansClass();");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Class, value->type);
        Assert::AreEqual(KataScript::Type::String, value->getClass()->variables["color"]->type);
        Assert::AreEqual("orange"s, value->getClass()->variables["color"]->getString());

        value = interpreter.resolveVariable("j"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("orange"s, value->getString());

        value = interpreter.resolveVariable("k"s);
        Assert::AreEqual(KataScript::Type::Class, value->type);
        Assert::AreEqual(KataScript::Type::String, value->getClass()->variables["color"]->type);
        Assert::AreEqual("white"s, value->getClass()->variables["color"]->getString());

        value = interpreter.resolveVariable("l"s);
        Assert::AreEqual(KataScript::Type::Class, value->type);
        Assert::AreEqual(KataScript::Type::String, value->getClass()->variables["color"]->type);
        Assert::AreEqual("brown"s, value->getClass()->variables["color"]->getString());

        value = interpreter.resolveVariable("k2"s);
        Assert::AreEqual(KataScript::Type::Class, value->type);
        Assert::AreEqual(KataScript::Type::String, value->getClass()->variables["color"]->type);
        Assert::AreEqual("white"s, value->getClass()->variables["color"]->getString());
    }

    TEST_METHOD(ClassFromClassFromCPP) {
        auto interp = &interpreter;
        auto global = interp->resolveScope("global");
        auto beansConstructor = interpreter.newClass("beansClass", { 
            {"color", std::make_shared<KataScript::Value>("white")} }, 
            [interp](KataScript::ClassRef classs, KataScript::ScopeRef scope, const KataScript::List& vars) {
            if (vars.size() > 0) {
                interp->resolveVariable("color", classs, scope) = vars[0];
            }
            return std::make_shared<KataScript::Value>();
            }, {
            {"changeColor", [interp](KataScript::ClassRef classs, KataScript::ScopeRef scope, const KataScript::List& vars) {
            if (vars.size() > 0) {
                interp->resolveVariable("color", classs, scope) = vars[0];
            }
            return std::make_shared<KataScript::Value>();
            } },
        {"isRipe", [interp](KataScript::ClassRef classs, KataScript::ScopeRef scope, const KataScript::List&) {
            auto color = interp->resolveVariable("color", classs, scope);
            if (color->type == KataScript::Type::String) { return std::make_shared<KataScript::Value>(color->getString() == "brown"); }
            return std::make_shared<KataScript::Value>(false);
            } },
            });

        interpreter.newClass("beanMaker", {  }, 
            [interp](KataScript::ClassRef classs, KataScript::ScopeRef scope, const KataScript::List& vars) {
                return std::make_shared<KataScript::Value>();
            }, {
            {"makeBean", [interp, beansConstructor, global](KataScript::ClassRef classs, KataScript::ScopeRef scope, const KataScript::List& vars) {
                // by default this would run in our scope, so run in global where beansConstructor actually is
                return interp->callFunction(beansConstructor, global, vars);
            } },
            });

        interpreter.evaluate("n = beanMaker(); i = n.makeBean(\"orange\");");
        interpreter.evaluate("j = i.color;");
        interpreter.evaluate("k = n.makeBean();");
        interpreter.evaluate("l = beansClass(); l.changeColor(\"brown\")");
        interpreter.evaluate("k2 = beanMaker().makeBean();");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Class, value->type);
        Assert::AreEqual(KataScript::Type::String, value->getClass()->variables["color"]->type);
        Assert::AreEqual("orange"s, value->getClass()->variables["color"]->getString());

        value = interpreter.resolveVariable("j"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("orange"s, value->getString());

        value = interpreter.resolveVariable("k"s);
        Assert::AreEqual(KataScript::Type::Class, value->type);
        Assert::AreEqual(KataScript::Type::String, value->getClass()->variables["color"]->type);
        Assert::AreEqual("white"s, value->getClass()->variables["color"]->getString());

        value = interpreter.resolveVariable("l"s);
        Assert::AreEqual(KataScript::Type::Class, value->type);
        Assert::AreEqual(KataScript::Type::String, value->getClass()->variables["color"]->type);
        Assert::AreEqual("brown"s, value->getClass()->variables["color"]->getString());

        value = interpreter.resolveVariable("k2"s);
        Assert::AreEqual(KataScript::Type::Class, value->type);
        Assert::AreEqual(KataScript::Type::String, value->getClass()->variables["color"]->type);
        Assert::AreEqual("white"s, value->getClass()->variables["color"]->getString());
    }

    TEST_METHOD(ClassFromClassFromCPPInScope) {
        auto interp = &interpreter;
        auto pizzascope = interp->resolveScope("pizza");
        
        auto beansConstructor = interpreter.newClass("beansClass", pizzascope, {
            {"color", std::make_shared<KataScript::Value>("white")} },
            [interp](KataScript::ClassRef classs, KataScript::ScopeRef scope, const KataScript::List& vars) {
            if (vars.size() > 0) {
                interp->resolveVariable("color", classs, scope) = vars[0];
            }
            return std::make_shared<KataScript::Value>();
            },{
            {"changeColor", [interp](KataScript::ClassRef classs, KataScript::ScopeRef scope, const KataScript::List& vars) {
            if (vars.size() > 0) {
                interp->resolveVariable("color", classs, scope) = vars[0];
            }
            return std::make_shared<KataScript::Value>();
            } },
        {"isRipe", [interp](KataScript::ClassRef classs, KataScript::ScopeRef scope, const KataScript::List&) {
            auto color = interp->resolveVariable("color", classs, scope);
            if (color->type == KataScript::Type::String) { return std::make_shared<KataScript::Value>(color->getString() == "brown"); }
            return std::make_shared<KataScript::Value>(false);
            } },
            });

        interpreter.newClass("beanMaker", pizzascope, {  }, 
            [interp](KataScript::ClassRef classs, KataScript::ScopeRef scope, const KataScript::List& vars) {
                return std::make_shared<KataScript::Value>();
            },{
            {"makeBean", [interp, beansConstructor, pizzascope](KataScript::ClassRef classs, KataScript::ScopeRef scope, const KataScript::List& vars) {
                // by default this would run in our scope, so run in the scope where beansConstructor actually is
                return interp->callFunction(beansConstructor, pizzascope, vars);
            } },
            });

        interpreter.evaluate("n = beanMaker(); i = n.makeBean(\"orange\");", pizzascope);
        interpreter.evaluate("j = i.color;", pizzascope);
        interpreter.evaluate("k = n.makeBean();", pizzascope);
        interpreter.evaluate("l = beansClass(); l.changeColor(\"brown\")", pizzascope);
        interpreter.evaluate("k2 = beanMaker().makeBean();", pizzascope);

        auto value = interpreter.resolveVariable("i"s, pizzascope);
        Assert::AreEqual(KataScript::Type::Class, value->type);
        Assert::AreEqual(KataScript::Type::String, value->getClass()->variables["color"]->type);
        Assert::AreEqual("orange"s, value->getClass()->variables["color"]->getString());

        value = interpreter.resolveVariable("j"s, pizzascope);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("orange"s, value->getString());

        value = interpreter.resolveVariable("k"s, pizzascope);
        Assert::AreEqual(KataScript::Type::Class, value->type);
        Assert::AreEqual(KataScript::Type::String, value->getClass()->variables["color"]->type);
        Assert::AreEqual("white"s, value->getClass()->variables["color"]->getString());

        value = interpreter.resolveVariable("l"s, pizzascope);
        Assert::AreEqual(KataScript::Type::Class, value->type);
        Assert::AreEqual(KataScript::Type::String, value->getClass()->variables["color"]->type);
        Assert::AreEqual("brown"s, value->getClass()->variables["color"]->getString());

        value = interpreter.resolveVariable("k2"s, pizzascope);
        Assert::AreEqual(KataScript::Type::Class, value->type);
        Assert::AreEqual(KataScript::Type::String, value->getClass()->variables["color"]->type);
        Assert::AreEqual("white"s, value->getClass()->variables["color"]->getString());
    }

    TEST_METHOD(EvalFromScope) {
        auto scope = interpreter.resolveScope("tebby jon jonz");
        interpreter.evaluate("mightyness = 60;");
        interpreter.evaluate("stuffyness = 0;");
        interpreter.evaluate("var stuffyness = 120;", scope);

        auto value = interpreter.resolveVariable("mightyness"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(60), value->getInt());

        value = interpreter.resolveVariable("stuffyness"s);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(0), value->getInt());

        value = interpreter.resolveVariable("mightyness"s, scope);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(60), value->getInt());

        value = interpreter.resolveVariable("stuffyness"s, scope);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(120), value->getInt());
    }

    TEST_METHOD(EvalFromScope2) {
        auto scope = interpreter.resolveScope("tebby jon jonz");
        interpreter.evaluate("var timesEncountered = 6;"s+
        "    func incTimes() {"+
        "    timesEncountered += 1;"+
        "}"+
        "func tooMany() {"+
         "   return timesEncountered > 5;"+
        "}", scope);

        interpreter.evaluate("incTimes();", scope);
        interpreter.evaluate("if (tooMany()) { incTimes(); }", scope);

        auto value = interpreter.resolveVariable("timesEncountered"s, scope);
        Assert::AreEqual(KataScript::Type::Int, value->type);
        Assert::AreEqual(KataScript::Int(8), value->getInt());
    }

    TEST_METHOD(SaveLoad) {
        interpreter.evaluate("import file; var path = \"test.txt\";"s +
            "var content = \"test content\";" +
            "saveFile(content, path);" +
            "var i = readFile(path);");

        auto value = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::String, value->type);
        Assert::AreEqual("test content"s, value->getString());
    }

    TEST_METHOD(SaveLoadFailNoImport) {
        bool didExcept = interpreter.evaluate("var path = \"test.txt\";"s +
                "var content = \"test content\";" +
                "saveFile(content, path);" +
                "var i = readFile(path);");

        Assert::AreEqual(didExcept, true);
    }

    TEST_METHOD(ExceptOnNonExistantFunction) {
        bool didExcept = interpreter.evaluate("var path = notReal();"s);

        Assert::AreEqual(didExcept, true);
    }

    TEST_METHOD(InsertScope) {
        auto scope = std::make_shared<KataScript::Scope>(KataScript::Scope("test", { {"yip", std::make_shared<KataScript::Value>(KataScript::Int(42))} }));
        auto inscope = interpreter.insertScope(scope, interpreter.resolveScope("global"));

        auto val = interpreter.resolveVariable("yip"s, inscope);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(42), val->getInt());

        Assert::AreEqual((size_t)scope.get(), (size_t)inscope.get());
    }

    TEST_METHOD(InsertScopeCallFromParent) {
        interpreter.evaluate("func getFalseAnswer(){return 69;}");
        auto scope = std::make_shared<KataScript::Scope>(KataScript::Scope("test", { {"yip", std::make_shared<KataScript::Value>(KataScript::Int(42))} }));
        auto inscope = interpreter.insertScope(scope, interpreter.resolveScope("global"));
        interpreter.evaluate("yip = getFalseAnswer();", scope);
        auto val = interpreter.resolveVariable("yip"s, inscope);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(69), val->getInt());

        Assert::AreEqual((size_t)scope.get(), (size_t)inscope.get());
    }

    TEST_METHOD(Factorial) {
        interpreter.evaluate("yip = 4!; n = 6!"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(24), val->getInt());

        val = interpreter.resolveVariable("n"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(720), val->getInt());
    }

    TEST_METHOD(NegateNull) {
        interpreter.evaluate("yip = !null;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(1), val->getInt());
    }

    TEST_METHOD(PrefixInc) {
        interpreter.evaluate("yip = ++5;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(6), val->getInt());
    }

    TEST_METHOD(PrefixIncMulti) {
        interpreter.evaluate("yip = (++++5);"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(7), val->getInt());
    }

    TEST_METHOD(PostfixInc) {
        interpreter.evaluate("yip = 5++;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(5), val->getInt());
    }

    TEST_METHOD(PrefixIncNull) {
        interpreter.evaluate("yip = ++null;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(1), val->getInt());
    }

    TEST_METHOD(PostfixIncNull) {
        interpreter.evaluate("yip = null++;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Null, val->type);
    }

    TEST_METHOD(PrefixIncNonExisting) {
        interpreter.evaluate("yip = ++n;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(1), val->getInt());

        val = interpreter.resolveVariable("n"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(1), val->getInt());
    }

    TEST_METHOD(PostfixIncNonExisting) {
        interpreter.evaluate("yip = n++;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Null, val->type);

        val = interpreter.resolveVariable("n"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(1), val->getInt());
    }

    TEST_METHOD(PrefixIncPEMDAS) {
        interpreter.evaluate("yip = 1 + ++1 + 1;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        auto n = 1;
        Assert::AreEqual(KataScript::Int(1 + ++n + 1), val->getInt());
    }

    TEST_METHOD(PrefixIncPEMDAS2) {
        interpreter.evaluate("yip = 1 + ++++++1 + 1;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        auto n = 1;
        Assert::AreEqual(KataScript::Int(1 + ++++++n + 1), val->getInt());
    }

    TEST_METHOD(PostfixIncPEMDAS) {
        interpreter.evaluate("yip = 1 + 1++ + 1;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        auto n = 1;
        Assert::AreEqual(KataScript::Int(1 + n++ + 1), val->getInt());
    }

    TEST_METHOD(PostfixIncPEMDAS2) {
        interpreter.evaluate("n = 1; yip = 1 + n++ + 1;"s);

        auto val = interpreter.resolveVariable("n"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        auto n = 1;
        n++;
        Assert::AreEqual(KataScript::Int(n), val->getInt());
    }

    TEST_METHOD(PrefixDec) {
        interpreter.evaluate("yip = --5;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(4), val->getInt());
    }

    TEST_METHOD(PostfixDec) {
        interpreter.evaluate("yip = 5--;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(5), val->getInt());
    }

    TEST_METHOD(PrefixDecNull) {
        interpreter.evaluate("yip = --null;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(-1), val->getInt());
    }

    TEST_METHOD(PostfixDecNull) {
        interpreter.evaluate("yip = null--;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Null, val->type);
    }

    TEST_METHOD(PrefixDecNonExisting) {
        interpreter.evaluate("yip = --n;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(-1), val->getInt());

        val = interpreter.resolveVariable("n"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(-1), val->getInt());
    }

    TEST_METHOD(PostfixDecNonExisting) {
        interpreter.evaluate("yip = n--;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Null, val->type);

        val = interpreter.resolveVariable("n"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(-1), val->getInt());
    }

    TEST_METHOD(PrefixDecPEMDAS) {
        interpreter.evaluate("yip = 1 + --1 + 1;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        auto n = 1;
        Assert::AreEqual(KataScript::Int(1 + --n + 1), val->getInt());
    }

    TEST_METHOD(PrefixDecPEMDAS2) {
        interpreter.evaluate("yip = 1 + ------1 + 1;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        auto n = 1;
        Assert::AreEqual(KataScript::Int(0), val->getInt());
    }

    TEST_METHOD(PostfixDecPEMDAS) {
        interpreter.evaluate("yip = 1 + 1-- + 1;"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        auto n = 1;
        Assert::AreEqual(KataScript::Int(1 + n-- + 1), val->getInt());
    }

    TEST_METHOD(PostfixDecPEMDAS2) {
        interpreter.evaluate("n = 1; yip = 1 + n-- + 1;"s);

        auto val = interpreter.resolveVariable("n"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        auto n = 1;
        n--;
        Assert::AreEqual(KataScript::Int(n), val->getInt());
    }

    TEST_METHOD(PrefixPostfix) {
        interpreter.evaluate("yip = 1; yip++; ++yip; print(++yip); print(yip++)"s);

        auto val = interpreter.resolveVariable("yip"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(5), val->getInt());
    }

    TEST_METHOD(ThreadsBasic) {
        interpreter.evaluate("import thread;i = 0;fn n() {print(++i);}threads = [];foreach(a; range(1,20)) { threads.pushback(newThread(n)); }foreach(t; threads) { joinThread(t);}"s);

        auto val = interpreter.resolveVariable("i"s);
        Assert::AreEqual(KataScript::Type::Int, val->type);
        Assert::AreEqual(KataScript::Int(20), val->getInt());
    }
    
	// todo add more tests

	};
}

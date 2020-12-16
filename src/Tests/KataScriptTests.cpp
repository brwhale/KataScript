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
		Assert::AreEqual(1, value->getList()[0]->getInt());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[1]->type);
		Assert::AreEqual(2, value->getList()[1]->getInt());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[2]->type);
		Assert::AreEqual(3, value->getList()[2]->getInt());
		Assert::AreEqual(KataScript::KSType::FLOAT, value->getList()[3]->type);
		Assert::AreEqual(4.f, value->getList()[3]->getFloat());
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
		interpreter.evaluate("i = array(1,2,3,4);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::ARRAY, value->type);
		Assert::AreEqual(4ull, value->getArray().size());
		Assert::AreEqual(1, value->getStdVector<int>()[0]);
		Assert::AreEqual(2, value->getStdVector<int>()[1]);
		Assert::AreEqual(3, value->getStdVector<int>()[2]);
		Assert::AreEqual(4, value->getStdVector<int>()[3]);
	}

	TEST_METHOD(AssignArrayOfVec3) {
		interpreter.evaluate("i = array(vec3(1,3,5), vec3(2,2,2), vec3(7,8,9));"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::ARRAY, value->type);
		Assert::AreEqual(3ull, value->getArray().size());
		Assert::AreEqual(KataScript::vec3(1, 3, 5), value->getStdVector<KataScript::vec3>()[0]);
		Assert::AreEqual(KataScript::vec3(2, 2, 2), value->getStdVector<KataScript::vec3>()[1]);
		Assert::AreEqual(KataScript::vec3(7, 8, 9), value->getStdVector<KataScript::vec3>()[2]);
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
		Assert::AreEqual(1, value->getStdVector<int>()[0]);
		Assert::AreEqual(2, value->getStdVector<int>()[1]);
		Assert::AreEqual(3, value->getStdVector<int>()[2]);
		Assert::AreEqual(4, value->getStdVector<int>()[3]);
	}

	TEST_METHOD(AssignChangeTypes) {
		interpreter.evaluate("i = list(1);"s);
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
		interpreter.evaluate("i = array(1,2) + array(3,4);"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::ARRAY, value->type);
		Assert::AreEqual(4ull, value->getArray().size());
		Assert::AreEqual(1, value->getStdVector<int>()[0]);
		Assert::AreEqual(2, value->getStdVector<int>()[1]);
		Assert::AreEqual(3, value->getStdVector<int>()[2]);
		Assert::AreEqual(4, value->getStdVector<int>()[3]);
	}

	TEST_METHOD(AddLists) {
		interpreter.evaluate("i = [1,2.0] + [3,4.0];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::LIST, value->type);
		Assert::AreEqual(4ull, value->getList().size());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[0]->type);
		Assert::AreEqual(1, value->getList()[0]->getInt());
		Assert::AreEqual(KataScript::KSType::FLOAT, value->getList()[1]->type);
		Assert::AreEqual(2.f, value->getList()[1]->getFloat());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[2]->type);
		Assert::AreEqual(3, value->getList()[2]->getInt());
		Assert::AreEqual(KataScript::KSType::FLOAT, value->getList()[3]->type);
		Assert::AreEqual(4.f, value->getList()[3]->getFloat());
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

	TEST_METHOD(AddEqualsInts) {
		interpreter.evaluate("i = 2; i += 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(5, value->getInt());
	}

	TEST_METHOD(AddEqualsFloats) {
		interpreter.evaluate("i = 2.4; i += 3.4;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(5.8f, value->getFloat());
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
		Assert::AreEqual(1, value->getStdVector<int>()[0]);
		Assert::AreEqual(2, value->getStdVector<int>()[1]);
		Assert::AreEqual(3, value->getStdVector<int>()[2]);
		Assert::AreEqual(4, value->getStdVector<int>()[3]);
	}

	TEST_METHOD(AddEqualsLists) {
		interpreter.evaluate("i = [1,2.0]; i += [3,4.0];"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::LIST, value->type);
		Assert::AreEqual(4ull, value->getList().size());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[0]->type);
		Assert::AreEqual(1, value->getList()[0]->getInt());
		Assert::AreEqual(KataScript::KSType::FLOAT, value->getList()[1]->type);
		Assert::AreEqual(2.f, value->getList()[1]->getFloat());
		Assert::AreEqual(KataScript::KSType::INT, value->getList()[2]->type);
		Assert::AreEqual(3, value->getList()[2]->getInt());
		Assert::AreEqual(KataScript::KSType::FLOAT, value->getList()[3]->type);
		Assert::AreEqual(4.f, value->getList()[3]->getFloat());
	}

	TEST_METHOD(SubEqualsInts) {
		interpreter.evaluate("i = 2; i -= 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(-1, value->getInt());
	}

	TEST_METHOD(SubEqualsFloats) {
		interpreter.evaluate("i = 2.4; i -= 3.4;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(-1.0f, value->getFloat());
	}

	TEST_METHOD(MulEqualsInts) {
		interpreter.evaluate("i = 2; i *= 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(6, value->getInt());
	}

	TEST_METHOD(MulEqualsFloats) {
		interpreter.evaluate("i = 2.5; i *= 3.0;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::FLOAT, value->type);
		Assert::AreEqual(7.5f, value->getFloat());
	}

	TEST_METHOD(DivEqualsInts) {
		interpreter.evaluate("i = 18; i /= 3;"s);
		auto value = interpreter.resolveVariable("i"s);
		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(6, value->getInt());
	}

	TEST_METHOD(DivEqualsFloats) {
		interpreter.evaluate("i = 1.5; i /= 3.0;"s);
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

	TEST_METHOD(ArrayAccess) {
		interpreter.evaluate("j = array(1,2,3,4); i = j[2];"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(3, value->getInt());
	}

	TEST_METHOD(ArrayAppend) {
		interpreter.evaluate("i = array(0); i += 7;"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::KSType::ARRAY, value->type);
		Assert::AreEqual(2ull, value->getArray().size());
		Assert::AreEqual(7, value->getStdVector<int>()[1]);
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

	TEST_METHOD(FunctionResultForMath) {
		interpreter.evaluate("func j(){return 999;} i = 1 + j() - 2;"s);
		auto value = interpreter.resolveVariable("i"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(998, value->getInt());
	}

	TEST_METHOD(FunctionResultForLoopExpression) {
		interpreter.evaluate("func j(){return 4;} a = 0; for(i=0;i<=j();i++){a+=i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(10, value->getInt());
	}

	TEST_METHOD(LoopFor3State) {
		interpreter.evaluate("a = 0; for(i=0;i<=4;i++){a+=i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(10, value->getInt());
	}

	TEST_METHOD(LoopFor2State) {
		interpreter.evaluate("a = 0; i = 0; for(i<=4;i++){a+=i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(10, value->getInt());
	}

	TEST_METHOD(LoopFor1State) {
		interpreter.evaluate("a = 0; i = 0; for(i<=4){a+=i; ++i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(10, value->getInt());
	}

	TEST_METHOD(LoopWhile) {
		interpreter.evaluate("a = 0; i = 0; while(i<=4){a+=i; ++i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(10, value->getInt());
	}

	TEST_METHOD(LoopForEach) {
		interpreter.evaluate("a = 0; b = [1,2,3,4]; foreach(i; b){a+=i;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(10, value->getInt());
	}

	TEST_METHOD(IfElseSimpleIfTrue) {
		interpreter.evaluate("a = 0; b = true; if(b){a+=10;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(10, value->getInt());
	}

	TEST_METHOD(IfElseSimpleIfFalse) {
		interpreter.evaluate("a = 10; b = false; if(b){a-=10;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(10, value->getInt());
	}

	TEST_METHOD(IfElseSimpleIfTrueElse) {
		interpreter.evaluate("a = 0; b = true; if(b){a+=10;}else{a=100;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(10, value->getInt());
	}

	TEST_METHOD(IfElseSimpleIfFalseElse) {
		interpreter.evaluate("a = 0; b = false; if(b){a-=10;}else{a=10;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(10, value->getInt());
	}

	TEST_METHOD(IfElseComplexIfTrueElse) {
		interpreter.evaluate("a = 0; b = true; if(b){a+=10;}else if(b){a=23;}else{a=100;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(10, value->getInt());
	}

	TEST_METHOD(IfElseComplexIfFalseElse) {
		interpreter.evaluate("a = 0; b = false; if(b){a-=10;}else if(b){a=23;}else{a=10;}"s);
		auto value = interpreter.resolveVariable("a"s);

		Assert::AreEqual(KataScript::KSType::INT, value->type);
		Assert::AreEqual(10, value->getInt());
	}

    TEST_METHOD(IfInsideNestedForLoops) {
        interpreter.evaluate("v = [1,2,3,4]; a=0; for(i=0;i<4;i++){for(j=i+1;j<4;j++){ if (i * j < 2){a += v[i] + v[j];}}}"s);

        auto value = interpreter.resolveVariable("a"s);

        Assert::AreEqual(KataScript::KSType::INT, value->type);
        Assert::AreEqual(12, value->getInt());
    }

    TEST_METHOD(IfElseInsideNestedForLoops) {
        interpreter.evaluate("v = [1,2,3,4]; a=0; for(i=0;i<4;i++){for(j=i+1;j<4;j++){ if (i * j < 2){a += v[i] + v[j];}else{a+=1;}}}"s);

        auto value = interpreter.resolveVariable("a"s);

        Assert::AreEqual(KataScript::KSType::INT, value->type);
        Assert::AreEqual(15, value->getInt());
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
        Assert::AreEqual(0, value->getInt());
    }

    TEST_METHOD(FunctionIfScoping) {
        interpreter.evaluate("func j(s) { if (s) { print(s); } a = 2; return a; }");

        auto value = interpreter.resolveVariable("a"s);
        Assert::AreEqual(KataScript::KSType::NONE, value->type);
        Assert::AreEqual(0, value->getInt());
    }

	// todo add more tests

	};
}

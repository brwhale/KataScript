#define KATASCRIPT_IMPL
#include "../Library/KataScript.hpp"

KataScript::KataScriptInterpreter interp(KataScript::ModulePrivilege::allPrivilege);

void integrationExample();

void interpret() {
    std::string s;
    while (getline(std::cin, s) && s != "quit") {
        interp.readLine(s);
    }
}

int main(int argc, char** argv) {
	//integrationExample();
	if (argc == 1) {
		std::cout << "KataScript Interpreter:\n";
        interpret();
	} else if (argc == 2) {
		// run script from file
        return interp.evaluateFile(std::string(argv[1]));
	} else {
		std::cout << "Usage: \n\tKataScript -> Starts Interpreter\n\tKataScript [filepath] -> Execute Script File\n";
	}

	return 0;
}

// This is the source for the integration examples in the github README
// https://github.com/brwhale/KataScript/blob/main/README.md#c-integration
//-------------------------------------------------------------------------------------------------//

KataScript::KSInt integrationExample(KataScript::KSInt a, KataScript::KSInt b) {
	return (a * b) + b;
}

void integrationExample() {
	// Demo c++ integration
	// Step 1: Create a function wrapper
	auto newfunc = interp.newFunction("integrationExample", [](const KataScript::KSList& args) {
		// KataScript doesn't enforce argument counts, so make sure you have enough
		if (args.size() < 2) {
			return std::make_shared<KataScript::KSValue>();
		}
		// Dereference arguments
		auto a = *args[0];
		auto b = *args[1];
		// Coerce types
		a.hardconvert(KataScript::KSType::Int);
		b.hardconvert(KataScript::KSType::Int);
		// Call c++ code
		auto result = integrationExample(a.getInt(), b.getInt());
		// Wrap and return
		return std::make_shared<KataScript::KSValue>(result);
		});

	// Step 2: Call into KataScript
	// send command into script interperereter
	interp.readLine("i = integrationExample(4, 3);");

	// get a value from the interpereter
	auto varRef = interp.resolveVariable("i");

	// or just call a function directly
	varRef = interp.callFunction(newfunc, KataScript::KSInt(4), KataScript::KSInt(3));

	// Setp 3: Unwrap your result
	// if the type is known
    int64_t i = varRef->getInt();
    std::cout << i << "\n";

	// visit style
	std::visit([](auto&& arg) {std::cout << arg << "\n"; }, varRef->value);

	// switch style
	switch (varRef->type) {
	case KataScript::KSType::Int:
		std::cout << varRef->getInt() << "\n";
		break;
	case KataScript::KSType::Float:
		std::cout << varRef->getFloat() << "\n";
		break;
	case KataScript::KSType::String:
		std::cout << varRef->getString() << "\n";
		break;
	default: break;
	}

    // create a KataScript class from C++:
    interp.newClass("beansClass", { {"color", std::make_shared<KataScript::KSValue>("white")} }, { 
        // constructor is required
        {"beansClass", [](const KataScript::KSList& vars) {
            if (vars.size() > 0) {
                interp.resolveVariable("color") = vars[0];
            }
            return std::make_shared<KataScript::KSValue>();
            } },
        // add as many functions as you want
        {"changeColor", [](const KataScript::KSList& vars) {
            if (vars.size() > 0) {
                interp.resolveVariable("color") = vars[0];
            }
            return std::make_shared<KataScript::KSValue>();
            } },
        {"isRipe", [](const KataScript::KSList&) {
            auto& color = interp.resolveVariable("color");
            if (color->type == KataScript::KSType::String) { return std::make_shared<KataScript::KSValue>(color->getString() == "brown"); }
            return std::make_shared<KataScript::KSValue>(false);
            } },
        });

    // use the class
    interp.readLine("bean = beansClass(\"grey\");");
    interp.readLine("ripe = bean.isRipe();");

    // get values from the interpereter
    auto beanRef = interp.resolveVariable("bean");
    auto ripeRef = interp.resolveVariable("ripe");

    // read the values!
    if (beanRef->type == KataScript::KSType::Class && ripeRef->type == KataScript::KSType::Int) {
        auto colorRef = beanRef->getClass()->variables["color"];
        if (colorRef->type == KataScript::KSType::String) {
            std::cout << "My bean is " << beanRef->getClass()->variables["color"]->getString() << " and it is " << (ripeRef->getBool() ? "ripe" : "unripe") << "\n";
        }
    }
}

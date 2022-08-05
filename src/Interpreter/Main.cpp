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

KataScript::Int integrationExample(KataScript::Int a, KataScript::Int b) {
	return (a * b) + b;
}

void integrationExample() {
	// Demo c++ integration
	// Step 1: Create a function wrapper
	auto newfunc = interp.newFunction("integrationExample", [](const KataScript::List& args) {
		// KataScript doesn't enforce argument counts, so make sure you have enough
		if (args.size() < 2) {
			return std::make_shared<KataScript::Value>();
		}
		// Dereference arguments
		auto a = *args[0];
		auto b = *args[1];
		// Coerce types
		a.hardconvert(KataScript::Type::Int);
		b.hardconvert(KataScript::Type::Int);
		// Call c++ code
		auto result = integrationExample(a.getInt(), b.getInt());
		// Wrap and return
		return std::make_shared<KataScript::Value>(result);
		});

	// Step 2: Call into KataScript
	// send command into script interperereter
	interp.readLine("i = integrationExample(4, 3);");

	// get a value from the interpereter
	auto varRef = interp.resolveVariable("i");

	// or just call a function directly
	varRef = interp.callFunction(newfunc, KataScript::Int(4), KataScript::Int(3));

	// Setp 3: Unwrap your result
	// if the type is known
    int64_t i = varRef->getInt();
    std::cout << i << "\n";

	// visit style
	std::visit([](auto&& arg) {std::cout << arg << "\n"; }, varRef->value);

	// switch style
	switch (varRef->type) {
	case KataScript::Type::Int:
		std::cout << varRef->getInt() << "\n";
		break;
	case KataScript::Type::Float:
		std::cout << varRef->getFloat() << "\n";
		break;
	case KataScript::Type::String:
		std::cout << varRef->getString() << "\n";
		break;
	default: break;
	}

    // create a KataScript class from C++:
    interp.newClass("beansClass", { {"color", std::make_shared<KataScript::Value>("white")} }, { 
        // constructor is required
        {"beansClass", [](const KataScript::List& vars) {
            if (vars.size() > 0) {
                interp.resolveVariable("color") = vars[0];
            }
            return std::make_shared<KataScript::Value>();
            } },
        // add as many functions as you want
        {"changeColor", [](const KataScript::List& vars) {
            if (vars.size() > 0) {
                interp.resolveVariable("color") = vars[0];
            }
            return std::make_shared<KataScript::Value>();
            } },
        {"isRipe", [](const KataScript::List&) {
            auto& color = interp.resolveVariable("color");
            if (color->type == KataScript::Type::String) { return std::make_shared<KataScript::Value>(color->getString() == "brown"); }
            return std::make_shared<KataScript::Value>(false);
            } },
        });

    // use the class
    interp.readLine("bean = beansClass(\"grey\");");
    interp.readLine("ripe = bean.isRipe();");

    // get values from the interpereter
    auto beanRef = interp.resolveVariable("bean");
    auto ripeRef = interp.resolveVariable("ripe");

    // read the values!
    if (beanRef->type == KataScript::Type::Class && ripeRef->type == KataScript::Type::Int) {
        auto colorRef = beanRef->getClass()->variables["color"];
        if (colorRef->type == KataScript::Type::String) {
            std::cout << "My bean is " << beanRef->getClass()->variables["color"]->getString() << " and it is " << (ripeRef->getBool() ? "ripe" : "unripe") << "\n";
        }
    }
}

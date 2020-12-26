#define KATASCRIPT_IMPL
#include "../Library/KataScript.hpp"
#include <fstream>

KataScript::KataScriptInterpreter interp;

void integrationExample();

int main(int argc, char** argv) {
	//integrationExample();
	std::string s;
	if (argc == 1) {
		std::cout << "KataScript Interpreter:\n";
		while (getline(std::cin, s), s != "quit") {
			interp.readLine(s);
		}
	} else if (argc == 2) {
		// run script from file
		auto file = std::ifstream(argv[1]);
        if (file) {
            file.seekg(0, std::ios::end);
            s.reserve(file.tellg());
            file.seekg(0, std::ios::beg);

            s.assign((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
            interp.evaluate(s);
        } else {
			printf("file: %s not found", argv[1]);
			return 1;
		}
	} else {
		printf("Usage: \n\tKataScript -> Starts Interpreter\n\tKataScript [filepath] -> Execute Script File");
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
		a.hardconvert(KataScript::KSType::INT);
		b.hardconvert(KataScript::KSType::INT);
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
	case KataScript::KSType::INT:
		std::cout << varRef->getInt() << "\n";
		break;
	case KataScript::KSType::FLOAT:
		std::cout << varRef->getFloat() << "\n";
		break;
	case KataScript::KSType::STRING:
		std::cout << varRef->getString() << "\n";
		break;
	}
}

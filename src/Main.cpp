#include "Scripting/KataScript.hpp"
#include <fstream>

KataScript::KataScriptInterpreter interp;

int main(int argc, char** argv) {
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
			while (getline(file, s)) {
				interp.readLine(s);
			}
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

int integrationExample(int a) {	
	return a * a;
}

void integrationExample() {
	// demo c++ integration
	interp.newFunction("integrationExample", [](KataScript::KSList args) {
		// Dereference argument
		auto val = *args[0];
		// Coerce type
		val.hardconvert(KataScript::KSType::INT);
		// Call c++ code
		auto result = integrationExample(val.getInt());
		// Wrap and return
		return std::make_shared<KataScript::KSValue>(result);
		});

	// send command into script interperereter
	interp.readLine("i = integrationExample(4);");

	// get a value from the interpereter
	auto varRef = interp.resolveVariable("i");

	// visit style
	std::visit([](auto&& arg) {std::cout << arg; }, varRef->value);

	// if the type is known
	int i = varRef->getInt();

	// switch style
	switch (varRef->type) {
	case KataScript::KSType::INT:
		std::cout << varRef->getInt();
		break;
	case KataScript::KSType::FLOAT:
		std::cout << varRef->getFloat();
		break;
	case KataScript::KSType::STRING:
		std::cout << varRef->getString();
		break;
	}
}

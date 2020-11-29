#include "Main.h"
KataScript::KataScriptInterpereter interp;

int integrationExample(int a) {
	return a * a;
}

void integrationExample() {
	// demo c++ integration
	interp.newFunction("integrationExample", [](KataScript::KSFunctionArgs args) {
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

int main() {	
	interp.active = true;

	//integrationExample();

	std::cout << "Testing kata script:\n";

	std::string s;
	while (interp.active) {
		getline(std::cin, s);
		interp.readLine(s);
	}

	return 0;
}
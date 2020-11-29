#include "Main.h"
KataScript::KataScriptInterpereter interp;

int integrationExample(int a) {
	return a * a;
}

int main() {	
	interp.active = true;

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

	std::cout << "Testing kata script:\n";

	std::string s;
	while (interp.active) {
		getline(std::cin, s);
		interp.readLine(s);
	}

	return 0;
}
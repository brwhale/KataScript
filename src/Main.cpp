#include "Main.h"
KataScript::KataScriptInterpereter interp;

int main() {	
	interp.active = true;
	std::cout << "Testing kata script:\n";

	std::string s;
	while (interp.active) {
		getline(std::cin, s);
		interp.readLine(s);
	}

	return 0;
}
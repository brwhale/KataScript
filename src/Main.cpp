#include "Main.h"
KataScriptInterpereter interp;

int main() {	
	interp.active = true;
	std::cout << "Testing kata script:\n";

	string s;
	while (interp.active) {
		getline(std::cin, s);
		interp.readLine(s);
	}

	return 0;
}
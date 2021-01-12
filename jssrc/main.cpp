#include <stdio.h>
#include <string>
#include <emscripten.h>
#define KATASCRIPT_IMPL
#include "../src/Library/KataScript.hpp"

KataScript::KataScriptInterpreter interp;

void print(const std::string& s) {
	EM_ASM({
		var inp = document.getElementById('content');
		inp.innerHTML += UTF8ToString($0).replace("\n","</br>") + '</br>';
		inp.scrollTop = inp.scrollHeight;
	}, s.c_str());
}

EMSCRIPTEN_KEEPALIVE
extern "C" void readLineLen(const char* s, int len) {
	interp.readLine(std::string(s, len));
}

int main() {
	interp.newFunction("print", [](KataScript::KSList args) {
		std::string printstr = "";
		for (auto&& arg : args) {
			printstr += arg->getPrintString();
		}
		print(printstr.c_str());
		return std::make_shared<KataScript::KSValue>();
	});
	print(std::string("KataScript Interpreter Ready!\n").c_str());
}


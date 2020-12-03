#include <stdio.h>
#include <string>
#include <emscripten.h>
#define KATASCRIPT_IMPL
#include "../src/Scripting/KataScript.hpp"

KataScript::KataScriptInterpreter interp;

EM_JS(void, print, (const char* s), {
	console.log('I received: ' + UTF8ToString(s));
});

EMSCRIPTEN_KEEPALIVE
void readLine(const char* s) {
	interp.readLine(std::string(s));
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
	print(std::string("stringaa").c_str());
}


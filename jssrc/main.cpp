#include <stdio.h>
#include <string>
#include <emscripten.h>
#define KATASCRIPT_IMPL
#include "../src/Scripting/KataScript.hpp"

KataScript::KataScriptInterpreter interp;

EM_JS(void, print, (const char* s), {
		  console.log('I received: ' + UTF8ToString(s));
		  });


int main() {
//const char* a = getHtml("textarea");
//interp.readLine(std::string(a));
print(std::string("stringgggggggg").c_str());
}


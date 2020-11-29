#pragma once

#include "Scripting/KataScriptStringUtils.hpp"

// these are wrapped with the intention of being defined
// by the implementation so you can redirect the output wherever you want
inline void print(const string& msg) {
	printf("%s\n", msg.c_str());
}

inline int print(const char* fmt, ...) {
	int result = 0;
	va_list args;
	va_start(args, fmt);

	result = vprintf(fmt, args);

	va_end(args);
	return result;
}

#include "Scripting/KataScript.hpp"
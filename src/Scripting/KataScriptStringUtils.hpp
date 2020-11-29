#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <variant>
#include <functional>
#include <unordered_map>
#include <cstdarg>
#include <charconv>
#include <iostream>
#include <memory>
#include <algorithm>

using std::min;
using std::max;
using std::string;
using std::vector;
using std::variant;
using std::function;
using std::unordered_map;
using std::shared_ptr;
using std::make_shared;
using namespace std::string_literals;

inline double fromChars(const string& token) {
	double x;
	std::from_chars(token.data(), token.data() + token.size(), x);
	return x;
}

inline bool fromChars(const string& token, double& x) {
	return std::from_chars(token.data(), token.data() + token.size(), x).ec == std::errc();
}

template<typename T, typename C>
inline bool contains(const C& container, const T& element) {
	return find(container.begin(), container.end(), element) != container.end();
}

#pragma warning( push )
#pragma warning( disable : 4996)
// TODO: replace this with std format once that happens
inline string stringformat(const char* format, ...) {
	va_list args;
	va_start(args, format);
	std::unique_ptr<char[]> buf(new char[1000]);
	vsprintf(buf.get(), format, args);
	return string(buf.get());
}
#pragma warning( pop )
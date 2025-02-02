#pragma once

#include <vector>
#include <string>
#include <charconv>

namespace KataScript {
    using std::string_view;
	using std::string;
    using std::vector;

    // Convert a string into a double
    inline std::pair<double, bool> fromChars(const string& token) {
        double x;
        bool b;
#ifdef _MSC_VER
        // std::from_chars is amazing, but only works properly in MSVC
        auto result = std::from_chars(token.data(), token.data() + token.size(), x);
        b = result.ec == std::errc{};
#else
        b = true;
        try {
            x = std::stod(token);
        } catch (const std::exception&) {
            b = false;
        }
#endif
        return { x, b };
    }

    inline std::pair<double, bool> fromChars(string_view token) {
        double x;
        bool b;
#ifdef _MSC_VER
        // std::from_chars is amazing, but only works properly in MSVC
        auto result = std::from_chars(token.data(), token.data() + token.size(), x);
        b = result.ec == std::errc{};
#else
        b = true;
        try {
            x = std::stod(string(token));
        } catch (const std::exception&) {
            b = false;
        }
#endif
        return { x, b };
    }

    // Does a collection contain a specific item?
    // works on strings, vectors, etc
    template<typename T, typename C>
    inline bool contains(const C& container, const T& element) {
        return std::find(container.begin(), container.end(), element) != container.end();
    }

    inline bool endswith(const string& v, const string& end) {
        if (end.size() > v.size()) {
            return false;
        }
        return equal(end.rbegin(), end.rend(), v.rbegin());
    }

    inline bool startswith(const string& v, const string& start) {
        if (start.size() > v.size()) {
            return false;
        }
        return equal(start.begin(), start.end(), v.begin());
    }

    inline vector<string> split(const string& input, const string& delimiter) {
        vector<string> ret;
        if (input.empty()) return ret;
        size_t pos = 0;
        size_t lpos = 0;
        auto dlen = delimiter.length();
        while ((pos = input.find(delimiter, lpos)) != string::npos) {
            ret.push_back(input.substr(lpos, pos - lpos));
            lpos = pos + dlen;
        }
        if (lpos < input.length()) {
            ret.push_back(input.substr(lpos, input.length()));
        }
        return ret;
    }

    inline vector<string_view> split(string_view input, char delimiter) {
		vector<string_view> ret;
		if (input.empty()) return ret;
		size_t pos = 0;
		size_t lpos = 0;
		while ((pos = input.find(delimiter, lpos)) != string::npos) {
			ret.push_back(input.substr(lpos, pos - lpos));
			lpos = pos + 1;
		}
		ret.push_back(input.substr(lpos, input.length()));
		return ret;
	}

    inline void replaceEscapedLiterals(string& input) {
        size_t pos = 0;
        size_t lpos = 0;
        while ((pos = input.find('\\', lpos)) != string::npos) {
            if (pos + 1 < input.size()) {
                switch (input[pos + 1]) {
                case 'n':
                    input.replace(pos, 2, "\n");
                    break;
                case 't':
                    input.replace(pos, 2, "\t");
                    break;                
                default:
                    ++pos;
                    break;
                }
            }
            lpos = ++pos;
        }
    }
}

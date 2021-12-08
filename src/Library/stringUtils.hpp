#pragma once

#include <vector>
#include <string>
#include <charconv>

namespace KataScript {
	using std::string;
    using std::vector;

    // Convert a string into a double
    inline double fromChars(const string& token) {
        double x;      
    #ifdef _MSC_VER
        // std::from_chars is amazing, but only works properly in MSVC
        std::from_chars(token.data(), token.data() + token.size(), x);
    #else
        x = std::stod(token);
    #endif
        return x;
    }

    #pragma warning( push )
#pragma warning( disable : 4996)
    // general purpose string formatting
	// TODO: replace this with std format once that happens
    // or with just anything that doesn't arbitralily choose a buffer size
	inline string stringformat(const char* format, ...) {
		va_list args;
		va_start(args, format);
		std::unique_ptr<char[]> buf(new char[1000]);
		vsprintf(buf.get(), format, args);
		return string(buf.get());
	}
#pragma warning( pop )

    // Does a collection contain a specific item?
    // works on strings, vectors, etc
    template<typename T, typename C>
    inline bool contains(const C& container, const T& element) {
        return find(container.begin(), container.end(), element) != container.end();
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

    inline vector<string> split(const string& input, char delimiter) {
		vector<string> ret;
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

    inline string replaceWhitespaceLiterals(string input) {
        size_t pos = 0;
        size_t lpos = 0;
        while ((pos = input.find('\\', lpos)) != string::npos) {
            if (pos + 1 < input.size()) {
                switch (input[pos + 1]) {
                case 't':
                    input.replace(pos, 2, "\t");
                    break;
                case 'n':
                    input.replace(pos, 2, "\n");
                    break;
                default:
                    break;
                }
            }
            lpos = pos;
        }
        return input;
    }

    inline string replace(string input, const string& lookfor, const string& replacewith) {
        size_t pos = 0;
        size_t lpos = 0;
        while ((pos = input.find(lookfor, lpos)) != string::npos) {
            input.replace(pos, lookfor.size(), replacewith);
            lpos = pos + replacewith.size();
        }
        return input;
    }
}
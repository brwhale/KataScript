#pragma once

#include <vector>
#include <string>
#include <charconv>

namespace KataScript {
    using std::string_view;
	using std::string;
    using std::vector;

    // Convert a string into a number
    template <class T>
    inline std::pair<T, bool> fromChars(const string& token) {
        T x;
        auto result = std::from_chars(token.data(), token.data() + token.size(), x);
        auto b = result.ec == std::errc{};
        return { x, b };
    }

    template <class T>
    inline std::pair<T, bool> fromChars(string_view token) {
        T x;
        auto result = std::from_chars(token.data(), token.data() + token.size(), x);
        auto b = result.ec == std::errc{};
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

    inline vector<string_view> split(string_view input, string_view delimiter) {
        vector<string_view> ret;
        if (input.empty()) return ret;
        size_t pos = 0;
        size_t lpos = 0;
        auto dlen = delimiter.length();
        while ((pos = input.find(delimiter, lpos)) != string::npos) {
            ret.push_back(input.substr(lpos, pos - lpos));
            lpos = pos + dlen;
        }
        ret.push_back(input.substr(lpos, input.size()));
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
        ret.push_back(input.substr(lpos, input.size()));
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

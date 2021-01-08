// copyright Garrett Skelton 2020
// MIT license
#pragma once

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
#include <exception>
#include <cmath>

namespace KataScript {
    // I don't like typing std:: all the time
    // so we're bringing these types into our namespace
	using std::min;
	using std::max;
	using std::string;
	using std::vector;
	using std::variant;
	using std::function;
	using std::unordered_map;
    using std::runtime_error;
	using std::shared_ptr;
	using std::make_shared;
	using namespace std::string_literals;

#ifdef KATASCRIPT_USE_32_BIT_NUMBERS
    using KSInt = int;
    using KSFloat = float;
#else
    using KSInt = int64_t;
    using KSFloat = double;
#endif // KATASCRIPT_USE_32_BIT_TYPES

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

    // Does a collection contain a specific item?
    // works on strings, vectors, etc
	template<typename T, typename C>
	inline bool contains(const C& container, const T& element) {
		return find(container.begin(), container.end(), element) != container.end();
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

	// our basic Type flag
	enum class KSType : uint8_t {
		// these are capital, otherwise they'd conflict with the c++ types of the same names
		NONE = 0, // void
		INT,
		FLOAT,
		VEC3,
		FUNCTION,
		STRING,
		ARRAY,
		LIST,
        DICTIONARY,
        STRUCT
	};

    // Get strings of type names for debugging and typeof function
	inline string getTypeName(KSType t) {
		switch (t) {
		case KSType::NONE:
			return "NONE";
			break;
		case KSType::INT:
			return "INT";
			break;
		case KSType::FLOAT:
			return "FLOAT";
			break;
		case KSType::VEC3:
			return "VEC3";
			break;
		case KSType::FUNCTION:
			return "FUNCTION";
			break;
		case KSType::STRING:
			return "STRING";
			break;
		case KSType::ARRAY:
			return "ARRAY";
			break;
		case KSType::LIST:
			return "LIST";
			break;
        case KSType::DICTIONARY:
            return "DICTIONARY";
            break;
        case KSType::STRUCT:
            return "STRUCT";
            break;
		default:
			return "UNKNOWN";
			break;
		}
	}

    inline size_t typeHashBits(KSType type) {
        return ((size_t)INT32_MAX << ((size_t)type));
    }

	// vec3 type is compatible with glm::vec3
	struct vec3 {
		// data
		float x, y, z;

		// constructors
		constexpr vec3() : x(0), y(0), z(0) {}
		constexpr vec3(vec3 const& v) : x(v.x), y(v.y), z(v.z) {}
		constexpr vec3(float scalar) : x(scalar), y(scalar), z(scalar) {}
		constexpr vec3(float a, float b, float c) : x(a), y(b), z(c) {}

		// operators
		vec3& operator=(vec3 const& v) {
			x = v.x;
			y = v.y;
			z = v.z;
			return *this;
		}
		vec3& operator+=(float scalar) {
			x += scalar;
			y += scalar;
			z += scalar;
			return *this;
		}
		vec3& operator+=(vec3 const& v) {
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}
		vec3& operator-=(float scalar) {
			x -= scalar;
			y -= scalar;
			z -= scalar;
			return *this;
		}
		vec3& operator-=(vec3 const& v) {
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}
		vec3& operator*=(float scalar) {
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}
		vec3& operator*=(vec3 const& v) {
			x *= v.x;
			y *= v.y;
			z *= v.z;
			return *this;
		}
		vec3& operator/=(float scalar) {
			x /= scalar;
			y /= scalar;
			z /= scalar;
			return *this;
		}
		vec3& operator/=(vec3 const& v) {
			x /= v.x;
			y /= v.y;
			z /= v.z;
			return *this;
		}
		vec3& operator++() {
			++x;
			++y;
			++z;
			return *this;
		}
		vec3& operator--() {
			--x;
			--y;
			--z;
			return *this;
		}
		vec3 operator++(int) {
			vec3 r(*this);
			++*this;
			return r;
		}
		vec3 operator--(int) {
			vec3 r(*this);
			--*this;
			return r;
		}
        // This can cast to glm::vec3
		template<typename T>
		operator T() {
			return T(x, y, z);
		}
	};
    // static vec3 operators
	inline vec3 operator+(vec3 const& v) {
		return v;
	}
	inline vec3 operator-(vec3 const& v) {
		return vec3(-v.x,-v.y,-v.z);
	}
	inline vec3 operator+(vec3 const& v, float scalar) {
		return vec3(
			v.x + scalar, 
			v.y + scalar, 
			v.z + scalar);
	}
	inline vec3 operator+(float scalar, vec3 const& v) {
		return vec3(
			v.x + scalar,
			v.y + scalar,
			v.z + scalar);
	}
	inline vec3 operator+(vec3 const& v1, vec3 const& v2) {
		return vec3(
			v1.x + v2.x,
			v1.y + v2.y,
			v1.z + v2.z);
	}
	inline vec3 operator-(vec3 const& v, float scalar) {
		return vec3(
			v.x - scalar,
			v.y - scalar,
			v.z - scalar);
	}
	inline vec3 operator-(float scalar, vec3 const& v) {
		return vec3(
			v.x - scalar,
			v.y - scalar,
			v.z - scalar);
	}
	inline vec3 operator-(vec3 const& v1, vec3 const& v2) {
		return vec3(
			v1.x - v2.x,
			v1.y - v2.y,
			v1.z - v2.z);
	}
	inline vec3 operator*(vec3 const& v, float scalar) {
		return vec3(
			v.x * scalar,
			v.y * scalar,
			v.z * scalar);
	}
	inline vec3 operator*(float scalar, vec3 const& v) {
		return vec3(
			v.x * scalar,
			v.y * scalar,
			v.z * scalar);
	}
	inline vec3 operator*(vec3 const& v1, vec3 const& v2) {
		return vec3(
			v1.x * v2.x,
			v1.y * v2.y,
			v1.z * v2.z);
	}
	inline vec3 operator/(vec3 const& v, float scalar) {
		return vec3(
			v.x / scalar,
			v.y / scalar,
			v.z / scalar);
	}
	inline vec3 operator/(float scalar, vec3 const& v) {
		return vec3(
			v.x / scalar,
			v.y / scalar,
			v.z / scalar);
	}
	inline vec3 operator/(vec3 const& v1, vec3 const& v2) {
		return vec3(
			v1.x / v2.x,
			v1.y / v2.y,
			v1.z / v2.z);
	}
	inline bool operator==(vec3 const& v1, vec3 const& v2) {
		return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
	}
	inline bool operator!=(vec3 const& v1, vec3 const& v2) {
		return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
	}
	inline std::ostream& operator<<(std::ostream& os, vec3 const& v) {
		return (os << v.x << ", " << v.y << ", " << v.z);
	}

	// forward declare our value type so we can build abstract collections
	struct KSValue;

    // KataScript uses shared_ptr for ref counting, anything with a name
    // like fooRef is a a shared_ptr to foo
	using KSValueRef = shared_ptr<KSValue>;

    // Backing for the List type and for function arguments
	using KSList = vector<KSValueRef>;

	// forward declare function for functions as values
	struct KSFunction;
	using KSFunctionRef = shared_ptr<KSFunction>;

    // variant allows us to have a union with a little more safety and ease
	using KSArrayVariant = 
        variant<
        vector<KSInt>,
        vector<KSFloat>,
        vector<vec3>, 
        vector<KSFunctionRef>,
        vector<string>
        >;

    // Backing for the Array type
	struct KSArray {
        KSType type;
		KSArrayVariant value;
		
        // constructors
        KSArray() : type(KSType::INT) { value = vector<KSInt>(); }
		KSArray(vector<KSInt> a) : type(KSType::INT), value(a) {}
		KSArray(vector<KSFloat> a) : type(KSType::FLOAT), value(a) {}
		KSArray(vector<vec3> a) : type(KSType::VEC3), value(a) {}
		KSArray(vector<KSFunctionRef> a) : type(KSType::FUNCTION), value(a) {}
		KSArray(vector<string> a) : type(KSType::STRING), value(a) {}
		KSArray(KSArrayVariant a, KSType t) : type(t), value(a) {}

		bool operator==(const KSArray& o) const;

		bool operator!=(const KSArray& o) const {
			return !(operator==(o));
		}

        // doing these switches is tedious
        // so ideally they all get burried beneath template functions
        // and we get a nice clean api out the other end

        // get the size without caring about the underlying type
		size_t size() const {
			switch (type) {
			case KSType::NONE:
				return get<vector<KSInt>>(value).size();
				break;
			case KSType::INT:
				return get<vector<KSInt>>(value).size();
				break;
			case KSType::FLOAT:
				return get<vector<KSFloat>>(value).size();
				break;
			case KSType::VEC3:
				return get<vector<vec3>>(value).size();
				break;
			case KSType::FUNCTION:
				return get<vector<KSFunctionRef>>(value).size();
				break;
			case KSType::STRING:
				return get<vector<string>>(value).size();
				break;
			default:
				return 0;
				break;
			}			
		}

        // Helper for push back
        template <typename T>
        void insert(const KSArrayVariant& b) {
            auto& aa = get<vector<T>>(value);
            auto& bb = get<vector<T>>(b);
            aa.insert(aa.end(), bb.begin(), bb.end());
        }

        // Default is unsupported
		template<typename T>
		void push_back(const T& t) {
			throw runtime_error("Unsupported type");
		}

        // implementation for push_back int
		template<>
		void push_back(const KSInt& t) {
			switch (type) {
			case KSType::NONE:
			case KSType::INT:
				get<vector<KSInt>>(value).push_back(t);
				break;
			default:
				throw runtime_error("Unsupported type");
				break;
			}
		}

        // implementation for push_back KSFloat
		template<>
		void push_back(const KSFloat& t) {
			switch (type) {
			case KSType::FLOAT:
				get<vector<KSFloat>>(value).push_back(t);
				break;
			default:
				throw runtime_error("Unsupported type");
				break;
			}
		}
        
        // implementation for push_back vec3
		template<>
		void push_back(const vec3& t) {
			switch (type) {
			case KSType::VEC3:
				get<vector<vec3>>(value).push_back(t);
				break;
			default:
				throw runtime_error("Unsupported type");
				break;
			}
		}

        // implementation for push_back string
		template<>
		void push_back(const string& t) {
			switch (type) {
			case KSType::STRING:
				get<vector<string>>(value).push_back(t);
				break;
			default:
				throw runtime_error("Unsupported type");
				break;
			}
		}

        // implementation for push_back function
        template<>
        void push_back(const KSFunctionRef& t) {
            switch (type) {
            case KSType::FUNCTION:
                get<vector<KSFunctionRef>>(value).push_back(t);
                break;
            default:
                throw runtime_error("Unsupported type");
                break;
            }
        }

        // implementation for push_back another array
		template<>
		void push_back(const KSArray& barr) {
			if (type == barr.type) {
				switch (type) {
				case KSType::INT:
                    insert<KSInt>(barr.value);
				    break;
				case KSType::FLOAT:
                    insert<KSFloat>(barr.value);
                    break;
				case KSType::VEC3:
                    insert<vec3>(barr.value);
                    break;
                case KSType::FUNCTION:
                    insert<KSFunctionRef>(barr.value);
                    break;
				case KSType::STRING:
                    insert<string>(barr.value);
                    break;
				default:
					break;
				}
			}
		}

        void pop_back() {
            switch (type) {
            case KSType::NONE:
            case KSType::INT:
                get<vector<KSInt>>(value).pop_back();
                break;
            case KSType::FLOAT:
                get<vector<KSFloat>>(value).pop_back();
                break;
            case KSType::VEC3:
                get<vector<vec3>>(value).pop_back();
                break;
            case KSType::FUNCTION:
                get<vector<KSFunctionRef>>(value).pop_back();
                break;
            case KSType::STRING:
                get<vector<string>>(value).pop_back();
                break;
            default:
                break;
            }
        }
	};

    using KSDictionary = unordered_map<size_t, KSValueRef>;

    struct KSScope;

    struct KSStruct {
        // this is the main storage object for all functions and variables
        string name;
        unordered_map<string, KSValueRef> variables;
        unordered_map<string, KSFunctionRef> functions;
        KSStruct(const string& name_) : name(name_) {}
        KSStruct(const KSStruct& o);
        KSStruct(const KSScope& o);
    };

    // Now that we have our collection types defined, we can finally define our value variant
    using KSValueVariant = 
        variant<
        KSInt,
        KSFloat,
        vec3, 
        KSFunctionRef,
        string,
        KSArray, 
        KSList,
        KSDictionary,
        KSStruct
        >;
	
	// our basic Object/Value type
	struct KSValue {
        KSType type;
		KSValueVariant value;
		
        // Construct a KSValue from any underlying type
		KSValue() : type(KSType::NONE) {}
		KSValue(KSInt a) : type(KSType::INT), value(a) {}
		KSValue(KSFloat a) : type(KSType::FLOAT), value(a) {}
		KSValue(vec3 a) : type(KSType::VEC3), value(a) {}
		KSValue(KSFunctionRef a) : type(KSType::FUNCTION), value(a) {}
		KSValue(string a) : type(KSType::STRING), value(a) {}
		KSValue(KSArray a) : type(KSType::ARRAY), value(a) {}
		KSValue(KSList a) : type(KSType::LIST), value(a) {}
        KSValue(KSDictionary a) : type(KSType::DICTIONARY), value(a) {}
        KSValue(KSStruct a) : type(KSType::STRUCT), value(a) {}
		KSValue(KSValueVariant a, KSType t) : type(t), value(a) {}
		~KSValue() {};

        // get a string that represents this value
		string getPrintString() {
			auto t = *this;
			t.hardconvert(KSType::STRING);
			return get<string>(t.value);
		}

        // get this value as an int
        KSInt& getInt() {
			return get<KSInt>(value);
		}

        // get this value as a float
        KSFloat& getFloat() {
			return get<KSFloat>(value);
		}

        // get this value as a vec3
		vec3& getVec3() {
			return get<vec3>(value);
		}

        // get this value as a function
		KSFunctionRef& getFunction() {
			return get<KSFunctionRef>(value);
		}

        // get this value as a string
		string& getString() {
			return get<string>(value);
		}

        // get this value as an array
		KSArray& getArray() {
			return get<KSArray>(value);
		}

        // get this value as an std::vector<T>
		template <typename T>
		vector<T>& getStdVector();

        // get this value as a list
		KSList& getList() {
			return get<KSList>(value);
		}

        KSDictionary& getDictionary() {
            return get<KSDictionary>(value);
        }

        KSStruct& getStruct() {
            return get<KSStruct>(value);
        }

        // get a boolean representing the truthiness of this value
		bool getBool() {
			// non zero or "true" are true
			bool truthiness = false;
			switch (type) {
			case KSType::INT:
				truthiness = getInt();
				break;
			case KSType::FLOAT:
				truthiness = getFloat() != 0;
				break;
			case KSType::VEC3:
				truthiness = getVec3() != vec3(0);
				break;
			case KSType::STRING:
				truthiness = getString() == "true";
				break;
			case KSType::ARRAY:
				truthiness = getArray().size() > 0;
				break;
			case KSType::LIST:
				truthiness = getList().size() > 0;
				break;
			default:
				break;
			}
			return truthiness;
		}

        size_t getHash();

        // convert this value up to the newType
        void upconvert(KSType newType);

        // convert this value to the newType even if it's a downcast
        void hardconvert(KSType newType);
	};

    // cout << operators for examples

	// define cout operator for KSList
	inline std::ostream& operator<<(std::ostream& os, const KSList& values) {
		os << KSValue(values).getPrintString();

		return os;
	}

    // define cout operator for KSArray
	inline std::ostream& operator<<(std::ostream& os, const KSArray& arr) {
		os << KSValue(arr).getPrintString();
		
		return os;
	}

    // define cout operator for KSDictionary
    inline std::ostream& operator<<(std::ostream& os, const KSDictionary& dict) {
        os << KSValue(dict).getPrintString();

        return os;
    }

    // define cout operator for KSStruct
    inline std::ostream& operator<<(std::ostream& os, const KSStruct& dict) {
        os << KSValue(dict).getPrintString();

        return os;
    }

	// functions for working with KSValues

	// makes both values have matching types
	inline void upconvert(KSValue& a, KSValue& b) {
		if (a.type != b.type) {
			if (a.type < b.type) {
				a.upconvert(b.type);
			} else {
				b.upconvert(a.type);
			}
		}
	}

    // makes both values have matching types but doesn't allow converting between numbers and non-numbers
	inline void upconvertThrowOnNonNumberToNumberCompare(KSValue& a, KSValue& b) {
		if (a.type != b.type) {
			if (max((int)a.type, (int)b.type) > (int)KSType::VEC3) {
				throw runtime_error(stringformat(
                    "Types `%s %s` and `%s %s` are incompatible for this operation",
					getTypeName(a.type).c_str(), a.getPrintString().c_str(),
                    getTypeName(b.type).c_str(), b.getPrintString().c_str()).c_str());
			}
			if (a.type < b.type) {
				a.upconvert(b.type);
			} else {
				b.upconvert(a.type);
			}
		}
	}

	// math operators
	inline KSValue operator + (KSValue a, KSValue b) {
		upconvert(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() + b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() + b.getFloat() };
			break;
		case KSType::VEC3:
			return KSValue{ a.getVec3() + b.getVec3() };
			break;
		case KSType::STRING:
			return KSValue{ a.getString() + b.getString() };
			break;
		case KSType::ARRAY:
		{
			auto arr = KSArray(a.getArray());
			arr.push_back(b.getArray());
			return KSValue{ arr };
		}
		break;
		case KSType::LIST:
		{
			auto list = KSList(a.getList());
			auto& blist = b.getList();
			list.insert(list.end(), blist.begin(), blist.end());
			return KSValue{ list };
		}
			break;
        case KSType::DICTIONARY:
        {
            auto list = KSDictionary(a.getDictionary());
            list.merge(b.getDictionary());
            return KSValue{ list };
        }
        break;
		default:
			throw runtime_error(stringformat("Operator + not defined for type `%s`",
                getTypeName(a.type).c_str()).c_str());
			break;
		}
	}

	inline KSValue operator - (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() - b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() - b.getFloat() };
			break;
		case KSType::VEC3:
			return KSValue{ a.getVec3() - b.getVec3() };
			break;
		default:
			throw runtime_error(stringformat("Operator - not defined for type `%s`", 
                getTypeName(a.type).c_str()).c_str());
			break;
		}
	}

	inline KSValue operator * (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() * b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() * b.getFloat() };
			break;
		case KSType::VEC3:
			return KSValue{ a.getVec3() * b.getVec3() };
			break;
		default:
			throw runtime_error(stringformat("Operator * not defined for type `%s`", 
                getTypeName(a.type).c_str()).c_str());
			break;
		}
	}

	inline KSValue operator / (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() / b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() / b.getFloat() };
			break;
		case KSType::VEC3:
			return KSValue{ a.getVec3() / b.getVec3() };
			break;
		default:
			throw runtime_error(stringformat("Operator / not defined for type `%s`", 
                getTypeName(a.type).c_str()).c_str());
			break;
		}
	}

	inline KSValue operator += (KSValue& a, KSValue b) {
        if ((int)a.type < (int)KSType::ARRAY || b.type == KSType::LIST) {
            upconvert(a, b);
        }
		switch (a.type) {
		case KSType::INT:
			a.getInt() += b.getInt();
			break;
		case KSType::FLOAT:
			a.getFloat() += b.getFloat();
			break;
		case KSType::VEC3:

			a.getVec3() += b.getVec3();
			break;
		case KSType::STRING:
			a.getString() += b.getString();
			break;
		case KSType::ARRAY:
		{
			auto& arr = a.getArray();
            if (arr.type == b.type 
                || (b.type == KSType::ARRAY && b.getArray().type == arr.type)) {
                switch (b.type) {
                case KSType::INT:
                    arr.push_back(b.getInt());
                    break;
                case KSType::FLOAT:
                    arr.push_back(b.getFloat());
                    break;
                case KSType::VEC3:
                    arr.push_back(b.getVec3());
                    break;
                case KSType::FUNCTION:
                    arr.push_back(b.getFunction());
                    break;
                case KSType::STRING:
                    arr.push_back(b.getString());
                    break;
                case KSType::ARRAY:
                    arr.push_back(b.getArray());
                    break;
                default:
                    break;
                }
            }
			return KSValue{ arr };
		}
		break;
		case KSType::LIST:
		{
            auto& list = a.getList();
            switch (b.type) {
            case KSType::INT:
            case KSType::FLOAT:
            case KSType::VEC3:
            case KSType::FUNCTION:
            case KSType::STRING:
                list.push_back(make_shared<KSValue>(b.value, b.type));
                break;
            default:
            {
                b.upconvert(KSType::LIST);
                auto& blist = b.getList();
                list.insert(list.end(), blist.begin(), blist.end());
            }
                break;
            }			
		}
		break;
        case KSType::DICTIONARY:
        {
            auto& dict = a.getDictionary();
            b.upconvert(KSType::DICTIONARY);
            dict.merge(b.getDictionary());
        }
        break;
		default:
			throw runtime_error(stringformat("Operator += not defined for type `%s`",
                getTypeName(a.type).c_str()).c_str());
			break;
		}
		return a;
	}

	inline KSValue operator -= (KSValue& a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			a.getInt() -= b.getInt();
			break;
		case KSType::FLOAT:
			a.getFloat() -= b.getFloat();
			break;
		case KSType::VEC3:
			a.getVec3() -= b.getVec3();
			break;
		default:
			throw runtime_error(stringformat("Operator -= not defined for type `%s`", 
                getTypeName(a.type).c_str()).c_str());
			break;
		}
		return a;
	}

	inline KSValue operator *= (KSValue& a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			a.getInt() *= b.getInt();
			break;
		case KSType::FLOAT:
			a.getFloat() *= b.getFloat();
			break;
		case KSType::VEC3:
			a.getVec3() *= b.getVec3();
			break;
		default:
			throw runtime_error(stringformat("Operator *= not defined for type `%s`", 
                getTypeName(a.type).c_str()).c_str());
			break;
		}
		return a;
	}

	inline KSValue operator /= (KSValue& a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			a.getInt() /= b.getInt();
			break;
		case KSType::FLOAT:
			a.getFloat() /= b.getFloat();
			break;
		case KSType::VEC3:
			a.getVec3() /= b.getVec3();
			break;
		default:
			throw runtime_error(stringformat("Operator /= not defined for type `%s`", 
                getTypeName(a.type).c_str()).c_str());
			break;
		}
		return a;
	}

	inline KSValue operator % (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() % b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ std::fmod(a.getFloat(), b.getFloat()) };
			break;
		default:
			throw runtime_error(stringformat("Operator %% not defined for type `%s`", 
                getTypeName(a.type).c_str()).c_str());
			break;
		}
	}

	// comparison operators
	bool operator != (KSValue a, KSValue b);
	inline bool operator == (KSValue a, KSValue b) {
        if (a.type != b.type) {
            return false;
        }
		switch (a.type) {
        case KSType::NONE:
            return true;
		case KSType::INT:
			return a.getInt() == b.getInt();
			break;
		case KSType::FLOAT:
			return a.getFloat() == b.getFloat();
			break;
		case KSType::VEC3:
			return a.getVec3() == b.getVec3();
			break;
		case KSType::STRING:
			return a.getString() == b.getString();
			break;
		case KSType::ARRAY:
			return a.getArray() == b.getArray();
			break;
		case KSType::LIST:
		{
			auto& alist = a.getList();
			auto& blist = b.getList();
			if (alist.size() != blist.size()) {
				return false;
			}
			for (size_t i = 0; i < alist.size(); ++i) {
				if (*alist[i] != *blist[i]) {
					return false;
				}
			}
			return true;
		}
			break;
		default:
            throw runtime_error(stringformat("Operator == not defined for type `%s`",
                getTypeName(a.type).c_str()).c_str());
			break;
		}
		return true;
	}

	inline bool operator != (KSValue a, KSValue b) {
        if (a.type != b.type) {
            return true;
        }		
		switch (a.type) {
        case KSType::NONE:
            return false;
		case KSType::INT:
			return a.getInt() != b.getInt();
			break;
		case KSType::FLOAT:
			return a.getFloat() != b.getFloat();
			break;
		case KSType::VEC3:
			return a.getVec3() != b.getVec3();
			break;
		case KSType::STRING:
			return a.getString() != b.getString();
			break;
		case KSType::ARRAY:
		case KSType::LIST:
			return !(a == b);
			break;
		default:
            throw runtime_error(stringformat("Operator != not defined for type `%s`",
                getTypeName(a.type).c_str()).c_str());
			break;
		}
		return false;
	}

	inline bool operator || (KSValue& a, KSValue& b) {
		return a.getBool() || b.getBool();
	}

	inline bool operator && (KSValue& a, KSValue& b) {
		return a.getBool() && b.getBool();
	}

	inline bool operator < (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			return a.getInt() < b.getInt();
			break;
		case KSType::FLOAT:
			return a.getFloat() < b.getFloat();
			break;
		case KSType::STRING:
			return a.getString() < b.getString();
			break;
		case KSType::ARRAY:
			return a.getArray().size() < b.getArray().size();
			break;
		case KSType::LIST:
			return a.getList().size() < b.getList().size();
			break;
        case KSType::DICTIONARY:
            return a.getDictionary().size() < b.getDictionary().size();
            break;
		default:
			break;
		}
		return false;
	}

	inline bool operator > (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			return a.getInt() > b.getInt();
			break;
		case KSType::FLOAT:
			return a.getFloat() > b.getFloat();
			break;
		case KSType::STRING:
			return a.getString() > b.getString();
			break;
		case KSType::ARRAY:
			return a.getArray().size() > b.getArray().size();
			break;
		case KSType::LIST:
			return a.getList().size() > b.getList().size();
			break;
        case KSType::DICTIONARY:
            return a.getDictionary().size() > b.getDictionary().size();
            break;
		default:
			break;
		}
		return false;
	}

	inline bool operator <= (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			return a.getInt() <= b.getInt();
			break;
		case KSType::FLOAT:
			return a.getFloat() <= b.getFloat();
			break;
		case KSType::STRING:
			return a.getString() <= b.getString();
			break;
		case KSType::ARRAY:
			return a.getArray().size() <= b.getArray().size();
			break;
		case KSType::LIST:
			return a.getList().size() <= b.getList().size();
			break;
        case KSType::DICTIONARY:
            return a.getDictionary().size() <= b.getDictionary().size();
            break;
		default:
			break;
		}
		return false;
	}

	inline bool operator >= (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			return a.getInt() >= b.getInt();
			break;
		case KSType::FLOAT:
			return a.getFloat() >= b.getFloat();
			break;
		case KSType::STRING:
			return a.getString() >= b.getString();
			break;
		case KSType::ARRAY:
			return a.getArray().size() >= b.getArray().size();
			break;
		case KSType::LIST:
			return a.getList().size() >= b.getList().size();
			break;
        case KSType::DICTIONARY:
            return a.getDictionary().size() >= b.getDictionary().size();
            break;
		default:
			break;
		}
		return false;
	}

	// operator precedence for pemdas
	enum class KSOperatorPrecedence : int {
		assign = 0,
        boolean,
		compare,        
		addsub,
		muldiv,
		incdec,
		func
	};

	// KSLambda is a "native function" it's how you wrap c++ code for use inside KataScript
	using KSLambda = function<KSValueRef(const KSList&)>;

	// forward declare so we can cross refernce types
	// KSExpression is a 'generic' expression
	struct KSExpression;
	using KSExpressionRef = shared_ptr<KSExpression>;
	struct KSScope;
	using KSScopeRef = shared_ptr<KSScope>;

    enum class KSFunctionType : uint8_t {
        FREE,
        CONSTRUCTOR,
        MEMBER,
    };

	// our basic function type
	struct KSFunction {
        KSOperatorPrecedence opPrecedence;
        KSFunctionType type = KSFunctionType::FREE;
        string name;
		vector<string> argNames;

		// to calculate a result
		// either we have a KataScript body
		vector<KSExpressionRef> subexpressions;
		// or a KSLambda 
		KSLambda lambda;

		static KSOperatorPrecedence getPrecedence(const string& n) {
			if (n.size() > 2) {
				return KSOperatorPrecedence::func;
			}
            if (n == "||" || n == "&&") {
                return KSOperatorPrecedence::boolean;
            }
			if (contains("!<>|&"s, n[0]) || n == "==") {
				return KSOperatorPrecedence::compare;
			}
			if (contains(n, '=')) {
				return KSOperatorPrecedence::assign;
			}
			if (contains("/*%"s, n[0])) {
				return KSOperatorPrecedence::muldiv;
			}
			if (contains("-+"s, n[0])) {
				return KSOperatorPrecedence::addsub;
			}
			return KSOperatorPrecedence::func;
		}

		KSFunction(const string& name_, const KSLambda& l) 
            : name(name_), opPrecedence(getPrecedence(name_)), lambda(l) {}
		// when using a KataScript function body
        // the operator precedence will always be "func" level (aka the highest)
		KSFunction(const string& name_, const vector<string>& argNames_, const vector<KSExpressionRef>& body_) 
			: name(name_), subexpressions(body_), argNames(argNames_), opPrecedence(KSOperatorPrecedence::func) {}
		// default constructor makes a function with no args that returns void
		KSFunction(const string& name) 
            : KSFunction(name, [](KSList) { return make_shared<KSValue>(); }) {}
		KSFunction() : KSFunction("anon", nullptr) {}
        KSFunction(const KSFunction& o) = default;
	};

	// describes an expression tree with a function at the root
	struct KSFunctionExpression {
		KSValueRef function;
		vector<KSExpressionRef> subexpressions;

		KSFunctionExpression(const KSFunctionExpression& o) {
			function = o.function;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<KSExpression>(*sub));
			}
		}
		KSFunctionExpression(KSFunctionRef fnc) : function(new KSValue(fnc)) {}
		KSFunctionExpression(KSValueRef fncvalue) : function(fncvalue) {}
		KSFunctionExpression() {}

		void clear() {
			subexpressions.clear();
		}

		~KSFunctionExpression() {
			clear();
		}
	};

	struct KSReturn {
		KSExpressionRef expression;

		KSReturn(const KSReturn& o) {
			expression = o.expression ? make_shared<KSExpression>(*o.expression) : nullptr;
		}
		KSReturn(KSExpressionRef e) : expression(e) {}
		KSReturn() {}
	};

	struct KSIf {
		KSExpressionRef testExpression;
		vector<KSExpressionRef> subexpressions;

		KSIf(const KSIf& o) {
			testExpression = o.testExpression ? make_shared<KSExpression>(*o.testExpression) : nullptr;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<KSExpression>(*sub));
			}
		}
		KSIf() {}
	};

	using KSIfElse = vector<KSIf>;

	struct KSLoop {
		KSExpressionRef initExpression;
		KSExpressionRef testExpression;
		KSExpressionRef iterateExpression;
		vector<KSExpressionRef> subexpressions;

		KSLoop(const KSLoop& o) {
			initExpression = o.initExpression ? make_shared<KSExpression>(*o.initExpression) : nullptr;
			testExpression = o.testExpression ? make_shared<KSExpression>(*o.testExpression) : nullptr;
			iterateExpression = o.iterateExpression ? make_shared<KSExpression>(*o.iterateExpression) : nullptr;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<KSExpression>(*sub));
			}
		}
		KSLoop() {}
	};

	struct KSForeach {
        KSExpressionRef listExpression;
		string iterateName;
		vector<KSExpressionRef> subexpressions;

		KSForeach(const KSForeach& o) {
            listExpression = o.listExpression ? make_shared<KSExpression>(*o.listExpression) : nullptr;
			iterateName = o.iterateName;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<KSExpression>(*sub));
			}
		}
		KSForeach() {}
	};

    struct KSResolveVar {
        string name;

        KSResolveVar(const KSResolveVar& o) {
            name = o.name;
        }
        KSResolveVar() {}
        KSResolveVar(const string& n) : name(n) {}
    };

    struct KSDefineVar {
        string name;
        KSExpressionRef defineExpression;

        KSDefineVar(const KSDefineVar& o) {
            name = o.name;
            defineExpression = o.defineExpression ? make_shared<KSExpression>(*o.defineExpression) : nullptr;
        }
        KSDefineVar() {}
        KSDefineVar(const string& n) : name(n) {}
        KSDefineVar(const string& n, KSExpressionRef defExpr) : name(n), defineExpression(defExpr) {}
    };

	enum class KSExpressionType : uint8_t {
		VALUE,
        RESOLVEVAR,
        DEFINEVAR,
		FUNCTIONDEF,
		FUNCTIONCALL,
		RETURN,
		LOOP,
		FOREACH,
		IFELSE
	};

    using KSExpressionVariant = 
        variant<
        KSValueRef,
        KSResolveVar, 
        KSDefineVar, 
        KSFunctionExpression,
        KSReturn, 
        KSLoop, 
        KSForeach,
        KSIfElse
        >;

	// forward declare so we can use the parser to process functions
	class KataScriptInterpreter;
	// describes a 'generic' expression tree, with either a value or function at the root
	struct KSExpression {
		// either we have a value, or a function expression which then has sub-expressions
        KSExpressionVariant expression;
        KSExpressionType type;
		KSExpressionRef parent = nullptr;

		KSExpression(KSValueRef val) 
            : type(KSExpressionType::FUNCTIONCALL), expression(KSFunctionExpression(val)), parent(nullptr) {}
		KSExpression(KSFunctionRef val, KSExpressionRef par) 
            : type(KSExpressionType::FUNCTIONDEF), expression(KSFunctionExpression(val)), parent(par) {}
		KSExpression(KSValueRef val, KSExpressionRef par) 
            : type(KSExpressionType::VALUE), expression(val), parent(par) {}
		KSExpression(KSForeach val, KSExpressionRef par = nullptr) 
            : type(KSExpressionType::FOREACH), expression(val), parent(par) {}
		KSExpression(KSLoop val, KSExpressionRef par = nullptr) 
            : type(KSExpressionType::LOOP), expression(val), parent(par) {}
		KSExpression(KSIfElse val, KSExpressionRef par = nullptr) 
            : type(KSExpressionType::IFELSE), expression(val), parent(par) {}
		KSExpression(KSReturn val, KSExpressionRef par = nullptr) 
            : type(KSExpressionType::RETURN), expression(val), parent(par) {}
        KSExpression(KSResolveVar val, KSExpressionRef par = nullptr) 
            : type(KSExpressionType::RESOLVEVAR), expression(val), parent(par) {}
        KSExpression(KSDefineVar val, KSExpressionRef par = nullptr) 
            : type(KSExpressionType::DEFINEVAR), expression(val), parent(par) {}

		KSExpressionRef back() {
			switch (type) {
			case KSExpressionType::FUNCTIONDEF:
				return get<KSFunctionExpression>(expression).function->getFunction()->subexpressions.back();
				break;
			case KSExpressionType::FUNCTIONCALL:
				return get<KSFunctionExpression>(expression).subexpressions.back();
				break;
			case KSExpressionType::LOOP:
				return get<KSLoop>(expression).subexpressions.back();
				break;
			case KSExpressionType::FOREACH:
				return get<KSForeach>(expression).subexpressions.back();
				break;
			case KSExpressionType::IFELSE:
				return get<KSIfElse>(expression).back().subexpressions.back();
				break;
			default:
				break;
			}
			return nullptr;
		}

        auto begin() {
            switch (type) {
            case KSExpressionType::FUNCTIONCALL:
                return get<KSFunctionExpression>(expression).subexpressions.begin();
                break;
            case KSExpressionType::FUNCTIONDEF:
                return get<KSFunctionExpression>(expression).function->getFunction()->subexpressions.begin();
                break;
            case KSExpressionType::LOOP:
                return get<KSLoop>(expression).subexpressions.begin();
                break;
            case KSExpressionType::FOREACH:
                return get<KSForeach>(expression).subexpressions.begin();
                break;
            case KSExpressionType::IFELSE:
                return get<KSIfElse>(expression).back().subexpressions.begin();
                break;
            default:
                return vector<KSExpressionRef>::iterator();
                break;
            }
        }

        auto end() {
            switch (type) {
            case KSExpressionType::FUNCTIONCALL:
                return get<KSFunctionExpression>(expression).subexpressions.end();
                break;
            case KSExpressionType::FUNCTIONDEF:
                return get<KSFunctionExpression>(expression).function->getFunction()->subexpressions.end();
                break;
            case KSExpressionType::LOOP:
                return get<KSLoop>(expression).subexpressions.end();
                break;
            case KSExpressionType::FOREACH:
                return get<KSForeach>(expression).subexpressions.end();
                break;
            case KSExpressionType::IFELSE:
                return get<KSIfElse>(expression).back().subexpressions.end();
                break;
            default:
                return vector<KSExpressionRef>::iterator();
                break;
            }
        }

		void push_back(KSExpressionRef ref) {
			switch (type) {
			case KSExpressionType::FUNCTIONCALL:
                get<KSFunctionExpression>(expression).subexpressions.push_back(ref);
				break;
			case KSExpressionType::FUNCTIONDEF:
                get<KSFunctionExpression>(expression).function->getFunction()->subexpressions.push_back(ref);
				break;
			case KSExpressionType::LOOP:
                get<KSLoop>(expression).subexpressions.push_back(ref);
				break;
			case KSExpressionType::FOREACH:
                get<KSForeach>(expression).subexpressions.push_back(ref);
				break;
			case KSExpressionType::IFELSE:
                get<KSIfElse>(expression).back().subexpressions.push_back(ref);
				break;
			default:
				break;
			}
		}

		void push_back(const KSIf& ref) {
			switch (type) {
			case KSExpressionType::IFELSE:
                get<KSIfElse>(expression).push_back(ref);
				break;
			default:
				break;
			}
		}

        bool needsToReturn(KSExpressionRef expr, KSValueRef& returnVal, KataScriptInterpreter* i);

        // walk the tree depth first and replace any function expressions 
		// with a value expression of their results
		void consolidate(KataScriptInterpreter* i);
	};

	struct KSScope {
		// this is the main storage object for all functions and variables
		string name;
		KSScopeRef parent;
        bool structScope = false;
		unordered_map<string, KSValueRef> variables;
		unordered_map<string, KSScopeRef> scopes; 
		unordered_map<string, KSFunctionRef> functions;
		KSScope(const string& name_, KSScopeRef scope) : name(name_), parent(scope) {}
        KSScope(const KSScope& o) : name(o.name), parent(o.parent), scopes(o.scopes), functions(o.functions) {
            for (auto&& v : o.variables) {
                variables[v.first] = make_shared<KSValue>(v.second->value, v.second->type);
            }
        }
	};

	// state enum for state machine for token by token parsing
	enum class KSParseState : uint8_t {
		beginExpression,
		readLine,
        defineVar,
		defineFunc,
        defineStruct,
		funcArgs,
		returnLine,		
		ifCall,
		expectIfEnd,
		loopCall,
		forEach,
	};

	// finally we have our interpereter
	class KataScriptInterpreter {
		friend KSExpression;
		KSScopeRef globalScope = make_shared<KSScope>("global", nullptr);
		KSScopeRef currentScope = globalScope;
        KSStruct* currentStruct = nullptr;
		vector<KSScopeRef> modules;

		KSExpressionRef currentExpression;

		KSParseState parseState = KSParseState::beginExpression;
		vector<string> parseStrings;
		int outerNestLayer = 0;		
		
		KSExpressionRef getExpression(const vector<string>& strings);
		KSValueRef getValue(const vector<string>& strings);
		
		void clearParseStacks();
		void closeDanglingIfExpression();
		void parse(const string& token);
		KSFunctionRef& newLibraryFunction(const string& name, const KSLambda& lam, KSScopeRef scope);
		KSFunctionRef& newFunction(const string& name, const vector<string>& argNames, const vector<KSExpressionRef>& body);
		KSValueRef getValue(KSExpressionRef expr);
		void newScope(const string& name);
        KSScopeRef resolveScope(const string& name);
		void closeCurrentScope();
		bool closeCurrentExpression();
		KSValueRef callFunction(const string& name, const KSList& args);
		KSValueRef callFunction(const KSFunctionRef fnc, const KSList& args);
	public:
		KSFunctionRef& newFunction(const string& name, const KSLambda& lam);		
		template <typename ... Ts>
		KSValueRef callFunction(const KSFunctionRef fnc, Ts...args) {
			KSList argsList = { make_shared<KSValue>(args)... };
			return callFunction(fnc, argsList);
		}
		KSValueRef& resolveVariable(const string& name, KSScopeRef = nullptr);
		KSFunctionRef resolveFunction(const string& name);
		void readLine(const string& text);
		void evaluate(const string& script);
		void clearState();
		KataScriptInterpreter();
	};

    // split impl
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

#ifdef KATASCRIPT_IMPL
	// implementations
	template <typename T>
	vector<T>& KSValue::getStdVector() {
		return get<vector<T>>(get<KSArray>(value).value);
	}

    KSStruct::KSStruct(const KSStruct& o) : name(o.name), functions(o.functions) {
        for (auto&& v : o.variables) {
            variables[v.first] = make_shared<KSValue>(v.second->value, v.second->type);
        }
    }

    KSStruct::KSStruct(const KSScope& o) : name(o.name), functions(o.functions) {
        for (auto&& v : o.variables) {
            variables[v.first] = make_shared<KSValue>(v.second->value, v.second->type);
        }
    }

    size_t KSValue::getHash() {
        size_t hash = 0;
        switch (type) {
        default: break;
        case KSType::INT:
            hash = (size_t)getInt();
            break;
        case KSType::FLOAT:
            hash = std::hash<KSFloat>{}(getFloat());
            break;
        case KSType::VEC3:
            hash = std::hash<float>{}(getVec3().x) ^ std::hash<float>{}(getVec3().y) ^ std::hash<float>{}(getVec3().z);
            break;
        case KSType::FUNCTION:
            hash = std::hash<size_t>{}((size_t)getFunction().get());
            break;
        case KSType::STRING:
            hash = std::hash<string>{}(getString());
            break;
        }
        return hash ^ typeHashBits(type);
    }

    // convert this value up to the newType
    void KSValue::upconvert(KSType newType) {
        if (newType > type) {
            switch (newType) {
            default:
                throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                    getTypeName(type).c_str(), getTypeName(newType).c_str()));
                break;
            case KSType::INT:
                value = KSInt(0);
                break;
            case KSType::FLOAT:
                switch (type) {
                default:
                    throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::NONE:
                    value = 0.f;
                    break;
                case KSType::INT:
                    value = (KSFloat)getInt();
                    break;
                }
                break;
            
            case KSType::VEC3:
                switch (type) {
                default:
                    throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::NONE:
                    value = vec3();
                    break;
                case KSType::INT:
                    value = vec3((float)getInt());
                    break;
                case KSType::FLOAT:
                    value = vec3((float)getFloat());
                    break;
                }
                break;
            case KSType::STRING:
                switch (type) {
                default:
                    throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::NONE:
                    value = "null"s;
                    break;
                case KSType::INT:
                    value = stringformat("%lld", getInt());
                    break;
                case KSType::FLOAT:
                    value = stringformat("%f", getFloat());
                    break;
                case KSType::VEC3:
                {
                    auto& vec = getVec3();
                    value = stringformat("%f, %f, %f", vec.x, vec.y, vec.z);
                    break;
                }
                case KSType::FUNCTION:
                    value = getFunction()->name;
                    break;
                }
                break;
            case KSType::ARRAY:
                switch (type) {
                default:
                    throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::NONE:
                    value = KSArray();
                    getArray().push_back(KSInt(0));
                    break;
                case KSType::INT:
                    value = KSArray(vector<KSInt>{ getInt() });
                    break;
                case KSType::FLOAT:
                    value = KSArray(vector<KSFloat>{ getFloat() });
                    break;
                case KSType::VEC3:
                    value = KSArray(vector<vec3>{ getVec3() });
                    break;
                case KSType::STRING:
                {
                    auto str = getString();
                    value = KSArray(vector<string>{ });
                    auto& arry = getStdVector<string>();
                    for (auto&& ch : str) {
                        arry.push_back(""s + ch);
                    }
                }
                break;
                }
                break;
            case KSType::LIST:
                switch (type) {
                default:
                    throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::NONE:
                case KSType::INT:
                case KSType::FLOAT:
                case KSType::VEC3:
                    value = KSList({ make_shared<KSValue>(value, type) });
                    break;
                case KSType::STRING:
                {
                    auto str = getString();
                    value = KSList();
                    auto& list = getList();
                    for (auto&& ch : str) {
                        list.push_back(make_shared<KSValue>(""s + ch));
                    }
                }
                break;
                case KSType::ARRAY:
                    KSArray arr = getArray();
                    value = KSList();
                    auto& list = getList();
                    switch (arr.type) {
                    case KSType::INT:
                        for (auto&& item : get<vector<KSInt>>(arr.value)) {
                            list.push_back(make_shared<KSValue>(item));
                        }
                        break;
                    case KSType::FLOAT:
                        for (auto&& item : get<vector<KSFloat>>(arr.value)) {
                            list.push_back(make_shared<KSValue>(item));
                        }
                        break;
                    case KSType::VEC3:
                        for (auto&& item : get<vector<vec3>>(arr.value)) {
                            list.push_back(make_shared<KSValue>(item));
                        }
                        break;
                    case KSType::STRING:
                        for (auto&& item : get<vector<string>>(arr.value)) {
                            list.push_back(make_shared<KSValue>(item));
                        }
                        break;
                    default:
                        throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                            getTypeName(type).c_str(), getTypeName(newType).c_str()));
                        break;
                    }
                    break;
                }
                break;
            case KSType::DICTIONARY:
                switch (type) {
                default:
                    throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::NONE:
                case KSType::INT:
                case KSType::FLOAT:
                case KSType::VEC3:
                case KSType::STRING:
                    value = KSDictionary();
                    break;
                case KSType::ARRAY:
                {
                    KSArray arr = getArray();
                    value = KSDictionary();
                    auto hashbits = typeHashBits(KSType::INT);
                    auto& dict = getDictionary();
                    size_t index = 0;
                    switch (arr.type) {
                    case KSType::INT:
                        for (auto&& item : get<vector<KSInt>>(arr.value)) {
                            dict[index++ ^ hashbits] = make_shared<KSValue>(item);
                        }
                        break;
                    case KSType::FLOAT:
                        for (auto&& item : get<vector<KSFloat>>(arr.value)) {
                            dict[index++ ^ hashbits] = make_shared<KSValue>(item);
                        }
                        break;
                    case KSType::VEC3:
                        for (auto&& item : get<vector<vec3>>(arr.value)) {
                            dict[index++ ^ hashbits] = make_shared<KSValue>(item);
                        }
                        break;
                    case KSType::STRING:
                        for (auto&& item : get<vector<string>>(arr.value)) {
                            dict[index++ ^ hashbits] = make_shared<KSValue>(item);
                        }
                        break;
                    default:
                        throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                            getTypeName(type).c_str(), getTypeName(newType).c_str()));
                        break;
                    }
                }
                break;
                case KSType::LIST:
                {
                    auto hashbits = typeHashBits(KSType::INT);
                    KSList list = getList();
                    value = KSDictionary();
                    auto& dict = getDictionary();
                    size_t index = 0;
                    for (auto&& item : list) {
                        dict[index++ ^ hashbits] = item;
                    }
                }
                break;
                }
                break;
            }
            type = newType;
        }
    }

    // convert this value to the newType even if it's a downcast
    void KSValue::hardconvert(KSType newType) {
        if (newType >= type) {
            upconvert(newType);
        } else {
            switch (newType) {
            default:
                throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                    getTypeName(type).c_str(), getTypeName(newType).c_str()));
                break;
            case KSType::NONE:
                value = KSInt(0);
                break;
            case KSType::INT:
                switch (type) {
                default:
                    break;
                case KSType::FLOAT:
                    value = (KSInt)getFloat();
                    break;
                case KSType::STRING:
                    value = (KSInt)fromChars(getString());
                    break;
                case KSType::ARRAY:
                    value = (KSInt)getArray().size();
                    break;
                case KSType::LIST:
                    value = (KSInt)getList().size();
                    break;
                }
                break;
            case KSType::FLOAT:
                switch (type) {
                default:
                    throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::STRING:
                    value = (KSFloat)fromChars(getString());
                    break;
                case KSType::ARRAY:
                    value = (KSFloat)getArray().size();
                    break;
                case KSType::LIST:
                    value = (KSFloat)getList().size();
                    break;
                }
                break;
            case KSType::STRING:
                switch (type) {
                default:
                    throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::ARRAY:
                {
                    string newval;
                    auto& arr = getArray();
                    switch (arr.type) {
                    case KSType::INT:
                        for (auto&& item : get<vector<KSInt>>(arr.value)) {
                            newval += KSValue(item).getPrintString() + ", ";
                        }
                        break;
                    case KSType::FLOAT:
                        for (auto&& item : get<vector<KSFloat>>(arr.value)) {
                            newval += KSValue(item).getPrintString() + ", ";
                        }
                        break;
                    case KSType::VEC3:
                        for (auto&& item : get<vector<vec3>>(arr.value)) {
                            newval += KSValue(item).getPrintString() + ", ";
                        }
                        break;
                    case KSType::STRING:
                        for (auto&& item : get<vector<string>>(arr.value)) {
                            newval += KSValue(item).getPrintString() + ", ";
                        }
                        break;
                    default:
                        throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                            getTypeName(type).c_str(), getTypeName(newType).c_str()));
                        break;
                    }
                    if (arr.size()) {
                        newval.pop_back();
                        newval.pop_back();
                    }
                    value = newval;
                }
                break;
                case KSType::LIST:
                {
                    string newval;
                    auto& list = getList();
                    for (auto val : list) {
                        newval += val->getPrintString() + ", ";
                    }
                    if (newval.size()) {
                        newval.pop_back();
                        newval.pop_back();
                    }
                    value = newval;
                }
                break;
                case KSType::DICTIONARY:
                {
                    string newval;
                    auto& dict = getDictionary();
                    for (auto&& val : dict) {
                        newval += stringformat("`%u: %s`, ", val.first, val.second->getPrintString().c_str());
                    }
                    if (newval.size()) {
                        newval.pop_back();
                        newval.pop_back();
                    }
                    value = newval;
                }
                break;
                case KSType::STRUCT:
                {
                    auto& strct = getStruct();
                    string newval = strct.name + ":\n"s;
                    for (auto&& val : strct.variables) {
                        newval += stringformat("`%s: %s`\n", val.first.c_str(), val.second->getPrintString().c_str());
                    }
                    value = newval;
                }
                break;
                }
                break;
            case KSType::ARRAY:
            {
                switch (type) {
                default:
                    throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::DICTIONARY:
                {
                    KSArray arr;
                    auto dict = getDictionary();
                    auto listType = dict.begin()->second->type;
                    switch (listType) {
                    case KSType::INT:
                        arr = KSArray(vector<KSInt>{});
                        for (auto&& item : dict) {
                            if (item.second->type == listType) {
                                arr.push_back(item.second->getInt());
                            }
                        }
                        break;
                    case KSType::FLOAT:
                        arr = KSArray(vector<KSFloat>{});
                        for (auto&& item : dict) {
                            if (item.second->type == listType) {
                                arr.push_back(item.second->getFloat());
                            }
                        }
                        break;
                    case KSType::VEC3:
                        arr = KSArray(vector<vec3>{});
                        for (auto&& item : dict) {
                            if (item.second->type == listType) {
                                arr.push_back(item.second->getVec3());
                            }
                        }
                        break;
                    case KSType::FUNCTION:
                        arr = KSArray(vector<KSFunctionRef>{});
                        for (auto&& item : dict) {
                            if (item.second->type == listType) {
                                arr.push_back(item.second->getFunction());
                            }
                        }
                        break;
                    case KSType::STRING:
                        arr = KSArray(vector<string>{});
                        for (auto&& item : dict) {
                            if (item.second->type == listType) {
                                arr.push_back(item.second->getString());
                            }
                        }
                        break;
                    default:
                        throw runtime_error("Array cannot contain collections");
                        break;
                    }
                    value = arr;
                }
                break;
                case KSType::LIST:
                {
                    auto list = getList();
                    auto listType = list[0]->type;
                    KSArray arr;
                    switch (listType) {
                    case KSType::INT:
                        arr = KSArray(vector<KSInt>{});
                        for (auto&& item : list) {
                            if (item->type == listType) {
                                arr.push_back(item->getInt());
                            }
                        }
                        break;
                    case KSType::FLOAT:
                        arr = KSArray(vector<KSFloat>{});
                        for (auto&& item : list) {
                            if (item->type == listType) {
                                arr.push_back(item->getFloat());
                            }
                        }
                        break;
                    case KSType::VEC3:
                        arr = KSArray(vector<vec3>{});
                        for (auto&& item : list) {
                            if (item->type == listType) {
                                arr.push_back(item->getVec3());
                            }
                        }
                        break;
                    case KSType::FUNCTION:
                        arr = KSArray(vector<KSFunctionRef>{});
                        for (auto&& item : list) {
                            if (item->type == listType) {
                                arr.push_back(item->getFunction());
                            }
                        }
                        break;
                    case KSType::STRING:
                        arr = KSArray(vector<string>{});
                        for (auto&& item : list) {
                            if (item->type == listType) {
                                arr.push_back(item->getString());
                            }
                        }
                        break;
                    default:
                        throw runtime_error("Array cannot contain collections");
                        break;
                    }
                    value = arr;
                }
                break;
                }
                break;
            }
            break;
            case KSType::LIST:
            {
                switch (type) {
                default:
                    throw runtime_error(stringformat("Conversion not defined for types `%s` to `%s`",
                        getTypeName(type).c_str(), getTypeName(newType).c_str()));
                    break;
                case KSType::DICTIONARY:
                {
                    KSList list;
                    for (auto&& item : getDictionary()) {
                        list.push_back(item.second);
                    }
                    value = list;
                }
                break;
                case KSType::STRUCT:
                    value = KSList({make_shared<KSValue>(value, type)});
                    break;
                }
            }
            break;
            case KSType::DICTIONARY:
            {
                KSDictionary dict;
                for (auto&& item : getStruct().variables) {
                    dict[std::hash<string>()(item.first) ^ typeHashBits(KSType::STRING)] = item.second;
                }
            }
            }

        }
        type = newType;
    }

	bool KSArray::operator==(const KSArray& o) const {
		if (size() != o.size()) {
			return false;
		}
		if (type != o.type) {
			return false;
		}
		switch (type) {
		case KSType::INT:
		{
			auto& aarr = get<vector<KSInt>>(value);
			auto& barr = get<vector<KSInt>>(o.value);
			for (size_t i = 0; i < size(); ++i) {
				if (aarr[i] != barr[i]) {
					return false;
				}
			}
		}
		break;
		case KSType::FLOAT:
		{
			auto& aarr = get<vector<KSFloat>>(value);
			auto& barr = get<vector<KSFloat>>(o.value);
			for (size_t i = 0; i < size(); ++i) {
				if (aarr[i] != barr[i]) {
					return false;
				}
			}
		}
		break;
		case KSType::VEC3:
		{
			auto& aarr = get<vector<vec3>>(value);
			auto& barr = get<vector<vec3>>(o.value);
			for (size_t i = 0; i < size(); ++i) {
				if (aarr[i] != barr[i]) {
					return false;
				}
			}
		}
		break;
		case KSType::STRING:
		{
			auto& aarr = get<vector<string>>(value);
			auto& barr = get<vector<string>>(o.value);
			for (size_t i = 0; i < size(); ++i) {
				if (aarr[i] != barr[i]) {
					return false;
				}
			}
		}
		break;
		default:
			break;
		}
		return true;
	}

	// tokenizer special characters
	const string WhitespaceChars = " \t\n"s;
	const string GrammarChars = " \t\n,.(){}[];+-/*%<>=!&|\""s;
	const string MultiCharTokenStartChars = "+-/*<>=!&|"s;
	const string NumericChars = "0123456789."s;
    const string NumericStartChars = "0123456789.-"s;

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

	vector<string> KSTokenize(const string& input) {
		bool exitFromComment = false;
		vector<string> ret;
		if (input.empty()) return ret;
		size_t pos = 0;
		size_t lpos = 0;
		while ((pos = input.find_first_of(GrammarChars, lpos)) != string::npos) {
			size_t len = pos - lpos;
            if (input[pos] == '.' && pos + 1 < input.size() && contains(NumericChars, input[pos+1])) {
                pos = input.find_first_of(GrammarChars, pos + 1);
                ret.push_back(input.substr(lpos, pos - lpos));
                lpos = pos;
                continue;
            }
			if (len) {
				ret.push_back(input.substr(lpos, pos - lpos));
				lpos = pos;
			} else {
				if (input[pos] == '\"' && pos > 0 && input[pos-1] != '\\') {
					auto testpos = lpos + 1;
					auto unescaped = "\""s;
					bool loop = true;
					while (loop) {						
						pos = input.find('\"', testpos);
						if (pos == string::npos) {
							throw runtime_error(stringformat("Quote mismatch at %s", 
                                input.substr(lpos, input.size() - lpos).c_str()).c_str());
						}
						loop = (input[pos - 1] == '\\');
						unescaped += input.substr(testpos, ++pos - testpos);
						testpos = pos;
						
						if (loop) {
							unescaped.pop_back();
							unescaped.pop_back();
							unescaped += '\"';
						}
					}
					ret.push_back(unescaped);
					lpos = pos;
					continue;
				}
			}
            if (input[pos] == '-' && contains(NumericChars, input[pos + 1]) 
                && (ret.size() == 0 || contains(MultiCharTokenStartChars, ret.back().back()))) {
                pos = input.find_first_of(GrammarChars, pos + 1);
                if (input[pos] == '.' && pos + 1 < input.size() && contains(NumericChars, input[pos + 1])) {
                    pos = input.find_first_of(GrammarChars, pos + 1);
                }
                ret.push_back(input.substr(lpos, pos - lpos));
                lpos = pos;
            } else if (!contains(WhitespaceChars, input[pos])) {
				auto stride = 1;
				if (contains(MultiCharTokenStartChars, input[pos]) && contains(MultiCharTokenStartChars, input[pos + 1])) {
					if (input[pos] == '/' && input[pos + 1] == '/') {
						exitFromComment = true;
						break;
					}
					++stride;
				}
				ret.push_back(input.substr(lpos, stride));
				lpos += stride;
			} else {
				++lpos;
			}
		}
		if (!exitFromComment && lpos < input.length()) {
			ret.push_back(input.substr(lpos, input.length()));
		}
		return ret;
	}

	// functions for figuring out the type of token

	bool isStringLiteral(const string& test) {
		return (test.size() > 1 && test[0] == '"');
	}

	bool isFloatLiteral(const string& test) {
		return (test.size() > 1 && contains(test, '.'));
	}

	bool isVarOrFuncToken(const string& test) {
		return (test.size() > 0 && !contains(NumericStartChars, test[0]));
	}

	bool isMathOperator(const string& test) {
		if (test.size() == 1) {
			return contains("+-*/%<>=!"s, test[0]);
		}
		if (test.size() == 2) {
			return contains("=+-&|"s, test[1]) && contains("<>=!+-/*&|"s, test[0]);
		}
		return false;
	}

    bool isMemberCall(const string& test) {
        if (test.size() == 1) {
            return '.' == test[0];
        }
        return false;
    }

	// scope control lets you have object lifetimes
	void KataScriptInterpreter::newScope(const string& name) {
		// if the scope exists we just use it as is
		auto iter = currentScope->scopes.find(name);
		if (iter != currentScope->scopes.end()) {
			currentScope = iter->second;
		} else {
			auto parent = currentScope;
			auto& newScope = currentScope->scopes[name];
			newScope = make_shared<KSScope>(name, parent);
			currentScope = newScope;
		}
	}

    KSScopeRef KataScriptInterpreter::resolveScope(const string& name) {
        // if the scope exists we just use it as is
        auto scope = currentScope;
        unordered_map<string, KSScopeRef>::iterator iter;
        while (scope->scopes.end() == (iter = scope->scopes.find(name)) && scope->parent) {
            scope = scope->parent;
        } 
        if (scope->scopes.end() != iter) {
            return iter->second;
        }
        throw runtime_error("Cannot resolve non-existant scope");
    }

	void KataScriptInterpreter::closeCurrentScope() {
		if (currentScope->parent) {
            if (currentScope->structScope) {
                currentScope = currentScope->parent;
            } else {
                auto name = currentScope->name;
                currentScope->functions.clear();
                currentScope->variables.clear();
                currentScope->scopes.clear();
                currentScope = currentScope->parent;
                currentScope->scopes.erase(name);
            }
		}
	}

	// call function by name
	KSValueRef KataScriptInterpreter::callFunction(const string& name, const KSList& args) {
		return callFunction(resolveFunction(name), args);
	}

    void each(KSExpressionRef collection, function<void(KSExpressionRef)> func) {
        func(collection);
        for (auto&& ex : *collection) {
            each(ex, func);            
        }
    }

	// call function by reference
	KSValueRef KataScriptInterpreter::callFunction(const KSFunctionRef fnc, const KSList& args) {
		if (fnc->subexpressions.size()) {
            auto oldscope = currentScope;
            // get function scope
            newScope(fnc->name);
			auto limit = min(args.size(), fnc->argNames.size());
            vector<string> newVars;
			for (size_t i = 0; i < limit; ++i) {
                auto& ref = currentScope->variables[fnc->argNames[i]];
                if (ref == nullptr) {
                    newVars.push_back(fnc->argNames[i]);
                }
                ref = args[i];
			}
        
			KSValueRef returnVal = nullptr;
			for (auto&& sub : fnc->subexpressions) {
				if (sub->type == KSExpressionType::RETURN) {
					returnVal = getValue(sub);
					break;
				} else{
					auto result = *sub;
                    result.consolidate(this);
                    if (result.type == KSExpressionType::RETURN) {
                        returnVal = get<KSValueRef>(result.expression);
                        break;
                    }
				}
			}

            if (fnc->type == KSFunctionType::CONSTRUCTOR) {
                for (auto&& vr : newVars) {
                    currentScope->variables.erase(vr);
                }
                returnVal = make_shared<KSValue>(KSStruct(*currentScope));
            }

            closeCurrentScope();
            currentScope = oldscope;
			return returnVal ? returnVal : make_shared<KSValue>();
		} else if (fnc->lambda) {
			return fnc->lambda(args);
        } else {
            //empty func
            return make_shared<KSValue>();
        }
	}

	KSFunctionRef& KataScriptInterpreter::newFunction(
        const string& name,
        const vector<string>& argNames,
        const vector<KSExpressionRef>& body
    ) {
        bool isConstructor = currentScope->structScope && currentScope->name == name && currentScope->parent;
        auto fnScope = isConstructor ? currentScope->parent : currentScope;
		auto& ref = fnScope->functions[name];
		ref = make_shared<KSFunction>(name, argNames, body);
        ref->type = isConstructor 
            ? KSFunctionType::CONSTRUCTOR 
            : currentScope->structScope 
                ? KSFunctionType::MEMBER 
                : KSFunctionType::FREE;
		auto& funcvar = resolveVariable(name, fnScope);
		funcvar->type = KSType::FUNCTION;
		funcvar->value = ref;
		return ref;
	}

	KSFunctionRef& KataScriptInterpreter::newFunction(const string& name, const KSLambda& lam) {
		auto& ref = currentScope->functions[name];
		ref = make_shared<KSFunction>(name, lam);
		auto& funcvar = resolveVariable(name);
		funcvar->type = KSType::FUNCTION;
		funcvar->value = ref;
		return ref;
	}

	KSFunctionRef& KataScriptInterpreter::newLibraryFunction(
        const string& name,
        const KSLambda& lam,
        KSScopeRef scope) {
		auto oldScope = currentScope;
		currentScope = scope;
		auto& ref = newFunction(name, lam);
		currentScope = oldScope;
		return ref;
	}

	// name resolution for variables
	KSValueRef& KataScriptInterpreter::resolveVariable(const string& name, KSScopeRef scope) {
        if (currentStruct) {
            auto iter = currentStruct->variables.find(name);
            if (iter != currentStruct->variables.end()) {
                return iter->second;
            }
        }
		if (!scope) {
			scope = currentScope;
		}
		auto initialScope = scope;
		while (scope) {            
			auto iter = scope->variables.find(name);
			if (iter != scope->variables.end()) {
				return iter->second;
			} else {
                scope = scope->parent;
			}
		}
		if (!scope) {
			for (auto m : modules) {
				auto iter = m->variables.find(name);
				if (iter != m->variables.end()) {
					return iter->second;
				}
			}
		}
		auto& varr = initialScope->variables[name];
		varr = make_shared<KSValue>();
		return varr;
	}

	// name lookup for callfunction api method
	KSFunctionRef KataScriptInterpreter::resolveFunction(const string& name) {
		auto scope = currentScope;
		while (scope) {
			auto iter = scope->functions.find(name);
			if (iter != scope->functions.end()) {
				return iter->second;
			} else {
				scope = scope->parent;
			}
		}
		return make_shared<KSFunction>(name);
	}

	// recursively build an expression tree from a list of tokens
	KSExpressionRef KataScriptInterpreter::getExpression(const vector<string>& strings) {
		KSExpressionRef root = nullptr;
		size_t i = 0;
		while (i < strings.size()) {
			if (isMathOperator(strings[i])) {
				auto prev = root;
				root = make_shared<KSExpression>(resolveVariable(strings[i], modules[0]));
				auto curr = prev;
				if (curr) {
					// find operations of lesser precedence
					if (curr->type == KSExpressionType::FUNCTIONCALL) {
						auto curfunc = get<KSFunctionExpression>(curr->expression).function->getFunction();
						auto newfunc = get<KSFunctionExpression>(root->expression).function->getFunction();
						if (curfunc && (int)curfunc->opPrecedence < (int)newfunc->opPrecedence) {
							while (get<KSFunctionExpression>(curr->expression).subexpressions.back()->type == KSExpressionType::FUNCTIONCALL) {
								curfunc = get<KSFunctionExpression>(get<KSFunctionExpression>(curr->expression).subexpressions.back()->expression).function->getFunction();
								if (curfunc && (int)curfunc->opPrecedence < (int)newfunc->opPrecedence) {
									curr = get<KSFunctionExpression>(curr->expression).subexpressions.back();
								} else {
									break;
								}
							}
							// swap values around to correct the otherwise incorect order of operations
                            get<KSFunctionExpression>(root->expression).subexpressions.push_back(get<KSFunctionExpression>(curr->expression).subexpressions.back());
                            get<KSFunctionExpression>(curr->expression).subexpressions.pop_back();
							vector<string> minisub = { strings[++i] };
							auto j = i + 1;
							string checkstr;
							if (strings[i] == "[" || strings[i] == "(") {
								checkstr = strings[i];
							} 
							if (checkstr.size() == 0 && (strings.size() > j && (strings[j] == "[" || strings[j] == "("))) {
								checkstr = strings[++i];
								minisub.push_back(strings[i]);
							}
							if (checkstr.size()) {
								auto endstr = checkstr == "[" ? "]"s : ")"s;
								int nestLayers = 1;
								while (nestLayers > 0 && ++i < strings.size()) {
									if (strings[i] == endstr) {
										--nestLayers;
									} else if (strings[i] == checkstr) {
										++nestLayers;		
									} 
									minisub.push_back(strings[i]);
                                    if (nestLayers == 0) {
                                        if (i+1 < strings.size() && (strings[i + 1] == "[" || strings[i + 1] == "(")) {
                                            ++nestLayers;
                                            checkstr = strings[++i];
                                            endstr = checkstr == "[" ? "]"s : ")"s;
                                        }
                                    }
								}
							}
                            get<KSFunctionExpression>(root->expression).subexpressions.push_back(getExpression(move(minisub)));
                            get<KSFunctionExpression>(curr->expression).subexpressions.push_back(root);
							root = prev;
						} else {
                            get<KSFunctionExpression>(root->expression).subexpressions.push_back(curr);
						}
					} else {
                        get<KSFunctionExpression>(root->expression).subexpressions.push_back(curr);
					}
				}
			} else if (isStringLiteral(strings[i])) {
				// trim quotation marks
				auto val = strings[i].substr(1, strings[i].size() - 2);
                auto newExpr = make_shared<KSExpression>(resolveVariable("copy", modules[0]));
                get<KSFunctionExpression>(newExpr->expression).subexpressions.push_back(
                    make_shared<KSExpression>(make_shared<KSValue>(val), nullptr)
                );
				if (root) {
                    get<KSFunctionExpression>(root->expression).subexpressions.push_back(newExpr);
				} else {
					root = newExpr;
				}
            } else if (strings[i] == "(" || strings[i] == "[" || isVarOrFuncToken(strings[i])) {
                if (strings[i] == "(" || i + 2 < strings.size() && strings[i + 1] == "(") {
                    // function
                    KSExpressionRef cur = nullptr;
                    if (strings[i] == "(") {
                        if (root) {
                            if (root->type == KSExpressionType::FUNCTIONCALL
                                && (get<KSFunctionExpression>(root->expression).function->getFunction()->opPrecedence == KSOperatorPrecedence::func)) {
                                cur = make_shared<KSExpression>(make_shared<KSValue>());
                                get<KSFunctionExpression>(cur->expression).subexpressions.push_back(root);
                                root = cur;
                            } else {
                                get<KSFunctionExpression>(root->expression).subexpressions.push_back(make_shared<KSExpression>(resolveVariable("identity", modules[0])));
                                cur = get<KSFunctionExpression>(root->expression).subexpressions.back();
                            }
                        } else {
                            root = make_shared<KSExpression>(resolveVariable("identity", modules[0]));
                            cur = root;
                        }
                    } else {
                        auto var = resolveVariable(strings[i]);
                        if (var->type != KSType::NONE && var->type != KSType::FUNCTION) {
                            throw runtime_error(stringformat("Attempted to call non-function value as function: `%s %s`", getTypeName(var->type).c_str(), var->getPrintString().c_str()).c_str());
                        }
                        if (root) {
                            get<KSFunctionExpression>(root->expression).subexpressions.push_back(make_shared<KSExpression>(var));
                            cur = get<KSFunctionExpression>(root->expression).subexpressions.back();
                        } else {
                            root = make_shared<KSExpression>(var);
                            cur = root;
                        }
                        ++i;
                    }
                    vector<string> minisub;
                    int nestLayers = 1;
                    while (nestLayers > 0 && ++i < strings.size()) {
                        if (nestLayers == 1 && strings[i] == ",") {
                            if (minisub.size()) {
                                get<KSFunctionExpression>(cur->expression).subexpressions.push_back(getExpression(move(minisub)));
                                minisub.clear();
                            }
                        } else if (strings[i] == ")" || strings[i] == "]") {
                            if (--nestLayers > 0) {
                                minisub.push_back(strings[i]);
                            } else {
                                if (minisub.size()) {
                                    get<KSFunctionExpression>(cur->expression).subexpressions.push_back(getExpression(move(minisub)));
                                    minisub.clear();
                                }
                            }
                        } else if (strings[i] == "(" || strings[i] == "[" || !(strings[i].size() == 1 && contains("+-*%/"s, strings[i][0])) && i + 2 < strings.size() && strings[i + 1] == "(") {
                            ++nestLayers;
                            if (strings[i] == "(") {
                                minisub.push_back(strings[i]);
                            } else {
                                minisub.push_back(strings[i]);
                                ++i;
                                if (strings[i] == ")" || strings[i] == "]") {
                                    --nestLayers;
                                }
                                if (nestLayers > 0) {
                                    minisub.push_back(strings[i]);
                                }
                            }
                        } else {
                            minisub.push_back(strings[i]);
                        }
                    }
                } else if (strings[i] == "[" || i + 2 < strings.size() && strings[i + 1] == "[") {
                    // list
                    bool indexOfIndex = i > 0 && (strings[i - 1] == "]" || strings[i - 1] == ")" || strings[i - 1].back() == '\"');
                    KSExpressionRef cur = nullptr;
                    if (!indexOfIndex && strings[i] == "[") {
                        // list literal / collection literal
                        if (root) {
                            get<KSFunctionExpression>(root->expression).subexpressions.push_back(
                                make_shared<KSExpression>(make_shared<KSValue>(KSList()), nullptr));
                            cur = get<KSFunctionExpression>(root->expression).subexpressions.back();
                        } else {
                            root = make_shared<KSExpression>(make_shared<KSValue>(KSList()), nullptr);
                            cur = root;
                        }
                        vector<string> minisub;
                        int nestLayers = 1;
                        while (nestLayers > 0 && ++i < strings.size()) {
                            if (nestLayers == 1 && strings[i] == ",") {
                                if (minisub.size()) {
                                    auto val = *getValue(move(minisub));
                                    get<KSValueRef>(cur->expression)->getList().push_back(make_shared<KSValue>(val.value, val.type));
                                    minisub.clear();
                                }
                            } else if (strings[i] == "]" || strings[i] == ")") {
                                if (--nestLayers > 0) {
                                    minisub.push_back(strings[i]);
                                } else {
                                    if (minisub.size()) {
                                        auto val = *getValue(move(minisub));
                                        get<KSValueRef>(cur->expression)->getList().push_back(make_shared<KSValue>(val.value, val.type));
                                        minisub.clear();
                                    }
                                }
                            } else if (strings[i] == "[" || strings[i] == "(") {
                                ++nestLayers;
                                minisub.push_back(strings[i]);
                            } else {
                                minisub.push_back(strings[i]);
                            }
                        }
                        auto& list = get<KSValueRef>(cur->expression)->getList();
                        if (list.size()) {
                            bool canBeArray = true;
                            auto type = list[0]->type;
                            for (auto& val : list) {
                                if (val->type == KSType::NONE || val->type != type || (int)val->type >= (int)KSType::ARRAY) {
                                    canBeArray = false;
                                    break;
                                }
                            }
                            if (canBeArray) {
                                get<KSValueRef>(cur->expression)->hardconvert(KSType::ARRAY);
                            }
                        }
                    } else {
                        // list access
                        auto indexexpr = make_shared<KSExpression>(resolveVariable("listindex", modules[0]));
                        if (indexOfIndex) {
                            cur = root;
                            auto parent = root;
                            while (cur->type == KSExpressionType::FUNCTIONCALL && get<KSFunctionExpression>(cur->expression).function->getFunction()->opPrecedence != KSOperatorPrecedence::func) {
                                parent = cur;
                                cur = get<KSFunctionExpression>(cur->expression).subexpressions.back();
                            }
                            get<KSFunctionExpression>(indexexpr->expression).subexpressions.push_back(cur);
                            if (cur == root) {
                                root = indexexpr;
                                cur = indexexpr;
                            } else {
                                get<KSFunctionExpression>(parent->expression).subexpressions.pop_back();
                                get<KSFunctionExpression>(parent->expression).subexpressions.push_back(indexexpr);
                                cur = get<KSFunctionExpression>(parent->expression).subexpressions.back();
                            }
                        } else {
                            if (root) {
                                get<KSFunctionExpression>(root->expression).subexpressions.push_back(indexexpr);
                                cur = get<KSFunctionExpression>(root->expression).subexpressions.back();
                            } else {
                                root = indexexpr;
                                cur = root;
                            }
                            get<KSFunctionExpression>(cur->expression).subexpressions.push_back(make_shared<KSExpression>(KSResolveVar(strings[i])));
                            ++i;
                        }

                        vector<string> minisub;
                        int nestLayers = 1;
                        while (nestLayers > 0 && ++i < strings.size()) {
                            if (strings[i] == "]") {
                                if (--nestLayers > 0) {
                                    minisub.push_back(strings[i]);
                                } else {
                                    if (minisub.size()) {
                                        get<KSFunctionExpression>(cur->expression).subexpressions.push_back(getExpression(move(minisub)));
                                        minisub.clear();
                                    }
                                }
                            } else if (strings[i] == "[") {
                                ++nestLayers;
                                minisub.push_back(strings[i]);
                            } else {
                                minisub.push_back(strings[i]);
                            }
                        }
                    }
                } else {
                    // variable
                    KSExpressionRef newExpr;
                    if (strings[i] == "true") {
                        newExpr = make_shared<KSExpression>(make_shared<KSValue>(KSInt(1)), nullptr);
                    } else if (strings[i] == "false") {
                        newExpr = make_shared<KSExpression>(make_shared<KSValue>(KSInt(0)), nullptr);
                    } else if (strings[i] == "null") {
                        newExpr = make_shared<KSExpression>(make_shared<KSValue>(), nullptr);
                    } else {
                        newExpr = make_shared<KSExpression>(KSResolveVar(strings[i]), nullptr);
                    }

                    if (root) {
                        get<KSFunctionExpression>(root->expression).subexpressions.push_back(newExpr);
                    } else {
                        root = newExpr;
                    }
                }
            } else if (isMemberCall(strings[i])) {
                // member var
                bool isfunc = strings.size() > i + 2 && strings[i + 2] == "("s;
                auto memberExpr = make_shared<KSExpression>(resolveVariable(isfunc?"structindex"s:"listindex"s, modules[0]));
                auto& membExpr = get<KSFunctionExpression>(memberExpr->expression);
                KSExpressionRef argsInsert;
                if (root->type == KSExpressionType::FUNCTIONCALL && get<KSFunctionExpression>(root->expression).subexpressions.size()) {
                    auto& rootExpr = get<KSFunctionExpression>(root->expression);
                    membExpr.subexpressions.push_back(
                        rootExpr.subexpressions.back());
                    rootExpr.subexpressions.pop_back();
                    membExpr.subexpressions.push_back(make_shared<KSExpression>(make_shared<KSValue>(strings[++i]), nullptr));
                    rootExpr.subexpressions.push_back(memberExpr);
                    argsInsert = rootExpr.subexpressions.back();
                } else {
                    membExpr.subexpressions.push_back(root);
                    membExpr.subexpressions.push_back(make_shared<KSExpression>(make_shared<KSValue>(strings[++i]), nullptr));
                    root = memberExpr;
                    argsInsert = root;
                }
                
                if (isfunc) {
                    bool addedArgs = false;
                    auto previ = i;
                    ++i;
                    vector<string> minisub;
                    int nestLayers = 1;
                    while (nestLayers > 0 && ++i < strings.size()) {
                        if (nestLayers == 1 && strings[i] == ",") {
                            if (minisub.size()) {
                                get<KSFunctionExpression>(argsInsert->expression).subexpressions.push_back(getExpression(move(minisub)));
                                minisub.clear();
                                addedArgs = true;
                            }
                        } else if (strings[i] == ")") {
                            if (--nestLayers > 0) {
                                minisub.push_back(strings[i]);
                            } else {
                                if (minisub.size()) {
                                    get<KSFunctionExpression>(argsInsert->expression).subexpressions.push_back(getExpression(move(minisub)));
                                    minisub.clear();
                                    addedArgs = true;
                                }
                            }
                        } else if ( strings[i] == "(") {
                            ++nestLayers;
                            minisub.push_back(strings[i]);
                        } else {
                            minisub.push_back(strings[i]);
                        }
                    }
                    if (!addedArgs) {
                        i = previ;
                    }
                }
			} else {
				// number
				auto val = fromChars(strings[i]);
                bool isFloat = contains(strings[i], '.');
                auto newExpr = make_shared<KSExpression>(resolveVariable("copy", modules[0]));
                get<KSFunctionExpression>(newExpr->expression).subexpressions.push_back(
                    make_shared<KSExpression>(KSValueRef(isFloat ? new KSValue((KSFloat)val) : new KSValue((KSInt)val)), nullptr)
                );
				if (root) {
                    get<KSFunctionExpression>(root->expression).subexpressions.push_back(newExpr);
				} else {
					root = newExpr;
				}
			}
			++i;
		}

		return root;
	}

    bool KSExpression::needsToReturn(KSExpressionRef exp, KSValueRef& returnVal, KataScriptInterpreter* i) {
        if (exp->type == KSExpressionType::RETURN) {
            returnVal = i->getValue(exp);
            return true;
        } else {
            auto result = *exp;
            result.consolidate(i);
            if (result.type == KSExpressionType::RETURN) {
                returnVal = get<KSValueRef>(result.expression);
                return true;
            }
        }
        return false;
    }

	// evaluate an expression
	void KSExpression::consolidate(KataScriptInterpreter* i) {
		switch (type) {
        case KSExpressionType::DEFINEVAR:
        {
            auto& def = get<KSDefineVar>(expression);
            auto& varr = i->currentScope->variables[def.name];
            if (def.defineExpression) {
                auto val = i->getValue(def.defineExpression);
                varr = make_shared<KSValue>(val->value, val->type);
            } else {
                varr = make_shared<KSValue>();
            }
            expression = varr;
            type = KSExpressionType::VALUE;
        }
            break;
        case KSExpressionType::RESOLVEVAR:
            expression = i->resolveVariable(get<KSResolveVar>(expression).name);
            type = KSExpressionType::VALUE;
            break;
		case KSExpressionType::RETURN:
            expression = i->getValue(get<KSReturn>(expression).expression);
			type = KSExpressionType::VALUE;
			break;
		case KSExpressionType::FUNCTIONCALL:
		{
			KSList args;
            auto& funcExpr = get<KSFunctionExpression>(expression);
			for (auto&& sub : funcExpr.subexpressions) {
				sub->consolidate(i);
				args.push_back(get<KSValueRef>(sub->expression));
			}
            KSStruct* tempStruct = i->currentStruct;
			if (funcExpr.function->type == KSType::NONE) {
				if (args.size() && args[0]->type == KSType::FUNCTION) {
                    funcExpr.function = args[0];
					args.erase(args.begin());
                    // if we didn't convert into a function after that...
                    if (funcExpr.function->type == KSType::NONE) {
                        throw runtime_error("Unable to call non-existant function");
                    }
                } else if (args.size() && args[0]->type == KSType::LIST) {
                    auto& list = args[0]->getList();
                    auto& struc = list[0];
                    auto& func = list[1];
                    if (struc->type != KSType::STRUCT) {
                        throw runtime_error("Unable to call member of non-struct");
                    }
                    if (func->type != KSType::FUNCTION) {
                        throw runtime_error("Unable to call non-function");
                    }
                    i->currentStruct = &struc->getStruct();
                    funcExpr.function->type = KSType::FUNCTION;
                    funcExpr.function->value = func->getFunction();
                    args.erase(args.begin());
                    for (auto&& sub : funcExpr.subexpressions) {
                        sub->consolidate(i);
                        args.push_back(get<KSValueRef>(sub->expression));
                    }
                } else if (args.size()) {
                    expression = args[0];
                    i->currentStruct = tempStruct;
                    type = KSExpressionType::VALUE;
                    break;
                } else {
                    throw runtime_error("Unable to call non-existant function");
                }                
			}
            expression = i->callFunction(get<KSFunctionExpression>(expression).function->getFunction(), args);
            i->currentStruct = tempStruct;
			type = KSExpressionType::VALUE;
		}
			break;
		case KSExpressionType::LOOP:
		{
			i->newScope("loop");
			if (get<KSLoop>(expression).initExpression) {
				i->getValue(get<KSLoop>(expression).initExpression);
			}
            KSValueRef returnVal = nullptr;
			while (returnVal == nullptr && i->getValue(get<KSLoop>(expression).testExpression)->getBool()) {
				for (auto&& exp : get<KSLoop>(expression).subexpressions) {
                    if (needsToReturn(exp, returnVal, i)) {
                        break;
                    }
				}
				if (returnVal == nullptr && get<KSLoop>(expression).iterateExpression) {
					i->getValue(get<KSLoop>(expression).iterateExpression);
				}
			}
            if (returnVal) {
                expression = returnVal;
                type = KSExpressionType::RETURN;
            } else {
                expression = make_shared<KSValue>();
                type = KSExpressionType::VALUE;
            }
			i->closeCurrentScope();
		}
			break;
		case KSExpressionType::FOREACH:
		{
			i->newScope("loop");
			auto& varr = i->resolveVariable(get<KSForeach>(expression).iterateName);
            get<KSForeach>(expression).listExpression->consolidate(i);
			auto& list = get<KSValueRef>(get<KSForeach>(expression).listExpression->expression);
            KSValueRef returnVal = nullptr;
			if (list->type == KSType::LIST) {
				for (auto&& in : list->getList()) {
					*varr = *in;
					for (auto&& exp : get<KSForeach>(expression).subexpressions) {
                        if (needsToReturn(exp, returnVal, i)) {
                            break;
                        }
					}
				}
			} else if (list->type == KSType::ARRAY) {
				auto& arr = list->getArray();
				switch (arr.type) {
				case KSType::INT:
                {
                    auto vec = list->getStdVector<KSInt>();
                    for (auto&& in : vec) {
                        *varr = KSValue(in);
                        for (auto&& exp : get<KSForeach>(expression).subexpressions) {
                            if (needsToReturn(exp, returnVal, i)) {
                                break;
                            }
                        }
                    }
                }
					break;
				case KSType::FLOAT:
                {
                    auto vec = list->getStdVector<KSFloat>();
                    for (auto&& in : vec) {
                        *varr = KSValue(in);
                        for (auto&& exp : get<KSForeach>(expression).subexpressions) {
                            if (needsToReturn(exp, returnVal, i)) {
                                break;
                            }
                        }
                    }
                }
					break;
				case KSType::VEC3:
                {
                    auto vec = list->getStdVector<vec3>();
                    for (auto&& in : vec) {
                        *varr = KSValue(in);
                        for (auto&& exp : get<KSForeach>(expression).subexpressions) {
                            if (needsToReturn(exp, returnVal, i)) {
                                break;
                            }
                        }
                    }
                }
					break;
				case KSType::STRING:
                {
                    auto vec = list->getStdVector<string>();
                    for (auto&& in : vec) {
                        *varr = KSValue(in);
                        for (auto&& exp : get<KSForeach>(expression).subexpressions) {
                            if (needsToReturn(exp, returnVal, i)) {
                                break;
                            }
                        }
                    }
                }
					break;
				default:
					break;
				}
			}
            if (returnVal) {
                expression = returnVal;
                type = KSExpressionType::RETURN;
            } else {
                expression = make_shared<KSValue>();
                type = KSExpressionType::VALUE;
            }
			i->closeCurrentScope();
		}
		break;
		case KSExpressionType::IFELSE:
		{
            KSValueRef returnVal = nullptr;
			for (auto& express : get<KSIfElse>(expression)) {
				if (!express.testExpression || i->getValue(express.testExpression)->getBool()) {
					i->newScope("ifelse");
					for (auto exp : express.subexpressions) {
                        if (needsToReturn(exp, returnVal, i)) {
                            break;
                        }
					}
					i->closeCurrentScope();
					break;
				}
			}
            if (returnVal) {
                expression = returnVal;
                type = KSExpressionType::RETURN;
            } else {
                expression = make_shared<KSValue>();
                type = KSExpressionType::VALUE;
            }
		}
			break;
		default:
			break;
		}
	}

	// evaluate an expression from tokens
	KSValueRef KataScriptInterpreter::getValue(const vector<string>& strings) {
		auto expr = getExpression(strings);
		expr->consolidate(this);
		return get<KSValueRef>(expr->expression);
	}

	// evaluate an expression from expressionRef
	KSValueRef KataScriptInterpreter::getValue(KSExpressionRef exp) {
		// copy the expression so that we don't lose it when we consolidate
		auto expr = *exp;
		expr.consolidate(this);
		return get<KSValueRef>(expr.expression);
	}

	// general purpose clear to reset state machine for next statement
	void KataScriptInterpreter::clearParseStacks() {
		parseState = KSParseState::beginExpression;
		parseStrings.clear();
		outerNestLayer = 0;
	}

	// since the 'else' block in  an if/elfe is technically in a different scope
	// ifelse espressions are not closed immediately and instead left dangling
	// until the next expression is anything other than an 'else' or the else is unconditional
	void KataScriptInterpreter::closeDanglingIfExpression() {
		if (currentExpression && currentExpression->type == KSExpressionType::IFELSE) {
			if (currentExpression->parent) {
				currentExpression = currentExpression->parent;
			} else {
				getValue(currentExpression);
				currentExpression = nullptr;
			}
		}
	}

	bool KataScriptInterpreter::closeCurrentExpression() {
		if (currentExpression) {
			if (currentExpression->type != KSExpressionType::IFELSE) {
				if (currentExpression->parent) {
					currentExpression = currentExpression->parent;
				} else {
					if (currentExpression->type != KSExpressionType::FUNCTIONDEF) {
						getValue(currentExpression);
					}
					currentExpression = nullptr;
				}
				return true;
			}
		}
		return false;
	}

	bool lastStatementClosedScope = false;
    bool lastStatementWasElse = false;
    bool lastTokenEndCurlBraket = false;
    KSParseState prevState = KSParseState::beginExpression;
	// parse one token at a time, uses the state machine
	void KataScriptInterpreter::parse(const string& token) {
        auto tempState = parseState;
		switch (parseState) {
		case KSParseState::beginExpression:
		{
			bool wasElse = false;
			bool closeScope = false;
			bool closedExpr = false;
            bool isEndCurlBracket = false;
			if (token == "func") {
				parseState = KSParseState::defineFunc;
			} else if (token == "var") {
                parseState = KSParseState::defineVar;
            } else if (token == "for" || token == "while") {
				parseState = KSParseState::loopCall;
				if (currentExpression) {
					auto newexpr = make_shared<KSExpression>(KSLoop(), currentExpression);
					currentExpression->push_back(newexpr);
					currentExpression = newexpr;
				} else {
					currentExpression = make_shared<KSExpression>(KSLoop());
				}
			} else if (token == "foreach") {
				parseState = KSParseState::forEach;
				if (currentExpression) {
					auto newexpr = make_shared<KSExpression>(KSForeach(), currentExpression);
					currentExpression->push_back(newexpr);
					currentExpression = newexpr;
				} else {
					currentExpression = make_shared<KSExpression>(KSForeach());
				}
			} else if (token == "if") {
				parseState = KSParseState::ifCall;
				if (currentExpression) {
					auto newexpr = make_shared<KSExpression>(KSIfElse(), currentExpression);
					currentExpression->push_back(newexpr);
					currentExpression = newexpr;
				} else {
					currentExpression = make_shared<KSExpression>(KSIfElse());
				}
            } else if (token == "else") {
                parseState = KSParseState::expectIfEnd;
                wasElse = true;
            } else if (token == "struct") {
                parseState = KSParseState::defineStruct;
			} else if (token == "{") {
				bool skipNewScope = prevState == KSParseState::funcArgs || prevState == KSParseState::defineStruct;
				if (!skipNewScope) {
					newScope("anon"s);
				}
				clearParseStacks();
			} else if (token == "}") {
                wasElse = !currentExpression || currentExpression->type != KSExpressionType::IFELSE;
                bool wasFreefunc = !(currentExpression && currentExpression->type == KSExpressionType::FUNCTIONDEF 
                    && get<KSFunctionExpression>(currentExpression->expression).function->getFunction()->type != KSFunctionType::FREE);
				closedExpr = closeCurrentExpression();
                if (wasFreefunc) {
                    closeCurrentScope();
                }
				closeScope = true;
                isEndCurlBracket = true;
			} else if (token == "return") {
				parseState = KSParseState::returnLine;
			} else if (token == ";") {
				clearParseStacks();
			} else {
				parseState = KSParseState::readLine;
				parseStrings.push_back(token);
			}
			if (!closedExpr && (closeScope && lastStatementClosedScope || (!lastStatementWasElse && !wasElse && lastTokenEndCurlBraket))) {
                bool wasIfExpr = currentExpression && currentExpression->type == KSExpressionType::IFELSE;
				closeDanglingIfExpression();
                if (closeScope && wasIfExpr && currentExpression->type != KSExpressionType::IFELSE) {
                    closeCurrentExpression();
                    closeScope = false;
                }
			}
            lastStatementClosedScope = closeScope;
            lastTokenEndCurlBraket = isEndCurlBracket;
            lastStatementWasElse = wasElse;
		}
		break;
		case KSParseState::loopCall:
			if (token == ")") {
				if (--outerNestLayer <= 0) {
					vector<vector<string>> exprs = {};
					exprs.push_back({});
					for (auto&& str : parseStrings) {
						if (str == ";") {
							exprs.push_back({});
						} else {
							exprs.back().push_back(move(str));
						}
					}
                    auto& loop = get<KSLoop>(currentExpression->expression);
					switch (exprs.size()) {
					case 1:
                        loop.testExpression = getExpression(move(exprs[0]));
						break;
					case 2:
						loop.testExpression = getExpression(move(exprs[0]));
						loop.iterateExpression = getExpression(move(exprs[1]));
						break;
					case 3: 
                    {
                        auto name = exprs[0].front();
                        exprs[0].erase(exprs[0].begin(), exprs[0].begin()+1);
                        loop.initExpression = make_shared<KSExpression>(KSDefineVar(name, getExpression(move(exprs[0]))));
                        loop.testExpression = getExpression(move(exprs[1]));
                        loop.iterateExpression = getExpression(move(exprs[2]));
                    }
						break;
					default:
						break;
					}

					clearParseStacks();
					outerNestLayer = 0;
				} else {
					parseStrings.push_back(token);
				}
			} else if (token == "(") {
				if (++outerNestLayer > 1) {
					parseStrings.push_back(token);
				}
			} else {
				parseStrings.push_back(token);
			}
			break;
		case KSParseState::forEach:
			if (token == ")") {
				if (--outerNestLayer <= 0) {
					vector<vector<string>> exprs = {};
					exprs.push_back({});
					for (auto&& str : parseStrings) {
						if (str == ";") {
							exprs.push_back({});
						} else {
							exprs.back().push_back(move(str));
						}
					}
					if (exprs.size() != 2) {
						clearParseStacks();
						throw runtime_error(stringformat("Syntax error, `foreach` requires 2 statements, %i statements supplied instead", (int)exprs.size()).c_str());
					}

					resolveVariable(exprs[0][0]);
					get<KSForeach>(currentExpression->expression).iterateName = exprs[0][0];
					get<KSForeach>(currentExpression->expression).listExpression = getExpression(exprs[1]);

					clearParseStacks();
					outerNestLayer = 0;
				} else {
					parseStrings.push_back(token);
				}
			} else if (token == "(") {
				if (++outerNestLayer > 1) {
					parseStrings.push_back(token);
				}
			} else {
				parseStrings.push_back(token);
			}
			break;
		case KSParseState::ifCall:			
			if (token == ")") {
				if (--outerNestLayer <= 0) {
					currentExpression->push_back(KSIf());
                    get<KSIfElse>(currentExpression->expression).back().testExpression = getExpression(move(parseStrings));
					clearParseStacks();
				} else {
					parseStrings.push_back(token);
				}
			} else if (token == "(") {
				if (++outerNestLayer > 1) {
					parseStrings.push_back(token);
				}
			} else {
				parseStrings.push_back(token);
			}
			break;
		case KSParseState::readLine:
            if (token == ";") {
                auto line = move(parseStrings);
                clearParseStacks();
                // we clear before evaluating lines so any exceptions can clear the offending code
				if (!currentExpression) {
					getValue(line);
				} else {
					currentExpression->push_back(getExpression(line));
				}
                
			} else {
				parseStrings.push_back(token);
			}
			break;
		case KSParseState::returnLine:
			if (token == ";") {
				if (currentExpression) {
					currentExpression->push_back(make_shared<KSExpression>(KSReturn(getExpression(move(parseStrings)))));
				}
				clearParseStacks();				
			} else {
				parseStrings.push_back(token);
			}
			break;
		case KSParseState::expectIfEnd:
			if (token == "if") {
				clearParseStacks();
				parseState = KSParseState::ifCall;
			} else if (token == "{") {
				newScope("anon"s);
				currentExpression->push_back(KSIf());
				clearParseStacks();
			} else {
				clearParseStacks();
				throw runtime_error(stringformat("Malformed Syntax: Incorrect token `%s` following `else` keyword",
					token.c_str()).c_str());
			}
			break;
        case KSParseState::defineVar:
            if (token == ";") {
                if (parseStrings.size() == 0) {
                    throw runtime_error(stringformat("Malformed Syntax: `var` keyword must be followed by user supplied name").c_str());
                }
                auto name = parseStrings.front();
                KSExpressionRef defineExpr = nullptr;
                if (parseStrings.size() > 2) {
                    parseStrings.erase(parseStrings.begin());
                    parseStrings.erase(parseStrings.begin());
                    defineExpr = getExpression(move(parseStrings));
                }
                if (currentExpression) {
                    currentExpression->push_back(make_shared<KSExpression>(KSDefineVar(name, defineExpr)));
                } else {
                    getValue(make_shared<KSExpression>(KSDefineVar(name, defineExpr)));
                }
                clearParseStacks();
            } else {
                parseStrings.push_back(token);
            }
            break;
        case KSParseState::defineStruct:
            newScope(token);
            currentScope->structScope = true;
            clearParseStacks();
            break;
        case KSParseState::defineFunc:
            parseStrings.push_back(token);
            parseState = KSParseState::funcArgs;
            break;
		case KSParseState::funcArgs:
			if (token == "(" || token == ",") {
				// eat these tokens
			} else if (token == ")") {
				auto fncName = move(parseStrings.front());
				parseStrings.erase(parseStrings.begin());
				auto& newfunc = newFunction(fncName, parseStrings, {});
				if (currentExpression) {
					auto newexpr = make_shared<KSExpression>(newfunc, currentExpression);
					currentExpression->push_back(newexpr);
					currentExpression = newexpr;
				} else {
					currentExpression = make_shared<KSExpression>(newfunc, nullptr);
				}
				clearParseStacks();
			} else {
				parseStrings.push_back(token);
			}
			break;
		default:
			break;
		}

        prevState = tempState;
	}

	void KataScriptInterpreter::readLine(const string& text) {
		try {
			for (auto& token : KSTokenize(text)) {
				parse(move(token));
			}
		} catch (std::exception e) {
			printf("Error: %s\n", e.what());
            clearParseStacks();
            currentScope = globalScope;
            currentStruct = nullptr;
            currentExpression = nullptr;
		}
	}

	void KataScriptInterpreter::evaluate(const string& script) {
		for (auto& line : split(script, '\n')) {
			readLine(move(line));
		}
		parse(";"s); // close any dangling if-expressions that may exist
	}

	void KataScriptInterpreter::clearState() {
		clearParseStacks();
		globalScope = make_shared<KSScope>("global", nullptr);
		currentScope = globalScope;
		currentExpression = nullptr;
	}

	KataScriptInterpreter::KataScriptInterpreter() {
		// register compiled functions and standard library:
		modules.push_back(make_shared<KSScope>("StandardLib"s, nullptr));
		auto libscope = modules.back();

        // math operators
        {
            newLibraryFunction("=", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("=");
                }
                if (args.size() == 1) {
                    return args[0];
                }
                *args[0] = *args[1];
                return args[0];
                }, libscope);

            newLibraryFunction("+", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("+");
                }
                if (args.size() == 1) {
                    return args[0];
                }
                return make_shared<KSValue>(*args[0] + *args[1]);
                }, libscope);

            newLibraryFunction("-", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("-");
                }
                if (args.size() == 1) {
                    auto zero = KSValue(KSInt(0));
                    upconvert(*args[0], zero);
                    return make_shared<KSValue>(zero - *args[0]);
                }
                return make_shared<KSValue>(*args[0] - *args[1]);
                }, libscope);

            newLibraryFunction("*", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("*");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>();
                }
                return make_shared<KSValue>(*args[0] * *args[1]);
                }, libscope);

            newLibraryFunction("/", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("/");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>();
                }
                return make_shared<KSValue>(*args[0] / *args[1]);
                }, libscope);

            newLibraryFunction("%", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("%");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>();
                }
                return make_shared<KSValue>(*args[0] % *args[1]);
                }, libscope);

            newLibraryFunction("==", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("==");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(*args[0] == *args[1]));
                }, libscope);

            newLibraryFunction("!=", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("!=");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(*args[0] != *args[1]));
                }, libscope);

            newLibraryFunction("||", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("||");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(1));
                }
                return make_shared<KSValue>((KSInt)(*args[0] || *args[1]));
                }, libscope);

            newLibraryFunction("&&", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("&&");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(*args[0] && *args[1]));
                }, libscope);

            newLibraryFunction("++", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                auto t = KSValue(KSInt(1));
                *args[0] = *args[0] + t;
                return args[0];
                }, libscope);

            newLibraryFunction("--", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                auto t = KSValue(KSInt(1));
                *args[0] = *args[0] - t;
                return args[0];
                }, libscope);

            newLibraryFunction("+=", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                if (args.size() == 1) {
                    return args[0];
                }
                *args[0] += *args[1];
                return args[0];
                }, libscope);

            newLibraryFunction("-=", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                if (args.size() == 1) {
                    return args[0];
                }
                *args[0] -= *args[1];
                return args[0];
                }, libscope);

            newLibraryFunction("*=", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                if (args.size() == 1) {
                    return args[0];
                }
                *args[0] *= *args[1];
                return args[0];
                }, libscope);

            newLibraryFunction("/=", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                if (args.size() == 1) {
                    return args[0];
                }
                *args[0] /= *args[1];
                return args[0];
                }, libscope);

            newLibraryFunction(">", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable(">");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(*args[0] > *args[1]));
                }, libscope);

            newLibraryFunction("<", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("<");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(*args[0] < *args[1]));
                }, libscope);

            newLibraryFunction(">=", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable(">=");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(*args[0] >= *args[1]));
                }, libscope);

            newLibraryFunction("<=", [this](const KSList& args) {
                if (args.size() == 0) {
                    return resolveVariable("<=");
                }
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(*args[0] <= *args[1]));
                }, libscope);

            newLibraryFunction("!", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSInt(0));
                }
                return make_shared<KSValue>((KSInt)(!args[0]->getBool()));
                }, libscope);
        }

        // aliases
        {
            newLibraryFunction("identity", [](KSList args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                return args[0];
                }, libscope);

            newLibraryFunction("copy", [](KSList args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                return make_shared<KSValue>(args[0]->value, args[0]->type);
                }, libscope);

            newLibraryFunction("listindex", [](KSList args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                if (args.size() == 1) {
                    return args[0];
                }

                auto var = args[0];

                if (args[1]->type != KSType::INT) {
                    var->upconvert(KSType::DICTIONARY);
                }

                switch (var->type) {
                case KSType::ARRAY:
                {
                    auto ival = args[1]->getInt();
                    auto& arr = var->getArray();
                    if (ival < 0 || ival >= (KSInt)arr.size()) {
                        throw runtime_error(stringformat("Out of bounds array access index %lld, array length %lld",
                            ival, arr.size()).c_str());
                    } else {
                        switch (arr.type) {
                        case KSType::INT:
                            return make_shared<KSValue>(get<vector<KSInt>>(arr.value)[ival]);
                            break;
                        case KSType::FLOAT:
                            return make_shared<KSValue>(get<vector<KSFloat>>(arr.value)[ival]);
                            break;
                        case KSType::VEC3:
                            return make_shared<KSValue>(get<vector<vec3>>(arr.value)[ival]);
                            break;
                        case KSType::STRING:
                            return make_shared<KSValue>(get<vector<string>>(arr.value)[ival]);
                            break;
                        default:
                            throw runtime_error("Attempting to access array of illegal type");
                            break;
                        }
                    }
                }
                break;
                default:
                    var = make_shared<KSValue>(var->value, var->type);
                    var->upconvert(KSType::LIST);
                    [[fallthrough]];
                case KSType::LIST:
                {
                    auto ival = args[1]->getInt();

                    auto& list = var->getList();
                    if (ival < 0 || ival >= (KSInt)list.size()) {
                        throw runtime_error(stringformat("Out of bounds list access index %lld, list length %lld",
                            ival, list.size()).c_str());
                    } else {
                        return list[ival];
                    }
                }
                break;
                case KSType::STRUCT:
                {
                    auto strval = args[1]->getString();
                    auto& struc = var->getStruct();
                    auto iter = struc.variables.find(strval);
                    if (iter == struc.variables.end()) {
                        throw runtime_error(stringformat("Struct `%s`, does not contain member `%s`",
                            struc.name.c_str(), strval.c_str()).c_str());
                    } else {
                        return iter->second;
                    }
                }
                break;
                case KSType::DICTIONARY:
                {
                    auto& dict = var->getDictionary();
                    auto& ref = dict[args[1]->getHash()];
                    if (ref == nullptr) {
                        ref = make_shared<KSValue>();
                    }
                    return ref;
                }
                break;

                }
                }, libscope);


            newLibraryFunction("structindex", [this, libscope](KSList args) {
                if (args.size() < 2 || args[1]->type != KSType::STRING) {
                    return make_shared<KSValue>();
                }
                auto& strval = args[1]->getString();
                if (args[0]->type != KSType::STRUCT) {
                    auto func = resolveVariable(strval);
                    auto list = KSList({ args[0] });
                    for (size_t i = 2; i < args.size(); ++i) {
                        list.push_back(args[i]);
                    }
                    return callFunction(func->getFunction(), list);
                }
                
                auto& struc = args[0]->getStruct();
                auto iter = struc.variables.find(strval);
                if (iter == struc.variables.end()) {
                    throw runtime_error(stringformat("Struct `%s`, does not contain member `%s`",
                        struc.name.c_str(), strval.c_str()).c_str());
                } else {
                    auto list = KSList();
                    for (size_t i = 2; i < args.size(); ++i) {
                        list.push_back(args[i]);
                    }
                    auto tempStruc = currentStruct;
                    currentStruct = &args[0]->getStruct();
                    auto res = callFunction(iter->second->getFunction(), list);
                    currentStruct = tempStruc;
                    return res;
                }
                }, libscope);
        }

        // casting
        {
            newLibraryFunction("bool", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSInt(0));
                }
                auto val = *args[0];
                val.hardconvert(KSType::INT);
                val.value = (KSInt)args[0]->getBool();
                return make_shared<KSValue>(val);
                }, libscope);

            newLibraryFunction("int", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSInt(0));
                }
                auto val = *args[0];
                val.hardconvert(KSType::INT);
                return make_shared<KSValue>(val);
                }, libscope);

            newLibraryFunction("float", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSFloat(0.0));
                }
                auto val = *args[0];
                val.hardconvert(KSType::FLOAT);
                return make_shared<KSValue>(val);
                }, libscope);

            newLibraryFunction("vec3", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(vec3());
                }
                if (args.size() < 3) {
                    auto val = *args[0];
                    val.hardconvert(KSType::FLOAT);
                    return make_shared<KSValue>(vec3((float)val.getFloat()));
                }
                auto x = *args[0];
                x.hardconvert(KSType::FLOAT);
                auto y = *args[1];
                y.hardconvert(KSType::FLOAT);
                auto z = *args[2];
                z.hardconvert(KSType::FLOAT);
                return make_shared<KSValue>(vec3((float)x.getFloat(), (float)y.getFloat(), (float)z.getFloat()));
                }, libscope);

            newLibraryFunction("string", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(""s);
                }
                auto val = *args[0];
                val.hardconvert(KSType::STRING);
                return make_shared<KSValue>(val);
                }, libscope);

            newLibraryFunction("array", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSArray());
                }
                auto list = make_shared<KSValue>(args);
                list->hardconvert(KSType::ARRAY);
                return list;
                }, libscope);

            newLibraryFunction("list", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSList());
                }
                return make_shared<KSValue>(args);
                }, libscope);

            newLibraryFunction("dictionary", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSDictionary());
                }
                if (args.size() == 1) {
                    auto val = *args[0];
                    val.hardconvert(KSType::DICTIONARY);
                    return make_shared<KSValue>(val);
                }
                auto dict = make_shared<KSValue>(KSDictionary());
                for (auto&& arg : args) {
                    auto val = *arg;
                    val.hardconvert(KSType::DICTIONARY);
                    dict->getDictionary().merge(val.getDictionary());
                }
                return dict;
                }, libscope);

            newLibraryFunction("toarray", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSArray());
                }
                auto val = *args[0];
                val.hardconvert(KSType::ARRAY);
                return make_shared<KSValue>(val);  
                }, libscope);

            newLibraryFunction("tolist", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSList());
                }
                auto val = *args[0];
                val.hardconvert(KSType::LIST);
                return make_shared<KSValue>(val);
                }, libscope);
        }

        // overal stdlib
        {
            newLibraryFunction("typeof", [](KSList args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>();
                }
                return make_shared<KSValue>(getTypeName(args[0]->type));
                }, libscope);

            newLibraryFunction("sqrt", [](const KSList& args) {
                if (args.size() == 0) {
                    return make_shared<KSValue>(KSFloat(0));
                }
                auto val = *args[0];
                val.hardconvert(KSType::FLOAT);
                return make_shared<KSValue>(sqrt(val.getFloat()));
                }, libscope);

            newLibraryFunction("pow", [](const KSList& args) {
                if (args.size() < 2) {
                    return make_shared<KSValue>(KSFloat(0));
                }
                auto val = *args[0];
                val.hardconvert(KSType::FLOAT);
                auto val2 = *args[1];
                val2.hardconvert(KSType::FLOAT);
                return make_shared<KSValue>(pow(val.getFloat(), val2.getFloat()));
                }, libscope);

            newLibraryFunction("min", [](const KSList& args) {
                if (args.size() < 2) {
                    return make_shared<KSValue>();
                }
                auto val = *args[0];
                auto val2 = *args[1];
                upconvertThrowOnNonNumberToNumberCompare(val, val2);
                if (val > val2) {
                    return make_shared<KSValue>(val2.value, val2.type);
                }
                return make_shared<KSValue>(val.value, val.type);
                }, libscope);

            newLibraryFunction("max", [](const KSList& args) {
                if (args.size() < 2) {
                    return make_shared<KSValue>();
                }
                auto val = *args[0];
                auto val2 = *args[1];
                upconvertThrowOnNonNumberToNumberCompare(val, val2);
                if (val < val2) {
                    return make_shared<KSValue>(val2.value, val2.type);
                }
                return make_shared<KSValue>(val.value, val.type);
                }, libscope);

            newLibraryFunction("swap", [](const KSList& args) {
                if (args.size() < 2) {
                    return make_shared<KSValue>();
                }
                auto v = *args[0];
                *args[0] = *args[1];
                *args[1] = v;

                return make_shared<KSValue>();
                }, libscope);

            newLibraryFunction("print", [](const KSList& args) {
                for (auto&& arg : args) {
                    printf("%s", arg->getPrintString().c_str());
                }
                printf("\n");
                return make_shared<KSValue>();
                }, libscope);

            newLibraryFunction("getline", [](const KSList& args) {
                string s;
                // blocking calls are fine
                getline(std::cin, s);
                if (args.size() > 0) {
                    args[0]->value = s;
                    args[0]->type = KSType::STRING;
                }
                return make_shared<KSValue>(s);
                }, libscope);

            newLibraryFunction("map", [this](const KSList& args) {
                if (args.size() < 2 || args[1]->type != KSType::FUNCTION) {
                    return make_shared<KSValue>();
                }
                auto ret = make_shared<KSValue>(KSList());
                auto& retList = ret->getList();
                auto func = args[1]->getFunction();

                if (args[0]->type == KSType::ARRAY) {
                    auto val = *args[0];
                    val.upconvert(KSType::LIST);
                    for (auto&& v : val.getList()) {
                        retList.push_back(callFunction(func, { v }));
                    }
                    return ret;
                }

                for (auto&& v : args[0]->getList()) {
                    retList.push_back(callFunction(func, { v }));
                }
                return ret;

                }, libscope);

            newLibraryFunction("fold", [this](const KSList& args) {
                if (args.size() < 3 || args[1]->type != KSType::FUNCTION) {
                    return make_shared<KSValue>();
                }

                auto func = args[1]->getFunction();
                auto iter = args[2];

                if (args[0]->type == KSType::ARRAY) {
                    auto val = *args[0];
                    val.upconvert(KSType::LIST);
                    for (auto&& v : val.getList()) {
                        iter = callFunction(func, { iter, v });
                    }
                    return iter;
                }

                for (auto&& v : args[0]->getList()) {
                    iter = callFunction(func, { iter, v });
                }
                return iter;

                }, libscope);
        }

        // collection functions

        {
            newLibraryFunction("length", [](const KSList& args) {
                if (args.size() == 0 || (int)args[0]->type < (int)KSType::STRING) {
                    return make_shared<KSValue>(KSInt(0));
                }
                if (args[0]->type == KSType::STRING) {
                    return make_shared<KSValue>((KSInt)args[0]->getString().size());
                }
                if (args[0]->type == KSType::ARRAY) {
                    return make_shared<KSValue>((KSInt)args[0]->getArray().size());
                }
                return make_shared<KSValue>((KSInt)args[0]->getList().size());
                }, libscope);

            newLibraryFunction("find", [](const KSList& args) {
                if (args.size() < 2 || (int)args[0]->type < (int)KSType::ARRAY) {
                    return make_shared<KSValue>();
                }
                if (args[0]->type == KSType::ARRAY) {
                    if (args[1]->type == args[0]->getArray().type) {
                        switch (args[0]->getArray().type) {
                        case KSType::INT:
                        {
                            auto& arry = args[0]->getStdVector<KSInt>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getInt());
                            if (iter == arry.end()) {
                                return make_shared<KSValue>();
                            }
                            return make_shared<KSValue>((KSInt)(iter - arry.begin()));
                        }
                        break;
                        case KSType::FLOAT:
                        {
                            auto& arry = args[0]->getStdVector<KSFloat>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getFloat());
                            if (iter == arry.end()) {
                                return make_shared<KSValue>();
                            }
                            return make_shared<KSValue>((KSInt)(iter - arry.begin()));
                        }
                        break;
                        case KSType::VEC3:
                        {
                            auto& arry = args[0]->getStdVector<vec3>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getVec3());
                            if (iter == arry.end()) {
                                return make_shared<KSValue>();
                            }
                            return make_shared<KSValue>((KSInt)(iter - arry.begin()));
                        }
                        break;
                        case KSType::STRING:
                        {
                            auto& arry = args[0]->getStdVector<string>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getString());
                            if (iter == arry.end()) {
                                return make_shared<KSValue>();
                            }
                            return make_shared<KSValue>((KSInt)(iter - arry.begin()));
                        }
                        break;
                        case KSType::FUNCTION:
                        {
                            auto& arry = args[0]->getStdVector<KSFunctionRef>();
                            auto iter = find(arry.begin(), arry.end(), args[1]->getFunction());
                            if (iter == arry.end()) {
                                return make_shared<KSValue>();
                            }
                            return make_shared<KSValue>((KSInt)(iter - arry.begin()));
                        }
                        break;
                        default:
                            break;
                        }
                    }
                    return make_shared<KSValue>();
                }
                auto& list = args[0]->getList();
                for (size_t i = 0; i < list.size(); ++i) {
                    if (*list[i] == *args[1]) {
                        return make_shared<KSValue>((KSInt)i);
                    }
                }
                return make_shared<KSValue>();
                }, libscope);

            newLibraryFunction("erase", [](const KSList& args) {
                if (args.size() < 2 || (int)args[0]->type < (int)KSType::ARRAY || args[1]->type != KSType::INT) {
                    return make_shared<KSValue>();
                }
                
                if (args[0]->type == KSType::ARRAY) {                    
                    switch (args[0]->getArray().type) {
                    case KSType::INT:
                        args[0]->getStdVector<KSInt>().erase(args[0]->getStdVector<KSInt>().begin() + args[1]->getInt());
                        break;
                    case KSType::FLOAT:
                        args[0]->getStdVector<KSFloat>().erase(args[0]->getStdVector<KSFloat>().begin() + args[1]->getInt());
                        break;
                    case KSType::VEC3:
                        args[0]->getStdVector<vec3>().erase(args[0]->getStdVector<vec3>().begin() + args[1]->getInt());
                        break;
                    case KSType::STRING:
                        args[0]->getStdVector<string>().erase(args[0]->getStdVector<string>().begin() + args[1]->getInt());
                        break;
                    case KSType::FUNCTION:
                        args[0]->getStdVector<KSFunctionRef>().erase(args[0]->getStdVector<KSFunctionRef>().begin() + args[1]->getInt());
                        break;
                    default:
                        break;
                    }
                } else {
                    args[0]->getList().erase(args[0]->getList().begin() + args[1]->getInt());
                }
                return make_shared<KSValue>();
                }, libscope);

            newLibraryFunction("pushback", [](const KSList& args) {
                if (args.size() < 2 || (int)args[0]->type < (int)KSType::ARRAY) {
                    return make_shared<KSValue>();
                }

                if (args[0]->type == KSType::ARRAY) {
                    if (args[0]->getArray().type == args[1]->type) {
                        switch (args[0]->getArray().type) {
                        case KSType::INT:
                            args[0]->getStdVector<KSInt>().push_back(args[1]->getInt());
                            break;
                        case KSType::FLOAT:
                            args[0]->getStdVector<KSFloat>().push_back(args[1]->getFloat());
                            break;
                        case KSType::VEC3:
                            args[0]->getStdVector<vec3>().push_back(args[1]->getVec3());
                            break;
                        case KSType::STRING:
                            args[0]->getStdVector<string>().push_back(args[1]->getString());
                            break;
                        case KSType::FUNCTION:
                            args[0]->getStdVector<KSFunctionRef>().push_back(args[1]->getFunction());
                            break;
                        default:
                            break;
                        }
                    }
                } else {
                    args[0]->getList().push_back(args[1]);
                }
                return make_shared<KSValue>();
                }, libscope);

            newLibraryFunction("popback", [](const KSList& args) {
                if (args.size() < 1 || (int)args[0]->type < (int)KSType::ARRAY) {
                    return make_shared<KSValue>();
                }
                if (args[0]->type == KSType::ARRAY) {
                    args[0]->getArray().pop_back();
                } else {
                    args[0]->getList().pop_back();
                }
                return make_shared<KSValue>();
                }, libscope);

            newLibraryFunction("popfront", [](const KSList& args) {
                if (args.size() < 1 || (int)args[0]->type < (int)KSType::ARRAY) {
                    return make_shared<KSValue>();
                }
                if (args[0]->type == KSType::ARRAY) {
                    switch (args[0]->getArray().type) {
                    case KSType::INT:
                    {
                        auto& arry = args[0]->getStdVector<KSInt>();
                        arry.erase(arry.begin());
                    }
                    break;
                    case KSType::FLOAT:
                    {
                        auto& arry = args[0]->getStdVector<KSFloat>();
                        arry.erase(arry.begin());
                    }
                    break;
                    case KSType::VEC3:
                    {
                        auto& arry = args[0]->getStdVector<vec3>();
                        arry.erase(arry.begin());
                    }
                    break;
                    case KSType::FUNCTION:
                    {
                        auto& arry = args[0]->getStdVector<KSFunctionRef>();
                        arry.erase(arry.begin());
                    }
                    break;
                    case KSType::STRING:
                    {
                        auto& arry = args[0]->getStdVector<string>();
                        arry.erase(arry.begin());
                    }
                    break;
                    
                    default:
                        break;
                    }
                    return args[0];
                } else {
                    auto& list = args[0]->getList();
                    list.erase(list.begin());
                }
                return make_shared<KSValue>();
                }, libscope);

            newLibraryFunction("front", [](const KSList& args) {
                if (args.size() < 1 || (int)args[0]->type < (int)KSType::ARRAY) {
                    return make_shared<KSValue>();
                }
                if (args[0]->type == KSType::ARRAY) {
                    switch (args[0]->getArray().type) {
                    case KSType::INT:
                        return make_shared<KSValue>(args[0]->getStdVector<KSInt>().front());
                    case KSType::FLOAT:
                        return make_shared<KSValue>(args[0]->getStdVector<KSFloat>().front());
                    case KSType::VEC3:
                        return make_shared<KSValue>(args[0]->getStdVector<vec3>().front());
                    case KSType::FUNCTION:
                        return make_shared<KSValue>(args[0]->getStdVector<KSFunctionRef>().front());
                    case KSType::STRING:
                        return make_shared<KSValue>(args[0]->getStdVector<string>().front());
                    default:
                        break;
                    }
                    return make_shared<KSValue>();
                } else {
                    return args[0]->getList().front();
                }
                //return make_shared<KSValue>();
                }, libscope);

            newLibraryFunction("back", [](const KSList& args) {
                if (args.size() < 1 || (int)args[0]->type < (int)KSType::ARRAY) {
                    return make_shared<KSValue>();
                }
                if (args[0]->type == KSType::ARRAY) {
                    switch (args[0]->getArray().type) {
                    case KSType::INT:
                        return make_shared<KSValue>(args[0]->getStdVector<KSInt>().back());
                    case KSType::FLOAT:
                        return make_shared<KSValue>(args[0]->getStdVector<KSFloat>().back());
                    case KSType::VEC3:
                        return make_shared<KSValue>(args[0]->getStdVector<vec3>().back());
                    case KSType::FUNCTION:
                        return make_shared<KSValue>(args[0]->getStdVector<KSFunctionRef>().back());
                    case KSType::STRING:
                        return make_shared<KSValue>(args[0]->getStdVector<string>().back());
                    default:
                        break;
                    }
                    return make_shared<KSValue>();
                } else {
                    return args[0]->getList().back();
                }
                //return make_shared<KSValue>();
                }, libscope);

            newLibraryFunction("range", [](const KSList& args) {
                if (args.size() == 2 && args[0]->type == args[1]->type) {
                    if (args[0]->type == KSType::INT) {
                        auto ret = make_shared<KSValue>(KSArray(vector<KSInt>{}));
                        auto& arry = ret->getStdVector<KSInt>();
                        auto a = args[0]->getInt();
                        auto b = args[1]->getInt();
                        if (b > a) {
                            arry.reserve(b - a);
                            for (KSInt i = a; i <= b; i++) {
                                arry.push_back(i);
                            }
                        } else {
                            arry.reserve(a - b);
                            for (KSInt i = a; i >= b; i--) {
                                arry.push_back(i);
                            }
                        }
                        return ret;
                    } else if (args[0]->type == KSType::FLOAT) {
                        auto ret = make_shared<KSValue>(KSArray(vector<KSFloat>{}));
                        auto& arry = ret->getStdVector<KSFloat>();
                        KSFloat a = args[0]->getFloat();
                        KSFloat b = args[1]->getFloat();
                        if (b > a) {
                            arry.reserve((KSInt)(b - a));
                            for (KSFloat i = a; i <= b; i++) {
                                arry.push_back(i);
                            }
                        } else {
                            arry.reserve((KSInt)(a - b));
                            for (KSFloat i = a; i >= b; i--) {
                                arry.push_back(i);
                            }
                        }
                        return ret;
                    }                    
                }
                if (args.size() < 3 || (int)args[0]->type < (int)KSType::STRING) {
                    return make_shared<KSValue>();
                }
                auto indexA = *args[1];
                indexA.hardconvert(KSType::INT);
                auto indexB = *args[2];
                indexB.hardconvert(KSType::INT);
                auto intdexA = indexA.getInt();
                auto intdexB = indexB.getInt();

                if (args[0]->type == KSType::STRING) {
                    return make_shared<KSValue>(args[0]->getString().substr(intdexA, intdexB));
                } else if (args[0]->type == KSType::ARRAY) {
                    if (args[0]->getArray().type == args[1]->type) {
                        switch (args[0]->getArray().type) {
                        case KSType::INT:
                            return make_shared<KSValue>(KSArray(vector<KSInt>(args[0]->getStdVector<KSInt>().begin() + intdexA, args[0]->getStdVector<KSInt>().begin() + intdexB)));
                            break;
                        case KSType::FLOAT:
                            return make_shared<KSValue>(KSArray(vector<KSFloat>(args[0]->getStdVector<KSFloat>().begin() + intdexA, args[0]->getStdVector<KSFloat>().begin() + intdexB)));
                            break;
                        case KSType::VEC3:
                            return make_shared<KSValue>(KSArray(vector<vec3>(args[0]->getStdVector<vec3>().begin() + intdexA, args[0]->getStdVector<vec3>().begin() + intdexB)));
                            break;
                        case KSType::STRING:
                            return make_shared<KSValue>(KSArray(vector<string>(args[0]->getStdVector<string>().begin() + intdexA, args[0]->getStdVector<string>().begin() + intdexB)));
                            break;
                        case KSType::FUNCTION:
                            return make_shared<KSValue>(KSArray(vector<KSFunctionRef>(args[0]->getStdVector<KSFunctionRef>().begin() + intdexA, args[0]->getStdVector<KSFunctionRef>().begin() + intdexB)));
                            break;
                        default:
                            break;
                        }
                    }
                } else {
                    return make_shared<KSValue>(KSList(args[0]->getList().begin() + intdexA, args[0]->getList().begin() + intdexB));
                }
                return make_shared<KSValue>();
                }, libscope);

            newLibraryFunction("contains", [](const KSList& args) {
                if (args.size() < 2 || (int)args[0]->type < (int)KSType::ARRAY) {
                    return make_shared<KSValue>(KSInt(0));
                }
                if (args[0]->type == KSType::ARRAY) {
                    auto item = *args[1];
                    switch (args[0]->getArray().type) {
                    case KSType::INT:
                        item.hardconvert(KSType::INT);
                        return make_shared<KSValue>((KSInt)contains(args[0]->getStdVector<KSInt>(), item.getInt()));
                    case KSType::FLOAT:
                        item.hardconvert(KSType::FLOAT);
                        return make_shared<KSValue>((KSInt)contains(args[0]->getStdVector<KSFloat>(), item.getFloat()));
                    case KSType::VEC3:
                        item.hardconvert(KSType::VEC3);
                        return make_shared<KSValue>((KSInt)contains(args[0]->getStdVector<vec3>(), item.getVec3()));
                    case KSType::STRING:
                        item.hardconvert(KSType::STRING);
                        return make_shared<KSValue>((KSInt)contains(args[0]->getStdVector<string>(), item.getString()));
                    default:
                        break;
                    }
                    return make_shared<KSValue>(KSInt(0));
                }
                auto& list = args[0]->getList();
                for (size_t i = 0; i < list.size(); ++i) {
                    if (*list[i] == *args[1]) {
                        return make_shared<KSValue>(KSInt(1));
                    }
                }
                return make_shared<KSValue>(KSInt(0));
                }, libscope);

            newLibraryFunction("split", [](const KSList& args) {
                if (args.size() > 0 && args[0]->type == KSType::STRING) {
                    if (args.size() == 1) {
                        vector<string> chars;
                        for (auto c : args[0]->getString()) {
                            chars.push_back(""s + c);
                        }
                        return make_shared<KSValue>(KSArray(chars));
                    }
                    return make_shared<KSValue>(KSArray(split(args[0]->getString(), args[1]->getPrintString())));
                }
                return make_shared<KSValue>();
                }, libscope);

            newLibraryFunction("sort", [](const KSList& args) {
                if (args.size() < 1 || (int)args[0]->type < (int)KSType::ARRAY) {
                    return make_shared<KSValue>();
                }
                if (args[0]->type == KSType::ARRAY) {                    
                    switch (args[0]->getArray().type) {
                    case KSType::INT:
                    {
                        auto& arry = args[0]->getStdVector<KSInt>();
                        std::sort(arry.begin(), arry.end());
                    }
                    break;
                    case KSType::FLOAT:
                    {
                        auto& arry = args[0]->getStdVector<KSFloat>();
                        std::sort(arry.begin(), arry.end());
                    }
                    break;
                    case KSType::STRING:
                    {
                        auto& arry = args[0]->getStdVector<string>();
                        std::sort(arry.begin(), arry.end());
                    }
                    break;
                    case KSType::VEC3:
                    {
                        auto& arry = args[0]->getStdVector<vec3>();
                        std::sort(arry.begin(), arry.end(), [](const vec3& a, const vec3& b) { return a.x < b.x; });
                    }
                    break;
                    default:
                        break;
                    }
                    return args[0];
                }
                auto& list = args[0]->getList();
                std::sort(list.begin(), list.end(), [](const KSValueRef& a, const KSValueRef& b) { return *a < *b; });
                return args[0];
                }, libscope);
        }
	}
#endif
}

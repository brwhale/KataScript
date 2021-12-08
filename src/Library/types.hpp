#pragma once

#include <vector>
#include <memory>
#include <variant>
#include <string>
#include <functional>
#include <unordered_map>
#include <algorithm>

#include "exception.hpp"

namespace KataScript {
    using std::vector;
    using std::get;
    using std::variant;
    using std::string;
    using std::shared_ptr;
    using std::function;
    using std::unordered_map;
    using std::min;
	using std::max;
    using std::make_shared;
    using namespace std::string_literals;

#ifdef KATASCRIPT_USE_32_BIT_NUMBERS
    using KSInt = int;
    using KSFloat = float;
#else
    using KSInt = int64_t;
    using KSFloat = double;
#endif // KATASCRIPT_USE_32_BIT_TYPES

	// our basic Type flag
	enum class KSType : uint8_t {
		// these are capital, otherwise they'd conflict with the c++ types of the same names
		Null = 0, // void
		Int,
		Float,
		Vec3,
		Function,
        UserPointer,
		String,
		Array,
		List,
        Dictionary,
        Class
	};

    // Get strings of type names for debugging and typeof function
	inline string getTypeName(KSType t) {
		switch (t) {
		case KSType::Null:
			return "null";
			break;
		case KSType::Int:
			return "int";
			break;
		case KSType::Float:
			return "float";
			break;
		case KSType::Vec3:
			return "vec3";
			break;
		case KSType::Function:
			return "function";
			break;
        case KSType::UserPointer:
            return "userpointer";
            break;
		case KSType::String:
			return "string";
			break;
		case KSType::Array:
			return "array";
			break;
		case KSType::List:
			return "list";
			break;
        case KSType::Dictionary:
            return "dictionary";
            break;
        case KSType::Class:
            return "class";
            break;
		default:
			return "unknown";
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
        vector<void*>,
        vector<string>
        >;

    // Backing for the Array type
	struct KSArray {
        KSType type;
		KSArrayVariant value;
		
        // constructors
        KSArray() : type(KSType::Int) { value = vector<KSInt>(); }
		KSArray(vector<KSInt> a) : type(KSType::Int), value(a) {}
		KSArray(vector<KSFloat> a) : type(KSType::Float), value(a) {}
		KSArray(vector<vec3> a) : type(KSType::Vec3), value(a) {}
		KSArray(vector<KSFunctionRef> a) : type(KSType::Function), value(a) {}
        KSArray(vector<void*> a) : type(KSType::UserPointer), value(a) {}
		KSArray(vector<string> a) : type(KSType::String), value(a) {}
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
			case KSType::Null:
				return get<vector<KSInt>>(value).size();
				break;
			case KSType::Int:
				return get<vector<KSInt>>(value).size();
				break;
			case KSType::Float:
				return get<vector<KSFloat>>(value).size();
				break;
			case KSType::Vec3:
				return get<vector<vec3>>(value).size();
				break;
			case KSType::Function:
				return get<vector<KSFunctionRef>>(value).size();
				break;
            case KSType::UserPointer:
                return get<vector<void*>>(value).size();
                break;
			case KSType::String:
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
			throw KSException("Unsupported type");
		}

        // implementation for push_back int
		void push_back(const KSInt& t) {
			switch (type) {
			case KSType::Null:
			case KSType::Int:
				get<vector<KSInt>>(value).push_back(t);
				break;
			default:
				throw KSException("Unsupported type");
				break;
			}
		}

        // implementation for push_back KSFloat
		void push_back(const KSFloat& t) {
			switch (type) {
			case KSType::Float:
				get<vector<KSFloat>>(value).push_back(t);
				break;
			default:
				throw KSException("Unsupported type");
				break;
			}
		}
        
        // implementation for push_back vec3
		void push_back(const vec3& t) {
			switch (type) {
			case KSType::Vec3:
				get<vector<vec3>>(value).push_back(t);
				break;
			default:
				throw KSException("Unsupported type");
				break;
			}
		}

        // implementation for push_back string
		void push_back(const string& t) {
			switch (type) {
			case KSType::String:
				get<vector<string>>(value).push_back(t);
				break;
			default:
				throw KSException("Unsupported type");
				break;
			}
		}

        // implementation for push_back function
        void push_back(const KSFunctionRef& t) {
            switch (type) {
            case KSType::Function:
                get<vector<KSFunctionRef>>(value).push_back(t);
                break;
            default:
                throw KSException("Unsupported type");
                break;
            }
        }

        // implementation for push_back function
        void push_back(void* t) {
            switch (type) {
            case KSType::UserPointer:
                get<vector<void*>>(value).push_back(t);
                break;
            default:
                throw KSException("Unsupported type");
                break;
            }
        }

        // implementation for push_back another array
		void push_back(const KSArray& barr) {
			if (type == barr.type) {
				switch (type) {
				case KSType::Int:
                    insert<KSInt>(barr.value);
				    break;
				case KSType::Float:
                    insert<KSFloat>(barr.value);
                    break;
				case KSType::Vec3:
                    insert<vec3>(barr.value);
                    break;
                case KSType::Function:
                    insert<KSFunctionRef>(barr.value);
                    break;
                case KSType::UserPointer:
                    insert<void*>(barr.value);
                    break;
				case KSType::String:
                    insert<string>(barr.value);
                    break;
				default:
					break;
				}
			}
		}

        void pop_back() {
            switch (type) {
            case KSType::Null:
            case KSType::Int:
                get<vector<KSInt>>(value).pop_back();
                break;
            case KSType::Float:
                get<vector<KSFloat>>(value).pop_back();
                break;
            case KSType::Vec3:
                get<vector<vec3>>(value).pop_back();
                break;
            case KSType::Function:
                get<vector<KSFunctionRef>>(value).pop_back();
                break;
            case KSType::UserPointer:
                get<vector<void*>>(value).pop_back();
                break;
            case KSType::String:
                get<vector<string>>(value).pop_back();
                break;
            default:
                break;
            }
        }
	};

    using KSDictionary = unordered_map<size_t, KSValueRef>;

    struct KSScope;
    using KSScopeRef = shared_ptr<KSScope>;

    struct KSClass {
        // this is the main storage object for all functions and variables
        string name;
        unordered_map<string, KSValueRef> variables;
        KSScopeRef functionScope;
        KSClass(const string& name_) : name(name_) {}
        KSClass(const KSClass& o);
        KSClass(const KSScopeRef& o);
    };

    using KSClassRef = shared_ptr<KSClass>;

    // Now that we have our collection types defined, we can finally define our value variant
    using KSValueVariant = 
        variant<
        KSInt,
        KSFloat,
        vec3, 
        KSFunctionRef,
        void*,
        string,
        KSArray, 
        KSList,
        KSDictionary,
        KSClassRef
        >;
	
	// our basic Object/Value type
	struct KSValue {
        KSType type;
		KSValueVariant value;
		
        // Construct a KSValue from any underlying type
		KSValue() : type(KSType::Null) {}
		KSValue(KSInt a) : type(KSType::Int), value(a) {}
		KSValue(KSFloat a) : type(KSType::Float), value(a) {}
		KSValue(vec3 a) : type(KSType::Vec3), value(a) {}
		KSValue(KSFunctionRef a) : type(KSType::Function), value(a) {}
        KSValue(void* a) : type(KSType::UserPointer), value(a) {}
		KSValue(string a) : type(KSType::String), value(a) {}
		KSValue(KSArray a) : type(KSType::Array), value(a) {}
		KSValue(KSList a) : type(KSType::List), value(a) {}
        KSValue(KSDictionary a) : type(KSType::Dictionary), value(a) {}
        KSValue(KSClassRef a) : type(KSType::Class), value(a) {}
		KSValue(KSValueVariant a, KSType t) : type(t), value(a) {}
		~KSValue() {};

        // get a string that represents this value
		string getPrintString() {
			auto t = *this;
			t.hardconvert(KSType::String);
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

        // get this value as a function
        void*& getPointer() {
            return get<void*>(value);
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

        KSClassRef& getClass() {
            return get<KSClassRef>(value);
        }

        // get a boolean representing the truthiness of this value
		bool getBool() {
			// non zero or "true" are true
			bool truthiness = false;
			switch (type) {
			case KSType::Int:
				truthiness = getInt();
				break;
			case KSType::Float:
				truthiness = getFloat() != 0;
				break;
			case KSType::Vec3:
				truthiness = getVec3() != vec3(0);
				break;
			case KSType::String:
				truthiness = getString() == "true";
				break;
			case KSType::Array:
				truthiness = getArray().size() > 0;
				break;
			case KSType::List:
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

    // define cout operator for KSClass
    inline std::ostream& operator<<(std::ostream& os, const KSClassRef& dict) {
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
			if (max((int)a.type, (int)b.type) > (int)KSType::Vec3) {
				throw KSException(stringformat(
                    "Types `%s %s` and `%s %s` are incompatible for this operation",
					getTypeName(a.type).c_str(), a.getPrintString().c_str(),
                    getTypeName(b.type).c_str(), b.getPrintString().c_str()));
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
		case KSType::Int:
			return KSValue{ a.getInt() + b.getInt() };
			break;
		case KSType::Float:
			return KSValue{ a.getFloat() + b.getFloat() };
			break;
		case KSType::Vec3:
			return KSValue{ a.getVec3() + b.getVec3() };
			break;
		case KSType::String:
			return KSValue{ a.getString() + b.getString() };
			break;
		case KSType::Array:
		{
			auto arr = KSArray(a.getArray());
			arr.push_back(b.getArray());
			return KSValue{ arr };
		}
		break;
		case KSType::List:
		{
			auto list = KSList(a.getList());
			auto& blist = b.getList();
			list.insert(list.end(), blist.begin(), blist.end());
			return KSValue{ list };
		}
			break;
        case KSType::Dictionary:
        {
            auto list = KSDictionary(a.getDictionary());
            list.merge(b.getDictionary());
            return KSValue{ list };
        }
        break;
		default:
			throw KSException(stringformat("Operator + not defined for type `%s`",
                getTypeName(a.type).c_str()));
			break;
		}
	}

	inline KSValue operator - (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::Int:
			return KSValue{ a.getInt() - b.getInt() };
			break;
		case KSType::Float:
			return KSValue{ a.getFloat() - b.getFloat() };
			break;
		case KSType::Vec3:
			return KSValue{ a.getVec3() - b.getVec3() };
			break;
		default:
			throw KSException(stringformat("Operator - not defined for type `%s`", 
                getTypeName(a.type).c_str()));
			break;
		}
	}

	inline KSValue operator * (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::Int:
			return KSValue{ a.getInt() * b.getInt() };
			break;
		case KSType::Float:
			return KSValue{ a.getFloat() * b.getFloat() };
			break;
		case KSType::Vec3:
			return KSValue{ a.getVec3() * b.getVec3() };
			break;
		default:
			throw KSException(stringformat("Operator * not defined for type `%s`", 
                getTypeName(a.type).c_str()));
			break;
		}
	}

	inline KSValue operator / (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::Int:
			return KSValue{ a.getInt() / b.getInt() };
			break;
		case KSType::Float:
			return KSValue{ a.getFloat() / b.getFloat() };
			break;
		case KSType::Vec3:
			return KSValue{ a.getVec3() / b.getVec3() };
			break;
		default:
			throw KSException(stringformat("Operator / not defined for type `%s`", 
                getTypeName(a.type).c_str()));
			break;
		}
	}

	inline KSValue operator += (KSValue& a, KSValue b) {
        if ((int)a.type < (int)KSType::Array || b.type == KSType::List) {
            upconvert(a, b);
        }
		switch (a.type) {
		case KSType::Int:
			a.getInt() += b.getInt();
			break;
		case KSType::Float:
			a.getFloat() += b.getFloat();
			break;
		case KSType::Vec3:

			a.getVec3() += b.getVec3();
			break;
		case KSType::String:
			a.getString() += b.getString();
			break;
		case KSType::Array:
		{
			auto& arr = a.getArray();
            if (arr.type == b.type 
                || (b.type == KSType::Array && b.getArray().type == arr.type)) {
                switch (b.type) {
                case KSType::Int:
                    arr.push_back(b.getInt());
                    break;
                case KSType::Float:
                    arr.push_back(b.getFloat());
                    break;
                case KSType::Vec3:
                    arr.push_back(b.getVec3());
                    break;
                case KSType::Function:
                    arr.push_back(b.getFunction());
                    break;
                case KSType::String:
                    arr.push_back(b.getString());
                    break;
                case KSType::UserPointer:
                    arr.push_back(b.getPointer());
                    break;
                case KSType::Array:
                    arr.push_back(b.getArray());
                    break;
                default:
                    break;
                }
            }
			return KSValue{ arr };
		}
		break;
		case KSType::List:
		{
            auto& list = a.getList();
            switch (b.type) {
            case KSType::Int:
            case KSType::Float:
            case KSType::Vec3:
            case KSType::Function:
            case KSType::String:
            case KSType::UserPointer:
                list.push_back(std::make_shared<KSValue>(b.value, b.type));
                break;
            default:
            {
                b.upconvert(KSType::List);
                auto& blist = b.getList();
                list.insert(list.end(), blist.begin(), blist.end());
            }
                break;
            }			
		}
		break;
        case KSType::Dictionary:
        {
            auto& dict = a.getDictionary();
            b.upconvert(KSType::Dictionary);
            dict.merge(b.getDictionary());
        }
        break;
		default:
			throw KSException(stringformat("Operator += not defined for type `%s`",
                getTypeName(a.type).c_str()));
			break;
		}
		return a;
	}

	inline KSValue operator -= (KSValue& a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::Int:
			a.getInt() -= b.getInt();
			break;
		case KSType::Float:
			a.getFloat() -= b.getFloat();
			break;
		case KSType::Vec3:
			a.getVec3() -= b.getVec3();
			break;
		default:
			throw KSException(stringformat("Operator -= not defined for type `%s`", 
                getTypeName(a.type).c_str()));
			break;
		}
		return a;
	}

	inline KSValue operator *= (KSValue& a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::Int:
			a.getInt() *= b.getInt();
			break;
		case KSType::Float:
			a.getFloat() *= b.getFloat();
			break;
		case KSType::Vec3:
			a.getVec3() *= b.getVec3();
			break;
		default:
			throw KSException(stringformat("Operator *= not defined for type `%s`", 
                getTypeName(a.type).c_str()));
			break;
		}
		return a;
	}

	inline KSValue operator /= (KSValue& a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::Int:
			a.getInt() /= b.getInt();
			break;
		case KSType::Float:
			a.getFloat() /= b.getFloat();
			break;
		case KSType::Vec3:
			a.getVec3() /= b.getVec3();
			break;
		default:
			throw KSException(stringformat("Operator /= not defined for type `%s`", 
                getTypeName(a.type).c_str()));
			break;
		}
		return a;
	}

	inline KSValue operator % (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::Int:
			return KSValue{ a.getInt() % b.getInt() };
			break;
		case KSType::Float:
			return KSValue{ std::fmod(a.getFloat(), b.getFloat()) };
			break;
		default:
			throw KSException(stringformat("Operator %% not defined for type `%s`", 
                getTypeName(a.type).c_str()));
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
        case KSType::Null:
            return true;
		case KSType::Int:
			return a.getInt() == b.getInt();
			break;
		case KSType::Float:
			return a.getFloat() == b.getFloat();
			break;
		case KSType::Vec3:
			return a.getVec3() == b.getVec3();
			break;
		case KSType::String:
			return a.getString() == b.getString();
			break;
		case KSType::Array:
			return a.getArray() == b.getArray();
			break;
		case KSType::List:
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
            throw KSException(stringformat("Operator == not defined for type `%s`",
                getTypeName(a.type).c_str()));
			break;
		}
		return true;
	}

	inline bool operator != (KSValue a, KSValue b) {
        if (a.type != b.type) {
            return true;
        }		
		switch (a.type) {
        case KSType::Null:
            return false;
		case KSType::Int:
			return a.getInt() != b.getInt();
			break;
		case KSType::Float:
			return a.getFloat() != b.getFloat();
			break;
		case KSType::Vec3:
			return a.getVec3() != b.getVec3();
			break;
		case KSType::String:
			return a.getString() != b.getString();
			break;
		case KSType::Array:
		case KSType::List:
			return !(a == b);
			break;
		default:
            throw KSException(stringformat("Operator != not defined for type `%s`",
                getTypeName(a.type).c_str()));
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
		case KSType::Int:
			return a.getInt() < b.getInt();
			break;
		case KSType::Float:
			return a.getFloat() < b.getFloat();
			break;
		case KSType::String:
			return a.getString() < b.getString();
			break;
		case KSType::Array:
			return a.getArray().size() < b.getArray().size();
			break;
		case KSType::List:
			return a.getList().size() < b.getList().size();
			break;
        case KSType::Dictionary:
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
		case KSType::Int:
			return a.getInt() > b.getInt();
			break;
		case KSType::Float:
			return a.getFloat() > b.getFloat();
			break;
		case KSType::String:
			return a.getString() > b.getString();
			break;
		case KSType::Array:
			return a.getArray().size() > b.getArray().size();
			break;
		case KSType::List:
			return a.getList().size() > b.getList().size();
			break;
        case KSType::Dictionary:
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
		case KSType::Int:
			return a.getInt() <= b.getInt();
			break;
		case KSType::Float:
			return a.getFloat() <= b.getFloat();
			break;
		case KSType::String:
			return a.getString() <= b.getString();
			break;
		case KSType::Array:
			return a.getArray().size() <= b.getArray().size();
			break;
		case KSType::List:
			return a.getList().size() <= b.getList().size();
			break;
        case KSType::Dictionary:
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
		case KSType::Int:
			return a.getInt() >= b.getInt();
			break;
		case KSType::Float:
			return a.getFloat() >= b.getFloat();
			break;
		case KSType::String:
			return a.getString() >= b.getString();
			break;
		case KSType::Array:
			return a.getArray().size() >= b.getArray().size();
			break;
		case KSType::List:
			return a.getList().size() >= b.getList().size();
			break;
        case KSType::Dictionary:
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
}
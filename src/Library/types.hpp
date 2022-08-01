#pragma once

#include <vector>
#include <memory>
#include <variant>
#include <string>
#include <functional>
#include <unordered_map>
#include <algorithm>

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

        template <typename T>
        vector<T>& getStdVector();

		bool operator==(const KSArray& o) const {
            if (size() != o.size()) {
                return false;
            }
            if (type != o.type) {
                return false;
            }
            switch (type) {
            case KSType::Int:
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
            case KSType::Float:
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
            case KSType::Vec3:
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
            case KSType::String:
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
        KSClass(const string& name_, const unordered_map<string, KSValueRef>& variables_) : name(name_), variables(variables_){}
        KSClass(const KSClass& o);
        KSClass(const KSScopeRef& o);
    };

    using KSClassRef = shared_ptr<KSClass>;

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
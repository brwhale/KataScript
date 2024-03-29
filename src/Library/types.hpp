#pragma once

#include <vector>
#include <memory>
#include <variant>
#include <string>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <mutex>

namespace KataScript {
    using std::vector;
    using std::get;
    using std::variant;
    using std::string;
    using std::string_view;
    using std::shared_ptr;
    using std::function;
    using std::unordered_map;
    using std::min;
	using std::max;
    using std::make_shared;
    using namespace std::string_literals;

#ifdef KATASCRIPT_USE_32_BIT_NUMBERS
    using Int = int;
    using Float = float;
#else
    using Int = int64_t;
    using Float = double;
#endif // KATASCRIPT_USE_32_BIT_TYPES

	// our basic Type flag
	enum class Type : uint8_t {
		// these are capital, otherwise they'd conflict with the c++ types of the same names
		Null = 0, // void
		Int,
		Float,
		Vec3,
		Function,
        UserPointer,
		String,
		Array,
        ArrayMember,
		List,
        Dictionary,
        Class
	};

    // Get strings of type names for debugging and typeof function
	inline string getTypeName(Type t) {
		switch (t) {
		case Type::Null:
			return "null";
			break;
		case Type::Int:
			return "int";
			break;
		case Type::Float:
			return "float";
			break;
		case Type::Vec3:
			return "vec3";
			break;
		case Type::Function:
			return "function";
			break;
        case Type::UserPointer:
            return "userpointer";
            break;
		case Type::String:
			return "string";
			break;
		case Type::Array:
			return "array";
			break;
        case Type::ArrayMember:
			return "arraymember";
			break;
		case Type::List:
			return "list";
			break;
        case Type::Dictionary:
            return "dictionary";
            break;
        case Type::Class:
            return "class";
            break;
		default:
			return "unknown";
			break;
		}
	}

    inline size_t typeHashBits(Type type) {
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
	struct Value;

    // KataScript uses shared_ptr for ref counting, anything with a name
    // like fooRef is a a shared_ptr to foo
	using ValueRef = shared_ptr<Value>;
    inline ValueRef makeNull() {
        return make_shared<Value>();
    }

    // Backing for the List type and for function arguments
	using List = vector<ValueRef>;

	// forward declare function for functions as values
	struct Function;
	using FunctionRef = shared_ptr<Function>;
    using UserPointer = void*;

    // variant allows us to have a union with a little more safety and ease
	using ArrayVariant = 
        variant<
        vector<Int>,
        vector<Float>,
        vector<vec3>, 
        vector<FunctionRef>,
        vector<UserPointer>,
        vector<string>
        >;

    inline Type getArrayType(const ArrayVariant arr) {
        switch (arr.index()) {
        case 0:
            return Type::Int;
        case 1:
            return Type::Float;
        case 2:
            return Type::Vec3;
        case 3:
            return Type::Function;
        case 4:
            return Type::UserPointer;
        case 5:
            return Type::String;
        default:
            return Type::Null;
        }
    }

    // Backing for the Array type
	struct Array {
		ArrayVariant value;
		
        // constructors
        Array() { value = vector<Int>(); }
		Array(vector<Int> a) : value(a) {}
		Array(vector<Float> a) : value(a) {}
		Array(vector<vec3> a) : value(a) {}
		Array(vector<FunctionRef> a) : value(a) {}
        Array(vector<UserPointer> a) : value(a) {}
		Array(vector<string> a) : value(a) {}
		Array(ArrayVariant a) : value(a) {}

        Type getType() const {
            return getArrayType(value);
        }

        template <typename T>
        vector<T>& getStdVector();

		bool operator==(const Array& o) const {
            if (size() != o.size()) {
                return false;
            }
            if (getType() != o.getType()) {
                return false;
            }
            switch (getType()) {
            case Type::Int:
            {
                auto& aarr = get<vector<Int>>(value);
                auto& barr = get<vector<Int>>(o.value);
                for (size_t i = 0; i < size(); ++i) {
                    if (aarr[i] != barr[i]) {
                        return false;
                    }
                }
            }
            break;
            case Type::Float:
            {
                auto& aarr = get<vector<Float>>(value);
                auto& barr = get<vector<Float>>(o.value);
                for (size_t i = 0; i < size(); ++i) {
                    if (aarr[i] != barr[i]) {
                        return false;
                    }
                }
            }
            break;
            case Type::Vec3:
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
            case Type::String:
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

		bool operator!=(const Array& o) const {
			return !(operator==(o));
		}

        // doing these switches is tedious
        // so ideally they all get burried beneath template functions
        // and we get a nice clean api out the other end

        void changeType(Type newType) {
            switch (newType) {
            case KataScript::Type::Null:
                value = vector<Int>();
                break;
            case KataScript::Type::Int:
                value = vector<Int>();
                break;
            case KataScript::Type::Float:
                value = vector<Float>();
                break;
            case KataScript::Type::Vec3:
                value = vector<vec3>();
                break;
            case KataScript::Type::Function:
                value = vector<FunctionRef>();
                break;
            case KataScript::Type::UserPointer:
                value = vector<UserPointer>();
                break;
            case KataScript::Type::String:
                value = vector<string>();
                break;
            case KataScript::Type::Array:
            case KataScript::Type::ArrayMember:
            case KataScript::Type::List:
            case KataScript::Type::Dictionary:
            case KataScript::Type::Class:
            default:
                throw Exception("Array cannot convert to array of "s + getTypeName(newType));
                break;
            }
        }

        // get the size without caring about the underlying type
		size_t size() const {
			switch (getType()) {
			case Type::Null:
				return get<vector<Int>>(value).size();
				break;
			case Type::Int:
				return get<vector<Int>>(value).size();
				break;
			case Type::Float:
				return get<vector<Float>>(value).size();
				break;
			case Type::Vec3:
				return get<vector<vec3>>(value).size();
				break;
			case Type::Function:
				return get<vector<FunctionRef>>(value).size();
				break;
            case Type::UserPointer:
                return get<vector<UserPointer>>(value).size();
                break;
			case Type::String:
				return get<vector<string>>(value).size();
				break;
			default:
				return 0;
				break;
			}			
		}

        // Helper for push back
        template <typename T>
        void insert(const ArrayVariant& b) {
            auto& aa = get<vector<T>>(value);
            auto& bb = get<vector<T>>(b);
            aa.insert(aa.end(), bb.begin(), bb.end());
        }

        // Default is unsupported
		template<typename T>
		void push_back(const T& t) {
			throw Exception("Unsupported type");
		}

        // implementation for push_back int
		void push_back(const Int& t) {
			switch (getType()) {
			case Type::Null:
			case Type::Int:
				get<vector<Int>>(value).push_back(t);
				break;
			default:
				throw Exception("Unsupported type");
				break;
			}
		}

        // implementation for push_back Float
		void push_back(const Float& t) {
			switch (getType()) {
			case Type::Float:
				get<vector<Float>>(value).push_back(t);
				break;
			default:
				throw Exception("Unsupported type");
				break;
			}
		}
        
        // implementation for push_back vec3
		void push_back(const vec3& t) {
			switch (getType()) {
			case Type::Vec3:
				get<vector<vec3>>(value).push_back(t);
				break;
			default:
				throw Exception("Unsupported type");
				break;
			}
		}

        // implementation for push_back string
		void push_back(const string& t) {
			switch (getType()) {
			case Type::String:
				get<vector<string>>(value).push_back(t);
				break;
			default:
				throw Exception("Unsupported type");
				break;
			}
		}

        // implementation for push_back function
        void push_back(const FunctionRef& t) {
            switch (getType()) {
            case Type::Function:
                get<vector<FunctionRef>>(value).push_back(t);
                break;
            default:
                throw Exception("Unsupported type");
                break;
            }
        }

        // implementation for push_back function
        void push_back(UserPointer t) {
            switch (getType()) {
            case Type::UserPointer:
                get<vector<UserPointer>>(value).push_back(t);
                break;
            default:
                throw Exception("Unsupported type");
                break;
            }
        }

        // implementation for push_back another array
		void push_back(const Array& barr) {
			if (getType() == barr.getType()) {
				switch (getType()) {
				case Type::Int:
                    insert<Int>(barr.value);
				    break;
				case Type::Float:
                    insert<Float>(barr.value);
                    break;
				case Type::Vec3:
                    insert<vec3>(barr.value);
                    break;
                case Type::Function:
                    insert<FunctionRef>(barr.value);
                    break;
                case Type::UserPointer:
                    insert<UserPointer>(barr.value);
                    break;
				case Type::String:
                    insert<string>(barr.value);
                    break;
				default:
					break;
				}
			}
		}

        void pop_back() {
            switch (getType()) {
            case Type::Null:
            case Type::Int:
                get<vector<Int>>(value).pop_back();
                break;
            case Type::Float:
                get<vector<Float>>(value).pop_back();
                break;
            case Type::Vec3:
                get<vector<vec3>>(value).pop_back();
                break;
            case Type::Function:
                get<vector<FunctionRef>>(value).pop_back();
                break;
            case Type::UserPointer:
                get<vector<UserPointer>>(value).pop_back();
                break;
            case Type::String:
                get<vector<string>>(value).pop_back();
                break;
            default:
                break;
            }
        }
	};

    struct ArrayMember {
        ValueRef arrayRef;
        Int index;

        void setValue(const ValueRef& val);
        ValueRef getValue() const;
    };

    using Dictionary = unordered_map<size_t, ValueRef>;
    using DictionaryRef = shared_ptr<Dictionary>;

    struct Scope;
    using ScopeRef = shared_ptr<Scope>;

    struct Class {
        // this is the main storage object for all functions and variables
        string name;
        unordered_map<string, ValueRef> variables;
        ScopeRef functionScope;
#ifndef KATASCRIPT_THREAD_UNSAFE
        std::mutex varInsert;
#endif

        Class(const string& name_) : name(name_) {}
        Class(const string& name_, const unordered_map<string, ValueRef>& variables_) : name(name_), variables(variables_){}
        Class(const Class& o);
        Class(const ScopeRef& o);
        ~Class();

        ValueRef& insertVar(const string& n, ValueRef val) {
#ifndef KATASCRIPT_THREAD_UNSAFE
            auto l = std::unique_lock(varInsert);
#endif
            auto& ref = variables[n];
            ref = val;
            return ref;
        }
    };

    using ClassRef = shared_ptr<Class>;

	// operator precedence for pemdas
	enum class OperatorPrecedence : int {
		assign = 0,
        boolean,
		compare,        
		addsub,
		muldiv,
		incdec,
		func
	};

	// Lambda is a "native function" it's how you wrap c++ code for use inside KataScript
    using Lambda = function<ValueRef(const List&)>;
    using ScopedLambda = function<ValueRef(ScopeRef, const List&)>;
    using ClassLambda = function<ValueRef(Class*, ScopeRef, const List&)>;

	// forward declare so we can cross refernce types
	// Expression is a 'generic' expression
	struct Expression;
	using ExpressionRef = shared_ptr<Expression>;

    enum class FunctionType : uint8_t {
        free,
        constructor,
        member,
    };

    using FunctionBodyVariant =
        variant<
        vector<ExpressionRef>,
        Lambda,
        ScopedLambda,
        ClassLambda
        >;

    enum class FunctionBodyType : uint8_t {
        Subexpressions,
        Lambda,
        ScopedLambda,
        ClassLambda
    };

	// our basic function type
	struct Function {
        OperatorPrecedence opPrecedence;
        FunctionType type = FunctionType::free;
        string name;
		vector<string> argNames;

        FunctionBodyVariant body;

        FunctionBodyType getBodyType() {
            return static_cast<FunctionBodyType>(body.index());
        }

		static OperatorPrecedence getPrecedence(const string& n) {
			if (n.size() > 2) {
				return OperatorPrecedence::func;
			}
            if (n == "||" || n == "&&") {
                return OperatorPrecedence::boolean;
            }
            if (n == "++" || n == "--") {
                return OperatorPrecedence::incdec;
            }
			if (contains("!<>|&"s, n[0]) || n == "==") {
				return OperatorPrecedence::compare;
			}
			if (contains(n, '=')) {
				return OperatorPrecedence::assign;
			}
			if (contains("/*%"s, n[0])) {
				return OperatorPrecedence::muldiv;
			}
			if (contains("-+"s, n[0])) {
				return OperatorPrecedence::addsub;
			}
			return OperatorPrecedence::func;
		}

		Function(const string& name_, const Lambda& l) 
            : name(name_), opPrecedence(getPrecedence(name_)), body(l) {}
        Function(const string& name_, const ScopedLambda& l)
            : name(name_), opPrecedence(getPrecedence(name_)), body(l) {}
        Function(const string& name_, const ClassLambda& l)
            : name(name_), opPrecedence(getPrecedence(name_)), body(l) {}
		// when using a KataScript function body
        // the operator precedence will always be "func" level (aka the highest)
		Function(const string& name_, const vector<string>& argNames_, const vector<ExpressionRef>& body_) 
			: name(name_), body(body_), argNames(argNames_), opPrecedence(OperatorPrecedence::func) {}
        Function(const string& name_, const vector<string>& argNames_) : Function(name_, argNames_, {}) {}
		// default constructor makes a function with no args that returns void
		Function(const string& name) 
            : Function(name, [](List) { return makeNull(); }) {}
		Function() : name("__anon") {}
        Function(const Function& o) = default;
	};
}
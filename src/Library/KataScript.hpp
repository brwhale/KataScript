// copyright Garrett Skelton 2020
// MIT license
#pragma once
#include <vector>
#include <string>
#include <variant>
#include <functional>
#include <map>
#include <unordered_map>
#include <cstdarg>
#include <charconv>
#include <iostream>
#include <memory>
#include <algorithm>
#include <exception>
#include <cmath>

namespace KataScript {
	using std::min;
	using std::max;
	using std::string;
	using std::vector;
	using std::variant;
	using std::function;
	using std::unordered_map;
	using std::map;
    using std::runtime_error;
	using std::shared_ptr;
	using std::make_shared;
	using namespace std::string_literals;

	inline double fromChars(const string& token) {
		double x;
#ifdef _MSC_VER
		std::from_chars(token.data(), token.data() + token.size(), x);
#else
		x = std::stod(token);
#endif
		return x;
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
		LIST
	};

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
		default:
			return "UNKNOWN";
			break;
		}
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
		template<typename T>
		operator T() {
			return T(x, y, z);
		}
	};

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

	// forward declare so lists can contain lists
	struct KSValue;
	using KSValueRef = shared_ptr<KSValue>;
	using KSList = vector<KSValueRef>;
	struct KSArray;
	// forward declare function for functions as values
	struct KSFunction;
	using KSFunctionRef = shared_ptr<KSFunction>;
	using KSArrayVariant = variant<vector<int>, vector<float>, vector<vec3>, vector<KSFunctionRef>, vector<string>>;

	struct KSArray {
		KSArrayVariant value;
		KSType type;

        KSArray() : type(KSType::INT) { value = vector<int>(); }
		KSArray(vector<int> a) : type(KSType::INT), value(a) {}
		KSArray(vector<float> a) : type(KSType::FLOAT), value(a) {}
		KSArray(vector<vec3> a) : type(KSType::VEC3), value(a) {}
		KSArray(vector<KSFunctionRef> a) : type(KSType::FUNCTION), value(a) {}
		KSArray(vector<string> a) : type(KSType::STRING), value(a) {}
		KSArray(KSArrayVariant a, KSType t) : type(t), value(a) {}

		bool operator==(const KSArray& o) const;

		bool operator!=(const KSArray& o) const {
			return !(operator==(o));
		}

		size_t size() const {
			switch (type) {
			case KSType::NONE:
				return get<vector<int>>(value).size();
				break;
			case KSType::INT:
				return get<vector<int>>(value).size();
				break;
			case KSType::FLOAT:
				return get<vector<float>>(value).size();
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

		template<typename T>
		void push_back(const T& t) {
			throw runtime_error("Unsupported type");
		}

		template<>
		void push_back(const int& t) {
			switch (type) {
			case KSType::NONE:
			case KSType::INT:
				get<vector<int>>(value).push_back(t);
				break;
			default:
				throw runtime_error("Unsupported type");
				break;
			}
		}

		template<>
		void push_back(const float& t) {
			switch (type) {
			case KSType::FLOAT:
				get<vector<float>>(value).push_back(t);
				break;
			default:
				throw runtime_error("Unsupported type");
				break;
			}
		}

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

		template<>
		void push_back(const KSArray& barr) {
			if (type == barr.type) {
				switch (type) {
				case KSType::INT:
				{
					auto& aa = get<vector<int>>(value);
					auto& bb = get<vector<int>>(barr.value);
					aa.insert(aa.end(), bb.begin(), bb.end());
				}
				break;
				case KSType::FLOAT:
				{
					auto& aa = get<vector<float>>(value);
					auto& bb = get<vector<float>>(barr.value);
					aa.insert(aa.end(), bb.begin(), bb.end());
				}
				break;
				case KSType::VEC3:
				{
					auto& aa = get<vector<vec3>>(value);
					auto& bb = get<vector<vec3>>(barr.value);
					aa.insert(aa.end(), bb.begin(), bb.end());
				}
				break;
				case KSType::STRING:
				{
					auto& aa = get<vector<string>>(value);
					auto& bb = get<vector<string>>(barr.value);
					aa.insert(aa.end(), bb.begin(), bb.end());
				}
				break;
				default:
					break;
				}
			}
		}
	};

    using KSValueVariant = variant<int, float, vec3, KSFunctionRef, string, KSArray, KSList>;
	
	// our basic Object/Value type
	struct KSValue {
		KSValueVariant value;
		KSType type;

		KSValue() : type(KSType::NONE) {}
		KSValue(int a) : type(KSType::INT), value(a) {}
		KSValue(float a) : type(KSType::FLOAT), value(a) {}
		KSValue(vec3 a) : type(KSType::VEC3), value(a) {}
		KSValue(KSFunctionRef a) : type(KSType::FUNCTION), value(a) {}
		KSValue(string a) : type(KSType::STRING), value(a) {}
		KSValue(KSArray a) : type(KSType::ARRAY), value(a) {}
		KSValue(KSList a) : type(KSType::LIST), value(a) {}
		KSValue(KSValueVariant a, KSType t) : type(t), value(a) {}
		~KSValue() {};

		string getPrintString() {
			auto t = *this;
			t.hardconvert(KSType::STRING);
			return get<string>(t.value);
		}

		int& getInt() {
			return get<int>(value);
		}

		float& getFloat() {
			return get<float>(value);
		}

		vec3& getVec3() {
			return get<vec3>(value);
		}

		KSFunctionRef& getFunction() {
			return get<KSFunctionRef>(value);
		}

		string& getString() {
			return get<string>(value);
		}

		KSArray& getArray() {
			return get<KSArray>(value);
		}

		template <typename T>
		vector<T>& getStdVector();

		KSList& getList() {
			return get<KSList>(value);
		}

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

		void upconvert(KSType newType) {
			if (type == KSType::FUNCTION || newType == KSType::FUNCTION) {
				throw runtime_error("cannot convert functions");
			}
			if (newType > type) {
				switch (newType) {
				default:
					break;
				case KSType::INT:
					value = 0;
					break;
				case KSType::FLOAT:
					switch (type) {
					default:
						break;
					case KSType::NONE:
						value = 0.f;
						break;
					case KSType::INT:
						value = (float)getInt();
						break;
					}
					break;
				case KSType::VEC3:
					switch (type) {
					default:
						break;
					case KSType::NONE:
						value = vec3();
						break;
					case KSType::INT:
						value = vec3((float)getInt());
						break;
					case KSType::FLOAT:
						value = vec3(getFloat());
						break;
					}
					break;
				case KSType::STRING:
					switch (type) {
					default:
						break;
					case KSType::NONE:
						value = "null"s;
						break;
					case KSType::INT:
						value = stringformat("%i", getInt());
						break;
					case KSType::FLOAT:
						value = stringformat("%f", getFloat());
						break;
					case KSType::VEC3:
						auto& vec = getVec3();
						value = stringformat("%f, %f, %f", vec.x, vec.y, vec.z);
						break;
					}
					break;
				case KSType::ARRAY:
					switch (type) {
					default:
						break;
					case KSType::NONE:
						value = KSArray();
						getArray().push_back(0);
					case KSType::INT:
						value = KSArray(vector<int>{ getInt() });
						break;
					case KSType::FLOAT:
						value = KSArray(vector<float>{ getFloat() } );
						break;
					case KSType::VEC3:
					{
						auto& vec = getVec3();
						value = KSArray(vector<float>{ vec.x, vec.y, vec.z });
					}
						break;
					case KSType::STRING:
					{
						auto str = getPrintString();
						value = KSArray(vector<string>{ });
						auto& arr = getArray();
						for (auto c : str) {
							arr.push_back(""s + c);
						}
					}
					break;
					}
				break;
				case KSType::LIST:
					switch (type) {
					default:
						break;
					case KSType::NONE:
					case KSType::INT:
					case KSType::FLOAT:
						value = KSList({ make_shared<KSValue>(value, type) });
						break;
					case KSType::VEC3:
					{
						auto& vec = getVec3();
						value = KSList({ make_shared<KSValue>(vec.x), make_shared<KSValue>(vec.y), make_shared<KSValue>(vec.z) });
					}
						break;
					case KSType::STRING:
					{
						auto str = getPrintString();
						value = KSList({ });
						auto& list = getList();
						for (auto c : str) {
							list.push_back(make_shared<KSValue>(""s + c));
						}
					}
						break;
					case KSType::ARRAY:
						KSArray arr = getArray();
						value = KSList();
						auto& list = getList();
						switch (arr.type) {
						case KSType::INT:
							for (auto&& item : get<vector<int>>(arr.value)) {
								list.push_back(make_shared<KSValue>(item));
							}
							break;
						case KSType::FLOAT:
							for (auto&& item : get<vector<float>>(arr.value)) {
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
							break;
						}						
						break;
					}
				break;
				}
				type = newType;
			}
		}

		void hardconvert(KSType newType) {
			if (newType >= type) {
				upconvert(newType);
			} else {				
				switch (newType) {
				default:
					break;
				case KSType::NONE:
					value = 0;
					break;
				case KSType::INT:
					switch (type) {
					default:
						break;
					case KSType::FLOAT:
						value = (int)getFloat();
						break;
					case KSType::STRING:
						value = (int)fromChars(getString());
						break;
					case KSType::ARRAY:
						value = (int)getArray().size();
						break;
					case KSType::LIST:
						value = (int)getList().size();
						break;
					}
					break;
				case KSType::FLOAT:
					switch (type) {
					default:
						break;
					case KSType::STRING:
						value = (float)fromChars(getString());
						break;
					case KSType::ARRAY:
						value = (float)getArray().size();
						break;
					case KSType::LIST:
						value = (float)getList().size();
						break;
					}
					break;
				case KSType::STRING:
					switch (type) {
					default:
						break;
					case KSType::ARRAY:
					{
						string newval;
						auto& arr = getArray();
						switch (arr.type) {
						case KSType::INT:
							for (auto&& item : get<vector<int>>(arr.value)) {
								newval += KSValue(item).getPrintString() + ", ";
							}
							break;
						case KSType::FLOAT:
							for (auto&& item : get<vector<float>>(arr.value)) {
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
						if (list.size()) {
							newval.pop_back();
							newval.pop_back();
						}
						value = newval;
					}
						break;
					}				
					break;
				case KSType::ARRAY:
				{
					auto list = getList();
					auto listType = list[0]->type;
					KSArray arr;
					switch (listType) {
					case KSType::INT:
						arr = KSArray(vector<int>{});
						for (auto&& item : list) {
							if (item->type == listType) {
								arr.push_back(item->getInt());
							}
						}
						break;
					case KSType::FLOAT:
						arr = KSArray(vector<float>{});
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
				type = newType;
			}
		}
	};

	// define cout operator for KSValues
	inline std::ostream& operator<<(std::ostream& os, const KSList& values) {
		os << KSValue(values).getPrintString();

		return os;
	}

	inline std::ostream& operator<<(std::ostream& os, const KSArray& arr) {
		os << KSValue(arr).getPrintString();
		
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

	inline void upconvertThrowOnNonNumberToNumberCompare(KSValue& a, KSValue& b) {
		if (a.type != b.type) {
			if (max((int)a.type, (int)b.type) > (int)KSType::VEC3) {
				throw runtime_error(stringformat("Types `%s %s` and `%s %s` are incompatible for this operation",
					getTypeName(a.type).c_str(), a.getPrintString().c_str(), getTypeName(b.type).c_str(), b.getPrintString().c_str()).c_str());
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
		default:
			throw runtime_error(stringformat("Operator + not defined for type `%s`", getTypeName(a.type).c_str()).c_str());
			break;
		}
		return a;
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
			throw runtime_error(stringformat("Operator - not defined for type `%s`", getTypeName(a.type).c_str()).c_str());
			break;
		}
		return a;
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
			throw runtime_error(stringformat("Operator * not defined for type `%s`", getTypeName(a.type).c_str()).c_str());
			break;
		}
		return a;
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
			throw runtime_error(stringformat("Operator / not defined for type `%s`", getTypeName(a.type).c_str()).c_str());
			break;
		}
		return a;
	}

	inline KSValue operator += (KSValue& a, KSValue b) {
		upconvert(a, b);
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
			arr.push_back(b.getArray());
			return KSValue{ arr };
		}
		break;
		case KSType::LIST:
		{
			auto& list = a.getList();
			auto& blist = b.getList();
			list.insert(list.end(), blist.begin(), blist.end());
		}
		break;
		default:
			throw runtime_error(stringformat("Operator += not defined for type `%s`", getTypeName(a.type).c_str()).c_str());
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
			throw runtime_error(stringformat("Operator -= not defined for type `%s`", getTypeName(a.type).c_str()).c_str());
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
			throw runtime_error(stringformat("Operator *= not defined for type `%s`", getTypeName(a.type).c_str()).c_str());
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
			throw runtime_error(stringformat("Operator /= not defined for type `%s`", getTypeName(a.type).c_str()).c_str());
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
			throw runtime_error(stringformat("Operator %% not defined for type `%s`", getTypeName(a.type).c_str()).c_str());
			break;
		}
		return a;
	}

	// comparison operators
	KSValue operator != (KSValue a, KSValue b);
	inline KSValue operator == (KSValue a, KSValue b) {
		upconvert(a, b); // allow 5 == "5" to be true
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() == b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() == b.getFloat() };
			break;
		case KSType::VEC3:
			return KSValue{ a.getVec3() == b.getVec3() };
			break;
		case KSType::STRING:
			return KSValue{ a.getString() == b.getString() };
			break;
		case KSType::ARRAY:
			return KSValue{ a.getArray() == b.getArray() };
			break;
		case KSType::LIST:
		{
			auto& alist = a.getList();
			auto& blist = b.getList();
			if (alist.size() != blist.size()) {
				return KSValue{ 0 };
			}
			for (size_t i = 0; i < alist.size(); ++i) {
				if ((*alist[i] != *blist[i]).getBool()) {
					return KSValue{ 0 };
				}
			}
			return KSValue{ 1 };
		}
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator != (KSValue a, KSValue b) {
		upconvert(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() != b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() != b.getFloat() };
			break;
		case KSType::VEC3:
			return KSValue{ a.getVec3() != b.getVec3() };
			break;
		case KSType::STRING:
			return KSValue{ a.getString() != b.getString() };
			break;
		case KSType::ARRAY:
		case KSType::LIST:
			return KSValue{ !(a == b).getBool() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator || (KSValue& a, KSValue& b) {
		return a.getBool() || b.getBool();
	}

	inline KSValue operator && (KSValue& a, KSValue& b) {
		return a.getBool() && b.getBool();
	}

	inline KSValue operator < (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() < b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() < b.getFloat() };
			break;
		case KSType::STRING:
			return KSValue{ a.getString() < b.getString() };
			break;
		case KSType::ARRAY:
			return KSValue{ a.getArray().size() < b.getArray().size() };
			break;
		case KSType::LIST:
			return KSValue{ a.getList().size() < b.getList().size() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator > (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() > b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() > b.getFloat() };
			break;
		case KSType::STRING:
			return KSValue{ a.getString() > b.getString() };
			break;
		case KSType::ARRAY:
			return KSValue{ a.getArray().size() > b.getArray().size() };
			break;
		case KSType::LIST:
			return KSValue{ a.getList().size() > b.getList().size() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator <= (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() <= b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() <= b.getFloat() };
			break;
		case KSType::STRING:
			return KSValue{ a.getString() <= b.getString() };
			break;
		case KSType::ARRAY:
			return KSValue{ a.getArray().size() <= b.getArray().size() };
			break;
		case KSType::LIST:
			return KSValue{ a.getList().size() <= b.getList().size() };
			break;
		default:
			break;
		}
		return a;
	}

	inline KSValue operator >= (KSValue a, KSValue b) {
		upconvertThrowOnNonNumberToNumberCompare(a, b);
		switch (a.type) {
		case KSType::INT:
			return KSValue{ a.getInt() >= b.getInt() };
			break;
		case KSType::FLOAT:
			return KSValue{ a.getFloat() >= b.getFloat() };
			break;
		case KSType::STRING:
			return KSValue{ a.getString() >= b.getString() };
			break;
		case KSType::ARRAY:
			return KSValue{ a.getArray().size() >= b.getArray().size() };
			break;
		case KSType::LIST:
			return KSValue{ a.getList().size() >= b.getList().size() };
			break;
		default:
			break;
		}
		return a;
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

	// our basic function type
	struct KSFunction {
		string name;
		KSOperatorPrecedence opPrecedence;
		vector<string> argNames;
		KSScopeRef scope;

		// to calculate a result
		// either we have a KataScript body
		vector<KSExpressionRef> subexpressions;
		// or a KSLambda 
		KSLambda lambda;

		KSOperatorPrecedence getPrecedence() {
			if (name.size() > 2) {
				return KSOperatorPrecedence::func;
			}
            if (name == "||" || name == "&&") {
                return KSOperatorPrecedence::boolean;
            }
			if (contains("!<>|&"s, name[0]) || name == "==") {
				return KSOperatorPrecedence::compare;
			}
			if (contains(name, '=')) {
				return KSOperatorPrecedence::assign;
			}
			if (contains("/*%"s, name[0])) {
				return KSOperatorPrecedence::muldiv;
			}
			if (contains("-+"s, name[0])) {
				return KSOperatorPrecedence::addsub;
			}
			return KSOperatorPrecedence::func;
		}

		KSFunction(const string& name_, const KSLambda& l, const KSScopeRef& s) : scope(s), lambda(l), name(name_), opPrecedence(getPrecedence()) {}
		// when using a KataScript function body, the operator precedence will always be "func" level (aka the highest)
		KSFunction(const string& name_, const vector<string>& argNames_, const vector<KSExpressionRef>& body_, const KSScopeRef& s) 
			: scope(s), name(name_), subexpressions(body_), argNames(argNames_), opPrecedence(KSOperatorPrecedence::func) {}
		// default constructor makes a function with no args that returns void
		KSFunction(const string& name, const KSScopeRef& s) : KSFunction(name, [](KSList) { return make_shared<KSValue>(); }, s) {}
		KSFunction() : KSFunction("anon", nullptr) {}

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
		KSExpressionRef list;
		string iterateName;
		vector<KSExpressionRef> subexpressions;

		KSForeach(const KSForeach& o) {
			list = o.list;
			iterateName = o.iterateName;
			for (auto sub : o.subexpressions) {
				subexpressions.push_back(make_shared<KSExpression>(*sub));
			}
		}
		KSForeach() {}
	};

	enum class KSExpressionType : uint8_t {
		NONE,
		VALUE,
		FUNCTIONDEF,
		FUNCTIONCALL,
		RETURN,
		LOOP,
		FOREACH,
		IFELSE
	};

	// forward declare so we can use the parser to process functions
	class KataScriptInterpreter;
	// describes a 'generic' expression tree, with either a value or function at the root
	struct KSExpression {
		// either we have a value, or a function expression which then has sub-expressions
		KSExpressionType type;
		KSFunctionExpression expr;
		KSLoop loop;
		KSForeach foreach;
		KSIfElse ifelse;
		KSValueRef value;
		KSReturn returnExpression;
		KSExpressionRef parent = nullptr;

		KSExpression(KSValueRef val) : type(KSExpressionType::FUNCTIONCALL), expr(val), parent(nullptr) {}
		KSExpression(KSFunctionRef fnc, KSExpressionRef par) : type(KSExpressionType::FUNCTIONDEF), expr(fnc), parent(par) {}
		KSExpression(KSValueRef val, KSExpressionRef par) : type(KSExpressionType::VALUE), value(val), parent(par) {}
		KSExpression(KSForeach val, KSExpressionRef par = nullptr) : type(KSExpressionType::FOREACH), foreach(val), parent(par) {}
		KSExpression(KSLoop val, KSExpressionRef par = nullptr) : type(KSExpressionType::LOOP), loop(val), parent(par) {}
		KSExpression(KSIfElse val, KSExpressionRef par = nullptr) : type(KSExpressionType::IFELSE), ifelse(val), parent(par) {}
		KSExpression(KSReturn val, KSExpressionRef par = nullptr) : type(KSExpressionType::RETURN), returnExpression(val), parent(par) {}

		bool isCompletedIfElse() {
			return (ifelse.size() > 1 &&
				!ifelse.back().testExpression ||
				ifelse.back().testExpression->type == KSExpressionType::NONE
				);
		}

		KSExpressionRef back() {
			switch (type) {
			case KSExpressionType::FUNCTIONDEF:
				return expr.function->getFunction()->subexpressions.back();
				break;
			case KSExpressionType::FUNCTIONCALL:
				return expr.subexpressions.back();
				break;
			case KSExpressionType::LOOP:
				return loop.subexpressions.back();
				break;
			case KSExpressionType::FOREACH:
				return foreach.subexpressions.back();
				break;
			case KSExpressionType::IFELSE:
				return ifelse.back().subexpressions.back();
				break;
			default:
				break;
			}
			return nullptr;
		}

		void push_back(KSExpressionRef ref) {
			switch (type) {
			case KSExpressionType::FUNCTIONCALL:
				expr.subexpressions.push_back(ref);
				break;
			case KSExpressionType::FUNCTIONDEF:
				expr.function->getFunction()->subexpressions.push_back(ref);
				break;
			case KSExpressionType::LOOP:
				loop.subexpressions.push_back(ref);
				break;
			case KSExpressionType::FOREACH:
				foreach.subexpressions.push_back(ref);
				break;
			case KSExpressionType::IFELSE:
				ifelse.back().subexpressions.push_back(ref);
				break;
			default:
				break;
			}
		}

		void push_back(const KSIf& ref) {
			switch (type) {
			case KSExpressionType::IFELSE:
				ifelse.push_back(ref);
				break;
			default:
				break;
			}
		}

		// walk the tree depth first and replace any function expressions 
		// with a value expression of their results
		void consolidate(KataScriptInterpreter* i);
	};

	struct KSScope {
		// this is the main storage object for all functions and variables
		string name;
		KSScopeRef parent;
		bool keepAlive;
		unordered_map<string, KSValueRef> variables;
		unordered_map<string, KSScopeRef> scopes; 
		unordered_map<string, KSFunctionRef> functions;
		KSScope(const string& name_, KSScopeRef scope, bool persist = false) : name(name_), parent(scope), keepAlive(persist) {}
	};

	// state enum for state machine for token by token parsing
	enum class KSParseState : uint8_t {
		beginExpression,
		readLine,
		defineFunc,
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
		void newScope(const string& name, bool keepAlive = false);
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
			auto& aarr = get<vector<int>>(value);
			auto& barr = get<vector<int>>(o.value);
			for (size_t i = 0; i < size(); ++i) {
				if (aarr[i] != barr[i]) {
					return false;
				}
			}
		}
		break;
		case KSType::FLOAT:
		{
			auto& aarr = get<vector<float>>(value);
			auto& barr = get<vector<float>>(o.value);
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
	const string GrammarChars = " \t\n,(){}[];+-/*%<>=!&|\""s;
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
							throw runtime_error(stringformat("Quote mismatch at %s", input.substr(lpos, input.size() - lpos).c_str()).c_str());
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
			if (input[pos] == '-' && contains(NumericChars, input[pos + 1])) {
				pos = input.find_first_of(GrammarChars, pos + 1);
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

	// scope control lets you have object lifetimes
	void KataScriptInterpreter::newScope(const string& name, bool keepAlive) {
		// if the scope exists we just use it as is
		auto iter = currentScope->scopes.find(name);
		if (iter != currentScope->scopes.end()) {
			currentScope = iter->second;
		} else {
			auto parent = currentScope;
			auto& newScope = currentScope->scopes[name];
			newScope = make_shared<KSScope>(name, parent, keepAlive);
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
        return nullptr;
    }

	void KataScriptInterpreter::closeCurrentScope() {
		if (currentScope->parent) {
			if (currentScope->keepAlive) {
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

	// call function by reference
	KSValueRef KataScriptInterpreter::callFunction(const KSFunctionRef fnc, const KSList& args) {
		if (fnc->subexpressions.size()) {
            auto oldscope = currentScope;
            currentScope = resolveScope(fnc->name);
			int limit = (int)min(args.size(), fnc->argNames.size());
			for (int i = 0; i < limit; ++i) {
				*resolveVariable(fnc->argNames[i], fnc->scope) = *args[i];
			}
			KSValueRef returnVal = nullptr;
			for (auto&& sub : fnc->subexpressions) {
				if (sub->type == KSExpressionType::RETURN) {
					returnVal = getValue(sub);
					break;
				} else{
					getValue(sub);
				}
			}
            currentScope = oldscope;
			return returnVal ? returnVal : make_shared<KSValue>();
		} else {
			return fnc->lambda(args);
		}
	}

	KSFunctionRef& KataScriptInterpreter::newFunction(const string& name, const vector<string>& argNames, const vector<KSExpressionRef>& body) {
		auto& ref = currentScope->functions[name];
		auto parentScope = currentScope;
		newScope(name, true);
		for (auto&& arg : argNames) {
			currentScope->variables[arg] = make_shared<KSValue>();
		}
		ref = make_shared<KSFunction>(name, argNames, body, currentScope);
		auto& funcvar = resolveVariable(name, parentScope);
		funcvar->type = KSType::FUNCTION;
		funcvar->value = ref;
		return ref;
	}

	KSFunctionRef& KataScriptInterpreter::newFunction(const string& name, const KSLambda& lam) {
		auto& ref = currentScope->functions[name];
		auto parentScope = currentScope;
		newScope(name, true);
		closeCurrentScope();
		ref = make_shared<KSFunction>(name, lam, currentScope);
		auto& funcvar = resolveVariable(name, parentScope);
		funcvar->type = KSType::FUNCTION;
		funcvar->value = ref;
		return ref;
	}

	KSFunctionRef& KataScriptInterpreter::newLibraryFunction(const string& name, const KSLambda& lam, KSScopeRef scope) {
		auto oldScope = currentScope;
		currentScope = scope;
		auto& ref = newFunction(name, lam);
		currentScope = oldScope;
		return ref;
	}

	// name resolution for variables
	KSValueRef& KataScriptInterpreter::resolveVariable(const string& name, KSScopeRef scope) {
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
		return make_shared<KSFunction>(name, currentScope->scopes[name]);
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
						auto curfunc = curr->expr.function->getFunction();
						auto newfunc = root->expr.function->getFunction();
						if (curfunc && (int)curfunc->opPrecedence < (int)newfunc->opPrecedence) {
							while (curr->expr.subexpressions.back()->type == KSExpressionType::FUNCTIONCALL) {
								auto curfunc = curr->expr.subexpressions.back()->expr.function->getFunction();
								if (curfunc && (int)curfunc->opPrecedence < (int)newfunc->opPrecedence) {
									curr = curr->expr.subexpressions.back();
								} else {
									break;
								}
							}
							// swap values around to correct the otherwise incorect order of operations
							root->expr.subexpressions.push_back(curr->expr.subexpressions.back());
							curr->expr.subexpressions.pop_back();
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
							root->expr.subexpressions.push_back(getExpression(move(minisub)));
							curr->expr.subexpressions.push_back(root);
							root = prev;
						} else {
							root->expr.subexpressions.push_back(curr);
						}
					} else {
						root->expr.subexpressions.push_back(curr);
					}
				}
			} else if (isStringLiteral(strings[i])) {
				// trim quotation marks
				auto val = strings[i].substr(1, strings[i].size() - 2);
				auto newExpr = make_shared<KSExpression>(make_shared<KSValue>(val), nullptr);
				if (root) {
					root->expr.subexpressions.push_back(newExpr);
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
								&& (root->expr.function->getFunction()->opPrecedence == KSOperatorPrecedence::func)) {

								cur = make_shared<KSExpression>(make_shared<KSValue>());
								cur->expr.subexpressions.push_back(root);
								root = cur;
							} else {
								root->expr.subexpressions.push_back(make_shared<KSExpression>(resolveVariable("identity", modules[0])));
								cur = root->expr.subexpressions.back();
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
							root->expr.subexpressions.push_back(make_shared<KSExpression>(var));
							cur = root->expr.subexpressions.back();
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
								cur->expr.subexpressions.push_back(getExpression(move(minisub)));
								minisub.clear();
							}
						} else if (strings[i] == ")" || strings[i] == "]") {
							if (--nestLayers > 0) {
								minisub.push_back(strings[i]);
							} else {
								if (minisub.size()) {
									cur->expr.subexpressions.push_back(getExpression(move(minisub)));
									minisub.clear();
								}
							}
						} else if (strings[i] == "(" || strings[i] == "[" || !(strings[i].size() == 1 && contains("+-*%/"s, strings[i][0])) && i + 2 < strings.size() && strings[i + 1] == "(") {
							++nestLayers;
							if (strings[i] == "(") {
								minisub.push_back(strings[i]);
							} else {
								minisub.push_back(strings[i]);
								minisub.push_back(strings[++i]);
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
							root->expr.subexpressions.push_back(make_shared<KSExpression>(make_shared<KSValue>(KSList()), nullptr));
							cur = root->expr.subexpressions.back();
						} else {
							root = make_shared<KSExpression>(make_shared<KSValue>(KSList()), nullptr);
							cur = root;
						}
						vector<string> minisub;
						int nestLayers = 1;
						while (nestLayers > 0 && ++i < strings.size()) {
							if (nestLayers == 1 && strings[i] == ",") {
								if (minisub.size()) {
									cur->value->getList().push_back(getValue(move(minisub)));
									minisub.clear();
								}
							} else if (strings[i] == "]" || strings[i] == ")") {
								if (--nestLayers > 0) {
									minisub.push_back(strings[i]);
								} else {
									if (minisub.size()) {
										cur->value->getList().push_back(getValue(move(minisub)));
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
						auto& list = cur->value->getList();
						if (list.size()) {
							bool canBeArray = true;
							auto type = list[0]->type;
							for (auto& val : list) {
								if (val->type != type || (int)val->type >= (int)KSType::ARRAY) {
									canBeArray = false;
									break;
								}
							}
							if (canBeArray) {
								cur->value->hardconvert(KSType::ARRAY);
							}
						}
					} else {
						// list access
						auto indexexpr = make_shared<KSExpression>(resolveVariable("listindex", modules[0]));
						if (indexOfIndex) {
                            cur = root;
                            auto parent = root;
                            while (cur->type == KSExpressionType::FUNCTIONCALL && cur->expr.function->getFunction()->opPrecedence != KSOperatorPrecedence::func) {
                                parent = cur;
                                cur = cur->expr.subexpressions.back();
                            }
                            indexexpr->expr.subexpressions.push_back(cur);
                            if (cur == root) {
                                root = indexexpr;
                                cur = indexexpr;
                            } else {
                                parent->expr.subexpressions.pop_back();
                                parent->expr.subexpressions.push_back(indexexpr);
                                cur = parent->expr.subexpressions.back();
                            }
						} else {
							if (root) {
								root->expr.subexpressions.push_back(indexexpr);
								cur = root->expr.subexpressions.back();
							} else {
								root = indexexpr;
								cur = root;
							}
							auto var = resolveVariable(strings[i]);
							if (var->type != KSType::NONE && var->type != KSType::LIST && var->type != KSType::ARRAY) {
								var = make_shared<KSValue>(var->value, var->type);
								var->upconvert(KSType::LIST);
							}
							cur->expr.subexpressions.push_back(make_shared<KSExpression>(var, nullptr));
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
										cur->expr.subexpressions.push_back(getExpression(move(minisub)));
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
						newExpr = make_shared<KSExpression>(make_shared<KSValue>(1), nullptr);
					} else if (strings[i] == "false") {
						newExpr = make_shared<KSExpression>(make_shared<KSValue>(0), nullptr);
					} else if (strings[i] == "null") {
						newExpr = make_shared<KSExpression>(make_shared<KSValue>(), nullptr);
					} else {
						newExpr = make_shared<KSExpression>(resolveVariable(strings[i]), nullptr);
					}
					if (root) {
						root->expr.subexpressions.push_back(newExpr);
					} else {
						root = newExpr;
					}
				}
			} else {
				// number
				auto val = fromChars(strings[i]);
				auto newExpr = make_shared<KSExpression>(KSValueRef(contains(strings[i], '.') ? new KSValue((float)val) : new KSValue((int)val)), nullptr);
				if (root) {
					root->expr.subexpressions.push_back(newExpr);
				} else {
					root = newExpr;
				}
			}
			++i;
		}

		return root;
	}

	// evaluate an expression
	void KSExpression::consolidate(KataScriptInterpreter* i) {
		switch (type) {
		case KSExpressionType::RETURN:
			value = i->getValue(returnExpression.expression);
			type = KSExpressionType::VALUE;
			break;
		case KSExpressionType::FUNCTIONCALL:
		{
			KSList args;
			for (auto&& sub : expr.subexpressions) {
				sub->consolidate(i);
				args.push_back(sub->value);
			}
			if (expr.function->type == KSType::NONE) {
				if (args.size() && args[0]->type == KSType::FUNCTION) {
					expr.function = args[0];
					args.erase(args.begin());
				} else {
					throw runtime_error("Unable to call non-existant function");
				}
			}
			value = i->callFunction(expr.function->getFunction(), args);
			type = KSExpressionType::VALUE;
			expr.clear();
		}
			break;
		case KSExpressionType::LOOP:
		{
			i->newScope("loop");
			if (loop.initExpression) {
				i->getValue(loop.initExpression);
			}
			while (i->getValue(loop.testExpression)->getBool()) {				
				for (auto&& expr : loop.subexpressions) {
					i->getValue(expr);
				}
				if (loop.iterateExpression) {
					i->getValue(loop.iterateExpression);
				}
			}
			value = 0;
			type = KSExpressionType::VALUE;
			i->closeCurrentScope();
		}
			break;
		case KSExpressionType::FOREACH:
		{
			i->newScope("loop");
			auto& var = i->resolveVariable(foreach.iterateName);
			auto list = *foreach.list;
			list.consolidate(i);
			if (list.value->type == KSType::LIST) {
				for (auto&& in : list.value->getList()) {
					*var = *in;
					for (auto&& expr : foreach.subexpressions) {
						i->getValue(expr);
					}
				}
			} else if (list.value->type == KSType::ARRAY) {
				auto& arr = list.value->getArray();
				switch (arr.type) {
				case KSType::INT:
					for (auto&& in : list.value->getStdVector<int>()) {
						*var = KSValue(in);
						for (auto&& expr : foreach.subexpressions) {
							i->getValue(expr);
						}
					}
					break;
				case KSType::FLOAT:
					for (auto&& in : list.value->getStdVector<float>()) {
						*var = KSValue(in);
						for (auto&& expr : foreach.subexpressions) {
							i->getValue(expr);
						}
					}
					break;
				case KSType::VEC3:
					for (auto&& in : list.value->getStdVector<vec3>()) {
						*var = KSValue(in);
						for (auto&& expr : foreach.subexpressions) {
							i->getValue(expr);
						}
					}
					break;
				case KSType::STRING:
					for (auto&& in : list.value->getStdVector<string>()) {
						*var = KSValue(in);
						for (auto&& expr : foreach.subexpressions) {
							i->getValue(expr);
						}
					}
					break;
				default:
					break;
				}
				
			}
			value = 0;
			type = KSExpressionType::VALUE;
			i->closeCurrentScope();
		}
		break;
		case KSExpressionType::IFELSE:
		{
			for (auto& express : ifelse) {
				if (!express.testExpression || i->getValue(express.testExpression)->getBool()) {
					i->newScope("ifelse");
					for (auto sub : express.subexpressions) {
						i->getValue(sub);
					}
					i->closeCurrentScope();
					break;
				}
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
		return expr->value;
	}

	// evaluate an expression from expressionRef
	KSValueRef KataScriptInterpreter::getValue(KSExpressionRef exp) {
		// copy the expression so that we don't lose it when we consolidate
		auto expr = *exp;
		expr.consolidate(this);
		return expr.value;
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
			if (currentExpression->type != KSExpressionType::IFELSE || currentExpression->isCompletedIfElse()) {
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

	bool lastStatementClosed = false;
	// parse one token at a time, uses the state machine
	void KataScriptInterpreter::parse(const string& token) {
		switch (parseState) {
		case KSParseState::beginExpression:
		{
			bool wasElse = false;
			bool closeScope = false;
			bool closedExpr = false;
			if (token == "func") {
				parseState = KSParseState::defineFunc;
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
				wasElse = true;
			} else if (token == "else") {
				parseState = KSParseState::expectIfEnd;
				wasElse = true;
			} else if (token == "{") {
				bool isFunc = currentExpression && currentExpression->type == KSExpressionType::FUNCTIONDEF;
				if (!isFunc) {
					newScope("anon"s);
				}
				clearParseStacks();
			} else if (token == "}") {
				closedExpr = closeCurrentExpression();
				closeCurrentScope();
				closeScope = true;
				wasElse = true;
			} else if (token == "return") {
				parseState = KSParseState::returnLine;
			} else if (token == ";") {
				clearParseStacks();
			} else {
				parseState = KSParseState::readLine;
				parseStrings.push_back(token);
			}
			if (!closedExpr && 
				(closeScope && (lastStatementClosed || (currentExpression && currentExpression->isCompletedIfElse())))
                || (!wasElse && lastStatementClosed)) {
                bool wasIfExpr = currentExpression && currentExpression->type == KSExpressionType::IFELSE;
				closeDanglingIfExpression();
                if (lastStatementClosed && wasIfExpr) {
                    closeCurrentExpression();
                }
			}
			lastStatementClosed = closeScope;
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
					switch (exprs.size()) {
					case 1:
						currentExpression->loop.testExpression = getExpression(move(exprs[0]));
						break;
					case 2:
						currentExpression->loop.testExpression = getExpression(move(exprs[0]));
						currentExpression->loop.iterateExpression = getExpression(move(exprs[1]));
						break;
					case 3:
						currentExpression->loop.initExpression = getExpression(move(exprs[0]));
						currentExpression->loop.testExpression = getExpression(move(exprs[1]));
						currentExpression->loop.iterateExpression = getExpression(move(exprs[2]));
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
					currentExpression->foreach.iterateName = exprs[0][0];
					currentExpression->foreach.list = getExpression(exprs[1]);

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
					currentExpression->ifelse.back().testExpression = getExpression(move(parseStrings));
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
            if (token == "=") {
                resolveVariable(parseStrings[0]);
                parseStrings.push_back(token);
            }
            else if (token == ";") {
				auto line = move(parseStrings);
				clearParseStacks();
				if (!currentExpression) {
					getValue(move(line));
				} else {
					currentExpression->push_back(getExpression(move(line)));
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
	}

	void KataScriptInterpreter::readLine(const string& text) {
		try {
			for (auto& token : KSTokenize(text)) {
				parse(move(token));
			}
		} catch (std::exception e) {
			printf("Error: %s\n", e.what());
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
		modules.push_back(make_shared<KSScope>("StandardLib"s, nullptr, true));
		auto libscope = modules.back();
		newLibraryFunction("identity", [](KSList args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			return args[0];
			}, libscope);

		newLibraryFunction("typeof", [](KSList args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			return make_shared<KSValue>(getTypeName(args[0]->type));
			}, libscope);

		newLibraryFunction("listindex", [](KSList args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			if (args.size() == 1) {
				return args[0];
			}
			args[1]->hardconvert(KSType::INT);
			auto ival = args[1]->getInt();

			auto var = args[0];
			if (var->type == KSType::ARRAY) {
				auto& arr = var->getArray();
				if (ival < 0 || ival >= arr.size()) {
					throw runtime_error(stringformat("Out of bounds array access index %i, array length %i",
						ival, arr.size()).c_str());
				} else {
					switch (arr.type) {
					case KSType::INT:
						return make_shared<KSValue>(get<vector<int>>(arr.value)[ival]);
						break;
					case KSType::FLOAT:
						return make_shared<KSValue>(get<vector<float>>(arr.value)[ival]);
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
			} else {
				if (var->type != KSType::LIST) {
					var = make_shared<KSValue>(var->value, var->type);
					var->upconvert(KSType::LIST);
				}
				auto& list = var->getList();
				if (ival < 0 || ival >= list.size()) {
					throw runtime_error(stringformat("Out of bounds list access index %i, list length %i",
						ival, list.size()).c_str());
				} else {
					return list[ival];
				}
			}
			}, libscope);

		newLibraryFunction("sqrt", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>(0.f);
			}
			args[0]->hardconvert(KSType::FLOAT);
			args[0]->value = sqrtf(get<float>(args[0]->value));
			return args[0];
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
			return make_shared<KSValue>(s);
			}, libscope);

		newLibraryFunction("=", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			if (args.size() == 1) {
				return args[0];
			}
			*args[0] = *args[1];
			return args[0];
			}, libscope);

		newLibraryFunction("+", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			if (args.size() == 1) {
				return args[0];
			}
			return make_shared<KSValue>(*args[0] + *args[1]);
			}, libscope);

		newLibraryFunction("-", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			if (args.size() == 1) {
				auto zero = KSValue(0);
				upconvert(*args[0], zero);
				return make_shared<KSValue>(zero - *args[0]);
			}
			return make_shared<KSValue>(*args[0] - *args[1]);
			}, libscope);

		newLibraryFunction("*", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>();
			}
			return make_shared<KSValue>(*args[0] * *args[1]);
			}, libscope);

		newLibraryFunction("/", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>();
			}
			return make_shared<KSValue>(*args[0] / *args[1]);
			}, libscope);

		newLibraryFunction("%", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>();
			}
			return make_shared<KSValue>(*args[0] % *args[1]);
			}, libscope);

		newLibraryFunction("==", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(*args[0] == *args[1]);
			}, libscope);

		newLibraryFunction("!=", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(*args[0] != *args[1]);
			}, libscope);

		newLibraryFunction("||", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(1);
			}
			return make_shared<KSValue>(*args[0] || *args[1]);
			}, libscope);

		newLibraryFunction("&&", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(*args[0] && *args[1]);
			}, libscope);

		newLibraryFunction("++", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			auto t = KSValue(1);
			*args[0] = *args[0] + t;
			return args[0];
			}, libscope);

		newLibraryFunction("--", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>();
			}
			auto t = KSValue(1);
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

		newLibraryFunction(">", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(*args[0] > *args[1]);
			}, libscope);

		newLibraryFunction("<", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(*args[0] < *args[1]);
			}, libscope);

		newLibraryFunction(">=", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(*args[0] >= *args[1]);
			}, libscope);

		newLibraryFunction("<=", [](const KSList& args) {
			if (args.size() < 2) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(*args[0] <= *args[1]);
			}, libscope);

		newLibraryFunction("!", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>(0);
			}
			return make_shared<KSValue>(!args[0]->getBool());
			}, libscope);

		newLibraryFunction("length", [](const KSList& args) {
			if (args.size() == 0 || (int)args[0]->type < (int)KSType::STRING) {
				return make_shared<KSValue>(0);
			}
			if (args[0]->type == KSType::STRING) {
				return make_shared<KSValue>((int)args[0]->getString().size());
			}
			if (args[0]->type == KSType::ARRAY) {
				return make_shared<KSValue>((int)args[0]->getArray().size());
			}
			return make_shared<KSValue>((int)args[0]->getList().size());
			}, libscope);

        newLibraryFunction("find", [](const KSList& args) {
            if (args.size() < 2 || (int)args[0]->type < (int)KSType::ARRAY) {
                return make_shared<KSValue>();
            }
            if (args[0]->type == KSType::ARRAY) {
                auto item = *args[1];
                switch (args[0]->getArray().type) {
                case KSType::INT:
                {                    
                    item.hardconvert(KSType::INT);
                    auto lookfor = item.getInt();
                    auto& arry = args[0]->getStdVector<int>();
                    auto iter = find(arry.begin(), arry.end(), lookfor);
                    if (iter == arry.end()) {
                        return make_shared<KSValue>();
                    }
                    return make_shared<KSValue>((int)(iter - arry.begin()));
                }
                    break;
                case KSType::FLOAT:
                {
                    item.hardconvert(KSType::FLOAT);
                    auto lookfor = item.getFloat();
                    auto& arry = args[0]->getStdVector<float>();
                    auto iter = find(arry.begin(), arry.end(), lookfor);
                    if (iter == arry.end()) {
                        return make_shared<KSValue>();
                    }
                    return make_shared<KSValue>((int)(iter - arry.begin()));
                }
                    break;
                case KSType::VEC3:
                {
                    item.hardconvert(KSType::VEC3);
                    auto lookfor = item.getVec3();
                    auto& arry = args[0]->getStdVector<vec3>();
                    auto iter = find(arry.begin(), arry.end(), lookfor);
                    if (iter == arry.end()) {
                        return make_shared<KSValue>();
                    }
                    return make_shared<KSValue>((int)(iter - arry.begin()));
                }
                    break;
                case KSType::STRING:
                {
                    item.hardconvert(KSType::STRING);
                    auto lookfor = item.getString();
                    auto& arry = args[0]->getStdVector<string>();
                    auto iter = find(arry.begin(), arry.end(), lookfor);
                    if (iter == arry.end()) {
                        return make_shared<KSValue>();
                    }
                    return make_shared<KSValue>((int)(iter - arry.begin()));
                }
                    break;
                default:
                    break;
                }
                return make_shared<KSValue>((int)args[0]->getArray().size());
            }
            auto& list = args[0]->getList();
            for (int i = 0; i < list.size(); ++i) {
                if ((*list[i] == *args[1]).getBool()) {
                    return make_shared<KSValue>(i);
                }
            }
            return make_shared<KSValue>();
            }, libscope);

        newLibraryFunction("split", [](const KSList& args) {
            if (args.size() > 1 && args[0]->type == KSType::STRING) {
                if (args.size() == 1) {
                    return args[0];
                }
                return make_shared<KSValue>(KSArray(split(args[0]->getString(), args[1]->getString())));
            }
            return make_shared<KSValue>();
            }, libscope);

		newLibraryFunction("bool", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>(0);
			}
			auto val = *args[0];
			val.hardconvert(KSType::INT);
			val.value = args[0]->getBool();
			return make_shared<KSValue>(val);
			}, libscope);

		newLibraryFunction("int", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>(0);
			}
			auto val = *args[0];
			val.hardconvert(KSType::INT);
			return make_shared<KSValue>(val);
			}, libscope);

		newLibraryFunction("float", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>(0.f);
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
				return make_shared<KSValue>(vec3(val.getFloat()));
			}
			auto x = *args[0];
			x.hardconvert(KSType::FLOAT);
			auto y = *args[1];
			y.hardconvert(KSType::FLOAT);
			auto z = *args[2];
			z.hardconvert(KSType::FLOAT);
			return make_shared<KSValue>(vec3(x.getFloat(), y.getFloat(), z.getFloat()));
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
			if (args.size() == 1) {
				auto val = *args[0];
				val.hardconvert(KSType::ARRAY);
				return make_shared<KSValue>(val);
			}
			auto list = make_shared<KSValue>(args);
			list->hardconvert(KSType::ARRAY);
			return list;
			}, libscope);

		newLibraryFunction("list", [](const KSList& args) {
			if (args.size() == 0) {
				return make_shared<KSValue>(""s);
			}
			if (args.size() == 1) {
				auto val = *args[0];
				val.hardconvert(KSType::LIST);
				return make_shared<KSValue>(val);
			}
			return make_shared<KSValue>(args);
			}, libscope);
	}
#endif
}
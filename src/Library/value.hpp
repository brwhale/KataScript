#pragma once

namespace KataScript {
    // Now that we have our collection types defined, we can finally define our value variant
    using ValueVariant =
        variant<
        Int,
        Float,
        vec3,
        FunctionRef,
        void*,
        string,
        Array,
        List,
        Dictionary,
        ClassRef
        >;

    // our basic Object/Value type
    struct Value {
        Type type;
        ValueVariant value;

        // Construct a Value from any underlying type
        Value() : type(Type::Null) {}
        Value(bool a) : type(Type::Int), value(static_cast<Int>(a)) {}
        Value(Int a) : type(Type::Int), value(a) {}
        Value(Float a) : type(Type::Float), value(a) {}
        Value(vec3 a) : type(Type::Vec3), value(a) {}
        Value(FunctionRef a) : type(Type::Function), value(a) {}
        Value(void* a) : type(Type::UserPointer), value(a) {}
        Value(string a) : type(Type::String), value(a) {}
        Value(const char* a) : type(Type::String), value(string(a)) {}
        Value(Array a) : type(Type::Array), value(a) {}
        Value(List a) : type(Type::List), value(a) {}
        Value(Dictionary a) : type(Type::Dictionary), value(a) {}
        Value(ClassRef a) : type(Type::Class), value(a) {}
        Value(ValueVariant a, Type t) : type(t), value(a) {}
        ~Value() {};

        // get a string that represents this value
        string getPrintString() {
            auto t = *this;
            t.hardconvert(Type::String);
            return get<string>(t.value);
        }

        // get this value as an int
        Int& getInt() {
            return get<Int>(value);
        }

        // get this value as a float
        Float& getFloat() {
            return get<Float>(value);
        }

        // get this value as a vec3
        vec3& getVec3() {
            return get<vec3>(value);
        }

        // get this value as a function
        FunctionRef& getFunction() {
            return get<FunctionRef>(value);
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
        Array& getArray() {
            return get<Array>(value);
        }

        // get this value as an std::vector<T>
        template <typename T>
        vector<T>& getStdVector();

        // get this value as a list
        List& getList() {
            return get<List>(value);
        }

        Dictionary& getDictionary() {
            return get<Dictionary>(value);
        }

        ClassRef& getClass() {
            return get<ClassRef>(value);
        }

        // get a boolean representing the truthiness of this value
        bool getBool() {
            // non zero or "true" are true
            bool truthiness = false;
            switch (type) {
            case Type::Int:
                truthiness = getInt();
                break;
            case Type::Float:
                truthiness = getFloat() != 0;
                break;
            case Type::Vec3:
                truthiness = getVec3() != vec3(0);
                break;
            case Type::String:
                truthiness = getString().size() > 0;
                break;
            case Type::Array:
                truthiness = getArray().size() > 0;
                break;
            case Type::List:
                truthiness = getList().size() > 0;
                break;
            default:
                break;
            }
            return truthiness;
        }

        size_t getHash();

        // convert this value up to the newType
        void upconvert(Type newType);

        // convert this value to the newType even if it's a downcast
        void hardconvert(Type newType);
    };

    // cout << operators for examples

    // define cout operator for List
    inline std::ostream& operator<<(std::ostream& os, const List& values) {
        os << Value(values).getPrintString();

        return os;
    }

    // define cout operator for Array
    inline std::ostream& operator<<(std::ostream& os, const Array& arr) {
        os << Value(arr).getPrintString();

        return os;
    }

    // define cout operator for Dictionary
    inline std::ostream& operator<<(std::ostream& os, const Dictionary& dict) {
        os << Value(dict).getPrintString();

        return os;
    }

    // define cout operator for Class
    inline std::ostream& operator<<(std::ostream& os, const ClassRef& dict) {
        os << Value(dict).getPrintString();

        return os;
    }

    // functions for working with Values

    // makes both values have matching types
    inline void upconvert(Value& a, Value& b) {
        if (a.type != b.type) {
            if (a.type < b.type) {
                a.upconvert(b.type);
            } else {
                b.upconvert(a.type);
            }
        }
    }

    // makes both values have matching types but doesn't allow converting between numbers and non-numbers
    inline void upconvertThrowOnNonNumberToNumberCompare(Value& a, Value& b) {
        if (a.type != b.type) {
            if (max((int)a.type, (int)b.type) > (int)Type::Vec3) {
                throw Exception(
                    "Types `"s + getTypeName(a.type) + " " + a.getPrintString() + "` and `" 
                    + getTypeName(b.type) + " " + b.getPrintString() + "` are incompatible for this operation");
            }
            if (a.type < b.type) {
                a.upconvert(b.type);
            } else {
                b.upconvert(a.type);
            }
        }
    }

    // math operators
    inline Value operator + (Value a, Value b) {
        upconvert(a, b);
        switch (a.type) {
        case Type::Int:
            return Value{ a.getInt() + b.getInt() };
            break;
        case Type::Float:
            return Value{ a.getFloat() + b.getFloat() };
            break;
        case Type::Vec3:
            return Value{ a.getVec3() + b.getVec3() };
            break;
        case Type::String:
            return Value{ a.getString() + b.getString() };
            break;
        case Type::Array:
        {
            auto arr = Array(a.getArray());
            arr.push_back(b.getArray());
            return Value{ arr };
        }
        break;
        case Type::List:
        {
            auto list = List(a.getList());
            auto& blist = b.getList();
            list.insert(list.end(), blist.begin(), blist.end());
            return Value{ list };
        }
        break;
        case Type::Dictionary:
        {
            auto list = Dictionary(a.getDictionary());
            list.merge(b.getDictionary());
            return Value{ list };
        }
        break;
        default:
            throw Exception("Operator + not defined for type `"s + getTypeName(a.type) + "`");
            break;
        }
    }

    inline Value operator - (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.type) {
        case Type::Int:
            return Value{ a.getInt() - b.getInt() };
            break;
        case Type::Float:
            return Value{ a.getFloat() - b.getFloat() };
            break;
        case Type::Vec3:
            return Value{ a.getVec3() - b.getVec3() };
            break;
        default:
            throw Exception("Operator - not defined for type `"s + getTypeName(a.type) + "`");
            break;
        }
    }

    inline Value operator * (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.type) {
        case Type::Int:
            return Value{ a.getInt() * b.getInt() };
            break;
        case Type::Float:
            return Value{ a.getFloat() * b.getFloat() };
            break;
        case Type::Vec3:
            return Value{ a.getVec3() * b.getVec3() };
            break;
        default:
            throw Exception("Operator * not defined for type `"s + getTypeName(a.type) + "`");
            break;
        }
    }

    inline Value operator / (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.type) {
        case Type::Int:
            return Value{ a.getInt() / b.getInt() };
            break;
        case Type::Float:
            return Value{ a.getFloat() / b.getFloat() };
            break;
        case Type::Vec3:
            return Value{ a.getVec3() / b.getVec3() };
            break;
        default:
            throw Exception("Operator / not defined for type `"s + getTypeName(a.type) + "`");
            break;
        }
    }

    inline Value operator += (Value& a, Value b) {
        if ((int)a.type < (int)Type::Array || b.type == Type::List) {
            upconvert(a, b);
        }
        switch (a.type) {
        case Type::Int:
            a.getInt() += b.getInt();
            break;
        case Type::Float:
            a.getFloat() += b.getFloat();
            break;
        case Type::Vec3:

            a.getVec3() += b.getVec3();
            break;
        case Type::String:
            a.getString() += b.getString();
            break;
        case Type::Array:
        {
            auto& arr = a.getArray();
            if (arr.type == b.type
                || (b.type == Type::Array && b.getArray().type == arr.type)) {
                switch (b.type) {
                case Type::Int:
                    arr.push_back(b.getInt());
                    break;
                case Type::Float:
                    arr.push_back(b.getFloat());
                    break;
                case Type::Vec3:
                    arr.push_back(b.getVec3());
                    break;
                case Type::Function:
                    arr.push_back(b.getFunction());
                    break;
                case Type::String:
                    arr.push_back(b.getString());
                    break;
                case Type::UserPointer:
                    arr.push_back(b.getPointer());
                    break;
                case Type::Array:
                    arr.push_back(b.getArray());
                    break;
                default:
                    break;
                }
            }
            return Value{ arr };
        }
        break;
        case Type::List:
        {
            auto& list = a.getList();
            switch (b.type) {
            case Type::Int:
            case Type::Float:
            case Type::Vec3:
            case Type::Function:
            case Type::String:
            case Type::UserPointer:
                list.push_back(std::make_shared<Value>(b.value, b.type));
                break;
            default:
            {
                b.upconvert(Type::List);
                auto& blist = b.getList();
                list.insert(list.end(), blist.begin(), blist.end());
            }
            break;
            }
        }
        break;
        case Type::Dictionary:
        {
            auto& dict = a.getDictionary();
            b.upconvert(Type::Dictionary);
            dict.merge(b.getDictionary());
        }
        break;
        default:
            throw Exception("Operator += not defined for type `"s + getTypeName(a.type) + "`");
            break;
        }
        return a;
    }

    inline Value operator -= (Value& a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.type) {
        case Type::Int:
            a.getInt() -= b.getInt();
            break;
        case Type::Float:
            a.getFloat() -= b.getFloat();
            break;
        case Type::Vec3:
            a.getVec3() -= b.getVec3();
            break;
        default:
            throw Exception("Operator -= not defined for type `"s + getTypeName(a.type) + "`");
            break;
        }
        return a;
    }

    inline Value operator *= (Value& a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.type) {
        case Type::Int:
            a.getInt() *= b.getInt();
            break;
        case Type::Float:
            a.getFloat() *= b.getFloat();
            break;
        case Type::Vec3:
            a.getVec3() *= b.getVec3();
            break;
        default:
            throw Exception("Operator *= not defined for type `"s + getTypeName(a.type) + "`");
            break;
        }
        return a;
    }

    inline Value operator /= (Value& a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.type) {
        case Type::Int:
            a.getInt() /= b.getInt();
            break;
        case Type::Float:
            a.getFloat() /= b.getFloat();
            break;
        case Type::Vec3:
            a.getVec3() /= b.getVec3();
            break;
        default:
            throw Exception("Operator /= not defined for type `"s + getTypeName(a.type) + "`");
            break;
        }
        return a;
    }

    inline Value operator % (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.type) {
        case Type::Int:
            return Value{ a.getInt() % b.getInt() };
            break;
        case Type::Float:
            return Value{ std::fmod(a.getFloat(), b.getFloat()) };
            break;
        default:
            throw Exception("Operator %% not defined for type `"s + getTypeName(a.type) + "`");
            break;
        }
    }

    // comparison operators
    bool operator != (Value a, Value b);
    inline bool operator == (Value a, Value b) {
        if (a.type != b.type) {
            return false;
        }
        switch (a.type) {
        case Type::Null:
            return true;
        case Type::Int:
            return a.getInt() == b.getInt();
            break;
        case Type::Float:
            return a.getFloat() == b.getFloat();
            break;
        case Type::Vec3:
            return a.getVec3() == b.getVec3();
            break;
        case Type::String:
            return a.getString() == b.getString();
            break;
        case Type::Array:
            return a.getArray() == b.getArray();
            break;
        case Type::List:
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
            throw Exception("Operator == not defined for type `"s + getTypeName(a.type) + "`");
            break;
        }
        return true;
    }

    inline bool operator != (Value a, Value b) {
        if (a.type != b.type) {
            return true;
        }
        switch (a.type) {
        case Type::Null:
            return false;
        case Type::Int:
            return a.getInt() != b.getInt();
            break;
        case Type::Float:
            return a.getFloat() != b.getFloat();
            break;
        case Type::Vec3:
            return a.getVec3() != b.getVec3();
            break;
        case Type::String:
            return a.getString() != b.getString();
            break;
        case Type::Array:
        case Type::List:
            return !(a == b);
            break;
        default:
            throw Exception("Operator != not defined for type `"s + getTypeName(a.type) + "`");
            break;
        }
        return false;
    }

    inline bool operator || (Value& a, Value& b) {
        return a.getBool() || b.getBool();
    }

    inline bool operator && (Value& a, Value& b) {
        return a.getBool() && b.getBool();
    }

    inline bool operator < (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.type) {
        case Type::Int:
            return a.getInt() < b.getInt();
            break;
        case Type::Float:
            return a.getFloat() < b.getFloat();
            break;
        case Type::String:
            return a.getString() < b.getString();
            break;
        case Type::Array:
            return a.getArray().size() < b.getArray().size();
            break;
        case Type::List:
            return a.getList().size() < b.getList().size();
            break;
        case Type::Dictionary:
            return a.getDictionary().size() < b.getDictionary().size();
            break;
        default:
            break;
        }
        return false;
    }

    inline bool operator > (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.type) {
        case Type::Int:
            return a.getInt() > b.getInt();
            break;
        case Type::Float:
            return a.getFloat() > b.getFloat();
            break;
        case Type::String:
            return a.getString() > b.getString();
            break;
        case Type::Array:
            return a.getArray().size() > b.getArray().size();
            break;
        case Type::List:
            return a.getList().size() > b.getList().size();
            break;
        case Type::Dictionary:
            return a.getDictionary().size() > b.getDictionary().size();
            break;
        default:
            break;
        }
        return false;
    }

    inline bool operator <= (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.type) {
        case Type::Int:
            return a.getInt() <= b.getInt();
            break;
        case Type::Float:
            return a.getFloat() <= b.getFloat();
            break;
        case Type::String:
            return a.getString() <= b.getString();
            break;
        case Type::Array:
            return a.getArray().size() <= b.getArray().size();
            break;
        case Type::List:
            return a.getList().size() <= b.getList().size();
            break;
        case Type::Dictionary:
            return a.getDictionary().size() <= b.getDictionary().size();
            break;
        default:
            break;
        }
        return false;
    }

    inline bool operator >= (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.type) {
        case Type::Int:
            return a.getInt() >= b.getInt();
            break;
        case Type::Float:
            return a.getFloat() >= b.getFloat();
            break;
        case Type::String:
            return a.getString() >= b.getString();
            break;
        case Type::Array:
            return a.getArray().size() >= b.getArray().size();
            break;
        case Type::List:
            return a.getList().size() >= b.getList().size();
            break;
        case Type::Dictionary:
            return a.getDictionary().size() >= b.getDictionary().size();
            break;
        default:
            break;
        }
        return false;
    }
}
#pragma once

namespace KataScript {
    struct Null {};
    // Now that we have our collection types defined, we can finally define our value variant
    using ValueVariant =
        variant<
        Null,
        Int,
        Float,
        vec3,
        FunctionRef,
        UserPointer,
        string,
        Array,
        ArrayMember,
        List,
        DictionaryRef,
        ClassRef
        >;

    // our basic Object/Value type
    struct Value {
        ValueVariant value;

        // Construct a Value from any underlying type
        explicit Value() : value(Null{}) {}
        explicit Value(bool a) :value(static_cast<Int>(a)) {}
        explicit Value(int a) :value(static_cast<Int>(a)) {}
        explicit Value(Int a) : value(a) {}
        explicit Value(Float a) : value(a) {}
        explicit Value(vec3 a) : value(a) {}
        explicit Value(FunctionRef a) : value(a) {}
        explicit Value(UserPointer a) : value(a) {}
        explicit Value(string a) : value(a) {}
        explicit Value(const char* a) : value(string(a)) {}
        explicit Value(Array a) : value(a) {}
        explicit Value(ArrayMember a) : value(a) {}
        explicit Value(List a) : value(a) {}
        explicit Value(DictionaryRef a) : value(a) {}
        explicit Value(ClassRef a) : value(a) {}
        explicit Value(ValueVariant a) : value(a) {}
        explicit Value(ValueRef o) : value(o->value) {}
        ~Value() {};

        Type getType() const {
            return static_cast<Type>(value.index());
        }

        // get a string that represents this value
        string getPrintString() const {
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
        UserPointer& getPointer() {
            return get<UserPointer>(value);
        }

        // get this value as a string
        string& getString() {
            return get<string>(value);
        }

        // get this value as an array
        Array& getArray() {
            return get<Array>(value);
        }

        // get this value as an array member
        ArrayMember& getArrayMember() {
            return get<ArrayMember>(value);
        }

        // get this value as an std::vector<T>
        template <typename T>
        vector<T>& getStdVector();

        // get this value as a list
        List& getList() {
            return get<List>(value);
        }

        DictionaryRef& getDictionary() {
            return get<DictionaryRef>(value);
        }

        ClassRef& getClass() {
            return get<ClassRef>(value);
        }

        // get a boolean representing the truthiness of this value
        bool getBool() {
            // non zero or "true" are true
            bool truthiness = false;
            switch (getType()) {
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
    inline std::ostream& operator<<(std::ostream& os, const Null&) {
        os << "null";

        return os;
    }

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

    // define cout operator for ArrayMember
    inline std::ostream& operator<<(std::ostream& os, const ArrayMember& arr) {
        os << arr.getValue()->getPrintString();

        return os;
    }

    // define cout operator for Dictionary
    inline std::ostream& operator<<(std::ostream& os, const DictionaryRef& dict) {
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
        if (a.getType() == Type::ArrayMember) {
            a = *a.getArrayMember().getValue();
        }
        if (b.getType() == Type::ArrayMember) {
            b = *b.getArrayMember().getValue();
        }
        if (a.getType() != b.getType()) {
            if (a.getType() < b.getType()) {
                a.upconvert(b.getType());
            } else {
                b.upconvert(a.getType());
            }
        }
    }

    // makes both values have matching types but doesn't allow converting between numbers and non-numbers
    inline void upconvertThrowOnNonNumberToNumberCompare(Value& a, Value& b) {
        if (a.getType() != b.getType()) {
            if (max((int)a.getType(), (int)b.getType()) > (int)Type::Vec3) {
                throw Exception(
                    "Types `"s + getTypeName(a.getType()) + " " + a.getPrintString() + "` and `" 
                    + getTypeName(b.getType()) + " " + b.getPrintString() + "` are incompatible for this operation");
            }
            if (a.getType() < b.getType()) {
                a.upconvert(b.getType());
            } else {
                b.upconvert(a.getType());
            }
        }
    }

    // math operators
    inline Value operator + (Value a, Value b) {
        upconvert(a, b);
        switch (a.getType()) {
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
            auto list = make_shared<Dictionary>(*a.getDictionary());
            list->merge(*b.getDictionary());
            return Value{ list };
        }
        break;
        default:
            throw Exception("Operator + not defined for type `"s + getTypeName(a.getType()) + "`");
            break;
        }
    }

    inline Value operator - (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.getType()) {
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
            throw Exception("Operator - not defined for type `"s + getTypeName(a.getType()) + "`");
            break;
        }
    }

    inline Value operator * (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.getType()) {
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
            throw Exception("Operator * not defined for type `"s + getTypeName(a.getType()) + "`");
            break;
        }
    }

    inline Value operator / (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.getType()) {
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
            throw Exception("Operator / not defined for type `"s + getTypeName(a.getType()) + "`");
            break;
        }
    }

    inline Value operator += (Value& a, Value b) {
        if ((int)a.getType() < (int)Type::Array || b.getType() == Type::List) {
            upconvert(a, b);
        }
        switch (a.getType()) {
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
            auto bType = b.getType() == Type::Array ? b.getArray().getType() : b.getType();
            if (arr.size() == 0) {
                arr.changeType(bType);
            }
            if (arr.getType() == b.getType()
                || (b.getType() == Type::Array && b.getArray().getType() == arr.getType())) {
                switch (b.getType()) {
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
            switch (b.getType()) {
            case Type::Int:
            case Type::Float:
            case Type::Vec3:
            case Type::Function:
            case Type::String:
            case Type::UserPointer:
                list.push_back(make_shared<Value>(b));
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
            dict->merge(*b.getDictionary());
        }
        break;
        default:
            throw Exception("Operator += not defined for type `"s + getTypeName(a.getType()) + "`");
            break;
        }
        return a;
    }

    inline Value operator -= (Value& a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.getType()) {
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
            throw Exception("Operator -= not defined for type `"s + getTypeName(a.getType()) + "`");
            break;
        }
        return a;
    }

    inline Value operator *= (Value& a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.getType()) {
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
            throw Exception("Operator *= not defined for type `"s + getTypeName(a.getType()) + "`");
            break;
        }
        return a;
    }

    inline Value operator /= (Value& a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.getType()) {
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
            throw Exception("Operator /= not defined for type `"s + getTypeName(a.getType()) + "`");
            break;
        }
        return a;
    }

    inline Value operator % (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.getType()) {
        case Type::Int:
            return Value{ a.getInt() % b.getInt() };
            break;
        case Type::Float:
            return Value{ std::fmod(a.getFloat(), b.getFloat()) };
            break;
        default:
            throw Exception("Operator %% not defined for type `"s + getTypeName(a.getType()) + "`");
            break;
        }
    }

    // comparison operators
    bool operator != (Value a, Value b);
    inline bool operator == (Value a, Value b) {
        if (a.getType() != b.getType()) {
            return false;
        }
        switch (a.getType()) {
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
            throw Exception("Operator == not defined for type `"s + getTypeName(a.getType()) + "`");
            break;
        }
        return true;
    }

    inline bool operator != (Value a, Value b) {
        if (a.getType() != b.getType()) {
            return true;
        }
        switch (a.getType()) {
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
            throw Exception("Operator != not defined for type `"s + getTypeName(a.getType()) + "`");
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
        switch (a.getType()) {
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
            return a.getDictionary()->size() < b.getDictionary()->size();
            break;
        default:
            break;
        }
        return false;
    }

    inline bool operator > (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.getType()) {
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
            return a.getDictionary()->size() > b.getDictionary()->size();
            break;
        default:
            break;
        }
        return false;
    }

    inline bool operator <= (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.getType()) {
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
            return a.getDictionary()->size() <= b.getDictionary()->size();
            break;
        default:
            break;
        }
        return false;
    }

    inline bool operator >= (Value a, Value b) {
        upconvertThrowOnNonNumberToNumberCompare(a, b);
        switch (a.getType()) {
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
            return a.getDictionary()->size() >= b.getDictionary()->size();
            break;
        default:
            break;
        }
        return false;
    }
}
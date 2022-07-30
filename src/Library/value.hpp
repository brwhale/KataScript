#pragma once

namespace KataScript {
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
        KSValue(bool a) : type(KSType::Int), value(static_cast<KSInt>(a)) {}
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
}
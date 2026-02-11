/*
--------------------------------------
این هدر فایل شامل یک استراکت برای مدیریت داده‌های داخل کد کاربر است
--------------------------------------
*/
#include <string>

#ifndef VALUE_HEADER
#define VALUE_HEADER

enum ValueType
{
    VALUE_NUMBER,
    VALUE_STRING,
    VALUE_BOOLEAN,
};

struct Value
{
    ValueType type; // type of value

    double number;      // number storage space
    std::string string; // string storage space
    bool boolean;       // boolean storage space

    Value() : type(VALUE_NUMBER), number(0.0), boolean(false), string("") {} // default constractor

    Value fromNumber(double n)
    {
        Value v;
        v.type = VALUE_NUMBER;
        v.number = n;
        return v;
    }

    Value fromString(std::string str)
    {
        Value v;
        v.type = VALUE_STRING;
        v.string = str;
        return v;
    }

    Value fromBool(bool f)
    {
        Value v;
        v.type = VALUE_BOOLEAN;
        v.boolean = f;
        return v;
    }

    double asNumber() const
    {
        switch (type)
        {
        case VALUE_NUMBER:
            return number;
            break;
        case VALUE_BOOLEAN:
            return boolean ? 1.0 : 0.0;
            break;
        case VALUE_STRING:
            return atof(string.c_str());
            break;

        default:
            break;
        }

        return 0.0;
    }

    bool asBool() const
    {
        switch (type)
        {
        case VALUE_NUMBER:
            return number != 0.0;
            break;
        case VALUE_BOOLEAN:
            return boolean;
            break;
        case VALUE_STRING:
            return !string.empty();
            break;

        default:
            break;
        }

        return false;
    }

    std::string asString() const
    {
        switch (type)
        {
        case VALUE_NUMBER:
            return std::to_string(number);
            break;
        case VALUE_BOOLEAN:
            return boolean ? "true" : "false";
            break;
        case VALUE_STRING:
            return string;
            break;

        default:
            break;
        }

        return "";
    }
};

#endif
#include "value.h"
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
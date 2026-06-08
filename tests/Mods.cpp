#include "Mods.h"
#include "../DataObject.h"
#include <algorithm>
#include <string>

namespace {
enum class DigitsType {
    Decimal,
    Hex,
    UnEvenHex,
    HexPrefixed,
    UnEvenHexPrefixed,
    String
};

DigitsType stringIntegerType(std::string const& _string, bool _wasPrefix = false)
{
    if (_string.size() >= 2 && _string.at(0) == '0' && _string.at(1) == 'x' && !_wasPrefix)
    {
        const DigitsType substringType = stringIntegerType(_string, true);
        if (substringType == DigitsType::Hex)
            return DigitsType::HexPrefixed;

        if (substringType == DigitsType::Decimal)
        {
            if (_string.size() % 2 == 0)
                return DigitsType::HexPrefixed;
            else
                return DigitsType::UnEvenHexPrefixed;
        }

        if (substringType == DigitsType::UnEvenHex)
            return DigitsType::UnEvenHexPrefixed;
    }

    bool isDecimalOnly = true;
    for (size_t i = _wasPrefix ? 2 : 0; i < _string.length(); i++)
    {
        if (!isxdigit(_string.at(i)))
            return DigitsType::String;

        if (isDecimalOnly && !isdigit(_string.at(i)))
            isDecimalOnly = false;
    }

    if (isDecimalOnly)
        return DigitsType::Decimal;

    if (_string.size() % 2 == 0)
        return DigitsType::Hex;

    return DigitsType::UnEvenHex;
}

void removeLeadingZeroesIfHex(std::string& _hexStr)
{
    if (_hexStr.size() <= 2 || _hexStr.at(0) != '0' || _hexStr.at(1) != 'x')
        return;

    auto const it = std::find_if_not(_hexStr.begin() + 2, _hexStr.end() - 1, [](char c) { return c == '0'; });
    _hexStr.erase(_hexStr.begin() + 2, it);
}
}

using namespace dataobject;

namespace dataobject::tests {
void mod_removeLeadingZerosFromHexValues(DataObject& _obj)
{
    if (_obj.type() == DataType::String)
    {
        std::string& str = _obj.asStringUnsafe();
        removeLeadingZeroesIfHex(str);
    }
}

void mod_removeLeadingZerosFromHexValueEVEN(DataObject& _obj)
{
    mod_removeLeadingZerosFromHexValues(_obj);
    if (_obj.type() == DataType::String)
    {
        const DigitsType t = stringIntegerType(_obj.asString());
        if (t == DigitsType::UnEvenHexPrefixed)
            _obj.asStringUnsafe().replace(0, 2, "0x0", 3);
    }
}

void mod_removeLeadingZerosFromHexKeyEVEN(DataObject& _obj)
{
    std::string& str = _obj.getKeyUnsafe();
    removeLeadingZeroesIfHex(str);
    const DigitsType t = stringIntegerType(str);
    if (t == DigitsType::UnEvenHexPrefixed)
        str.replace(0, 2, "0x0", 3);
}

void mod_valueToFH32(DataObject& _obj)
{
    if (_obj.type() == DataType::String)
    {
        const std::string& str = _obj.asString();
        if (str.substr(0, 2) == "0x")
        {
            const size_t paddingCount = 66 - str.size();
            _obj.asStringUnsafe().insert(2, std::string(paddingCount, '0'));
        }
    }
}

void mod_sortKeys(DataObject& _obj)
{
    if (_obj.isArray())
    {
        std::map<std::string, spDataObject> const map = _obj.getSubObjectKeys();
        if (map.size() > 1)
        {
            _obj.clearSubobjects();
            _obj.setAutosort(true);
            for (auto const& el: map)
                _obj.atKeyPointer(el.second->getKey()) = el.second;
        }
    }
}
}

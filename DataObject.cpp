#include "DataObject.h"
#include <math.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace dataobject;
using namespace std;

namespace {
std::string doubleToStringWithPrecision(double const _value, int precision)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << _value;
    return out.str();
};
}

/// Default dataobject is null
DataObject::DataObject() {}

/// Define dataobject of _type, pass the value later (will check the value and _type)
DataObject::DataObject(DataType _type)
{
    if (_type == DataType::Object || _type == DataType::Array)
        _initArray(_type);
    else if (_type == DataType::Null)
        m_value = DataNull();
    else if (_type == DataType::String)
        m_value = string();
    else if (_type == DataType::Integer)
        m_value = 0;
    else if (_type == DataType::UInteger64)
        m_value = static_cast<std::uint64_t>(0);
    else if (_type == DataType::Double)
        m_value = (double) 0.0;
    else if (_type == DataType::Bool)
        m_value = false;
}

/// Define dataobject of string
DataObject::DataObject(std::string&& _str) : m_value(std::move(_str)) {}

DataObject::DataObject(std::string const& _str) : m_value(_str)
{}

/// Define dataobject[_key] = string
DataObject::DataObject(std::string&& _key, std::string&& _str)
    : m_strKey(std::move(_key)), m_value(std::move(_str))
{}

DataObject::DataObject(std::string const& _key, std::string const& _str)
    : m_strKey(_key), m_value(_str)
{}

DataObject::DataObject(std::string&& _key, int const _val)
    : m_strKey(std::move(_key))
{
    setInt(_val);
}

/// Define dataobject of int
DataObject::DataObject(int const _int)
{
    setInt(_int);
}

DataObject::DataObject(std::uint64_t const _uint64)
{
    setInt(_uint64);
}

DataObject::DataObject(double const _double)
{
    setDouble(_double);
}

DataObject::DataObject(DataType const _type, std::uint64_t const _uint64)
{
    _assert(_type == DataType::UInteger64, "DataObject(DataType type, uint64_t _int) is not initialized as UInteger");
    setUInt64(_uint64);
}

DataObject::DataObject(DataType const _type, int const _int)
{
    _assert(_type == DataType::Integer, "DataObject(DataType type, int _int) is not initialized as Integer");
    setInt(_int);
}

DataObject::DataObject(DataType const _type, double const _double)
{
    _assert(_type == DataType::Double, "DataObject(DataType type, double _double) is not initialized as Double");
    setDouble(_double);
}

/// Define dataobject of bool
DataObject::DataObject(DataType const _type, SafeBool const _bool)
{
    string stype;
    switch (_type)
    {
        case DataType::Integer: stype = "INTEGER";
            break;
        case DataType::UInteger64: stype = "UINTEGER_64";
            break;
        case DataType::Array: stype = "Array";
            break;
        case DataType::Bool: stype = "Bool";
            break;
        case DataType::Double: stype = "Double";
            break;
        case DataType::NotInitialized: stype = "NotInit";
            break;
        case DataType::Null: stype = "Null";
            break;
        case DataType::String: stype = "String";
            break;
        case DataType::Object: stype = "Object";
            break;
    }
    _assert(_type == DataType::Bool,
            "DataObject(DataType type, bool _bool) is not initialized as Bool, requested " + stype);
    setBool(_bool);
}

/// Get dataobject type
DataType DataObject::type() const
{
    if (_isNotInit())
        return DataType::NotInitialized;
    return static_cast<DataType>(m_value.index());
}

constexpr bool DataObject::_isNotInit() const
{
    return std::holds_alternative<std::monostate>(m_value);
}

/// Set key of the dataobject
void DataObject::setKey(std::string&& _key) { m_strKey = std::move(_key); }
void DataObject::setKey(std::string const& _key) { m_strKey = _key; }

/// Get key of the dataobject
std::string const& DataObject::getKey() const { return m_strKey; }
std::string& DataObject::getKeyUnsafe() { return m_strKey; }

/// Get vector of subobjects
std::vector<spDataObject> const& DataObject::getSubObjects() const
{
    static std::vector<spDataObject> emptyVector;
    if (type() == DataType::Object)
        return std::get<DataObjecto>(m_value).first;
    if (type() == DataType::Array)
        return std::get<0>(std::get<DataArray>(m_value));
    return emptyVector;
}

/// Get map of keys to subobjects
std::map<string, spDataObject> const& DataObject::getSubObjectKeys() const
{
    static std::map<std::string, spDataObject> empty_map;
    if (type() == DataType::Object)
        return std::get<DataObjecto>(m_value).second;
    if (type() == DataType::Array)
        return std::get<1>(std::get<DataArray>(m_value));
    return empty_map;
}

std::map<string, spDataObject>& DataObject::_getSubObjectKeysUnsafe()
{
    static std::map<std::string, spDataObject> empty_map;
    if (type() == DataType::Object)
        return std::get<DataObjecto>(m_value).second;
    if (type() == DataType::Array)
        return std::get<1>(std::get<DataArray>(m_value));
    return empty_map; // Dangerous, Not Thread Safe, should never be here
}

/// Get ref vector of subobjects
std::vector<spDataObject>& DataObject::getSubObjectsUnsafe()
{
    static std::vector<spDataObject> empty_vector;
    if (type() == DataType::Object)
        return std::get<DataObjecto>(m_value).first;
    if (type() == DataType::Array)
        return std::get<0>(std::get<DataArray>(m_value));
    return empty_vector; // Dangerous, Not Thread Safe
}

/// Add new subobject
DataObject& DataObject::addSubObject(spDataObject const& _obj)
{
    return _addSubObject(_obj);
}

/// Add new subobject and set it's key
DataObject& DataObject::addSubObject(std::string&& _key, spDataObject const& _obj)
{
    return _addSubObject(_obj, std::forward<string&&>(_key));
}

DataObject& DataObject::addSubObject(std::string const& _key, spDataObject const& _obj)
{
    return _addSubObject(_obj, string(_key));
}

/// Set key for subobject _index
void DataObject::setSubObjectKey(size_t const _index, std::string&& _key)
{
    static const string c_errorAssert = "DataObject::setSubObjectKey is not array";
    _assert(isArray(), c_errorAssert);
    auto& sub_objects                  = getSubObjectsUnsafe();
    static const string c_errorAssert2 = "_index < m_subObjects.size() (DataObject::setSubObjectKey)";
    _assert(_index < sub_objects.size(), c_errorAssert2);
    if (sub_objects.size() > _index)
        sub_objects.at(_index).getContent().setKey(std::forward<string&&>(_key));
}

/// look if there is a subobject with _key
bool DataObject::count(std::string const& _key) const
{
    auto const& sub_object_keys = getSubObjectKeys();
    return sub_object_keys.contains(_key);
}

/// Get string value
std::string const& DataObject::asString() const
{
    static const string c_errorAssert = "m_type == DataType::String (DataObject::asString())";
    _assert(type() == DataType::String, c_errorAssert);
    return std::get<std::string>(m_value);
}

std::string& DataObject::asStringUnsafe()
{
    static const string c_errorAssert = "m_type == DataType::String (DataObject::asStringUnsafe())";
    _assert(type() == DataType::String, c_errorAssert);
    return std::get<std::string>(m_value);
}

std::string const DataObject::asStringAnyway() const
{
    switch (type())
    {
        case DataType::String: return asString();
        case DataType::Integer: return to_string(asInt());
        case DataType::UInteger64: return to_string(asUInt64());
        case DataType::Double: return doubleToStringWithPrecision(asDouble(), 4);
        case DataType::Bool: return asBool() ? "true" : "false";
        case DataType::Object: return "Object";
        case DataType::Array: return "Array";
        case DataType::Null: return "Null";
        case DataType::NotInitialized: return "Not initialized";
        default: return "N/A";
    }
}

/// Get int value
int DataObject::asInt() const
{
    static const string c_errorAssert = "m_type == DataType::Integer (DataObject::asInt())";
    _assert(type() == DataType::Integer, c_errorAssert);
    return std::get<int>(m_value);
}

std::uint64_t DataObject::asUInt64() const
{
    static const string c_errorAssert = "m_type == DataType::UInteger64 (DataObject::asUInt64())";
    _assert(type() == DataType::UInteger64, c_errorAssert);
    return std::get<std::uint64_t>(m_value);
}

/// Get int value
double DataObject::asDouble() const
{
    static const string c_errorAssert = "m_type == DataType::Double (DataObject::asDouble())";
    _assert(type() == DataType::Double, c_errorAssert);
    return std::get<double>(m_value);
}

/// Get bool value
bool DataObject::asBool() const
{
    static const string c_errorAssert = "m_type == DataType::Bool (DataObject::asBool())";
    _assert(type() == DataType::Bool, c_errorAssert);
    return std::get<bool>(m_value);
}

/// Set position in vector of the subobject with _key
void DataObject::setKeyPos(std::string const& _key, size_t const _pos)
{
    static const string c_errorAssert = "count(_key) (DataObject::setKeyPos)";
    _assert(count(_key), c_errorAssert);
    static const string c_errorAssert2 = "!_key.empty() (DataObject::setKeyPos)";
    _assert(!_key.empty(), c_errorAssert2);

    size_t element_pos = 0;
    auto& sub_objects  = getSubObjectsUnsafe();
    for (size_t i = 0; i < sub_objects.size(); i++)
        if (sub_objects.at(i)->getKey() == _key)
        {
            if (i == _pos)
                return; // item already at _pos;
            else
            {
                element_pos = i;
                break;
            }
        }

    spDataObject const data = sub_objects.at(element_pos);
    sub_objects.erase(sub_objects.begin() + element_pos);
    if (_pos >= sub_objects.size())
        sub_objects.push_back(data);
    else
        sub_objects.insert(sub_objects.begin() + _pos, 1, data);
}

/// replace this object with _value
void DataObject::replace(DataObject const& _value)
{
    clear();
    m_strKey = _value.getKey();
    switch (_value.type())
    {
        case DataType::String:
            m_value = _value.asString();
            break;
        case DataType::Integer:
            m_value = _value.asInt();
            break;
        case DataType::UInteger64:
            m_value = _value.asUInt64();
            break;
        case DataType::Double:
            m_value = _value.asDouble();
            break;
        case DataType::Bool:
            m_value = _value.asBool();
            break;
        case DataType::Object:
            m_value = std::pair{_value.getSubObjects(), _value.getSubObjectKeys()};
            break;
        case DataType::Array:
            m_value = std::tuple{_value.getSubObjects(), _value.getSubObjectKeys()};
            break;
        case DataType::Null:
            m_value = DataNull();
            break;
        default:
            break;
    }

    setAutosort(_value.isAutosort());
}

DataObjectK& DataObjectK::operator=(spDataObject const& _value)
{
    if (m_data.count(m_key))
    {
        m_data.removeKey(m_key);
        if (_value->getKey().empty())
            throw DataObjectException(
                "DataObjectK::operator=(spDataObject const& _value)  _value without key, but key required!");
        m_data.addSubObject(string(_value->getKey()), _value);
    }
    else
        m_data.addSubObject(string(m_key), _value);
    return *this;
}

spDataObject& DataObject::atKeyPointerUnsafe(std::string const& _key)
{
    auto& sub_object_keys = _getSubObjectKeysUnsafe();
    if (sub_object_keys.contains(_key))
        return sub_object_keys.at(_key);
    _assert(false, "count(_key) _key=" + _key + " (DataObject::atKeyPointerUnsafe)");
    return sub_object_keys.at(0);
}

DataObjectK DataObject::atKeyPointer(std::string const& _key)
{
    return DataObjectK(_key, *this);
}

DataObject const& DataObject::atKey(std::string const& _key) const
{
    auto const& sub_object_keys = getSubObjectKeys();
    if (sub_object_keys.contains(_key))
        return sub_object_keys.at(_key).getCContent();

    _assert(false, "count(_key) _key=" + _key + " (DataObject::atKey)");
    auto const& sub_objects = getSubObjects();
    return sub_objects.at(0).getCContent();
}

DataObject& DataObject::atKeyUnsafe(std::string const& _key)
{
    auto& sub_object_keys = _getSubObjectKeysUnsafe();
    if (sub_object_keys.contains(_key))
        return sub_object_keys.at(_key).getContent();
    _assert(false, "count(_key) _key=" + _key + " (DataObject::atKeyUnsafe)");
    auto& sub_objects = getSubObjectsUnsafe();
    return sub_objects.at(0).getContent();
}

DataObject const& DataObject::at(size_t const _pos) const
{
    auto const& sub_objects = getSubObjects();
    _assert((size_t) _pos < sub_objects.size(), "DataObject::at(int) out of range!");
    return sub_objects[_pos];
}

DataObject& DataObject::atUnsafe(size_t const _pos)
{
    auto& sub_objects = getSubObjectsUnsafe();
    _assert((size_t) _pos < sub_objects.size(), "DataObject::atUnsafe(int) out of range!");
    return sub_objects[_pos].getContent();
}

spDataObject& DataObject::atPointerUnsafe(size_t const _pos)
{
    auto& sub_objects = getSubObjectsUnsafe();
    _assert((size_t) _pos < sub_objects.size(), "DataObject::atPointerUnsafe(int) out of range!");
    return sub_objects[_pos];
}

DataObject const& DataObject::atLastElement() const
{
    auto const& subObjects       = getSubObjects();
    static const string c_assert = "atLastElement()";
    _assert(subObjects.size() > 0, c_assert);
    return subObjects.at(subObjects.size() - 1);
}

DataObject& DataObject::atLastElementUnsafe()
{
    auto& sub_objects            = getSubObjectsUnsafe();
    static const string c_assert = "atLastElementUnsafe()";
    _assert(sub_objects.size() > 0, c_assert);
    return sub_objects.at(sub_objects.size() - 1).getContent();
}

void DataObject::addArrayObject(spDataObject const& _obj)
{
    static const string c_assert =
            "m_type == DataType::NotInitialized || m_type == DataType::Array (DataObject::addArrayObject)";
    bool const not_init = type() == DataType::NotInitialized;
    _assert(not_init || type() == DataType::Array, c_assert);
    if (not_init)
        _initArray(DataType::Array);
    auto& subObjects = getSubObjectsUnsafe();
    subObjects.push_back(_obj);
    subObjects.at(subObjects.size() - 1).getContent().setAutosort(m_autosort);
}

void DataObject::renameKey(std::string const& _current_key, std::string&& _new_key)
{
    if (m_strKey == _current_key)
        m_strKey = _new_key;

    if (isArray())
    {
        auto& sub_object_keys = _getSubObjectKeysUnsafe();
        if (sub_object_keys.contains(_current_key))
        {
            spDataObject data = sub_object_keys.at(_current_key);
            sub_object_keys.erase(_current_key);
            data.getContent().setKey(_new_key);
            sub_object_keys.emplace(_new_key, data);
        }
    }
}

/// vector<element> erase method with `replace()` function
void DataObject::removeKey(std::string const& _key)
{
    static const string c_assert = "type() == DataType::Object";
    _assert(type() == DataType::Object, c_assert);
    auto& sub_objects     = getSubObjectsUnsafe();
    auto& sub_object_keys = _getSubObjectKeysUnsafe();

    sub_objects.erase(std::remove_if(sub_objects.begin(), sub_objects.end(),
                                     [&_key](const spDataObject& _obj) {
                                         return _obj->getKey() == _key;
                                     }), sub_objects.end());
    sub_object_keys.erase(_key);
}

void DataObject::clear(DataType const _type)
{
    m_strKey = "";
    if (isArray())
        clearSubobjects();
    DataVariant empty_variant;
    std::swap(m_value, empty_variant);
    if (_type == DataType::Object || _type == DataType::Array)
        _initArray(_type);
}

void DataObject::performModifier(void (*f)(DataObject&), ModifierOption const _opt,
                                 std::set<string> const& _exception_keys)
{
    if (!_exception_keys.contains(getKey()))
    {
        f(*this);
        if (_opt == ModifierOption::RECURSIVE && isArray())
        {
            auto& subObjects = getSubObjectsUnsafe();
            for (auto& el: subObjects)
                el.getContent().performModifier(f, _opt, _exception_keys);
        }
    }
}

bool DataObject::performSearch(bool (*f)(DataObject const&)) const
{
    if (isArray())
    {
        auto const& sub_objects = getSubObjects();
        for (auto const& el: sub_objects)
        {
            if (el->performSearch(f))
                return true;
        }
    }
    return f(*this);
}

std::string DataObject::asJsonNoFirstKey() const
{
    return asJson(0, true, true);
}

std::string DataObject::asJson(int level, bool pretty, bool nokey) const
{
    std::ostringstream out;
    auto printLevel = [level, pretty, &out]() -> void {
        if (pretty)
            for (int i = 0; i < level * 4; i++)
                out << " ";
    };

    auto printElements = [this, &out, level, pretty]() -> void {
        if (this->isArray())
        {
            auto const& sub_objects = getSubObjects();
            for (std::vector<spDataObject>::const_iterator it = sub_objects.begin();
                 it < sub_objects.end(); it++)
            {
                if ((*it).isEmpty())
                    out << "NaN";
                else
                    out << (*it)->asJson(level + 1, pretty);
                if (it + 1 != sub_objects.end())
                    out << ",";
                if (pretty)
                    out << std::endl;
            }
        }
    };

    string buffer;
    switch (type())
    {
        case DataType::NotInitialized:
            printLevel();
            if (!m_strKey.empty() && !nokey)
            {
                if (pretty)
                    out << "\"" << m_strKey << "\" : ";
                else
                    out << "\"" << m_strKey << "\":";
            }
            out << "notinit";
            break;
        case DataType::Null:
            printLevel();
            if (!m_strKey.empty() && !nokey)
            {
                if (pretty)
                    out << "\"" << m_strKey << "\" : ";
                else
                    out << "\"" << m_strKey << "\":";
            }
            out << "null";
            //out << "{}";  // why???
            break;
        case DataType::Object:
            if (!m_strKey.empty() && !nokey)
            {
                printLevel();
                if (pretty)
                    out << "\"" << m_strKey << "\" : {" << std::endl;
                else
                    out << "\"" << m_strKey << "\":{";
            }
            else
            {
                printLevel();
                out << "{";
                if (pretty)
                    out << std::endl;
            }
            printElements();
            printLevel();
            out << "}";
            break;
        case DataType::Array:
            if (!m_strKey.empty() && !nokey)
            {
                printLevel();
                if (pretty)
                    out << "\"" << m_strKey << "\" : [" << std::endl;
                else
                    out << "\"" << m_strKey << "\":[";
            }
            else
            {
                printLevel();
                out << "[";
                if (pretty)
                    out << std::endl;
            }
            printElements();
            printLevel();
            out << "]";
            break;
        case DataType::String:
        {
            printLevel();
            if (!m_strKey.empty() && !nokey)
            {
                if (pretty)
                    out << "\"" << m_strKey << "\" : ";
                else
                    out << "\"" << m_strKey << "\":";
            }

            //  threat special chars
            char ch_before = 0;
            for (auto const& ch: asString())
            {
                if (ch == 10)
                    buffer += "\\n";
                else if (ch == 9)
                    buffer += "\\t";
                else if (ch == '"' && ch_before != '\\')
                {
                    buffer += "\\";
                    buffer += "\"";
                }
                else
                    buffer += ch;
                ch_before = ch;
            }
            out << "\"" << buffer << "\"";
            break;
        }
        case DataType::Double:
            printLevel();
            if (!m_strKey.empty() && !nokey)
            {
                if (pretty)
                    out << "\"" << m_strKey << "\" : ";
                else
                    out << "\"" << m_strKey << "\":";
            }
            out << doubleToStringWithPrecision(std::get<double>(m_value), 4);
            break;
        case DataType::Integer:
            printLevel();
            if (!m_strKey.empty() && !nokey)
            {
                if (pretty)
                    out << "\"" << m_strKey << "\" : ";
                else
                    out << "\"" << m_strKey << "\":";
            }
            out << std::get<int>(m_value);
            break;
        case DataType::UInteger64:
            printLevel();
            if (!m_strKey.empty() && !nokey)
            {
                if (pretty)
                    out << "\"" << m_strKey << "\" : ";
                else
                    out << "\"" << m_strKey << "\":";
            }
            out << "\"u64::" << std::get<std::uint64_t>(m_value) << "\"";
            break;
        case DataType::Bool:
            printLevel();
            if (!m_strKey.empty() && !nokey)
            {
                if (pretty)
                    out << "\"" << m_strKey << "\" : ";
                else
                    out << "\"" << m_strKey << "\":";
            }
            if (std::get<bool>(m_value))
                out << "true";
            else
                out << "false";
            break;
        default:
            out << "unknown " << dataTypeAsString(type()) << std::endl;
            break;
    }
    return out.str();
}

std::string DataObject::dataTypeAsString(DataType const _type)
{
    switch (_type)
    {
        case String:
            return "string";
        case Integer:
            return "int";
        case UInteger64:
            return "uint_64";
        case Double:
            return "double";
        case Array:
            return "array";
        case Bool:
            return "bool";
        case Object:
            return "object";
        case NotInitialized:
            return "notinit";
        case Null:
            return "null";
        default:
            break;
    }
    return "";
}

size_t dataobject::findOrderedKeyPosition(string const& _key, vector<spDataObject> const& _objects)
{
    if (_objects.size() == 0)
        return 0;

    size_t m = 0;
    int L    = 0;
    int R    = _objects.size() - 1;
    while (L <= R)
    {
        m = floor((L + R) / 2);
        if (_objects.at(m)->getKey() < _key)
            L = m + 1;
        else if (_objects.at(m)->getKey() > _key)
            R = m - 1;
        else
            return m;
    }

    if (_objects.at(m)->getKey() > _key)
        return m;
    else
        return m + 1;
}

DataObject& DataObject::_addSubObject(spDataObject const& _obj, string&& _key_overwrite)
{
    if (type() == DataType::NotInitialized)
        _initArray(DataType::Object);

    size_t pos;
    string const key = _key_overwrite.empty() ? _obj->getKey() : _key_overwrite;
    if (key.empty() || !m_autosort)
    {
        auto& subObjects = getSubObjectsUnsafe();
        subObjects.push_back(_obj);
        pos = subObjects.size() - 1;
        if (!_key_overwrite.empty())
            setSubObjectKey(pos, std::forward<string&&>(_key_overwrite));
        else
            setSubObjectKey(pos, string(_obj->getKey()));
        subObjects.at(pos).getContent().setAutosort(m_autosort);
    }
    else
    {
        // find ordered position to insert key
        // better use it only when export as ordered json !!!
        auto& sub_objects = getSubObjectsUnsafe();
        pos               = findOrderedKeyPosition(key, sub_objects);
        if (pos == sub_objects.size())
            sub_objects.push_back(_obj);
        else
            sub_objects.insert(sub_objects.begin() + pos, 1, _obj);

        if (!_key_overwrite.empty())
            sub_objects.at(pos).getContent().setKey(std::forward<string&&>(_key_overwrite));
        else
            sub_objects.at(pos).getContent().setKey(string(_obj->getKey()));
        sub_objects.at(pos).getContent().setAutosort(m_autosort);
    }
    auto& sub_objects     = getSubObjectsUnsafe();
    auto& sub_object_keys = _getSubObjectKeysUnsafe();
    if (!key.empty())
        sub_object_keys.emplace(key, sub_objects.at(pos));
    return sub_objects.at(pos).getContent();
}

void DataObject::_assert(bool const _flag, std::string const& _comment) const
{
    if (!_flag)
    {
        std::ostringstream out;
        out << "Error in DataObject: " << std::endl;
        out << " key: '" << m_strKey << "'";
        out << " type: '" << dataTypeAsString(type()) << "'" << std::endl;
        out << " assert: " << _comment << std::endl;
        out << asJson() << std::endl;
        throw DataObjectException(out.str());
    }
}

void DataObject::setString(string&& _value)
{
    static const string c_assert = "In DataObject=(string) DataObject must be string or NotInitialized!";
    _assert(type() == DataType::String || type() == DataType::NotInitialized, c_assert);
    m_value = std::move(_value);
}

void DataObject::setInt(int _value)
{
    static const string c_assert = "In DataObject=(int) DataObject must be int or NotInitialized!";
    _assert(type() == DataType::Integer || type() == DataType::NotInitialized, c_assert);
    m_value = _value;
}

void DataObject::setUInt64(std::uint64_t _value)
{
    static const string c_assert = "In DataObject=(uint64_t) DataObject must be Uint or NotInitialized!";
    _assert(type() == DataType::UInteger64 || type() == DataType::NotInitialized, c_assert);
    m_value = _value;
}

void DataObject::setDouble(double _value)
{
    static const string c_assert = "In DataObject=(double) DataObject must be double or NotInitialized!";
    _assert(type() == DataType::Double || type() == DataType::NotInitialized, c_assert);
    m_value = _value;
}

void DataObject::setBool(bool const _value)
{
    static const string c_assert = "In DataObject:setBool(bool) DataObject must be bool or NotInitialized!";
    _assert(type() == DataType::Bool || type() == DataType::NotInitialized, c_assert);
    m_value = (bool) _value;
}

void DataObject::copyFrom(DataObject const& _other)
{
    clear();
    if (!_other.getKey().empty())
        m_strKey = _other.getKey();

    switch (_other.type())
    {
        case String: m_value = _other.asString();
            break;
        case Integer: m_value = _other.asInt();
            break;
        case UInteger64: m_value = _other.asUInt64();
            break;
        case Double: m_value = _other.asDouble();
            break;
        case Bool: m_value = _other.asBool();
            break;
        case Array:
            _initArray(DataType::Array);
            for (auto const& el: _other.getSubObjects())
            {
                spDataObject copy = el->copy();
                addArrayObject(copy);
            }
            break;
        case Object:
            _initArray(DataType::Object);
            for (auto const& el: _other.getSubObjects())
            {
                spDataObject copy = el->copy();
                addSubObject(copy);
            }
            break;
        case Null: m_value = DataNull();
            break;
        case NotInitialized: break;
    }
}

spDataObject DataObject::copy() const
{
    spDataObject c = sDataObject(type());
    if (!m_strKey.empty())
        (*c).setKey(string(m_strKey));
    switch (type())
    {
        case String: (*c).setString(string(asString()));
            break;
        case Integer: (*c).setInt(asInt());
            break;
        case UInteger64: (*c).setUInt64(asUInt64());
            break;
        case Double: (*c).setDouble(asDouble());
            break;
        case Bool: (*c).setBool(asBool());
            break;
        case Array:
            for (size_t i = 0; i < getSubObjects().size(); i++)
            {
                spDataObject copy = getSubObjects().at(i)->copy();
                (*c).addArrayObject(copy);
            }
            break;
        case Object:
            for (size_t i = 0; i < getSubObjects().size(); i++)
            {
                spDataObject copy = getSubObjects().at(i)->copy();
                (*c).addSubObject(copy);
            }
            break;
        case Null:
            break;
        case NotInitialized: break;
    }
    return c;
}

bool DataObject::operator==(DataObject const& _value) const
{
    if (type() != _value.type() ||
        (isArray() && getSubObjects().size() != _value.getSubObjects().size()))
        return false;
    bool equal = true;
    switch (type())
    {
        case DataType::Bool:
            equal = asBool() == _value.asBool();
            break;
        case DataType::Integer:
            equal = asInt() == _value.asInt();
            break;
        case DataType::UInteger64:
            equal = asUInt64() == _value.asUInt64();
            break;
        case DataType::Double:
            equal = asStringAnyway() == _value.asStringAnyway();
            break;
        case DataType::String:
            equal = asString() == _value.asString();
            break;
        case DataType::Array:
            for (size_t i = 0; i < getSubObjects().size(); i++)
            {
                equal = getSubObjects().at(i).getCContent() == _value.getSubObjects().at(i).getCContent();
                if (!equal)
                    break;
            }
            break;
        case DataType::Object:
            for (size_t i = 0; i < getSubObjects().size(); i++)
            {
                equal = getSubObjects().at(i).getCContent() == _value.getSubObjects().at(i).getCContent();
                if (!equal)
                    break;
            }
            break;
        default:
            _assert(false, "in DataObject::== unknown object type!");
            equal = false;
            break;
    }
    return equal;
}

bool DataObject::operator==(bool const _value) const
{
    DataObject const tmp(DataType::Bool, _value);
    return *this == tmp;
}

DataObject& DataObject::operator=(int const _value)
{
    setInt(_value);
    return *this;
}

DataObject& DataObject::operator=(std::uint64_t const _value)
{
    setUInt64(_value);
    return *this;
}

DataObject& DataObject::operator=(double const _value)
{
    setDouble(_value);
    return *this;
}

DataObject& DataObject::operator[](std::string const& _key)
{
    static const string c_assert =
            "m_type == DataType::NotInitialized || m_type == DataType::Object (DataObject& operator[])";
    _assert(type() == DataType::NotInitialized || type() == DataType::Object, c_assert);

    if (isArray() && getSubObjectKeys().contains(_key))
        return _getSubObjectKeysUnsafe().at(_key).getContent();

    spDataObject new_obj = sDataObject(DataType::NotInitialized);
    new_obj.getContent().setKey(string(_key));
    return _addSubObject(new_obj); // !could change the item order!
}

DataObject& DataObject::operator[](std::string&& _key)
{
    static const string c_assert =
            "m_type == DataType::NotInitialized || m_type == DataType::Object (DataObject& operator[])";
    _assert(type() == DataType::NotInitialized || type() == DataType::Object, c_assert);

    if (isArray() && getSubObjectKeys().contains(_key))
        return _getSubObjectKeysUnsafe().at(_key).getContent();

    spDataObject new_obj = sDataObject(DataType::NotInitialized);
    new_obj.getContent().setKey(std::forward<string&&>(_key));
    return _addSubObject(new_obj); // !could change the item order!
}

DataObject& DataObject::operator=(std::string&& _value)
{
    static const string c_assert = "In DataObject=(string) DataObject must be string or NotInitialized!";
    _assert(type() == DataType::String || type() == DataType::NotInitialized, c_assert);
    m_value = std::move(_value);
    return *this;
}

DataObject& DataObject::operator=(std::string const& _value)
{
    static const string c_assert = "In DataObject=(string) DataObject must be string or NotInitialized!";
    _assert(type() == DataType::String || type() == DataType::NotInitialized, c_assert);
    m_value = _value;
    return *this;
}

spDataObjectMove dataobject::move(spDataObject& _obj)
{
    spDataObjectMove m;
    m.assignPointer(_obj);
    _obj.null();
    return m;
}

void DataObject::clearSubobjects(DataType const _t)
{
    if (isArray())
    {
        getSubObjectsUnsafe().clear();
        _getSubObjectKeysUnsafe().clear();
    }
    if (_t == DataType::NotInitialized)
    {
        DataVariant empty_variant;
        std::swap(m_value, empty_variant);
        return;
    }
    switch (_t)
    {
        case DataType::Null: m_value = DataNull();
            break;
        case DataType::Object: _initArray(_t);
            break;
        case DataType::Array: _initArray(_t);
            break;
        default: _assert(false);
            break;
    }
}

void DataObject::_initArray(DataType const _type)
{
    VecSpData vec;
    MapKeyToObject map;
    if (_type == DataType::Object)
        m_value = std::make_pair(vec, map);
    else if (_type == DataType::Array)
        m_value = std::make_tuple(vec, map);
    else
        _assert(false, "_initArray got wrong DataType: " + dataTypeAsString(_type));
}

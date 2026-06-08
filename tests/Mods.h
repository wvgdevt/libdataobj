#pragma once
namespace dataobject {
class DataObject;
}

namespace dataobject::tests {
void mod_sortKeys(DataObject& _obj);
void mod_removeLeadingZerosFromHexValueEVEN(DataObject& _obj);
void mod_removeLeadingZerosFromHexKeyEVEN(DataObject& _obj);
void mod_removeLeadingZerosFromHexValues(DataObject& _obj);
void mod_valueToFH32(DataObject& _obj);
}

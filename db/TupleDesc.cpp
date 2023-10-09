#include <db/TupleDesc.h>

using namespace db;

//
// TDItem
//

bool TDItem::operator==(const TDItem &other) const {
    // compare TDItem objects based on the field type and field name
    return fieldType == other.fieldType && fieldName == other.fieldName;
}

std::size_t std::hash<db::TDItem>::operator()(const db::TDItem &item) const {
    // hash the field type
    size_t h1 = std::hash<Types::Type>()(item.fieldType);
    // hash the field name
    size_t h2 = std::hash<std::string>()(item.fieldName);
    return h1 ^ h2;
}

//
// TupleDesc
//

TupleDesc::TupleDesc(const std::vector<Types::Type> &types) {
    for(size_t i = 0; i < types.size(); ++i) {
        tditems.emplace_back(types[i], "");
    }
}

TupleDesc::TupleDesc(const std::vector<Types::Type> &types, const std::vector<std::string> &names) {
    // Assuming that types and names have equal length
    for(size_t i = 0; i < types.size(); ++i) {
        // emplace_back function should automatically construct tditem object from these arguments
        tditems.emplace_back(types[i], names[i]);
    }
}

size_t TupleDesc::numFields() const {
    return tditems.size();
}

std::string TupleDesc::getFieldName(size_t i) const {
    return tditems[i].fieldName;
}

Types::Type TupleDesc::getFieldType(size_t i) const {
    return tditems[i].fieldType;
}

int TupleDesc::fieldNameToIndex(const std::string &fieldName) const {
    // finds the position of the column
    for (size_t i = 0; i < tditems.size(); ++i) {
        if (tditems[i].fieldName == fieldName) {
            return i;
        }
    }
    throw std::invalid_argument("Invalid field name");
}

size_t TupleDesc::getSize() const {
    size_t totalSize = 0;
    
    for (size_t i = 0; i < tditems.size(); ++i) {
        // getlen supports int and string
        totalSize += db::Types::getLen(tditems[i].fieldType);
    }
    
    return totalSize;
}

TupleDesc TupleDesc::merge(const TupleDesc &td1, const TupleDesc &td2) {
    // this implmentation doesn't handle overlapping fields
    std::vector<Types::Type> types;
    std::vector<std::string> names;
    
    // Add types and names from td1.
    for (size_t i = 0; i < td1.numFields(); ++i) {
        types.push_back(td1.getFieldType(i));
        names.push_back(td1.getFieldName(i));
    }
    
    // Add types and names from td2.
    for (size_t i = 0; i < td2.numFields(); ++i) {
        types.push_back(td2.getFieldType(i));
        names.push_back(td2.getFieldName(i));
    }
    
    // Create a new TupleDesc with the combined types and names.
    return TupleDesc(types, names);
}

std::string TupleDesc::to_string() const {
    std::string result;
    for (size_t i = 0; i < tditems.size(); ++i) {
        // first add the field type followed by field name in parenthesis
        result += Types::to_string(tditems[i].fieldType);
        result += "(" + tditems[i].fieldName + ") ";
    }
    return result;
}

bool TupleDesc::operator==(const TupleDesc &other) const {
    // check size
    if (tditems.size() != other.tditems.size()) {
        return false;
    }

    // will pass this check only if both fieldtype and fieldname match
    for (size_t i = 0; i < tditems.size(); ++i) {
        bool typeEquals = tditems[i].fieldType != other.tditems[i].fieldType;
        bool nameEquals = tditems[i].fieldName != other.tditems[i].fieldName;
        if (typeEquals || nameEquals) {
            // If any TDItem is different, return false
            return false;
        }
    }
    // If all TDItems match and sizes are the same, return true
    return true;
}

TupleDesc::iterator TupleDesc::begin() const {
    tditems.begin();
}

TupleDesc::iterator TupleDesc::end() const {
    tditems.end();
}

std::size_t std::hash<db::TupleDesc>::operator()(const db::TupleDesc &td) const {

}

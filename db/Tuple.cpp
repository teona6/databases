#include <db/Tuple.h>

using namespace db;

//
// Tuple
//

Tuple::Tuple(const TupleDesc &td, RecordId *rid) {
    tupleDesc = td;
    recordId = rid;
    // create fields vector with empty slots
    fields = std::vector<const Field*>(td.numFields(), nullptr);
}

const TupleDesc &Tuple::getTupleDesc() const {
    return tupleDesc;
}

const RecordId *Tuple::getRecordId() const {
    return recordId;
}

void Tuple::setRecordId(const RecordId *id) {
    recordId = id;
}

const Field &Tuple::getField(int i) const {
    // assumes that a valid index is passed
    return *fields[i];
}

void Tuple::setField(int i, const Field *f) {
    // assumes a valid index is passed
    fields[i] = f;
}

Tuple::iterator Tuple::begin() const {
    return fields.begin();
}

Tuple::iterator Tuple::end() const {
    return fields.end();
}

std::string Tuple::to_string() const {
    // TODO pa1.1: implement
    
}

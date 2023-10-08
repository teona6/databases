#include <db/SeqScan.h>

using namespace db;

// TODO pa1.6: implement
SeqScan::SeqScan(TransactionId *tid, int tableid, const std::string &tableAlias) {
    this->tid = tid;
    this->tableid = tableid;
    this->tableAlias = tableAlias;
}

std::string SeqScan::getTableName() const {
    // TODO pa1.6: implement
    return Database::getCatalog().getTableName(tableid);
    
}

std::string SeqScan::getAlias() const {
    // TODO pa1.6: implement
    return tableAlias;
}

void SeqScan::reset(int tabid, const std::string &tableAlias) {
    // TODO pa1.6: implement
    this->tableid = tabid;
    this->tableAlias = tableAlias;
}

const TupleDesc &SeqScan::getTupleDesc() const {
    // TODO pa1.6: implement
    const TupleDesc& originalDesc = Database::getCatalog().getDatabaseFile(tableid)->getTupleDesc();

    // Extract field types and names from the originalDesc
    std::vector<Types::Type> fieldTypes;
    std::vector<std::string> fieldNames;

    for (size_t i = 0; i < originalDesc.numFields(); ++i) {
        fieldTypes.push_back(originalDesc.getFieldType(i));
        fieldNames.push_back(tableAlias + "." + originalDesc.getFieldName(i));
    }

    // Create a new TupleDesc with updated field names and return it
    return TupleDesc(fieldTypes, fieldNames);
}

SeqScan::iterator SeqScan::begin() const {
    // TODO pa1.6: implement
    return SeqScanIterator(*Database::getCatalog().getDatabaseFile(tableid), tid, 0);
}

SeqScan::iterator SeqScan::end() const {
    // TODO pa1.6: implement
    return SeqScanIterator(*Database::getCatalog().getDatabaseFile(tableid), tid, -1);
    
}

//
// SeqScanIterator
//

// TODO pa1.6: implement
SeqScanIterator::SeqScanIterator(DbFile& file, TransactionId* t, int position) {
    this->dbFile = &file;
    this->tid = t;
    this->currentPosition = position;
    if (currentPosition != -1) {
        // If the position is not -1, advance to the desired position
        heapFileIt = dynamic_cast<HeapFile&>(file).begin();
        for (int i = 0; i < currentPosition; ++i) {
            ++heapFileIt;
        }
    }
}


bool SeqScanIterator::operator!=(const SeqScanIterator &other) const {
    // TODO pa1.6: implement
    return heapFileIt != other.heapFileIt;
}

SeqScanIterator &SeqScanIterator::operator++() {
    // TODO pa1.6: implement
    ++heapFileIt;
    return *this;
}

const Tuple &SeqScanIterator::operator*() const {
    // TODO pa1.6: implement
    return *heapFileIt;
}

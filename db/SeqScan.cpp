#include <db/SeqScan.h>

using namespace db;

SeqScan::SeqScan(TransactionId *tid, int tableid, const std::string &tableAlias) {
    this->tid = tid;
    this->tableid = tableid;
    this->tableAlias = tableAlias;
}

std::string SeqScan::getTableName() const {
    return Database::getCatalog().getTableName(tableid);
    
}

std::string SeqScan::getAlias() const {
    return tableAlias;
}

void SeqScan::reset(int tabid, const std::string &tableAlias) {
    this->tableid = tabid;
    this->tableAlias = tableAlias;
}

const TupleDesc &SeqScan::getTupleDesc() const {
    const TupleDesc& originalDesc = Database::getCatalog().getDatabaseFile(tableid)->getTupleDesc();

    // Extract field types and names from the originalDesc
    std::vector<Types::Type> fieldTypes;
    std::vector<std::string> fieldNames;

    for (size_t i = 0; i < originalDesc.numFields(); ++i) {
        fieldTypes.push_back(originalDesc.getFieldType(i));
        fieldNames.push_back(tableAlias + "_" + originalDesc.getFieldName(i));
    }

    // Create a new TupleDesc with updated field names and return it
    return TupleDesc(fieldTypes, fieldNames);
}

SeqScan::iterator SeqScan::begin() const {
    return SeqScanIterator(*Database::getCatalog().getDatabaseFile(tableid), tid, 0);
}

SeqScan::iterator SeqScan::end() const {
    return SeqScanIterator(*Database::getCatalog().getDatabaseFile(tableid), tid, -1);
    
}

//
// SeqScanIterator
//

SeqScanIterator::SeqScanIterator(DbFile& file, TransactionId* t, int position) {
    this->dbFile = &file;
    this->tid = t;
    this->currentPosition = position;
    // NOTE: with current implementation iterator has to start at the beginning and go to end
    if (currentPosition == 0) {
        heapFileIt = dynamic_cast<HeapFile&>(file).begin();
    } else if (currentPosition == -1) {
        heapFileIt = dynamic_cast<HeapFile&>(file).end();
    }
}

bool SeqScanIterator::operator!=(const SeqScanIterator &other) const {
    return this->heapFileIt != other.heapFileIt;
}

SeqScanIterator &SeqScanIterator::operator++() {
    ++heapFileIt;
    return *this;
}

const Tuple &SeqScanIterator::operator*() const {
    return *heapFileIt;
}

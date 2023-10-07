#include <db/RecordId.h>
#include <stdexcept>

using namespace db;

//
// RecordId
//

// TODO pa1.4: implement
RecordId::RecordId(const PageId *pid, int tupleno) {
    this->pid = pid;
    this->tupleno = tupleno;
}

bool RecordId::operator==(const RecordId &other) const {
    // TODO pa1.4: implement
    return (*pid == *(other.pid)) && (tupleno == other.tupleno);
}

const PageId *RecordId::getPageId() const {
    // TODO pa1.4: implement
    return pid;
}

int RecordId::getTupleno() const {
    // TODO pa1.4: implement
    return tupleno;
}

//
// RecordId hash function
//

std::size_t std::hash<RecordId>::operator()(const RecordId &r) const {
    std::hash<PageId> pidHasher; // Assuming PageId has a suitable hash function
    std::hash<int> tuplenoHasher;
    return pidHasher(*r.getPageId()) ^ tuplenoHasher(r.getTupleno());
}

#include <db/RecordId.h>
#include <stdexcept>

using namespace db;

//
// RecordId
//

RecordId::RecordId(const PageId *pid, int tupleno) {
    if (pid == nullptr) {
        throw std::invalid_argument("PageId cannot be nullptr");
    }
    this->pid = pid;
    this->tupleno = tupleno;
}

bool RecordId::operator==(const RecordId &other) const {
    return (*this->pid == *other.pid && this->tupleno == other.tupleno);
}

const PageId *RecordId::getPageId() const {
    return pid;
}

int RecordId::getTupleno() const {
    return tupleno;
}

std::size_t std::hash<RecordId>::operator()(const RecordId &r) const {
    std::hash<PageId> pidHasher; // Assuming PageId has a suitable hash function
    std::hash<int> tuplenoHasher;
    return pidHasher(*r.getPageId()) ^ tuplenoHasher(r.getTupleno());
}

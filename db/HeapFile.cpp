#include <db/HeapFile.h>
#include <db/TupleDesc.h>
#include <db/Page.h>
#include <db/PageId.h>
#include <db/HeapPage.h>
#include <stdexcept>
#include <sys/stat.h>
#include <fcntl.h>


using namespace db;

//
// HeapFile
//

// TODO pa1.5: implement
HeapFile::HeapFile(const char *fname, const TupleDesc &td) {
    this->td = td;
    this->heapFID = std::hash<std::string>{}(std::string(fname));
    this->fname = fname;
}

int HeapFile::getId() const {
    // TODO pa1.5: implement
    return heapFID;
}

const TupleDesc &HeapFile::getTupleDesc() const {
    // TODO pa1.5: implement
    return td;
}

Page *HeapFile::readPage(const PageId &pid) {
    // TODO pa1.5: implement

    int pageSize = Database::getBufferPool().getPageSize();

    // Calculate the offset in the file based on the page number and the page size
    off_t offset = static_cast<off_t>(pid.pageNumber()) * pageSize;
    std::ifstream file(fname, std::ios::binary | std::ios::in);

    file.seekg(offset, std::ios::beg);
    uint8_t *pageData = new uint8_t[pageSize];
    file.read(reinterpret_cast<char*>(pageData), pageSize);

    file.close();

    // Create a new HeapPage instance with the read data and return it
    return new HeapPage(static_cast<const HeapPageId&>(pid), pageData);
}

int HeapFile::getNumPages () {
    // TODO pa1.5: implement

    std::ifstream file(fname, std::ios::binary | std::ios::ate);
    std::streamsize fileSize = file.tellg();

    file.close();

    int pageSize = Database::getBufferPool().getPageSize();

    return static_cast<int>(fileSize / pageSize);
}

HeapFileIterator HeapFile::begin() const {
    // TODO pa1.5: implement
    return HeapFileIterator(*this, 0, 0);
    
}

HeapFileIterator HeapFile::end() const {
    // TODO pa1.5: implement
    return HeapFileIterator(*this, const_cast<HeapFile*>(this)->getNumPages(), 0);
}

//
// HeapFileIterator
//

// TODO pa1.5: implement
HeapFileIterator::HeapFileIterator(const HeapFile &file, int page = 0, int tuple = 0){
    heapFile = &file;
    currentPage = page;
    currentTuple = tuple;
}

bool HeapFileIterator::operator!=(const HeapFileIterator &other) const {
    // TODO pa1.5: implement
    return this->currentPage != other.currentPage || this->currentTuple != other.currentTuple;
}

Tuple &HeapFileIterator::operator*() const {
    // TODO pa1.5: implement
    HeapPageId pid(heapFile->getId(), currentPage);
    HeapPage *page = dynamic_cast<HeapPage*>(Database::getBufferPool().getPage(TransactionId(), &pid));
    HeapPageIterator it = page->begin();
    return *it;
}

HeapFileIterator &HeapFileIterator::operator++() {
    // TODO pa1.5: implement
    currentTuple++;
    HeapPageId pid(heapFile->getId(), currentPage);
    HeapPage *page = dynamic_cast<HeapPage*>(Database::getBufferPool().getPage(TransactionId(), &pid));
    
    while (currentPage < const_cast<HeapFile*>(heapFile)->getNumPages() && currentTuple >= page->getNumTuples()) {
        currentTuple = 0;
        currentPage++;
        if (currentPage < const_cast<HeapFile*>(heapFile)->getNumPages()) {
            page = dynamic_cast<HeapPage*>(Database::getBufferPool().getPage(TransactionId(), &pid));
        }
    }
    return *this;
    
}

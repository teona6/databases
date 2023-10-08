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
    int lastPage = const_cast<HeapFile*>(this)->getNumPages() - 1;  // The actual last page number
    HeapPageId pid(getId(), lastPage);
    auto rawPage = Database::getBufferPool().getPage(TransactionId(), &pid);
    HeapPage *lastHeapPage = dynamic_cast<HeapPage*>(rawPage);
    int lastTuple = lastHeapPage->getNumTuples() - 1;  // The actual last tuple number in the last page

    return HeapFileIterator(*this, lastPage, lastTuple); 
}

//
// HeapFileIterator
//

// TODO pa1.5: implement
HeapFileIterator::HeapFileIterator(const HeapFile &file, int page, int tuple) {
    heapFile = &file;
    this->currentPage = page;
    this->currentTuple = tuple;
}

bool HeapFileIterator::operator!=(const HeapFileIterator &other) const {
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
    // Get the current page
    HeapPageId pid(heapFile->getId(), currentPage);
    HeapPage *page = dynamic_cast<HeapPage*>(Database::getBufferPool().getPage(TransactionId(), &pid));

    int totalTuplesInCurrentPage = page->getNumTuples(); 

    if (currentTuple < totalTuplesInCurrentPage - 1) {
        currentTuple++;
    }else {
        int numPages = const_cast<HeapFile*>(heapFile)->getNumPages();
        if (currentPage < numPages - 1) { 
            currentPage++;
            currentTuple = 0;
        }
        // If we're on the last page and last tuple, set to the 'end' values
        else {
            currentPage = numPages;
            currentTuple = 0;
        }
    }

    return *this;
}


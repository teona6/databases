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

HeapFile::HeapFile(const char *fname, const TupleDesc &td) {
    this->td = td;
    this->heapFID = std::hash<std::string>{}(std::string(fname));
    this->fname = fname;
}

int HeapFile::getId() const {
    return heapFID;
}

const TupleDesc &HeapFile::getTupleDesc() const {
    return td;
}

Page *HeapFile::readPage(const PageId &pid) {
    // retrieve page size
    int pageSize = Database::getBufferPool().getPageSize();
    std::ifstream file(fname, std::ios::binary | std::ios::in);

    // Calculate the offset in the file based on the page number and the page size
    file.seekg(pid.pageNumber() * pageSize, std::ios::beg);
    uint8_t *pageData = new uint8_t[pageSize];
    file.read(reinterpret_cast<char*>(pageData), pageSize);
    file.close();

    // Create a new HeapPage instance 
    return new HeapPage(static_cast<const HeapPageId&>(pid), pageData);
}

int HeapFile::getNumPages () {
    std::ifstream file(fname, std::ios::binary | std::ios::ate);
    // get total file size and page size
    std::streamsize fileSize = file.tellg();
    int pageSize = Database::getBufferPool().getPageSize();

    return fileSize / pageSize;
}

HeapFileIterator HeapFile::begin() const {
    // iteration starts from the first page and first tuple
    return HeapFileIterator(*this, 0, 0);
    
}

HeapFileIterator HeapFile::end() const {
    // iteration goes up to the last page and last tuple
    int lastPage = const_cast<HeapFile*>(this)->getNumPages();
    HeapPageId pid(getId(), lastPage);
    HeapPage *lastHeapPage = dynamic_cast<HeapPage*>(Database::getBufferPool().getPage(TransactionId(), &pid));
    int lastTuple = lastHeapPage->getNumTuples();

    return HeapFileIterator(*this, lastPage, lastTuple); 
}

//
// HeapFileIterator
//

HeapFileIterator::HeapFileIterator(const HeapFile &file, int page, int tuple) {
    heapFile = &file;
    this->currentPage = page;
    this->currentTuple = tuple;
}

bool HeapFileIterator::operator!=(const HeapFileIterator &other) const {
    return this->currentPage != other.currentPage || this->currentTuple != other.currentTuple;
}

Tuple &HeapFileIterator::operator*() const {
    // this operator for now will be pointing at the begining of the page
    HeapPageId pid(heapFile->getId(), currentPage);
    HeapPage *page = dynamic_cast<HeapPage*>(Database::getBufferPool().getPage(TransactionId(), &pid));
    HeapPageIterator it = page->begin();
    return *it;
}

HeapFileIterator &HeapFileIterator::operator++() {
    // Get the current page
    HeapPageId pid(heapFile->getId(), currentPage);
    HeapPage *page = dynamic_cast<HeapPage*>(Database::getBufferPool().getPage(TransactionId(), &pid));
    int numTuples = page->getNumTuples(); 

    // accumulate the tuples count
    if (currentTuple < numTuples) {
        currentTuple++;
    }else {
        // once we reach the last tuple move to the next page
        int numPages = const_cast<HeapFile*>(heapFile)->getNumPages();
        if (currentPage < numPages) { 
            currentPage++;
            // reset the current tuple to 0 because we are moving to next page
            currentTuple = 0;
        }
        // If we're on the last page and last tuple, set the end values
        else {
            currentPage = numPages;
            currentTuple = 0;
        }
    }

    return *this;
}


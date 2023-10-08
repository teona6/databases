#include <db/BufferPool.h>
#include <db/Database.h>

using namespace db;

// TODO pa1.3: implement
BufferPool::BufferPool(int numPages) {
    // Initialize the number of pages
    this->numPages = numPages;
    this->currentPageCount = 0;
    this->pageArray = new Page*[numPages];
    this->pageIdArray = new PageId*[numPages];
}

Page *BufferPool::getPage(const TransactionId &tid, PageId *pid) {
    // TODO pa1.3: implement

    for (int i = 0; i < currentPageCount; ++i) {
        if (pageIdArray[i] && *pageIdArray[i] == *pid) {
            return pageArray[i];
        }
    }
    Page *newPage = Database::getCatalog().getDatabaseFile(pid->getTableId())->readPage(*pid);
    if (currentPageCount < numPages) {
        pageArray[currentPageCount] = newPage;
        *pageIdArray[currentPageCount] = *pid;
        currentPageCount++;
    }
    return newPage;
}

void BufferPool::evictPage() {
    // do nothing for now
}

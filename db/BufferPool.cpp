#include <db/BufferPool.h>
#include <db/Database.h>

using namespace db;

BufferPool::BufferPool(int numPages) {
    this->numPages = numPages;
    this->currentPageCount = 0;
    // for now pages will simpl ybe stored in a fixed size array
    this->pageArray = new Page*[numPages];
}

Page *BufferPool::getPage(const TransactionId &tid, PageId *pid) {

    // check if the page already exists in the bufferpool
    for (int i = 0; i < currentPageCount; ++i) {
        if (pageArray[i]->getId() == *pid) {
            return pageArray[i];
        }
    }
    // retrieve the new page
    Page *newPage = Database::getCatalog().getDatabaseFile(pid->getTableId())->readPage(*pid);
    // add the page to bufferpool
    if (currentPageCount < numPages) {
        pageArray[currentPageCount] = newPage;
        currentPageCount++;
    }
    return newPage;
}

void BufferPool::evictPage() {
    // do nothing for now
}

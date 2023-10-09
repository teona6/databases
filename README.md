
# Design Choices

The implemtation decisions for the `Tuple`, `Tupledesc`, `HeapPage` didn't require any design decisions, since I simply followed the assignment guide. However I will explain the design choices for the following.

## Catalog

My catalog is a simple array of Table objects. While this may not be the most efficient data structure for lookups, it meets the current requirements. A better approach might be a map data structure, where table names are keys. However, my initial attempts at integrating a map caused compiler errors on Gradescope (at that time we couldn't see those errors), so I decided to stick with the array for now.

During testing, the `getTableId` method required handling invalid arguments through exceptions. So I added similar exception handling in other methods within the file.

For the `addTable` method, my implementation checks if the `TableID` or Name (or both) match any existing table. If a match is detected, the table is updated in place. If no match is found, then the new table is added to the catalog.

The rest of the catalog implementation, including functionalities like finding the ID, is straightforward. I simply identify the correct table based on the ID and retrieve the relevant values.

## HeapFile

For now, I am only hashing the file name. Because to hash the full path, I had to include "filesystem", but it would fail on Gradescope without displaying the actual error. Later, I plan to resolve this and ensure the full path is hashed.

## SeqScan Iterator

The `SeqScan` iterator is a core component that facilitates sequential traversal over tuples in a table. This iterative process also involves `BufferPool`, `HeapFile`, and `HeapFileIterator`. So now I will explain my overall workflow.

When a SeqScan operation is initiated, as demonstrated by `db::SeqScan f(&tid, table.getId())` and `EXPECT_NE(f.begin(), f.end());` in the test, the iterator starts its traversal from the first tuple. The `f.begin` method activates the `SeqScan` constructor with the associated `dbfile`, `transaction id`, and an initial tuple position set to 0.

```
SeqScan::iterator SeqScan::begin() const {
    return SeqScanIterator(*Database::getCatalog().getDatabaseFile(tableid), tid, 0);
}
```

The `f.end` invokes the SeqScan iterator with the tuple position set to -1. This design choice currently means the iterator can only initiate from the start and doesn't support arbitrary position starts (I plan to add this feature later).

My `SeqScan` iterator relies on the `HeapFileIterator`. Depending on the currentPosition value, the HeapFileIterator starts with `heapFile.begin` or ends with `heapFile.end`.

```
if (currentPosition == 0) {
    heapFileIt = dynamic_cast<HeapFile&>(file).begin();
} else if (currentPosition == -1) {
    heapFileIt = dynamic_cast<HeapFile&>(file).end();
}
```

 When the iterator progresses `(operator++())`, it fetches the current page with the BufferPool's `getPage` method. This method internally first checks if the page already exists in the buffer pool (a simple array for now); if not, it fetches the page, adds it to the buffer pool, and then returns it.

After the relevant page is fetched, the total number of tuples on this page is calculated with `getNumTuples()`.

The iterator then starts progressing through the tuples. If there are more tuples on the current page, the iterator moves to the next tuple. Once all tuples on the page are covered, the iterator shifts to the next page.

The iterator maintains its position with two indices: currentPage and currentTuple. The traversal reaches its endpoint when the iterator encounters the last tuple of the last page.

# Changes to the API

No changes have been made to the API, except for adding the forward declaration to the Heapfile. This was because I wanted to use class `HeapFile` in my `HeapFileIterator`.

# Challenges

I spent about 7 hours cumulative on this assignment. I didn't encounter any major challenges. I spent a while figuring out the issue with the `Seqscan ` iterator. Simply because it internally depended on `Heapfileiterator`, and `Bufferpool` `getpage`. Particularly I kept getting stuck in the infinite loop. But then I modified the iterator `end` condition to depend on the total number of pages and number of tuples. And after this the infinite loop was resolved. Additionally sometimes the tests pass locally but faling on gradescope so if possible it would be nice to have the verbose output on the gradescope 
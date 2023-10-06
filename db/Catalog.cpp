#include <db/Catalog.h>
#include <db/TupleDesc.h>
#include <stdexcept>

using namespace db;

void Catalog::addTable(DbFile *file, const std::string &name, const std::string &pkeyField) {
    // loop through references to avoid copying
    for (auto &table : tables) {
        // check if a table with same name or id exist already
        if (table.file->getId() == file->getId() || table.name == name) {
            table.file = file;
            table.name = name;
            table.pkeyField = pkeyField;
            return;
        }
    }
    
    // If not, add the new table
    tables.push_back(Table(file, name, pkeyField));
}

int Catalog::getTableId(const std::string &name) const {
    for (const auto &table : tables) {
        // find table based on the name assuming that the names are unique
        if (table.name == name) {
            return table.file->getId();
        }
    }
    throw std::invalid_argument("Table not found");
}

const TupleDesc &Catalog::getTupleDesc(int tableid) const {
    for (const auto &table : tables) {
        // find the table based on the id
        if (table.file->getId() == tableid) {
            return table.file->getTupleDesc();
        }
    }
    throw std::invalid_argument("Table not found");
}

DbFile *Catalog::getDatabaseFile(int tableid) const {
    for (const auto &table : tables) {
        // find the table based on the id
        if (table.file->getId() == tableid) {
            return table.file;
        }
    }
    throw std::invalid_argument("Table not found");
}

std::string Catalog::getPrimaryKey(int tableid) const {
    for (const auto &table : tables) {
        // find the table based on the id
        if (table.file->getId() == tableid) {
            return table.pkeyField;
        }
    }
    throw std::invalid_argument("Table not found");
}

std::string Catalog::getTableName(int id) const {
    for (const auto &table : tables) {
        // find the table based on the id
        if (table.file->getId() == id) {
            return table.name;
        }
    }
    throw std::invalid_argument("Table not found");
}

void Catalog::clear() {
    tables.clear();
}


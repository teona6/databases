#include <db/Catalog.h>
#include <db/TupleDesc.h>
#include <stdexcept>

using namespace db;

void Catalog::addTable(DbFile *file, const std::string &name, const std::string &pkeyField) {
    int id = file->getId();
    Table newTable(file, name, pkeyField);

    // If ID or name already exists, update the entries
    auto itById = tablesById.find(id);
    if (itById != tablesById.end()) {
        // Remove the old name from idByName
        std::string oldName = itById->second.name;
        idByName.erase(oldName);
        // Update the entry in tablesById
        itById->second = std::move(newTable);
    } else {
        // Insert the new table into tablesById
        tablesById.emplace(id, std::move(newTable));
    }

    // Update or insert the entry in idByName
    idByName[name] = id;
}

int Catalog::getTableId(const std::string &name) const {
    auto it = idByName.find(name);
    if (it == idByName.end()) {
        throw std::invalid_argument("Invalid table id");
    }
    return it->second;
}
const TupleDesc &Catalog::getTupleDesc(int tableid) const {
    auto it = tablesById.find(tableid);
    if (it == tablesById.end()) {
        throw std::invalid_argument("Invalid table id");
    }
    return it->second.file->getTupleDesc();
}

DbFile *Catalog::getDatabaseFile(int tableid) const {
    auto it = tablesById.find(tableid);
    if (it == tablesById.end()) {
        throw std::invalid_argument("Invalid table id");
    }
    return it->second.file;
}

std::string Catalog::getPrimaryKey(int tableid) const {
    auto it = tablesById.find(tableid);
    return it->second.pkeyField;
}

std::string Catalog::getTableName(int id) const {
    auto it = tablesById.find(id);
    if (it == tablesById.end()) {
        throw std::invalid_argument("Invalid table id");
    }
    return it->second.name;
}

void Catalog::clear() {
    tablesById.clear();
    idByName.clear();
}
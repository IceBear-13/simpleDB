#ifndef INSERT_H
#define INSERT_H

#include "../storage.h"
#include "../table.h"
#include "../value.h"

class InsertQuery {
private:
  Storage& storage;
public:
  InsertQuery(Storage& storage) : storage(storage) {}
  
  void insertInto(const std::string& tableName, const std::vector<Value>& values) {
    Table& table = storage.getTable(tableName);
    table.insertRow(values);
  }

};

#endif
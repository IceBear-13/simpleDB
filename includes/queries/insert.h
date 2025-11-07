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
  
  /**
   * Inserts a new row into the specified table.
   * 
   * @param tableName Name of the table to insert into.
   * @param values Vector of Value objects representing the row to insert.
   * 
   * @example
   * InsertQuery insertQuery(storage);
   * insertQuery.insertInto("users", {Value(1), Value("Alice"), Value(30)});
   */
  void insertInto(const std::string& tableName, const std::vector<Value>& values) {
    Table& table = storage.getTable(tableName);
    table.insertRow(values);
  }

};

#endif
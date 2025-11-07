#ifndef CREATE_H
#define CREATE_H

#include "../storage.h"
#include "../table.h"
#include "../value.h"

class CreateQuery {
private:
  Storage& storage;
public:
  
  CreateQuery(Storage& storage) : storage(storage) {}

  /**
   * Creates a new table in the storage.
   * 
   * @param tableName Name of the table to create.
   * @param columns Vector of column names for the new table.
   * 
   * @example
   * CreateQuery createQuery(storage);
   * createQuery.createTable("users", {"id", "name", "age"});
   */
  void createTable(const std::string& tableName, const std::vector<std::string>& columns) {
    storage.createTable(tableName, columns);
  }
  
  
};


#endif
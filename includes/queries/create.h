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

  void createTable(const std::string& tableName, const std::vector<std::string>& columns) {
    storage.createTable(tableName, columns);
  }
  
  
};


#endif
#ifndef CREATE_H
#define CREATE_H

#include "../storage.h"
#include "../table.h"
#include "../value.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

class CreateProcessor {
private:
  Storage& storage;
  std::stringstream ss;
public:
    CreateProcessor(Storage& store) : storage(store) {}
    
    /**
     * Executes a simple SQL-like query.
     * Supports CREATE TABLE, INSERT INTO, SELECT, and WHERE statements.
     * 
     * @param query The SQL-like query string to execute.
     * @example
     * QueryProcessor qp(storage);
     * qp.execute("CREATE TABLE users id INT,name STRING,age INT");
     * qp.execute("INSERT INTO users VALUES 1,\"Alice\",30");
     * qp.execute("SELECT * FROM users");
     */
    void execute(const std::string query){
      ss.str(query);
      std::string tableToken, tableName;
      ss >> tableToken >> tableName;
      std::vector<std::string> columns;
      std::vector<Value::Type> columnTypes;
      std::string col, type;
      while(ss >> col >> type) {
        if(type.back() == ',') {
          type.pop_back();
        }
        columns.push_back(col);
        columnTypes.push_back(Value::stringToType(type));
      }
      storage.createTable(tableName, columns, columnTypes);
      std::cout << "Table " << tableName << " created with columns: ";
      for(const auto& column : columns) {
        std::cout << column << " ";
      }
      std::cout << std::endl;
    }

};


#endif
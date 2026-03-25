#ifndef QUERY_PROCESSOR_H
#define QUERY_PROCESSOR_H

#include "storage.h"
#include "table.h"
#include "value.h"
#include "query_handler/create.h"
#include "query_handler/insert.h"
#include "query_handler/select.h"
#include <iostream>
#include <string>
#include <sstream>

class QueryProcessor {
private:
  Storage& storage;

public:
  QueryProcessor(Storage& store) : storage(store){}
  
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
    std::stringstream ss(query);
    std::string command;

    ss >> command;
    
    if(command == "CREATE") {
      CreateProcessor createProcessor(storage);
      createProcessor.execute(query);
    } else if(command == "INSERT") {
      InsertProcessor insertProcessor(storage);
      insertProcessor.execute(query);
    } else if(command == "SELECT") {
      SelectProcessor selectProcessor(storage);
      selectProcessor.execute(query);

    } else {
      std::cerr << "Unknown command: " << command << std::endl;
    }
  } 
  
};

#endif

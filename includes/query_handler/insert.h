#ifndef INSERT_PROCESSOR_H
#define INSERT_PROCESSOR_H

#include "../storage.h"
#include "../table.h"
#include "../value.h"
#include "../indexing/index_manager.h"
#include "../indexing/btree.h"
#include "../queries/insert.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

class InsertProcessor {
private:
  Storage& storage;
public:
  InsertProcessor(Storage& storage) : storage(storage) {}

  void execute(const std::string& query){
      std::stringstream ss(query);
      std::string intoToken, tableName, valuesToken;
      ss >> intoToken >> tableName >> valuesToken;
      
      if(intoToken != "INTO" || valuesToken != "VALUES") {
        std::cerr << "Invalid INSERT syntax. Use: INSERT INTO tablename VALUES val1, val2, ..." << std::endl;
        return;
      }
      
      std::vector<Value> values;
      std::string val;
      while(ss >> val) {
        if(val.back() == ',') {
          val.pop_back();
        }

        if(val == "true" || val == "false") {
          values.emplace_back(val == "true");
        } else if(val.size() >= 2 && val.front() == '"' && val.back() == '"') {
          // Handle quoted strings
          std::string str_content = val.substr(1, val.size() - 2);
          values.emplace_back(str_content);
        } else {
          try {
            int intValue = std::stoi(val);
            values.emplace_back(intValue);
          } catch(...) {
            values.emplace_back(val);
          }
        }
      }
      
      try {
        InsertQuery insertQuery(storage);
        insertQuery.insertInto(tableName, values);
        // Also persist the table after insertion
        storage.persistTable(tableName);
        std::cout << "Inserted values into " << tableName << std::endl;
      } catch(const std::exception& e) {
        std::cerr << "Insert failed: " << e.what() << std::endl;
      } catch(const char* msg) {
        std::cerr << "Insert failed: " << msg << std::endl;
      }
    }

};


#endif
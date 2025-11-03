#ifndef QUERY_PROCESSOR_H
#define QUERY_PROCESSOR_H

#include "storage.h"
#include "table.h"
#include "value.h"
#include "queries/create.h"
#include "queries/insert.h"
#include "queries/select.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

class QueryProcessor {
private:
  Storage& storage;

public:
  QueryProcessor(Storage& store) : storage(store) {}
  
  void execute(const std::string query){
    std::stringstream ss(query);
    std::string command;

    ss >> command;
    
    if(command == "CREATE") {
      std::string tableToken, tableName;
      ss >> tableToken >> tableName;
      std::vector<std::string> columns;
      std::string col;
      while(ss >> col) {
        if(col.back() == ',') {
          col.pop_back();
        }
        columns.push_back(col);
      }
      storage.createTable(tableName, columns);
      std::cout << "Table " << tableName << " created with columns: ";
      for(const auto& column : columns) {
        std::cout << column << " ";
      }
      std::cout << std::endl;
    } else if(command == "INSERT") {
      std::string temp, into, tableName, valuesToken;
      ss >> temp >> tableName >> valuesToken;
      std::vector<Value> values;
      std::string val;
      while(ss >> val) {
        std::cout << val << " ";
        if(val.back() == ',') {
          val.pop_back();
        }

        if(val == "true" || val == "false") {
          values.push_back(Value(val == "true"));
        } else if(val.front() == '"' && val.back() == '"') {
          values.push_back(Value(val.substr(1, val.size() - 2).c_str()));
        } else {
          try {
            int intValue = std::stoi(val);
            values.push_back(Value(intValue));
          } catch(const std::invalid_argument&) {
            std::cerr << "Invalid value: " << val << std::endl;
          }
        }
      }
      InsertQuery insertQuery(storage);
      insertQuery.insertInto(tableName, values);
      std::cout << "Inserted values into " << tableName << std::endl;
    } else if(command == "SELECT") {
      std::vector<std::string> columns;
      std::string col;
      while(ss >> col && col != "FROM") {
        if(col.back() == ',') {
          col.pop_back();
        }
        columns.push_back(col);
      }
      std::string tableName;
      ss >> tableName;
      SelectQuery selectQuery(storage);
      if(columns.size() == 1 && columns[0] == "*") {
        const Table& table = selectQuery.selectAll(tableName);
        // Print all rows
        for(size_t i = 0; i < table.getRowCount(); i++) {
          const std::vector<Value>& row = table.getRow(i);
          for(const Value& val : row) {
            switch(val.getType()) {
              case Value::INT:
                std::cout << val.getInt() << " ";
                break;
              case Value::STRING:
                std::cout << val.getString() << " ";
                break;
              case Value::BOOL:
                std::cout << (val.getBool() ? "true" : "false") << " ";
                break;
              default:
                break;
            }
          }
          std::cout << std::endl;
        }
      } else {
        auto result = selectQuery.selectColumns(tableName, columns);
        size_t rowCount = result[columns[0]].size();
        for(size_t i = 0; i < rowCount; i++) {
          for(const std::string& colName : columns) {
            const Value& val = result[colName][i];
            switch(val.getType()) {
              case Value::INT:
                std::cout << val.getInt() << " ";
                break;
              case Value::STRING:
                std::cout << val.getString() << " ";
                break;
              case Value::BOOL:
                std::cout << (val.getBool() ? "true" : "false") << " ";
                break;
              default:
                break;
            }
          }
          std::cout << std::endl;
        }
      }
      // Where statement to be implemented later 
    } else {
      std::cerr << "Unknown command: " << command << std::endl;
    }
  } 
  
};

#endif

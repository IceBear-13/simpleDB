#ifndef QUERY_PROCESSOR_H
#define QUERY_PROCESSOR_H

#include "storage.h"
#include "table.h"
#include "value.h"
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
  
  /**
   * Executes a simple SQL-like query.
   * Supports CREATE TABLE, INSERT INTO, SELECT, and WHERE statements.
   * 
   * @param query The SQL-like query string to execute.
   * @example
   * QueryProcessor qp(storage);
   * qp.execute("CREATE TABLE users id,name,age");
   * qp.execute("INSERT INTO users VALUES 1,\"Alice\",30");
   * qp.execute("SELECT * FROM users");
   */
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
      std::string whereToken;
      ss >> tableName;
      ss >> whereToken;
      SelectQuery selectQuery(storage);
      if(whereToken == "WHERE") {
        std::string conditionColumn, equalsToken, conditionValueStr;
        ss >> conditionColumn >> equalsToken >> conditionValueStr;
        if(equalsToken != "=") {
          std::cerr << "Invalid WHERE clause syntax. Use: WHERE column = value" << std::endl;
          return;
        }
        Value conditionValue;
        if(conditionValueStr.front() == '"' && conditionValueStr.back() == '"') {
          conditionValue = Value(conditionValueStr.substr(1, conditionValueStr.size() - 2));
        } else if(conditionValueStr == "true" || conditionValueStr == "false") {
          conditionValue = Value(conditionValueStr == "true");
        } else {
          try {
            conditionValue = Value(std::stoi(conditionValueStr));
          } catch(...) {
            conditionValue = Value(conditionValueStr);
          }
        }
        auto result = selectQuery.selectWhere(tableName, columns, conditionValue, conditionColumn);
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
        }
        std::cout << std::endl;
        return;
      }
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

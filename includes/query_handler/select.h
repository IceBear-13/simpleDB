#ifndef SELECT_PROCESSOR_H
#define SELECT_PROCESSOR_H

#include "../storage.h"
#include "../table.h"
#include "../value.h"
#include "../queries/select.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>

class SelectProcessor {
private:
  Storage& storage;

  void printValue(const Value& val) const {
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

  void printTableRows(const Table& table, const std::vector<std::string>& columns) const {
    std::vector<size_t> colIndices;
    const auto& colIndexMap = table.getColumnIndexMap();
    
    for (const auto& col : columns) colIndices.push_back(colIndexMap.at(col));
    
    for(size_t i = 0; i < table.getRowCount(); i++) {
      const auto& row = table.getRow(i);
      for (size_t idx : colIndices) {
        printValue(row[idx]);
      }
      std::cout << "\n";
    }
  }

  void printResultMap(const std::unordered_map<std::string, std::vector<Value>>& result, 
                      const std::vector<std::string>& columns) const {
    if (result.empty()) return;
    size_t rowCount = result.at(columns[0]).size();
    for(size_t i = 0; i < rowCount; i++) {
      for(const auto& col : columns) {
        printValue(result.at(col)[i]);
      }
      std::cout << "\n";
    }
  }

  Value parseValue(const std::string& valStr) const {
    if(valStr.front() == '"' && valStr.back() == '"') {
      return Value(valStr.substr(1, valStr.size() - 2));
    } else if(valStr == "true" || valStr == "false") {
      return Value(valStr == "true");
    }
    try {
      return Value(std::stoi(valStr));
    } catch(...) {
      return Value(valStr);
    }
  }

public:
  SelectProcessor(Storage& store) : storage(store) {}

  void execute(const std::string& query) {
    std::stringstream ss(query);
    std::string selectCmd;
    ss >> selectCmd;  // consume "SELECT"
    
    // Parse column names
    std::vector<std::string> columns;
    std::string col;
    while(ss >> col && col != "FROM") {
      if(col.back() == ',') col.pop_back();
      columns.push_back(col);
    }
    
    std::string tableName, whereToken;
    ss >> tableName >> whereToken;
    
    SelectQuery selectQuery(storage);
    
    try {
      // Handle SELECT * FROM table
      if(columns.size() == 1 && columns[0] == "*") {
        const Table& table = selectQuery.selectAll(tableName);
        std::vector<std::string> allColumns = table.getColumnNames();
        printTableRows(table, allColumns);
        return;
      }
      
      // Handle WHERE clause
      if(whereToken == "WHERE") {
        std::string conditionColumn, equalsToken, conditionValueStr;
        ss >> conditionColumn >> equalsToken >> conditionValueStr;
        
        if(equalsToken != "=") {
          std::cerr << "Invalid WHERE clause syntax. Use: WHERE column = value\n";
          return;
        }
        
        auto result = selectQuery.selectWhere(tableName, columns, parseValue(conditionValueStr), conditionColumn);
        printResultMap(result, columns);
        return;
      }
      
      // Handle SELECT columns FROM table (no WHERE)
      auto result = selectQuery.selectColumns(tableName, columns);
      printResultMap(result, columns);
    } catch(const std::exception& e) {
      std::cerr << "SELECT failed: " << e.what() << std::endl;
    }
  }
};

#endif

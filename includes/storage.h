#ifndef STORAGE_H
#define STORAGE_H

#include "table.h"
#include "value.h"
#include <filesystem>
#include <string>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <cstdlib>  // for getenv

class Storage {
private:
  std::string dbName;
  std::unordered_map<std::string, Table> tables;
  
  std::string get_base_path() {
    // Option 1: Use home directory
    const char* home = getenv("HOME");
    if (home) {
      return std::string(home) + "/testDB/" + dbName;
    }
    // Option 2: Fallback to current directory
    return "./testDB/" + dbName;
  }
  
  std::string get_table_path(const std::string& table_name) {
    return get_base_path() + "/" + table_name + ".tbl";
  }
  
public:
  Storage(const std::string& name) : dbName(name) {
    std::filesystem::create_directories(get_base_path());
    loadAllTables();
  }

  void loadAllTables() {
    std::string basePath = get_base_path();
    if (!std::filesystem::exists(basePath)) {
      return; // No directory exists yet
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(basePath)) {
      if (entry.is_regular_file() && entry.path().extension() == ".tbl") {
        std::string tableName = entry.path().stem().string();
        try {
          loadTable(tableName);
          std::cout << "Loaded table: " << tableName << std::endl;
        } catch (const std::exception& e) {
          std::cerr << "Failed to load table " << tableName << ": " << e.what() << std::endl;
        } catch (const char* msg) {
          std::cerr << "Failed to load table " << tableName << ": " << msg << std::endl;
        }
      }
    }
  }

  void createTable(const std::string& tableName, const std::vector<std::string>& columns) {
    if (tables.find(tableName) != tables.end()) {
      throw "Table already exists";
    }
    tables[tableName] = Table(tableName, columns);
  }

  void persistTable(const std::string& tableName) {
    auto it = tables.find(tableName);
    if (it == tables.end()) {
      throw "Table not found";
    }

    std::ofstream outFile(get_table_path(tableName));
    if (!outFile) {
      throw "Failed to open file for writing";
    }

    Table& table = it->second;
    std::vector<std::string> columnNames = table.getColumnNames();
    
    // Write column count
    outFile << columnNames.size() << "\n";
    
    // Write column names
    for (const std::string& colName : columnNames) {
      outFile << colName << "\n";
    }
    
    // Write row count
    outFile << table.getRowCount() << "\n";
    
    // Write rows
    for(size_t i = 0; i < table.getRowCount(); i++) {
      const std::vector<Value>& row = table.getRow(i);
      for (size_t j = 0; j < row.size(); j++) {
        const Value& val = row[j];
        Value::Type type = val.getType();
        
        // Write type identifier
        outFile << static_cast<int>(type) << " ";
        
        // Write value based on type
        switch (type) {
          case Value::INT:
            outFile << val.getInt();
            break;
          case Value::STRING:
            outFile << "\"" << val.getString() << "\"";
            break;
          case Value::BOOL:
            outFile << (val.getBool() ? "true" : "false");
            break;
          default:
            break;
        }
        
        if (j < row.size() - 1) {
          outFile << " ";
        }
      }
      outFile << "\n";
    }
    
    outFile.close();
  }
  
  void loadTable(std::string& tableName) {
    std::ifstream inFile(get_table_path(tableName));
    if (!inFile) {
      throw "Failed to open file for reading";
    }

    size_t columnCount;
    inFile >> columnCount;
    inFile.ignore(); // Skip newline

    std::vector<std::string> columnNames;
    for (size_t i = 0; i < columnCount; ++i) {
      std::string colName;
      std::getline(inFile, colName);
      columnNames.push_back(colName);
    }

    Table table(tableName, columnNames);
    
    size_t rowCount;
    inFile >> rowCount;
    inFile.ignore(); // Skip newline
    
    for (size_t i = 0; i < rowCount; i++) {
      std::vector<Value> row;
      std::string line;
      std::getline(inFile, line);
      std::stringstream ss(line);
      
      for (size_t j = 0; j < columnCount; j++) {
        int typeInt;
        ss >> typeInt;
        Value::Type type = static_cast<Value::Type>(typeInt);
        
        std::string valueStr;
        ss >> valueStr;
        
        switch (type) {
          case Value::INT:
            row.push_back(Value(std::stoi(valueStr)));
            break;
          case Value::STRING:
            // Remove quotes
            if (valueStr.front() == '"' && valueStr.back() == '"') {
              valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            row.push_back(Value(valueStr.c_str()));
            break;
          case Value::BOOL:
            row.push_back(Value(valueStr == "true"));
            break;
          default:
            break;
        }
      }
      table.insertRow(row);
    }

    tables[tableName] = table;
  }
  
  Table& getTable(const std::string& tableName) {
    auto it = tables.find(tableName);
    if (it == tables.end()) {
      throw std::out_of_range("Table not found");
    }
    return it->second;
  }

  const Table& getTableConst(const std::string& tableName) const {
    auto it = tables.find(tableName);
    if (it == tables.end()) {
      throw std::out_of_range("Table not found");
    }
    return it->second;
  }

  std::vector<Table>& getAllTables() {
    std::vector<Table>* tableVec = new std::vector<Table>();

    for (auto& pair : tables) {
      tableVec->push_back(pair.second);
    }

    return *tableVec;
  }

};

#endif
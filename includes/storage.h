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

class Storage {
private:
  std::string dbName;
  std::unordered_map<std::string, Table> tables;
  std::string get_table_path(const std::string& table_name) {
    return "~/testDB/" + dbName + "/" + table_name + ".tbl";
  }
public:
  Storage(const std::string& name) : dbName(name) {
    std::filesystem::create_directory("~/testDB/" + dbName);
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

    std::ofstream outFile(get_table_path(tableName), std::ios::binary | std::ios::app);
    if (!outFile) {
      throw "Failed to open file for writing";
    }

    Table& table = it->second;
    std::vector<std::string> columnNames = table.getColumnNames();
    outFile << columnNames.size() << "\n";
    // Write column names
    for (const std::string& colName : columnNames) {
      size_t len = colName.size();
      outFile.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
      outFile.write(colName.c_str(), len);
      outFile << "\n"; // Delimiter between column names
    }

    outFile << "\n"; // New line after column names
    
    for(size_t i = 0; i < table.getRowCount(); i++) {
      const std::vector<Value>& row = table.getRow(i);
      for (const Value& val : row) {
        Value::Type type = val.getType();
        outFile.write(reinterpret_cast<const char*>(&type), sizeof(Value::Type));
        switch (type) {
          case Value::INT: {
            int intValue = val.getInt();
            outFile.write(reinterpret_cast<const char*>(&intValue), sizeof(int));
            break;
          }
          case Value::STRING: {
            const char* strValue = val.getString();
            size_t len = strlen(strValue);
            outFile.write(reinterpret_cast<const char*>(&len), sizeof(size_t));
            outFile.write(strValue, len);
            break;
          }
          case Value::BOOL: {
            bool boolValue = val.getBool();
            outFile.write(reinterpret_cast<const char*>(&boolValue), sizeof(bool));
            break;
          }
          default:
            break;
        }
        outFile << "|"; // Delimiter between values
      }
      outFile << "\n"; // New line for new row
    }

    
  }
  
  void loadTable(std::string& tableName) {
    std::ifstream inFile(get_table_path(tableName), std::ios::binary);
    if (!inFile) {
      throw "Failed to open file for reading";
    }

    size_t columnCount;
    inFile >> columnCount;

    std::vector<std::string> columnNames;
    for (size_t i = 0; i < columnCount; ++i) {
      size_t len;
      inFile.read(reinterpret_cast<char*>(&len), sizeof(size_t));
      if (!inFile) throw std::runtime_error("Failed reading column name length");

      std::string colName;
      colName.resize(len);
      inFile.read(&colName[0], len);
      if (!inFile) throw std::runtime_error("Failed reading column name");

      columnNames.push_back(std::move(colName));
    }

    Table table = Table(tableName, columnNames);
    std::string line;
    while (std::getline(inFile, line)) {
      std::vector<Value> row;
      size_t pos = 0;
      while (pos < line.size()) {
        Value::Type type;
        inFile.read(reinterpret_cast<char*>(&type), sizeof(Value::Type));
        switch (type) {
          case Value::INT: {
            int intValue;
            inFile.read(reinterpret_cast<char*>(&intValue), sizeof(int));
            row.push_back(Value(intValue));
            break;
          }
          case Value::STRING: {
            size_t len;
            inFile.read(reinterpret_cast<char*>(&len), sizeof(size_t));
            char* strValue = new char[len + 1];
            inFile.read(strValue, len);
            strValue[len] = '\0';
            row.push_back(Value(strValue));
            delete[] strValue;
            break;
          }
          case Value::BOOL: {
            bool boolValue;
            inFile.read(reinterpret_cast<char*>(&boolValue), sizeof(bool));
            row.push_back(Value(boolValue));
            break;
          }
          default:
            break;
        }
        pos = line.find("|", pos);
        if (pos == std::string::npos) break; // No more delimiters
        pos++; // Move past delimiter
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

};

#endif

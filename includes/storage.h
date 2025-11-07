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

/*
===========================================================================
Storage Class:
Manages multiple tables, handles persistence to disk, and provides methods
to create, load, and retrieve tables.

Storage class acts as an in memory/temporary database management system (DBMS) that allows
creating, storing, retrieving, and persisting tables. It uses the filesystem to
save and load table data, ensuring data durability across program executions.
===========================================================================
*/
class Storage {
private:
  std::string dbName;
  std::unordered_map<std::string, Table> tables;
  
  std::string get_base_path() {
    const char* home = getenv("HOME");
    if (home) {
      return std::string(home) + "/simpledb/" + dbName;
    }
    return "./simpledb/" + dbName;
  }
  
  std::string get_table_path(const std::string& table_name) {
    return get_base_path() + "/" + table_name + ".tbl";
  }
  
public:
  /**
   * Constructor that initializes the Storage with a database name.
   * Creates the necessary directory structure and loads existing tables from disk.
   * 
   * @param name Name of the database.
   * @example
   * Storage storage("myDatabase");
   */
  Storage(const std::string& name) : dbName(name) {
    std::filesystem::create_directories(get_base_path());
    loadAllTables();
  }

  /**
   * Loads all tables from disk into memory.
   * Skips files that cannot be loaded and logs errors.
   * 
   * @return void
   * 
   * @example
   * Storage storage("myDatabase");
   * storage.loadAllTables();
   */
  void loadAllTables() {
    std::string basePath = get_base_path();
    if (!std::filesystem::exists(basePath)) {
      return; 
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

  /**
   * Creates a new table with the given name and columns.
   * Throws an exception if the table already exists.
   * 
   * @param tableName Name of the table to create.
   * @param columns Vector of column names for the new table.
   * @throws const char* if the table already exists.
   * 
   * @example
   * Storage storage("myDatabase");
   * storage.createTable("users", {"id", "name", "email"});
   */
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
  
  /**
   * Loads a table from disk into memory.
   * Throws an exception if the table file cannot be read or is malformed.
   * 
   * @param tableName Name of the table to load.
   * @throws const char* if file operations fail or data is malformed.
   * 
   * @example
   * Storage storage("myDatabase");
   * storage.loadTable("users");
   */
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
            row.emplace_back(std::stoi(valueStr));
            break;
          case Value::STRING:
            // Remove quotes
            if (valueStr.size() >= 2 && valueStr.front() == '"' && valueStr.back() == '"') {
              valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            row.emplace_back(valueStr);
            break;
          case Value::BOOL:
            row.emplace_back(valueStr == "true");
            break;
          default:
            break;
        }
      }
      table.insertRow(row);
    }

    tables[tableName] = table;
  }
  
  /**
   * Retrieves a reference to a table by name.
   * Throws an exception if the table does not exist.
   * 
   * @param tableName Name of the table to retrieve.
   * @return Reference to the requested Table.
   * @throws std::out_of_range if the table is not found.
   * 
   * @example
   * Storage storage("myDatabase");
   * Table& usersTable = storage.getTable("users");
   */
  Table& getTable(const std::string& tableName) {
    auto it = tables.find(tableName);
    if (it == tables.end()) {
      throw std::out_of_range("Table not found");
    }
    return it->second;
  }

  /**
   * getTable method but returns the const version of the table.
   * Used when the table cannot be modified.
   * 
   * @param tableName Name of the table to retrieve.
   * @return Const reference to the requested Table.
   * @throws std::out_of_range if the table is not found.
   * 
   * @example
   * Storage storage("myDatabase");
   * const Table& usersTable = storage.getTableConst("users");
   */
  const Table& getTableConst(const std::string& tableName) const {
    auto it = tables.find(tableName);
    if (it == tables.end()) {
      throw std::out_of_range("Table not found");
    }
    return it->second;
  }

  /**
   * Returns a vector of all tables in the storage.
   * 
   * @return Vector of all Table objects.
   * @example
   * Storage storage("myDatabase");
   * std::vector<Table> allTables = storage.getAllTables();
   */
  std::vector<Table> getAllTables() const {
    std::vector<Table> tableVec;

    for (const auto& pair : tables) {
      tableVec.push_back(pair.second);
    }

    return tableVec;
  }

};

#endif
#ifndef TABLE_H
#define TABLE_H

#include "value.h"
#include <vector>
#include <string>
#include <unordered_map>

class Table {
private:
  std::string tableName;
  std::vector<std::string> columnNames;
  std::vector<Value::Type> columnTypes;
  std::unordered_map<std::string, size_t> columnIndexMap;
  std::vector<std::vector<Value>> rows;

public:
  Table() = default;
  Table(const std::string tableName, const std::vector<std::string>& cols, const std::vector<Value::Type>& types) : tableName(tableName), columnNames(cols), columnTypes(types) {
    if(columnTypes.size() != columnNames.size()) {
      throw "Column names and types size mismatch";
    }

    for (size_t i = 0; i < columnNames.size(); ++i) {
      columnIndexMap[columnNames[i]] = i;
    }
  }

  /**
   * Returns the names of all columns in the table.
   * 
   * @param void
   * @return Vector of column names.
   * 
   * @example
   * Table table("users", {"id", "name", "age"}, {Value::INT, Value::STRING, Value::INT});
   * std::vector<std::string> cols = table.getColumnNames();
   */
  std::vector<std::string> getColumnNames() const {
    return columnNames;
  }

  /**
   * Inserts a new row into the table.
   * 
   * @param vals Vector of Value objects representing the row to insert.
   * @throws const char* if the number of values does not match the number of columns.
   * 
   * @example
   * Table table("users", {"id", "name", "age"}, {Value::INT, Value::STRING, Value::INT});
   * table.insertRow({Value(1), Value("Alice"), Value(30)});
   */
  void insertRow(const std::vector<Value>& vals) {
    if (vals.size() != columnNames.size()) {
      throw "Column count mismatch";
    }

    for (size_t i = 0; i < vals.size(); ++i) {
      bool colTypeMatch = Value::isValidType(vals[i], columnTypes[i]);
      if (!colTypeMatch) {
        throw "Type mismatch for column";
      }
    }

    rows.push_back(vals);
  }

  /**
   * Retrieves a row by its index.
   * 
   * @param index Index of the row to retrieve.
   * @return Const reference to the requested row.
   * @throws const char* if the index is out of bounds.
   * 
   * @example
   * Table table("users", {"id", "name", "age"}, {Value::INT, Value::STRING, Value::INT});
   * const std::vector<Value>& row = table.getRow(0);
   */
  const std::vector<Value>& getRow(size_t index) const {
    if (index >= rows.size()) {
      throw "Row index out of bounds";
    }
    return rows[index];
  }

  /**
   * Returns the name of the table.
   * 
   * @return Const reference to the table name.
   * @example
   * Table table("users", {"id", "name", "age"}, {Value::INT, Value::STRING, Value::INT});
   * std::string name = table.getTableName();
   */
  const std::string& getTableName() const {
    return tableName;
  }

  /**
   * Getter for the column index map.
   * Returns the mapping of column names to their indices.
   * 
   * @return Const reference to the column index map.
   * @example
   * Table table("users", {"id", "name", "age"}, {Value::INT, Value::STRING, Value::INT});
   * const std::unordered_map<std::string, size_t>& colMap = table.getColumnIndexMap();
   */
  const std::unordered_map<std::string, size_t>& getColumnIndexMap() const {
    return columnIndexMap;
  }

  /**
   * Gets the value at the specified row and column.
   * 
   * @param rowIndex Index of the row.
   * @param colName Name of the column.
   * 
   * @return Value at the specified row and column.
   * @throws const char* if the row index is out of bounds or the column is not found.
   * 
   * @example
   * Table table("users", {"id", "name", "age"}, {Value::INT, Value::STRING, Value::INT});
   * Value val = table.getValue(0, "name");
   */
  const Value getValue(size_t rowIndex, const std::string& colName) const {
    if (rowIndex >= rows.size()) {
      throw "Row index out of bounds";
    }
    auto it = columnIndexMap.find(colName);
    if (it == columnIndexMap.end()) {
      throw "Column not found";
    }

    return rows[rowIndex][it->second];
  }

  /**
   * Sets the value at the specified row and column.
   * 
   * @param rowIndex Index of the row.
   * @param colName Name of the column.
   * @param val Value to set.
   * @throws const char* if the row index is out of bounds or the column is not found.
   * 
   * @example
   * Table table("users", {"id", "name", "age"}, {Value::INT, Value::STRING, Value::INT});
   * table.setValue(0, "age", Value(31));
   */
  void setValue(size_t rowIndex, const std::string& colName, const Value& val) {
    if (rowIndex >= rows.size()) {
      throw "Row index out of bounds";
    }

    auto it = columnIndexMap.find(colName);

    if (it == columnIndexMap.end()) {
      throw "Column not found";
    }

    bool colTypeMatch = Value::isValidType(val, columnTypes[it->second]);
    if (!colTypeMatch) {
      throw "Type mismatch for column";
    }

    rows[rowIndex][it->second] = val;
  }

  /**
   * Returns the number of rows in the table.
   * 
   * @return Number of rows.
   * @example
   * Table table("users", {"id", "name", "age"}, {Value::INT, Value::STRING, Value::INT});
   * size_t rowCount = table.getRowCount();
   */
  size_t getRowCount() const {
    return rows.size();
  }

  /**
   * Clears all rows from the table.
   * 
   * @example
   * Table table("users", {"id", "name", "age"}, {Value::INT, Value::STRING, Value::INT});
   * table.clearRows();
   */
  void clearRows() {
    rows.clear();
  }

  /**
   * Adds a new column to the table with a default value for existing rows.
   * 
   * @param colName Name of the new column.
   * @param defaultValue Default value for existing rows.
   * @throws const char* if the column already exists.
   * 
   * @example
   * Table table("users", {"id", "name"}, {Value::INT, Value::STRING});
   * table.addColumn("age", Value(0), Value::INT);
   */
  void addColumn(const std::string& colName, const Value& defaultValue, Value::Type type) {
    if (columnIndexMap.find(colName) != columnIndexMap.end()) {
      throw "Column already exists";
    }

    if (!Value::isValidType(defaultValue, type)) {
      throw "Default value type does not match column type";
    }

    columnNames.push_back(colName);
    columnTypes.push_back(type);
    columnIndexMap[colName] = columnNames.size() - 1;
    for (auto& row : rows) {
      row.push_back(defaultValue);
    }
  }

    /**
   * Gets the type of a specific column.
   * 
   * @param colName Name of the column.
   * @return Type of the column.
   * @throws const char* if the column is not found.
   * 
   * @example
   * Value::Type type = table.getColumnType("age");
   */
  Value::Type getColumnType(const std::string& colName) const {
    auto it = columnIndexMap.find(colName);
    if (it == columnIndexMap.end()) {
      throw "Column not found";
    }
    return columnTypes[it->second];
  }

  /**
  * Gets all column types.
  * 
  * @return Vector of column types.
  */
  const std::vector<Value::Type>& getColumnTypes() const {
    return columnTypes;
  }

};


#endif

#ifndef SELECT_H
#define SELECT_H

#include "../storage.h"
#include "../table.h"
#include "../value.h"
#include <vector>
#include <stdexcept>
#include <unordered_map>

class SelectQuery {
private:
  Storage& storage;
public:
  SelectQuery(Storage& storage) : storage(storage) {}

  const Table& selectAll(const std::string& tableName) const {
    const Table& table = storage.getTableConst(tableName);
    return table;
  }

  /**
   * Selects specific columns from a table.
   * 
   * @param tableName Name of the table to select from.
   * @param columnNames Vector of column names to select.
   * @return A map where keys are column names and values are vectors of Values for that column.
   * @throws std::out_of_range if any specified column does not exist.
   * 
   * @example
   * SelectQuery selectQuery(storage);
   * auto result = selectQuery.selectColumns("users", {"id", "name"});
   */
  std::unordered_map<std::string, std::vector<Value>> selectColumns(const std::string& tableName, const std::vector<std::string>& columnNames) {
    Table& table = storage.getTable(tableName);
    std::unordered_map<std::string, std::vector<Value>> result;
    const std::unordered_map<std::string, size_t>& colIndexMap = table.getColumnIndexMap();

    std::vector<size_t> colIndices;
    for (const std::string& colName : columnNames) {
      auto it = colIndexMap.find(colName);
      if (it == colIndexMap.end()) {
        throw std::out_of_range("Column not found: " + colName);
      }
      colIndices.push_back(it->second);
      result[colName] = std::vector<Value>();
    }
    
    for (size_t i = 0; i < table.getRowCount(); ++i) {
      const std::vector<Value>& row = table.getRow(i);
      for (size_t j = 0; j < colIndices.size(); ++j) {
        result[columnNames[j]].push_back(row[colIndices[j]]);
      }
    }
    return result;
  }

  /**
   * Selects specific columns from a table where a condition is met.
   * 
   * @param tableName Name of the table to select from.
   * @param columnNames Vector of column names to select.
   * @param conditionValue Value to match in the condition column.
   * @param conditionColumn Name of the column to apply the condition on.
   * @return A map where keys are column names and values are vectors of Values for that column.
   * @throws std::out_of_range if any specified column does not exist.
   * 
   * @example
   * SelectQuery selectQuery(storage);
   * auto result = selectQuery.selectWhere("users", {"id", "name"}, Value(30), "age");
   */
  std::unordered_map<std::string, std::vector<Value>> selectWhere(std::string& tableName,  std::vector<std::string>& columnNames, const Value& conditionValue, const std::string& conditionColumn) {
    Table& table = storage.getTable(tableName);
    std::unordered_map<std::string, std::vector<Value>> result;

    if(columnNames.size() == 1 && columnNames[0] == "*") {
      columnNames = table.getColumnNames();
    }

    const std::unordered_map<std::string, size_t>& colIndexMap = table.getColumnIndexMap();
    size_t conditionColIndex;
    try {
      conditionColIndex = colIndexMap.at(conditionColumn);  // Throws if not found
    } catch (const std::out_of_range&) {
      throw std::out_of_range("Condition column not found: " + conditionColumn);
    }

    /**
     * Get indices for requested columns
     * 
     * @throws std::out_of_range if any specified column does not exist.
     * 
     * @example
     * SelectQuery selectQuery(storage);
     * auto result = selectQuery.selectWhere("users", {"id", "name"}, Value(30), "age");
     */
    std::vector<size_t> colIndices;
    for (const std::string& colName : columnNames) {
      auto it = colIndexMap.find(colName);
      if (it == colIndexMap.end()) {
        throw std::out_of_range("Column not found: " + colName);
      }
      colIndices.push_back(it->second);
      result[colName] = std::vector<Value>();
    }

    for (size_t i = 0; i < table.getRowCount(); ++i) {
      const std::vector<Value>& row = table.getRow(i);
      if (row[conditionColIndex] == conditionValue) {
        for (size_t j = 0; j < colIndices.size(); ++j) {
          result[columnNames[j]].push_back(row[colIndices[j]]);
        }
      }
    }
    return result;
  }
};

#endif

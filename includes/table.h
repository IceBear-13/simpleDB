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
  std::unordered_map<std::string, size_t> columnIndexMap;
  std::vector<std::vector<Value>> rows;

public:
  Table() = default;
  Table(const std::string tableName, const std::vector<std::string>& cols) : tableName(tableName), columnNames(cols) {
    for (size_t i = 0; i < columnNames.size(); ++i) {
      columnIndexMap[columnNames[i]] = i;
    }
  }

  std::vector<std::string> getColumnNames() const {
    return columnNames;
  }

  void insertRow(const std::vector<Value>& vals) {
    if (vals.size() != columnNames.size()) {
      throw "Column count mismatch";
    }
    rows.push_back(vals);
  }

  const std::vector<Value>& getRow(size_t index) const {
    if (index >= rows.size()) {
      throw "Row index out of bounds";
    }
    return rows[index];
  }

  const std::string& getTableName() const {
    return tableName;
  }

  const std::unordered_map<std::string, size_t>& getColumnIndexMap() const {
    return columnIndexMap;
  }

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

  void setValue(size_t rowIndex, const std::string& colName, const Value& val) {
    if (rowIndex >= rows.size()) {
      throw "Row index out of bounds";
    }
    auto it = columnIndexMap.find(colName);
    if (it == columnIndexMap.end()) {
      throw "Column not found";
    }
    rows[rowIndex][it->second] = val;
  }

  size_t getRowCount() const {
    return rows.size();
  }

  void clearRows() {
    rows.clear();
  }

  void addColumn(const std::string& colName, const Value& defaultValue) {
    if (columnIndexMap.find(colName) != columnIndexMap.end()) {
      throw "Column already exists";
    }
    columnNames.push_back(colName);
    columnIndexMap[colName] = columnNames.size() - 1;
    for (auto& row : rows) {
      row.push_back(defaultValue);
    }
  }

};


#endif

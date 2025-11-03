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

  std::vector<std::vector<Value>> where(std::vector<std::vector<Value>>& tableData, size_t columnIndex, const Value& value) {
    std::vector<std::vector<Value>> result;
    for (const auto& row : tableData) {
      if (row[columnIndex].getType() == value.getType()) {
        bool match = false;
        switch (value.getType()) {
          case Value::INT:
            match = (row[columnIndex].getInt() == value.getInt());
            break;
          case Value::STRING:
            match = (strcmp(row[columnIndex].getString(), value.getString()) == 0);
            break;
          case Value::BOOL:
            match = (row[columnIndex].getBool() == value.getBool());
            break;
          default:
            break;
        }
        if (match) {
          result.push_back(row);
        }
      }
    }
    return result;
  }

};

#endif

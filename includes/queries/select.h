#ifndef SELECT_H
#define SELECT_H

#include "../storage.h"
#include "../table.h"
#include "../value.h"

class SelectQuery {
private:
  Storage& storage;
public:
  SelectQuery(Storage& storage) : storage(storage) {}

  const Table& selectAll(const std::string& tableName) const {
    const Table& table = storage.getTableConst(tableName);
    return table;
  }

  std::vector<std::vector<Value>> selectColumn(const std::string& tableName, const std::string& columnName) {
    Table& table = storage.getTable(tableName);
    std::vector<std::vector<Value>> result;
    const auto& colIndexMap = table.getColumnIndexMap();
    auto it = colIndexMap.find(columnName);
    if (it == colIndexMap.end()) {
      throw std::out_of_range("Column not found");
    }
    size_t colIndex = it->second;

    for (size_t i = 0; i < table.getRowCount(); ++i) {
      const std::vector<Value>& row = table.getRow(i);
      result.push_back({row[colIndex]});
    }
    return result;
  }
};

#endif

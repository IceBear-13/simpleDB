#ifndef COLUMN_H
#define COLUMN_H

#include "value.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <unordered_map>

class Column {
private:
  std::string columnName;
  Value::Type columnType;
  std::vector<Value> values;
  bool isPrimaryKey;
public:
  Column() = default;
  Column(const std::string& name, Value::Type type, bool primaryKey = false) : columnName(name), columnType(type), isPrimaryKey(primaryKey) {}
  
  const std::string& getName() const { return columnName; }
  Value::Type getType() const { return columnType; }
  bool getIsPrimaryKey() const { return isPrimaryKey; }
  
  void insertValue(const Value& val) { // Bound to be changed when btree is implemented
    if (!Value::isValidType(val, columnType)) {
      throw std::invalid_argument("Type mismatch for column");
    }
    values.push_back(val);
  }

  void setValue (const Value& val, size_t rowIndex) {
    if (!Value::isValidType(val, columnType)) {
      throw std::invalid_argument("Type mismatch for column");
    }
    if (rowIndex >= values.size()) {
      throw std::out_of_range("Row index out of range");
    }
    values[rowIndex] = val;
  }

  void deleteValue(size_t rowIndex) {
    if (rowIndex >= values.size()) {
      throw std::out_of_range("Row index out of range");
    }
    values.erase(values.begin() + rowIndex);
  }

  void findValue(const Value& val) const {
    for (size_t i = 0; i < values.size(); ++i) {
      if (values[i] == val) {
        std::cout << "Value found at index: " << i << std::endl;
        return;
      }
    }
    std::cout << "Value not found in column." << std::endl;
  }
  
  const std::vector<Value>& getValues() const { return values; }
};

#endif




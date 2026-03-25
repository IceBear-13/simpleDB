#ifndef ROW_H
#define ROW_H

#include <vector>
#include "value.h"

class Row {
private:
  std::vector<Value> values;
public:
  Row() = default;
  Row(const std::vector<Value>& vals) : values(vals) {}

  const std::vector<Value>& getValues() const { return values; }
  const Row& getRow() const { return *this; }


};

#endif

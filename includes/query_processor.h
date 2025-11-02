#ifndef QUERY_PROCESSOR_H
#define QUERY_PROCESSOR_H

#include "storage.h"
#include "table.h"
#include "value.h"
#include <iostream>
#include <string>
#include <vector>

class QueryProcessor {
private:
  Storage& storage;

public:
  QueryProcessor(Storage& store) : storage(store) {}
  
  
};

#endif

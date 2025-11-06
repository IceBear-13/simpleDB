#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <stdexcept>

class Value {
public:
  enum Type { INT, STRING, BOOL, NULL_TYPE };

private:
  Type type;
  union {
    int intValue;
    bool boolValue;
  };
  std::string stringValue; // Keep string outside the union

public:
  Value() : type(NULL_TYPE) {}
  
  Value(int v) : type(INT), intValue(v) {}
  
  Value(const std::string& v) : type(STRING), stringValue(v) {}
  
  Value(const char* v) : type(STRING), stringValue(v) {}
  
  Value(bool v) : type(BOOL), boolValue(v) {}

  Value(const Value& other) : type(other.type) {
    switch (type) {
      case INT:
        intValue = other.intValue;
        break;
      case STRING:
        stringValue = other.stringValue;
        break;
      case BOOL:
        boolValue = other.boolValue;
        break;
      case NULL_TYPE:
        break;
    }
  }

  // Assignment operator
  Value& operator=(const Value& other) {
    if (this != &other) {
      type = other.type;
      switch (type) {
        case INT:
          intValue = other.intValue;
          break;
        case STRING:
          stringValue = other.stringValue;
          break;
        case BOOL:
          boolValue = other.boolValue;
          break;
        case NULL_TYPE:
          break;
      }
    }
    return *this;
  }
  
  ~Value() = default;

  Type getType() const { return type; }

  int getInt() const {
    if (type != INT) throw std::runtime_error("Type mismatch: not an int");
    return intValue;
  }

  const std::string& getString() const {
    if (type != STRING) throw std::runtime_error("Type mismatch: not a string");
    return stringValue;
  }

  bool getBool() const {
    if (type != BOOL) throw std::runtime_error("Type mismatch: not a bool");
    return boolValue;
  }

  bool operator==(const Value& other) const {
    if (type != other.type) return false;
    switch (type) {
      case INT:
        return intValue == other.intValue;
      case STRING:
        return stringValue == other.stringValue;
      case BOOL:
        return boolValue == other.boolValue;
      case NULL_TYPE:
        return true; // Both are NULL_TYPE
    }
    return false; // Should never reach here
  }
};

#endif
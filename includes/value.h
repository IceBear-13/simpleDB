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

  /**
   * Assignment operator.
   * 
   * @param other The Value to assign from.
   * @return Reference to this Value.
   * 
   * @example
   * Value val1(42);
   * Value val2;
   * val2 = val1; // val2 now holds the same value as val1
   */
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

  /**
   * Gets the integer value.
   * 
   * @return The integer value.
   * @throws std::runtime_error if the Value is not of type INT.
   * 
   * @example
   * Value val(42);
   * int intValue = val.getInt(); // intValue will be 42
   * Value strVal("hello");
   * int invalidInt = strVal.getInt(); // Throws std::runtime_error
   */
  int getInt() const {
    if (type != INT) throw std::runtime_error("Type mismatch: not an int");
    return intValue;
  }

  /**
   * Gets the string value.
   * 
   * @return The string value.
   * @throws std::runtime_error if the Value is not of type STRING.
   * 
   * @example
   * Value val("hello");
   * std::string strValue = val.getString(); // strValue will be "hello"
   * Value intVal(42);
   * std::string invalidStr = intVal.getString(); // Throws std::runtime_error
   */
  const std::string& getString() const {
    if (type != STRING) throw std::runtime_error("Type mismatch: not a string");
    return stringValue;
  }

  /**
   * Gets the boolean value.
   * 
   * @return The boolean value.
   * @throws std::runtime_error if the Value is not of type BOOL.
   * 
   * @example
   * Value val(true);
   * bool boolValue = val.getBool(); // boolValue will be true
   * Value intVal(42);
   * bool invalidBool = intVal.getBool(); // Throws std::runtime_error
   */
  bool getBool() const {
    if (type != BOOL) throw std::runtime_error("Type mismatch: not a bool");
    return boolValue;
  }

  /**
   * Equality operator.
   * 
   * @param other The Value to compare with.
   * @return true if both Values are equal, false otherwise.
   * 
   * @example
   * Value val1(42);
   * Value val2(42);
   * bool isEqual = (val1 == val2); // isEqual will be true
   * Value val3("hello");
   * bool isNotEqual = (val1 == val3); // isNotEqual will be false
   */
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
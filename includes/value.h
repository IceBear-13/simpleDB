#ifndef VALUE_H
#define VALUE_H


class Value {
public:
  enum Type { INT, STRING, BOOL, NULL_TYPE };

  Type type;
  union {
    int intValue;
    const char* stringValue;
    bool boolValue;
  };

  Value() : type(NULL_TYPE) {}
  Value(int v) : type(INT), intValue(v) {}
  Value(const char* v) : type(STRING), stringValue(v) {}
  Value(bool v) : type(BOOL), boolValue(v) {}

  Type getType() const { return type; }

  int getInt() const {
    if (type == INT) return intValue;
    throw "Type mismatch: not an int";
  }

  const char* getString() const {
    if (type == STRING) return stringValue;
    throw "Type mismatch: not a string";
  }

  bool getBool() const {
    if (type == BOOL) return boolValue;
    throw "Type mismatch: not a bool";
  }
};

#endif

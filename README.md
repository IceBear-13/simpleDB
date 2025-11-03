# simpleDB

Minimal in-memory DBMS written in C++ (learning / prototype project).

## Overview

SimpleDBMS stores tables of rows composed of generic `Value`s. It provides a tiny SQL-like REPL with basic `CREATE`, `INSERT`, and `SELECT` support.

Key components:
- [`Value`](includes/value.h) — value container and [`Value::Type`](includes/value.h).  
- [`Table`](includes/table.h) — in-memory table, columns and rows, see methods like [`Table::insertRow`](includes/table.h), [`Table::getRow`](includes/table.h), [`Table::getRowCount`](includes/table.h), [`Table::getColumnNames`](includes/table.h) and [`Table::getColumnIndexMap`](includes/table.h).  
- [`Storage`](includes/storage.h) — holds all tables and file persistence: [`Storage::createTable`](includes/storage.h), [`Storage::persistTable`](includes/storage.h), [`Storage::loadTable`](includes/storage.h).  
- [`QueryProcessor`](includes/query_processor.h) — parses simple commands and dispatches to query classes: [`QueryProcessor::execute`](includes/query_processor.h).  
- Query classes: [`CreateQuery`](includes/queries/create.h), [`InsertQuery`](includes/queries/insert.h), [`SelectQuery`](includes/queries/select.h).  
- CLI entry point: [src/main.cpp](src/main.cpp).  
- Build config: [CMakeLists.txt](CMakeLists.txt).

## Repository layout

- includes/
  - [value.h](includes/value.h)
  - [table.h](includes/table.h)
  - [storage.h](includes/storage.h)
  - query classes: [queries/create.h](includes/queries/create.h), [queries/insert.h](includes/queries/insert.h), [queries/select.h](includes/queries/select.h)
  - [query_processor.h](includes/query_processor.h)
- src/
  - [main.cpp](src/main.cpp)
- [CMakeLists.txt](CMakeLists.txt)

## Build

Standard CMake workflow:

```sh
mkdir -p build
cd build
cmake ..
make
```
Note: adjust the project CMake configuration if file lists or include paths need correction.

## Run
From the build directory (or run the produced executable):

Follow the REPL prompts.

## Usage (REPL)
Supported commands (simple parser implemented in QueryProcessor::execute):

- Create table:
  CREATE TABLE table_name col1 col2 col3
- Insert row:
  INSERT INTO table_name VALUES (1, hello, true)
- Select:
  SELECT * FROM table_name
  SELECT col1, col3 FROM table_name

Example:
```
simpledb> CREATE TABLE users id name active
simpledb> INSERT INTO users VALUES (1, Alice, true)
simpledb> SELECT * FROM users
```
Columns do not enforce a fixed type: any cell may hold any Value::Type. See:

## Value
Table schema: Table (only stores column names)
Creation and insertion flow: Storage::createTable and InsertQuery::insertInto
If you want per-column typing, add a std::vector<Value::Type> to Table and validate in Table::insertRow.

## Persistence
Basic binary persistence is implemented in Storage::persistTable and loading in Storage::loadTable. Review and test file formats before relying on them.

## TODO / Ideas
- Enforce per-column types in Table.
- Improve parser in QueryProcessor (quoted strings, escaping, WHERE clause).
- Add safer memory handling for strings in Value (use std::string).
- Fix/complete CMakeLists.txt to reference correct source/header files.
- Add unit tests.

## References (source files)
- includes/value.h — Value, Value::Type
- includes/table.h — Table, Table::insertRow, Table::getRow
- includes/storage.h — Storage, Storage::createTable
- includes/query_processor.h — QueryProcessor, QueryProcessor::execute
- includes/queries/create.h — CreateQuery
- includes/queries/insert.h — InsertQuery
- includes/queries/select.h — SelectQuery
- src/main.cpp
- CMakeLists.txt

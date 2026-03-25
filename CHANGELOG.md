# Changelog

## [1.1.0] - 2026-03-25
### Added
- B-tree indexing for efficient WHERE queries
- SELECT with column projection and WHERE clause support
- INSERT with automatic index maintenance
- CREATE TABLE with type enforcement
- Query processor with refactored handler pattern
- File persistence for tables

### Changed
- Refactored SelectProcessor to query_handler/select.h
- Improved code organization with dedicated processors

### Known Limitations
- Linux/WSL only
- In-memory storage (tables loaded at startup)
- No JOIN support
- Simple parsing (no complex expressions)
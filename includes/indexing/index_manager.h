#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include "btree.h"
#include "value.h"
#include <unordered_map>
#include <string>

class IndexManager {
private:
    std::unordered_map<std::string, BTree<int>> intIndexes;
    std::unordered_map<std::string, BTree<std::string>> stringIndexes;
    std::unordered_map<std::string, BTree<bool>> boolIndexes;
    
public:
    void createIndex(const std::string& indexName, Value::Type getType);
    void insertIntoIndex(const std::string& indexName, const Value& key, size_t rowIndex);
    std::vector<size_t> searchIndex(const std::string& indexName, const Value& key);
    bool hasIndex(const std::string& indexName) const;
};

void IndexManager::createIndex(const std::string& indexName, Value::Type getType) {
    // Prevent overwriting an existing index of any getType
    if (hasIndex(indexName)) {
        throw std::runtime_error("Index already exists: " + indexName);
    }

    switch (getType) {
        case Value::Type::INT:
            intIndexes.emplace(indexName, BTree<int>(/*degree=*/3));
            break;
        case Value::Type::STRING:
            stringIndexes.emplace(indexName, BTree<std::string>(/*degree=*/3));
            break;
        case Value::Type::BOOL:
            boolIndexes.emplace(indexName, BTree<bool>(/*degree=*/3));
            break;
        default:
            throw std::runtime_error("Unsupported index getType for: " + indexName);
    }
}

void IndexManager::insertIntoIndex(const std::string& indexName, const Value& key, size_t rowIndex) {
    // Try INT
    if (auto it = intIndexes.find(indexName); it != intIndexes.end()) {
        if (key.getType() != Value::Type::INT) {
            throw std::runtime_error("Type mismatch: expected INT for index " + indexName);
        }
        it->second.insert(key.getInt(), rowIndex);
        return;
    }

    // Try STRING
    if (auto it = stringIndexes.find(indexName); it != stringIndexes.end()) {
        if (key.getType() != Value::Type::STRING) {
            throw std::runtime_error("Type mismatch: expected STRING for index " + indexName);
        }
        it->second.insert(key.getString(), rowIndex);
        return;
    }

    // Try BOOL
    if (auto it = boolIndexes.find(indexName); it != boolIndexes.end()) {
        if (key.getType() != Value::Type::BOOL) {
            throw std::runtime_error("Type mismatch: expected BOOL for index " + indexName);
        }
        it->second.insert(key.getBool(), rowIndex);
        return;
    }

    throw std::runtime_error("Index not found: " + indexName);
}

std::vector<size_t> IndexManager::searchIndex(const std::string& indexName, const Value& key) {
    if (auto it = intIndexes.find(indexName); it != intIndexes.end()) {
        if (key.getType() != Value::Type::INT) {
            throw std::runtime_error("Type mismatch: expected INT for index " + indexName);
        }
        return it->second.search(key.getInt());
    }
    if (auto it = stringIndexes.find(indexName); it != stringIndexes.end()) {
        if (key.getType() != Value::Type::STRING) {
            throw std::runtime_error("Type mismatch: expected STRING for index " + indexName);
        }
        return it->second.search(key.getString());
    }
    if (auto it = boolIndexes.find(indexName); it != boolIndexes.end()) {
        if (key.getType() != Value::Type::BOOL) {
            throw std::runtime_error("Type mismatch: expected BOOL for index " + indexName);
        }
        return it->second.search(key.getBool());
    }
    throw std::runtime_error("Index not found: " + indexName);
}

bool IndexManager::hasIndex(const std::string& indexName) const {
    return intIndexes.count(indexName) ||
           stringIndexes.count(indexName) ||
           boolIndexes.count(indexName);
}

#endif
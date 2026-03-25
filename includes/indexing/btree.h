#ifndef BTREE_H
#define BTREE_H

#include "value.h"
#include <vector>
#include <memory>
#include <iostream>

// B-Tree node storing keys and associated row indices.
// For duplicate keys, rowIndices[i] may contain multiple indices.
template<typename KeyType>
class BTreeNode {
public:
    std::vector<KeyType> keys;
    std::vector<std::vector<size_t>> rowIndices;  // Each key maps to a list of row positions
    std::vector<std::shared_ptr<BTreeNode<KeyType>>> children;
    bool isLeaf;
    int minDegree;  // Minimum degree (t)
    
    BTreeNode(int degree, bool leaf);

    // Insert into a non-full node.
    void insertNonFull(const KeyType& key, size_t rowIndex);

    // Split a full child.
    void splitChild(int i, std::shared_ptr<BTreeNode<KeyType>> child);

    // Return all row indices for the given key.
    std::vector<size_t> searchAll(const KeyType& key) const;

    void traverse() const;
};

template<typename KeyType>
class BTree {
private:
    std::shared_ptr<BTreeNode<KeyType>> root;
    int minDegree;
    
public:
    BTree(int degree) : minDegree(degree), root(nullptr) {}
    
    void insert(const KeyType& key, size_t rowIndex);
    std::vector<size_t> search(const KeyType& key) const;
    void traverse() const;
    bool isEmpty() const { return root == nullptr; }
};

// Implementation details
template<typename KeyType>
BTreeNode<KeyType>::BTreeNode(int degree, bool leaf) 
    : minDegree(degree), isLeaf(leaf) {}

template<typename KeyType>
void BTree<KeyType>::insert(const KeyType& key, size_t rowIndex) {
    if (root == nullptr) {
        root = std::make_shared<BTreeNode<KeyType>>(minDegree, true);
        root->keys.push_back(key);
        root->rowIndices.push_back({rowIndex});
    } else {
        if (root->keys.size() == static_cast<size_t>(2 * minDegree - 1)) {
            auto newRoot = std::make_shared<BTreeNode<KeyType>>(minDegree, false);
            newRoot->children.push_back(root);
            newRoot->splitChild(0, root);
            
            int i = 0;
            if (newRoot->keys[0] < key) i++;
            newRoot->children[i]->insertNonFull(key, rowIndex);
            
            root = newRoot;
        } else {
            root->insertNonFull(key, rowIndex);
        }
    }
}

template<typename KeyType>
void BTreeNode<KeyType>::insertNonFull(const KeyType& key, size_t rowIndex) {
    int i = static_cast<int>(keys.size()) - 1;
    
    if (isLeaf) {
        // Find position and handle duplicates
        while (i >= 0 && keys[i] > key) i--;

        if (i >= 0 && keys[i] == key) {
            // Duplicate key: append rowIndex
            rowIndices[i].push_back(rowIndex);
            return;
        }

        // Insert new key
        keys.push_back(KeyType());             // extend size
        rowIndices.push_back(std::vector<size_t>()); // extend size

        int j = static_cast<int>(keys.size()) - 2;
        while (j > i) {
            keys[j + 1] = keys[j];
            rowIndices[j + 1] = std::move(rowIndices[j]);
            j--;
        }

        keys[i + 1] = key;
        rowIndices[i + 1] = {rowIndex};
    } else {
        while (i >= 0 && keys[i] > key) i--;

        // Keep duplicates attached to the node that already stores the key.
        if (i >= 0 && keys[i] == key) {
            rowIndices[i].push_back(rowIndex);
            return;
        }

        int childIdx = i + 1;

        // If child is full, split it first
        if (children[childIdx]->keys.size() == static_cast<size_t>(2 * minDegree - 1)) {
            splitChild(childIdx, children[childIdx]);
            if (keys[childIdx] < key) childIdx++;
        }
        children[childIdx]->insertNonFull(key, rowIndex);
    }
}

template<typename KeyType>
std::vector<size_t> BTree<KeyType>::search(const KeyType& key) const {
    if (root == nullptr) return {};
    return root->searchAll(key);
}

template<typename KeyType>
std::vector<size_t> BTreeNode<KeyType>::searchAll(const KeyType& key) const {
    int i = 0;
    while (i < static_cast<int>(keys.size()) && key > keys[i]) i++;
    
    if (i < static_cast<int>(keys.size()) && keys[i] == key) {
        // Return all row indices associated with this key
        return rowIndices[i];
    }
    
    if (isLeaf) return {};
    
    return children[i]->searchAll(key);
}

template<typename KeyType>
void BTreeNode<KeyType>::splitChild(int i, std::shared_ptr<BTreeNode<KeyType>> child) {
    // Create new node that will store child->keys[minDegree .. 2*minDegree-2]
    auto newNode = std::make_shared<BTreeNode<KeyType>>(child->minDegree, child->isLeaf);
    
    // Move last (minDegree - 1) keys to newNode
    for (int j = 0; j < minDegree - 1; j++) {
        newNode->keys.push_back(child->keys[j + minDegree]);
        newNode->rowIndices.push_back(std::move(child->rowIndices[j + minDegree]));
    }
    
    if (!child->isLeaf) {
        for (int j = 0; j < minDegree; j++) {
            newNode->children.push_back(child->children[j + minDegree]);
        }
    }
    
    // Insert child's median key into current node
    keys.insert(keys.begin() + i, child->keys[minDegree - 1]);
    rowIndices.insert(rowIndices.begin() + i, std::move(child->rowIndices[minDegree - 1]));
    children.insert(children.begin() + i + 1, newNode);
    
    // Resize child
    child->keys.resize(minDegree - 1);
    child->rowIndices.resize(minDegree - 1);
    if (!child->isLeaf) {
        child->children.resize(minDegree);
    }
}

template<typename KeyType>
void BTree<KeyType>::traverse() const {
    if (root != nullptr) {
        root->traverse();
    }
}

template<typename KeyType>
void BTreeNode<KeyType>::traverse() const {
    for (size_t i = 0; i < keys.size(); i++) {
        if (!isLeaf) {
            children[i]->traverse();
        }
        std::cout << keys[i] << " ";
    }
    if (!isLeaf && !children.empty()) {
        children[keys.size()]->traverse();
    }
}

#endif

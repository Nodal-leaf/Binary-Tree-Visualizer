#ifndef TREE_SERVICE_H
#define TREE_SERVICE_H

#include "../models/BinarySearchTree.h"
#include "../models/BST.h"
#include "../models/AVLTree.h"
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

/**
 * @file TreeService.h
 * @brief Application service layer coordinating tree operations and state management.
 * 
 * Demonstrates:
 * - Separation of Concerns: Sits between HTTP controllers and domain tree models.
 * - Polymorphism: Manipulates trees exclusively through the BinarySearchTree base pointer.
 * - Thread Safety: Protects concurrent HTTP request access via std::mutex.
 */
class TreeService {
private:
    std::unique_ptr<BinarySearchTree> currentTree_;
    mutable std::mutex mutex_;

    void rebuildAsType(const std::string& targetType);

public:
    explicit TreeService(const std::string& initialType = "AVL");
    ~TreeService() = default;

    // Disallow copy/move of the service to preserve singleton-like state
    TreeService(const TreeService&) = delete;
    TreeService& operator=(const TreeService&) = delete;

    /**
     * @brief Switches the active tree variant between "BST" and "AVL".
     * Optionally preserves and re-inserts existing nodes into the new tree structure.
     * @param treeType "BST" or "AVL".
     * @param preserveData If true, re-inserts current values in level/inorder sequence.
     * @return Operation result JSON.
     */
    nlohmann::json switchTreeType(const std::string& treeType, bool preserveData = true);

    /**
     * @brief Inserts a key into the active tree.
     * @param value Integer key to insert.
     * @return Result JSON containing success status, path, rotations, and full tree state.
     */
    nlohmann::json insertValue(int value);

    /**
     * @brief Deletes a key from the active tree.
     * @param value Integer key to remove.
     * @return Result JSON containing success status, rotations, and full tree state.
     */
    nlohmann::json removeValue(int value);

    /**
     * @brief Searches for a key and returns the animation step path.
     * @param value Target key.
     * @return Result JSON with "found": bool and "path": array of visited node values.
     */
    nlohmann::json searchValue(int value) const;

    /**
     * @brief Clears the active tree.
     * @return Result JSON confirming reset.
     */
    nlohmann::json clearTree();

    /**
     * @brief Inserts multiple keys in sequence.
     * @param values Vector of integer keys.
     * @return Result JSON with summary and final tree state.
     */
    nlohmann::json batchInsert(const std::vector<int>& values);

    /**
     * @brief Retrieves the complete tree snapshot.
     * @return JSON representing tree structure, metadata, and traversals.
     */
    nlohmann::json getTreeState() const;

    /**
     * @brief Returns a specific traversal sequence ("inorder", "preorder", "postorder").
     */
    nlohmann::json getTraversal(const std::string& type) const;

    /**
     * @brief Populates the tree with sample values for quick interactive demonstration.
     */
    nlohmann::json loadSample();
};

#endif // TREE_SERVICE_H


#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include "Node.h"
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

/**
 * @file BinarySearchTree.h
 * @brief Abstract Base Class for Binary Search Tree data structures.
 * 
 * Demonstrates:
 * - Abstraction: Declares a clean contract for tree operations.
 * - Polymorphism: Pure virtual methods for insertion, deletion, and tree identification,
 *   allowing derived classes (BST, AVLTree) to be handled through base pointers.
 * - Encapsulation: Protects internal node pointers and state tracking.
 */
class BinarySearchTree {
protected:
    Node* root_;          ///< Pointer to root node
    int nodeCount_;       ///< Current number of nodes
    int nextNodeId_;      ///< Counter for issuing unique node IDs

public:
    /**
     * @brief Structure capturing details of a deletion event for GUI simulation.
     */
    struct DeletionEvent {
        bool occurred = false;
        int targetValue = 0;
        bool hasTwoChildren = false;
        int successorValue = 0;    ///< Inorder successor key swapped into target
        std::string description;

        nlohmann::json toJson() const {
            return {
                {"occurred", occurred},
                {"targetValue", targetValue},
                {"hasTwoChildren", hasTwoChildren},
                {"successorValue", successorValue},
                {"description", description}
            };
        }
    };

protected:
    DeletionEvent lastDeletion_;  ///< Details of last deletion operation

    // Recursive helper utilities
    void destroySubtree(Node* node);
    Node* cloneSubtree(const Node* node);
    int getNodeHeight(const Node* node) const;
    int calculateBalance(const Node* node) const;

    void inorderRecursive(const Node* node, std::vector<int>& result) const;
    void preorderRecursive(const Node* node, std::vector<int>& result) const;
    void postorderRecursive(const Node* node, std::vector<int>& result) const;

public:
    /**
     * @brief Default constructor initializing an empty tree.
     */
    BinarySearchTree();

    /**
     * @brief Virtual destructor ensuring safe polymorphic deletion.
     */
    virtual ~BinarySearchTree();

    // Copy and Move operations (Rule of 5)
    BinarySearchTree(const BinarySearchTree& other);
    BinarySearchTree& operator=(const BinarySearchTree& other);
    BinarySearchTree(BinarySearchTree&& other) noexcept;
    BinarySearchTree& operator=(BinarySearchTree&& other) noexcept;

    // --- Pure Virtual Functions (Polymorphism & Abstraction) ---
    /**
     * @brief Inserts a value into the tree.
     * @param value The integer key to insert.
     * @return true if inserted, false if value already exists.
     */
    virtual bool insert(int value) = 0;

    /**
     * @brief Deletes a value from the tree.
     * @param value The integer key to remove.
     * @return true if deleted, false if not found.
     */
    virtual bool remove(int value) = 0;

    /**
     * @brief Identifies the tree variant name.
     * @return String representation ("BST" or "AVL").
     */
    virtual std::string getTreeType() const = 0;

    // --- Concrete / Overridable Virtual Methods ---
    /**
     * @brief Searches for a key in the tree.
     * @param value Target key.
     * @return true if found, false otherwise.
     */
    virtual bool search(int value) const;

    /**
     * @brief Records the sequence of node values visited during search.
     * Useful for step-by-step GUI search animation.
     * @param value Target key.
     * @return Ordered list of visited node values.
     */
    virtual std::vector<int> getSearchPath(int value) const;

    /**
     * @brief Inorder traversal (Left, Root, Right) yielding sorted values.
     */
    virtual std::vector<int> inorder() const;

    /**
     * @brief Preorder traversal (Root, Left, Right).
     */
    virtual std::vector<int> preorder() const;

    /**
     * @brief Postorder traversal (Left, Right, Root).
     */
    virtual std::vector<int> postorder() const;

    /**
     * @brief Returns the height of the entire tree (0 if empty).
     */
    virtual int getHeight() const;

    /**
     * @brief Returns the total number of nodes currently in the tree.
     */
    virtual int getNodeCount() const;

    /**
     * @brief Checks if the tree has no nodes.
     */
    virtual bool isEmpty() const;

    /**
     * @brief Clears all nodes from the tree and resets state.
     */
    virtual void clear();

    /**
     * @brief Access telemetry about the last deletion operation.
     */
    const DeletionEvent& getLastDeletion() const { return lastDeletion_; }

    /**
     * @brief Resets last deletion telemetry.
     */
    void clearLastDeletion() { lastDeletion_ = DeletionEvent{}; }

    /**
     * @brief Read-only accessor to the root node.
     */
    const Node* getRoot() const;

    /**
     * @brief Serializes the complete tree state into a JSON object.
     * Contains tree type, node count, overall height, traversal arrays, and node hierarchy.
     */
    virtual nlohmann::json toJson() const;
};

#endif // BINARY_SEARCH_TREE_H


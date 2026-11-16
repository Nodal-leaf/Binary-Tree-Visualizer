#ifndef SPLAY_TREE_H
#define SPLAY_TREE_H

#include "BinarySearchTree.h"
#include <vector>
#include <string>

/**
 * @file SplayTree.h
 * @brief Concrete Derived Class representing a Splay Tree.
 *
 * A splay tree is a self-adjusting BST where every access (insert, search,
 * delete) splays the accessed node to the root using a sequence of rotations.
 * This amortises the cost of frequently accessed nodes.
 *
 * Demonstrates:
 * - Inheritance: Extends BinarySearchTree with self-adjusting splay behaviour.
 * - Polymorphism: Overrides insert, remove, search, getTreeType, and toJson.
 * - Encapsulation: Rotation helpers and splay logic are private.
 */
class SplayTree : public BinarySearchTree {
public:
    /**
     * @brief Captures one rotation step performed during a splay operation.
     */
    struct SplayStep {
        std::string type;         ///< "Zig", "Zag", "Zig-Zig", "Zag-Zag", "Zig-Zag", "Zag-Zig"
        int accessedValue;        ///< The key being splayed toward the root
        int newRootValue;         ///< Root value after this step
        std::string description;  ///< Human-readable explanation

        nlohmann::json toJson() const {
            return {
                {"type",          type},
                {"accessedValue", accessedValue},
                {"newRootValue",  newRootValue},
                {"description",   description}
            };
        }
    };

private:
    mutable std::vector<SplayStep> lastSplaySteps_; ///< Steps recorded during the last operation

    // Rotation primitives (same pointer mechanics as AVL rotations)
    Node* rotateRight(Node* y);
    Node* rotateLeft(Node* x);

    /**
     * @brief Core top-down splay: brings the node with @p key (or the last
     *        visited node if key is absent) to the root of the subtree rooted
     *        at @p root.
     * @param root  Sub-tree root to splay within.
     * @param key   Value to splay toward the root.
     * @return New root of the sub-tree after splaying.
     */
    Node* splay(Node* root, int key);

    // Helper: find the rightmost node (maximum) in a subtree
    Node* findMax(Node* node) const;

    // Update height of a node from its children
    void updateHeight(Node* node);

public:
    SplayTree();
    ~SplayTree() override = default;

    /**
     * @brief Inserts @p value into the splay tree and splays the new node to root.
     * @return true if inserted; false if duplicate.
     */
    bool insert(int value) override;

    /**
     * @brief Removes @p value from the splay tree.
     *        Splays the target to root, removes it, then merges the two
     *        subtrees by splaying the maximum of the left subtree.
     * @return true if removed; false if not found.
     */
    bool remove(int value) override;

    /**
     * @brief Searches for @p value, splaying the accessed (or last-visited)
     *        node to root even on a miss.
     * @return true if found; false otherwise.
     */
    bool search(int value) const override;

    /**
     * @brief Returns the tree type identifier.
     */
    std::string getTreeType() const override;

    /**
     * @brief Returns the splay steps recorded during the last mutating operation.
     */
    const std::vector<SplayStep>& getLastSplaySteps() const;

    /**
     * @brief Clears accumulated splay step logs.
     */
    void clearLastSplaySteps();

    /**
     * @brief Serializes the splay tree including splay steps into JSON.
     */
    nlohmann::json toJson() const override;
};

#endif // SPLAY_TREE_H

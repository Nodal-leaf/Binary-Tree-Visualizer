#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "BinarySearchTree.h"
#include <vector>
#include <string>

/**
 * @file AVLTree.h
 * @brief Concrete Derived Class representing a self-balancing AVL Tree.
 * 
 * Demonstrates:
 * - Inheritance: Extends BinarySearchTree with automatic rebalancing.
 * - Polymorphism: Overrides insert and remove to enforce the AVL height-balance invariant.
 * - Encapsulation: Internal balancing, rotation algorithms, and event logging remain protected.
 */
class AVLTree : public BinarySearchTree {
public:
    /**
     * @brief Structure capturing details of a rotation event for GUI simulation.
     */
    struct RotationStep {
        std::string type;          ///< "LL", "RR", "LR", or "RL"
        int rootValue;             ///< Key of the node that became unbalanced
        int pivotValue;            ///< Key of the child or grandchild acting as rotation pivot
        std::string description;   ///< Human-readable explanation of why and how rotation occurred

        nlohmann::json toJson() const {
            return {
                {"type", type},
                {"rootValue", rootValue},
                {"pivotValue", pivotValue},
                {"description", description}
            };
        }
    };

private:
    std::vector<RotationStep> lastRotations_; ///< Logs rotations performed during the last operation

    // Rotations
    Node* rotateRight(Node* y);           // LL Rotation
    Node* rotateLeft(Node* x);            // RR Rotation
    Node* rotateLeftRight(Node* node);     // LR Rotation
    Node* rotateRightLeft(Node* node);     // RL Rotation

    // Balancing & Utilities
    Node* balance(Node* node);
    Node* insertHelper(Node* current, int value, bool& inserted);
    Node* removeHelper(Node* current, int value, bool& removed);
    Node* findMin(Node* current) const;
    void updateHeight(Node* node);

public:
    AVLTree();
    ~AVLTree() override = default;

    /**
     * @brief Inserts a key and automatically performs rotations to maintain AVL balance.
     * @param value Integer key to insert.
     * @return true if inserted; false if duplicate.
     */
    bool insert(int value) override;

    /**
     * @brief Deletes a key and automatically performs rotations to maintain AVL balance.
     * @param value Integer key to remove.
     * @return true if removed; false if not found.
     */
    bool remove(int value) override;

    /**
     * @brief Returns the tree type identifier.
     */
    std::string getTreeType() const override;

    /**
     * @brief Retrieves logs of rotations that took place during the last operation.
     */
    const std::vector<RotationStep>& getLastRotations() const;

    /**
     * @brief Clears accumulated rotation logs.
     */
    void clearLastRotations();

    /**
     * @brief Serializes the AVL Tree including recent rotation events into JSON.
     */
    nlohmann::json toJson() const override;
};

#endif // AVL_TREE_H


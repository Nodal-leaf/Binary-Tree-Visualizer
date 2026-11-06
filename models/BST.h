#ifndef BST_H
#define BST_H

#include "BinarySearchTree.h"

/**
 * @file BST.h
 * @brief Concrete Derived Class representing a standard unbalanced Binary Search Tree.
 * 
 * Demonstrates:
 * - Inheritance: Inherits properties, traversal methods, and data members from BinarySearchTree.
 * - Polymorphism: Overrides insert, remove, and getTreeType without automatic rotations.
 */
class BST : public BinarySearchTree {
private:
    Node* insertHelper(Node* current, int value, bool& inserted);
    Node* removeHelper(Node* current, int value, bool& removed);
    Node* findMin(Node* current) const;
    void updateHeight(Node* node);

public:
    BST();
    ~BST() override = default;

    /**
     * @brief Inserts a new unique key into the BST.
     * @param value Integer value to insert.
     * @return true if successfully inserted; false if duplicate.
     */
    bool insert(int value) override;

    /**
     * @brief Deletes a key from the BST using inorder successor replacement.
     * @param value Integer value to remove.
     * @return true if found and removed; false otherwise.
     */
    bool remove(int value) override;

    /**
     * @brief Returns the identifier name of this tree type.
     */
    std::string getTreeType() const override;
};

#endif // BST_H


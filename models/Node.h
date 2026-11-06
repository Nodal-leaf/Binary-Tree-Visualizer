#ifndef NODE_H
#define NODE_H

#include <nlohmann/json.hpp>
#include <algorithm>

/**
 * @file Node.h
 * @brief Encapsulated Node class representing an element in a binary search tree.
 * 
 * Adheres to the principle of Encapsulation by keeping internal data
 * (value, height, id, child pointers) private and providing controlled
 * public accessors and mutators.
 */
class Node {
private:
    int value_;       ///< Stored numerical key
    int height_;      ///< Height of the node (1 for a leaf node)
    int id_;          ///< Unique node identifier for GUI animations/tracking
    Node* left_;      ///< Pointer to left child subtree
    Node* right_;     ///< Pointer to right child subtree

public:
    /**
     * @brief Parameterized constructor.
     * @param val The numerical key stored in the node.
     * @param id Unique node ID (defaults to 0).
     */
    explicit Node(int val, int id = 0);

    /**
     * @brief Destructor. Child cleanup is managed by tree classes.
     */
    ~Node() = default;

    // Disallow accidental copying of individual nodes (deep copies are handled by tree)
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    // Getters
    int getValue() const;
    int getHeight() const;
    int getId() const;
    Node* getLeft() const;
    Node* getRight() const;

    // Setters
    void setValue(int val);
    void setHeight(int height);
    void setId(int id);
    void setLeft(Node* left);
    void setRight(Node* right);

    /**
     * @brief Computes the balance factor: height(left) - height(right).
     * @return Integer balance factor (-1, 0, 1 for balanced AVL node).
     */
    int getBalanceFactor() const;

    /**
     * @brief Helper queries for node topology.
     */
    bool isLeaf() const;
    bool hasLeft() const;
    bool hasRight() const;
    bool hasBothChildren() const;

    /**
     * @brief Serializes the node and its subtrees to a JSON object.
     * @return nlohmann::json containing node details and recursive child objects.
     */
    nlohmann::json toJson() const;
};

#endif // NODE_H


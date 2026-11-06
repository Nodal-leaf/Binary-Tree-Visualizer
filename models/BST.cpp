#include "BST.h"
#include <algorithm>

BST::BST() : BinarySearchTree() {}

std::string BST::getTreeType() const {
    return "BST";
}

void BST::updateHeight(Node* node) {
    if (node != nullptr) {
        node->setHeight(1 + std::max(getNodeHeight(node->getLeft()), getNodeHeight(node->getRight())));
    }
}

Node* BST::findMin(Node* current) const {
    while (current != nullptr && current->getLeft() != nullptr) {
        current = current->getLeft();
    }
    return current;
}

Node* BST::insertHelper(Node* current, int value, bool& inserted) {
    if (current == nullptr) {
        inserted = true;
        return new Node(value, nextNodeId_++);
    }

    if (value < current->getValue()) {
        current->setLeft(insertHelper(current->getLeft(), value, inserted));
    } else if (value > current->getValue()) {
        current->setRight(insertHelper(current->getRight(), value, inserted));
    } else {
        // Duplicate key, cannot insert in strict BST
        inserted = false;
        return current;
    }

    updateHeight(current);
    return current;
}

bool BST::insert(int value) {
    bool inserted = false;
    root_ = insertHelper(root_, value, inserted);
    if (inserted) {
        nodeCount_++;
    }
    return inserted;
}

Node* BST::removeHelper(Node* current, int value, bool& removed) {
    if (current == nullptr) {
        removed = false;
        return nullptr;
    }

    if (value < current->getValue()) {
        current->setLeft(removeHelper(current->getLeft(), value, removed));
    } else if (value > current->getValue()) {
        current->setRight(removeHelper(current->getRight(), value, removed));
    } else {
        // Node found
        removed = true;

        // Case 1 & 2: 0 or 1 child
        if (current->getLeft() == nullptr) {
            Node* temp = current->getRight();
            delete current;
            return temp;
        } else if (current->getRight() == nullptr) {
            Node* temp = current->getLeft();
            delete current;
            return temp;
        }

        // Case 3: 2 children
        // Find inorder successor (minimum node in right subtree)
        Node* successor = findMin(current->getRight());
        current->setValue(successor->getValue());
        // Recursively remove the successor from the right subtree
        bool dummyRemoved = false;
        current->setRight(removeHelper(current->getRight(), successor->getValue(), dummyRemoved));
    }

    updateHeight(current);
    return current;
}

bool BST::remove(int value) {
    bool removed = false;
    root_ = removeHelper(root_, value, removed);
    if (removed) {
        nodeCount_--;
    }
    return removed;
}


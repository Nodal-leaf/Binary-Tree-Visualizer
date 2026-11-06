#include "AVLTree.h"
#include <algorithm>
#include <sstream>

AVLTree::AVLTree() : BinarySearchTree() {}

std::string AVLTree::getTreeType() const {
    return "AVL";
}

void AVLTree::updateHeight(Node* node) {
    if (node != nullptr) {
        node->setHeight(1 + std::max(getNodeHeight(node->getLeft()), getNodeHeight(node->getRight())));
    }
}

Node* AVLTree::findMin(Node* current) const {
    while (current != nullptr && current->getLeft() != nullptr) {
        current = current->getLeft();
    }
    return current;
}

Node* AVLTree::rotateRight(Node* y) {
    Node* x = y->getLeft();
    Node* T2 = x->getRight();

    // Perform rotation
    x->setRight(y);
    y->setLeft(T2);

    // Update heights
    updateHeight(y);
    updateHeight(x);

    return x;
}

Node* AVLTree::rotateLeft(Node* x) {
    Node* y = x->getRight();
    Node* T2 = y->getLeft();

    // Perform rotation
    y->setLeft(x);
    x->setRight(T2);

    // Update heights
    updateHeight(x);
    updateHeight(y);

    return y;
}

Node* AVLTree::rotateLeftRight(Node* node) {
    node->setLeft(rotateLeft(node->getLeft()));
    return rotateRight(node);
}

Node* AVLTree::rotateRightLeft(Node* node) {
    node->setRight(rotateRight(node->getRight()));
    return rotateLeft(node);
}

Node* AVLTree::balance(Node* node) {
    if (node == nullptr) {
        return nullptr;
    }

    updateHeight(node);
    int bf = node->getBalanceFactor();

    // Case 1: Left heavy (bf > 1)
    if (bf > 1) {
        if (node->getLeft() != nullptr && node->getLeft()->getBalanceFactor() >= 0) {
            // LL Rotation
            RotationStep step;
            step.type = "LL";
            step.rootValue = node->getValue();
            step.pivotValue = node->getLeft()->getValue();
            step.description = "Left-Left imbalance at node " + std::to_string(step.rootValue) +
                               ": performed single Right Rotation around " + std::to_string(step.pivotValue);
            lastRotations_.push_back(step);
            return rotateRight(node);
        } else if (node->getLeft() != nullptr) {
            // LR Rotation
            RotationStep step;
            step.type = "LR";
            step.rootValue = node->getValue();
            step.pivotValue = node->getLeft()->getValue();
            step.description = "Left-Right imbalance at node " + std::to_string(step.rootValue) +
                               ": performed Left Rotation on child " + std::to_string(step.pivotValue) +
                               " followed by Right Rotation on " + std::to_string(step.rootValue);
            lastRotations_.push_back(step);
            return rotateLeftRight(node);
        }
    }

    // Case 2: Right heavy (bf < -1)
    if (bf < -1) {
        if (node->getRight() != nullptr && node->getRight()->getBalanceFactor() <= 0) {
            // RR Rotation
            RotationStep step;
            step.type = "RR";
            step.rootValue = node->getValue();
            step.pivotValue = node->getRight()->getValue();
            step.description = "Right-Right imbalance at node " + std::to_string(step.rootValue) +
                               ": performed single Left Rotation around " + std::to_string(step.pivotValue);
            lastRotations_.push_back(step);
            return rotateLeft(node);
        } else if (node->getRight() != nullptr) {
            // RL Rotation
            RotationStep step;
            step.type = "RL";
            step.rootValue = node->getValue();
            step.pivotValue = node->getRight()->getValue();
            step.description = "Right-Left imbalance at node " + std::to_string(step.rootValue) +
                               ": performed Right Rotation on child " + std::to_string(step.pivotValue) +
                               " followed by Left Rotation on " + std::to_string(step.rootValue);
            lastRotations_.push_back(step);
            return rotateRightLeft(node);
        }
    }

    return node;
}

Node* AVLTree::insertHelper(Node* current, int value, bool& inserted) {
    if (current == nullptr) {
        inserted = true;
        return new Node(value, nextNodeId_++);
    }

    if (value < current->getValue()) {
        current->setLeft(insertHelper(current->getLeft(), value, inserted));
    } else if (value > current->getValue()) {
        current->setRight(insertHelper(current->getRight(), value, inserted));
    } else {
        inserted = false;
        return current;
    }

    // Rebalance node on recursion backtrack
    return balance(current);
}

bool AVLTree::insert(int value) {
    clearLastRotations();
    bool inserted = false;
    root_ = insertHelper(root_, value, inserted);
    if (inserted) {
        nodeCount_++;
    }
    return inserted;
}

Node* AVLTree::removeHelper(Node* current, int value, bool& removed) {
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

        if (current->getLeft() == nullptr || current->getRight() == nullptr) {
            Node* temp = (current->getLeft() != nullptr) ? current->getLeft() : current->getRight();
            if (temp == nullptr) {
                // No child (leaf node)
                delete current;
                return nullptr;
            } else {
                // Exactly one child: return child and delete current
                delete current;
                return balance(temp);
            }
        } else {
            // Two children
            Node* successor = findMin(current->getRight());
            current->setValue(successor->getValue());
            bool dummyRemoved = false;
            current->setRight(removeHelper(current->getRight(), successor->getValue(), dummyRemoved));
        }
    }

    if (current == nullptr) {
        return nullptr;
    }

    // Rebalance node on recursion backtrack
    return balance(current);
}

bool AVLTree::remove(int value) {
    clearLastRotations();
    bool removed = false;
    root_ = removeHelper(root_, value, removed);
    if (removed) {
        nodeCount_--;
    }
    return removed;
}

const std::vector<AVLTree::RotationStep>& AVLTree::getLastRotations() const {
    return lastRotations_;
}

void AVLTree::clearLastRotations() {
    lastRotations_.clear();
}

nlohmann::json AVLTree::toJson() const {
    nlohmann::json j = BinarySearchTree::toJson();
    nlohmann::json rotationsJson = nlohmann::json::array();
    for (const auto& r : lastRotations_) {
        rotationsJson.push_back(r.toJson());
    }
    j["lastRotations"] = rotationsJson;
    return j;
}

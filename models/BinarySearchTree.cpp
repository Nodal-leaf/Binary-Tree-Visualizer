#include "BinarySearchTree.h"
#include <algorithm>

BinarySearchTree::BinarySearchTree()
    : root_(nullptr), nodeCount_(0), nextNodeId_(1) {}

BinarySearchTree::~BinarySearchTree() {
    destroySubtree(root_);
    root_ = nullptr;
    nodeCount_ = 0;
}

BinarySearchTree::BinarySearchTree(const BinarySearchTree& other)
    : root_(nullptr), nodeCount_(other.nodeCount_), nextNodeId_(other.nextNodeId_) {
    root_ = cloneSubtree(other.root_);
}

BinarySearchTree& BinarySearchTree::operator=(const BinarySearchTree& other) {
    if (this != &other) {
        destroySubtree(root_);
        root_ = cloneSubtree(other.root_);
        nodeCount_ = other.nodeCount_;
        nextNodeId_ = other.nextNodeId_;
    }
    return *this;
}

BinarySearchTree::BinarySearchTree(BinarySearchTree&& other) noexcept
    : root_(other.root_), nodeCount_(other.nodeCount_), nextNodeId_(other.nextNodeId_) {
    other.root_ = nullptr;
    other.nodeCount_ = 0;
    other.nextNodeId_ = 1;
}

BinarySearchTree& BinarySearchTree::operator=(BinarySearchTree&& other) noexcept {
    if (this != &other) {
        destroySubtree(root_);
        root_ = other.root_;
        nodeCount_ = other.nodeCount_;
        nextNodeId_ = other.nextNodeId_;

        other.root_ = nullptr;
        other.nodeCount_ = 0;
        other.nextNodeId_ = 1;
    }
    return *this;
}

void BinarySearchTree::destroySubtree(Node* node) {
    if (node != nullptr) {
        destroySubtree(node->getLeft());
        destroySubtree(node->getRight());
        delete node;
    }
}

Node* BinarySearchTree::cloneSubtree(const Node* node) {
    if (node == nullptr) {
        return nullptr;
    }
    Node* newNode = new Node(node->getValue(), node->getId());
    newNode->setHeight(node->getHeight());
    newNode->setLeft(cloneSubtree(node->getLeft()));
    newNode->setRight(cloneSubtree(node->getRight()));
    return newNode;
}

int BinarySearchTree::getNodeHeight(const Node* node) const {
    return (node != nullptr) ? node->getHeight() : 0;
}

int BinarySearchTree::calculateBalance(const Node* node) const {
    return (node != nullptr) ? node->getBalanceFactor() : 0;
}

bool BinarySearchTree::search(int value) const {
    const Node* current = root_;
    while (current != nullptr) {
        if (value == current->getValue()) {
            return true;
        } else if (value < current->getValue()) {
            current = current->getLeft();
        } else {
            current = current->getRight();
        }
    }
    return false;
}

std::vector<int> BinarySearchTree::getSearchPath(int value) const {
    std::vector<int> path;
    const Node* current = root_;
    while (current != nullptr) {
        path.push_back(current->getValue());
        if (value == current->getValue()) {
            break;
        } else if (value < current->getValue()) {
            current = current->getLeft();
        } else {
            current = current->getRight();
        }
    }
    return path;
}

void BinarySearchTree::inorderRecursive(const Node* node, std::vector<int>& result) const {
    if (node != nullptr) {
        inorderRecursive(node->getLeft(), result);
        result.push_back(node->getValue());
        inorderRecursive(node->getRight(), result);
    }
}

std::vector<int> BinarySearchTree::inorder() const {
    std::vector<int> result;
    inorderRecursive(root_, result);
    return result;
}

void BinarySearchTree::preorderRecursive(const Node* node, std::vector<int>& result) const {
    if (node != nullptr) {
        result.push_back(node->getValue());
        preorderRecursive(node->getLeft(), result);
        preorderRecursive(node->getRight(), result);
    }
}

std::vector<int> BinarySearchTree::preorder() const {
    std::vector<int> result;
    preorderRecursive(root_, result);
    return result;
}

void BinarySearchTree::postorderRecursive(const Node* node, std::vector<int>& result) const {
    if (node != nullptr) {
        postorderRecursive(node->getLeft(), result);
        postorderRecursive(node->getRight(), result);
        result.push_back(node->getValue());
    }
}

std::vector<int> BinarySearchTree::postorder() const {
    std::vector<int> result;
    postorderRecursive(root_, result);
    return result;
}

int BinarySearchTree::getHeight() const {
    return getNodeHeight(root_);
}

int BinarySearchTree::getNodeCount() const {
    return nodeCount_;
}

bool BinarySearchTree::isEmpty() const {
    return root_ == nullptr;
}

void BinarySearchTree::clear() {
    destroySubtree(root_);
    root_ = nullptr;
    nodeCount_ = 0;
    nextNodeId_ = 1;
}

const Node* BinarySearchTree::getRoot() const {
    return root_;
}

nlohmann::json BinarySearchTree::toJson() const {
    nlohmann::json j;
    j["treeType"] = getTreeType();
    j["nodeCount"] = nodeCount_;
    j["height"] = getHeight();
    j["inorder"] = inorder();
    j["preorder"] = preorder();
    j["postorder"] = postorder();

    if (root_ != nullptr) {
        j["root"] = root_->toJson();
    } else {
        j["root"] = nullptr;
    }

    return j;
}


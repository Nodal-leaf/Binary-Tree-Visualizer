#include "Node.h"

Node::Node(int val, int id)
    : value_(val), height_(1), id_(id), left_(nullptr), right_(nullptr) {}

int Node::getValue() const {
    return value_;
}

int Node::getHeight() const {
    return height_;
}

int Node::getId() const {
    return id_;
}

Node* Node::getLeft() const {
    return left_;
}

Node* Node::getRight() const {
    return right_;
}

void Node::setValue(int val) {
    value_ = val;
}

void Node::setHeight(int height) {
    height_ = height;
}

void Node::setId(int id) {
    id_ = id;
}

void Node::setLeft(Node* left) {
    left_ = left;
}

void Node::setRight(Node* right) {
    right_ = right;
}

int Node::getBalanceFactor() const {
    int leftHeight = (left_ != nullptr) ? left_->getHeight() : 0;
    int rightHeight = (right_ != nullptr) ? right_->getHeight() : 0;
    return leftHeight - rightHeight;
}

bool Node::isLeaf() const {
    return left_ == nullptr && right_ == nullptr;
}

bool Node::hasLeft() const {
    return left_ != nullptr;
}

bool Node::hasRight() const {
    return right_ != nullptr;
}

bool Node::hasBothChildren() const {
    return left_ != nullptr && right_ != nullptr;
}

nlohmann::json Node::toJson() const {
    nlohmann::json j;
    j["id"] = id_;
    j["value"] = value_;
    j["height"] = height_;
    j["balanceFactor"] = getBalanceFactor();

    if (left_ != nullptr) {
        j["left"] = left_->toJson();
    } else {
        j["left"] = nullptr;
    }

    if (right_ != nullptr) {
        j["right"] = right_->toJson();
    } else {
        j["right"] = nullptr;
    }

    return j;
}


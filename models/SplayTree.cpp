#include "SplayTree.h"
#include <algorithm>
#include <string>





SplayTree::SplayTree() : BinarySearchTree() {}





void SplayTree::updateHeight(Node* node) {
    if (node != nullptr) {
        node->setHeight(1 + std::max(getNodeHeight(node->getLeft()),
                                     getNodeHeight(node->getRight())));
    }
}
Node* SplayTree::rotateRight(Node* y) {
    Node* x  = y->getLeft();
    Node* T2 = x->getRight();
    x->setRight(y);
    y->setLeft(T2);
    updateHeight(y);
    updateHeight(x);
    return x;
}
Node* SplayTree::rotateLeft(Node* x) {
    Node* y  = x->getRight();
    Node* T2 = y->getLeft();
    y->setLeft(x);
    x->setRight(T2);
    updateHeight(x);
    updateHeight(y);
    return y;
}
Node* SplayTree::findMax(Node* node) const {
    while (node != nullptr && node->getRight() != nullptr) {
        node = node->getRight();
    }
    return node;
}
















Node* SplayTree::splay(Node* root, int key) {
    if (root == nullptr || root->getValue() == key) {
        return root;
    }
    if (key < root->getValue()) {
        if (root->getLeft() == nullptr) {
            return root;
        }
        if (key < root->getLeft()->getValue()) {


            root->getLeft()->setLeft(splay(root->getLeft()->getLeft(), key));


            root = rotateRight(root);


            if (root->getLeft() != nullptr) {
                root = rotateRight(root);
            }
            SplayStep step;
            step.type          = "Zig-Zig";
            step.accessedValue = key;
            step.newRootValue  = root->getValue();
            step.description   = "Zig-Zig: target " + std::to_string(key) +
                                  " and parent both left children — two right rotations performed";
            lastSplaySteps_.push_back(step);
        } else if (key > root->getLeft()->getValue()) {


            root->getLeft()->setRight(splay(root->getLeft()->getRight(), key));


            if (root->getLeft()->getRight() != nullptr) {
                root->setLeft(rotateLeft(root->getLeft()));
            }


            root = (root->getLeft() != nullptr) ? rotateRight(root) : root;
            SplayStep step;
            step.type          = "Zig-Zag";
            step.accessedValue = key;
            step.newRootValue  = root->getValue();
            step.description   = "Zig-Zag: target " + std::to_string(key) +
                                  " is right child, parent is left child — left then right rotation";
            lastSplaySteps_.push_back(step);
        } else {
            root = rotateRight(root);
            SplayStep step;
            step.type          = "Zig";
            step.accessedValue = key;
            step.newRootValue  = root->getValue();
            step.description   = "Zig: target " + std::to_string(key) +
                                  " is left child of root — single right rotation";
            lastSplaySteps_.push_back(step);
        }
    } else {
        if (root->getRight() == nullptr) {
            return root;
        }
        if (key > root->getRight()->getValue()) {
            root->getRight()->setRight(splay(root->getRight()->getRight(), key));
            root = rotateLeft(root);
            if (root->getRight() != nullptr) {
                root = rotateLeft(root);
            }
            SplayStep step;
            step.type          = "Zag-Zag";
            step.accessedValue = key;
            step.newRootValue  = root->getValue();
            step.description   = "Zag-Zag: target " + std::to_string(key) +
                                  " and parent both right children — two left rotations performed";
            lastSplaySteps_.push_back(step);
        } else if (key < root->getRight()->getValue()) {
            root->getRight()->setLeft(splay(root->getRight()->getLeft(), key));
            if (root->getRight()->getLeft() != nullptr) {
                root->setRight(rotateRight(root->getRight()));
            }
            root = (root->getRight() != nullptr) ? rotateLeft(root) : root;
            SplayStep step;
            step.type          = "Zag-Zig";
            step.accessedValue = key;
            step.newRootValue  = root->getValue();
            step.description   = "Zag-Zig: target " + std::to_string(key) +
                                  " is left child, parent is right child — right then left rotation";
            lastSplaySteps_.push_back(step);
        } else {
            root = rotateLeft(root);
            SplayStep step;
            step.type          = "Zag";
            step.accessedValue = key;
            step.newRootValue  = root->getValue();
            step.description   = "Zag: target " + std::to_string(key) +
                                  " is right child of root — single left rotation";
            lastSplaySteps_.push_back(step);
        }
    }
    return root;
}





bool SplayTree::insert(int value) {
    clearLastSplaySteps();
    if (root_ == nullptr) {
        root_ = new Node(value, nextNodeId_++);
        nodeCount_++;
        SplayStep step;
        step.type          = "Zig";
        step.accessedValue = value;
        step.newRootValue  = value;
        step.description   = "Insert " + std::to_string(value) +
                              ": first node, becomes root";
        lastSplaySteps_.push_back(step);
        return true;
    }


    root_ = splay(root_, value);
    if (root_->getValue() == value) {
        return false;
    }


    Node* newNode = new Node(value, nextNodeId_++);
    if (value < root_->getValue()) {
        newNode->setRight(root_);
        newNode->setLeft(root_->getLeft());
        root_->setLeft(nullptr);
        updateHeight(root_);
    } else {
        newNode->setLeft(root_);
        newNode->setRight(root_->getRight());
        root_->setRight(nullptr);
        updateHeight(root_);
    }
    updateHeight(newNode);
    root_ = newNode;
    nodeCount_++;
    SplayStep step;
    step.type          = "Zig";
    step.accessedValue = value;
    step.newRootValue  = root_->getValue();
    step.description   = "Insert " + std::to_string(value) +
                          ": splayed to root via split-and-link";
    lastSplaySteps_.push_back(step);
    return true;
}
bool SplayTree::remove(int value) {
    clearLastSplaySteps();
    clearLastDeletion();
    if (root_ == nullptr) {
        return false;
    }


    root_ = splay(root_, value);
    if (root_->getValue() != value) {
        return false;
    }


    lastDeletion_.occurred       = true;
    lastDeletion_.targetValue    = value;
    lastDeletion_.hasTwoChildren = (root_->getLeft() != nullptr &&
                                    root_->getRight() != nullptr);
    lastDeletion_.description    = "Splay delete: splayed " +
                                   std::to_string(value) +
                                   " to root, then merged subtrees.";
    Node* leftSubtree  = root_->getLeft();
    Node* rightSubtree = root_->getRight();
    root_->setLeft(nullptr);
    root_->setRight(nullptr);
    delete root_;
    root_ = nullptr;
    nodeCount_--;
    if (leftSubtree == nullptr) {
        root_ = rightSubtree;
    } else if (rightSubtree == nullptr) {
        root_ = leftSubtree;
    } else {


        leftSubtree = splay(leftSubtree, value);
        leftSubtree->setRight(rightSubtree);
        updateHeight(leftSubtree);
        root_ = leftSubtree;
    }
    SplayStep step;
    step.type          = "Zag-Zag";
    step.accessedValue = value;
    step.newRootValue  = (root_ != nullptr) ? root_->getValue() : -1;
    step.description   = "Delete " + std::to_string(value) +
                          ": node removed, subtrees merged by splaying max of left";
    lastSplaySteps_.push_back(step);
    return true;
}
bool SplayTree::search(int value) const {


    SplayTree* self = const_cast<SplayTree*>(this);
    self->clearLastSplaySteps();
    if (root_ == nullptr) {
        return false;
    }
    self->root_ = self->splay(root_, value);
    SplayStep step;
    step.type          = "Zig";
    step.accessedValue = value;
    step.newRootValue  = self->root_->getValue();
    step.description   = "Search " + std::to_string(value) +
                          ": splayed closest node to root";
    lastSplaySteps_.push_back(step);
    return self->root_->getValue() == value;
}
std::string SplayTree::getTreeType() const {
    return "SPLAY";
}
const std::vector<SplayTree::SplayStep>& SplayTree::getLastSplaySteps() const {
    return lastSplaySteps_;
}
void SplayTree::clearLastSplaySteps() {
    lastSplaySteps_.clear();
}
nlohmann::json SplayTree::toJson() const {
    nlohmann::json j = BinarySearchTree::toJson();
    nlohmann::json stepsJson = nlohmann::json::array();
    for (const auto& s : lastSplaySteps_) {
        stepsJson.push_back(s.toJson());
    }
    j["lastSplaySteps"] = stepsJson;
    return j;
}

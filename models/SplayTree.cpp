#include "SplayTree.h"
#include <algorithm>
#include <string>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

SplayTree::SplayTree() : BinarySearchTree() {}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

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

    return x;  // new sub-tree root
}

Node* SplayTree::rotateLeft(Node* x) {
    Node* y  = x->getRight();
    Node* T2 = y->getLeft();

    y->setLeft(x);
    x->setRight(T2);

    updateHeight(x);
    updateHeight(y);

    return y;  // new sub-tree root
}

Node* SplayTree::findMax(Node* node) const {
    while (node != nullptr && node->getRight() != nullptr) {
        node = node->getRight();
    }
    return node;
}

// ---------------------------------------------------------------------------
// splay() – recursive bottom-up implementation.
//
// Brings the node containing `key` (or the closest ancestor when missing) to
// the root of the subtree.  Every meaningful rotation is logged as a SplayStep.
//
// Cases handled:
//   Zig      – key == root->left->value  (single right rotation)
//   Zag      – key == root->right->value (single left rotation)
//   Zig-Zig  – key < root->left->value   (two right rotations)
//   Zag-Zag  – key > root->right->value  (two left rotations)
//   Zig-Zag  – key in root->left->right  (left rot on child, right rot on root)
//   Zag-Zig  – key in root->right->left  (right rot on child, left rot on root)
// ---------------------------------------------------------------------------

Node* SplayTree::splay(Node* root, int key) {
    if (root == nullptr || root->getValue() == key) {
        return root;
    }

    if (key < root->getValue()) {
        // Key is in LEFT subtree
        if (root->getLeft() == nullptr) {
            // Key absent; closest ancestor is root
            return root;
        }

        if (key < root->getLeft()->getValue()) {
            // ---- Zig-Zig (Left-Left): target and parent are both left children ----
            // Recurse into left-left grandchild first
            root->getLeft()->setLeft(splay(root->getLeft()->getLeft(), key));

            // First rotation (root right)
            root = rotateRight(root);

            // Second rotation (if left child still exists)
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
            // ---- Zig-Zag (Left-Right): target is right child of left child ----
            // Recurse into left-right grandchild first
            root->getLeft()->setRight(splay(root->getLeft()->getRight(), key));

            // First rotation: left-rotate the left child
            if (root->getLeft()->getRight() != nullptr) {
                root->setLeft(rotateLeft(root->getLeft()));
            }

            // Second rotation: right-rotate the root
            root = (root->getLeft() != nullptr) ? rotateRight(root) : root;

            SplayStep step;
            step.type          = "Zig-Zag";
            step.accessedValue = key;
            step.newRootValue  = root->getValue();
            step.description   = "Zig-Zag: target " + std::to_string(key) +
                                  " is right child, parent is left child — left then right rotation";
            lastSplaySteps_.push_back(step);

        } else {
            // ---- Zig: key == root->left->value (target is direct left child) ----
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
        // Key is in RIGHT subtree
        if (root->getRight() == nullptr) {
            // Key absent; closest ancestor is root
            return root;
        }

        if (key > root->getRight()->getValue()) {
            // ---- Zag-Zag (Right-Right): target and parent are both right children ----
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
            // ---- Zag-Zig (Right-Left): target is left child of right child ----
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
            // ---- Zag: key == root->right->value (target is direct right child) ----
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

// ---------------------------------------------------------------------------
// Public interface
// ---------------------------------------------------------------------------

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

    // Splay the closest node to the insertion point to root
    root_ = splay(root_, value);

    if (root_->getValue() == value) {
        // Duplicate: already in tree
        return false;
    }

    // Create new node and split the tree at current root
    Node* newNode = new Node(value, nextNodeId_++);

    if (value < root_->getValue()) {
        // new node gets root's left subtree; root_ keeps its right subtree
        newNode->setRight(root_);
        newNode->setLeft(root_->getLeft());
        root_->setLeft(nullptr);
        updateHeight(root_);
    } else {
        // new node gets root's right subtree; root_ keeps its left subtree
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

    // Splay the target to root
    root_ = splay(root_, value);

    if (root_->getValue() != value) {
        // Not found
        return false;
    }

    // Target is at root
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
        // Splay maximum of left subtree (value-1 is always > everything in left,
        // so splay(leftSubtree, value) will bring the max to the root of leftSubtree).
        leftSubtree = splay(leftSubtree, value);
        // The new left-subtree root now has no right child (it was the max)
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
    // Use const_cast to perform the splay (structural change that is logically
    // const for a splay tree — it preserves the BST invariant and set of values).
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

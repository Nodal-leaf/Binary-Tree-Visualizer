#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include <string>
#include "../models/Node.h"
#include "../models/BinarySearchTree.h"
#include "../models/BST.h"
#include "../models/AVLTree.h"

// ANSI colors for clean test reporting
#define GREEN "\033[32m"
#define RED "\033[31m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

void testNodeEncapsulation() {
    std::cout << CYAN << "Running Test: Node Encapsulation & Queries..." << RESET << std::endl;
    Node node(42, 1);
    assert(node.getValue() == 42);
    assert(node.getId() == 1);
    assert(node.getHeight() == 1);
    assert(node.isLeaf() == true);
    assert(node.getBalanceFactor() == 0);

    node.setValue(100);
    assert(node.getValue() == 100);

    Node* leftChild = new Node(50, 2);
    Node* rightChild = new Node(150, 3);
    node.setLeft(leftChild);
    node.setRight(rightChild);

    assert(node.hasLeft() == true);
    assert(node.hasRight() == true);
    assert(node.hasBothChildren() == true);
    assert(node.isLeaf() == false);

    // Serialization
    nlohmann::json j = node.toJson();
    assert(j["value"] == 100);
    assert(j["left"]["value"] == 50);
    assert(j["right"]["value"] == 150);

    delete leftChild;
    delete rightChild;
    std::cout << GREEN << "✓ Node Encapsulation passed." << RESET << std::endl;
}

void testBSTOperations() {
    std::cout << CYAN << "Running Test: BST Operations (Insert, Traversals, Search, Delete)..." << RESET << std::endl;
    BST bst;
    assert(bst.isEmpty());
    assert(bst.getNodeCount() == 0);
    assert(bst.getHeight() == 0);

    // Insert elements
    std::vector<int> values = {50, 30, 70, 20, 40, 60, 80};
    for (int v : values) {
        bool inserted = bst.insert(v);
        assert(inserted);
    }
    assert(bst.getNodeCount() == 7);
    assert(!bst.insert(50)); // Duplicate rejected

    // Traversals
    std::vector<int> expectedInorder = {20, 30, 40, 50, 60, 70, 80};
    assert(bst.inorder() == expectedInorder);

    std::vector<int> expectedPreorder = {50, 30, 20, 40, 70, 60, 80};
    assert(bst.preorder() == expectedPreorder);

    std::vector<int> expectedPostorder = {20, 40, 30, 60, 80, 70, 50};
    assert(bst.postorder() == expectedPostorder);

    // Search & Search Path
    assert(bst.search(40) == true);
    assert(bst.search(999) == false);

    std::vector<int> searchPath40 = bst.getSearchPath(40);
    std::vector<int> expectedPath40 = {50, 30, 40};
    assert(searchPath40 == expectedPath40);

    // Deletion: Case 1 - Leaf Node (20)
    assert(bst.remove(20) == true);
    assert(bst.getNodeCount() == 6);
    assert(bst.search(20) == false);
    assert(bst.inorder() == (std::vector<int>{30, 40, 50, 60, 70, 80}));

    // Deletion: Case 2 - Node with 1 Child
    // Let's remove 30 (now only has right child 40)
    assert(bst.remove(30) == true);
    assert(bst.getNodeCount() == 5);
    assert(bst.search(30) == false);
    assert(bst.inorder() == (std::vector<int>{40, 50, 60, 70, 80}));

    // Deletion: Case 3 - Node with 2 Children (50 root)
    assert(bst.remove(50) == true);
    assert(bst.getNodeCount() == 4);
    assert(bst.search(50) == false);
    assert(bst.inorder() == (std::vector<int>{40, 60, 70, 80}));

    std::cout << GREEN << "✓ BST Operations passed." << RESET << std::endl;
}

void testAVLLRotations() {
    std::cout << CYAN << "Running Test: AVL LL Rotation..." << RESET << std::endl;
    AVLTree avl;
    avl.insert(30);
    avl.insert(20);
    avl.insert(10); // LL imbalance at 30

    assert(avl.getRoot()->getValue() == 20);
    assert(avl.getRoot()->getLeft()->getValue() == 10);
    assert(avl.getRoot()->getRight()->getValue() == 30);
    assert(avl.getHeight() == 2);
    assert(avl.getLastRotations().size() == 1);
    assert(avl.getLastRotations()[0].type == "LL");
    std::cout << GREEN << "✓ AVL LL Rotation verified: " << avl.getLastRotations()[0].description << RESET << std::endl;
}

void testAVLRRRotations() {
    std::cout << CYAN << "Running Test: AVL RR Rotation..." << RESET << std::endl;
    AVLTree avl;
    avl.insert(10);
    avl.insert(20);
    avl.insert(30); // RR imbalance at 10

    assert(avl.getRoot()->getValue() == 20);
    assert(avl.getRoot()->getLeft()->getValue() == 10);
    assert(avl.getRoot()->getRight()->getValue() == 30);
    assert(avl.getHeight() == 2);
    assert(avl.getLastRotations().size() == 1);
    assert(avl.getLastRotations()[0].type == "RR");
    std::cout << GREEN << "✓ AVL RR Rotation verified: " << avl.getLastRotations()[0].description << RESET << std::endl;
}

void testAVLLRRotations() {
    std::cout << CYAN << "Running Test: AVL LR Rotation..." << RESET << std::endl;
    AVLTree avl;
    avl.insert(30);
    avl.insert(10);
    avl.insert(20); // LR imbalance at 30

    assert(avl.getRoot()->getValue() == 20);
    assert(avl.getRoot()->getLeft()->getValue() == 10);
    assert(avl.getRoot()->getRight()->getValue() == 30);
    assert(avl.getHeight() == 2);
    assert(avl.getLastRotations().size() == 1);
    assert(avl.getLastRotations()[0].type == "LR");
    std::cout << GREEN << "✓ AVL LR Rotation verified: " << avl.getLastRotations()[0].description << RESET << std::endl;
}

void testAVLRLRotations() {
    std::cout << CYAN << "Running Test: AVL RL Rotation..." << RESET << std::endl;
    AVLTree avl;
    avl.insert(10);
    avl.insert(30);
    avl.insert(20); // RL imbalance at 10

    assert(avl.getRoot()->getValue() == 20);
    assert(avl.getRoot()->getLeft()->getValue() == 10);
    assert(avl.getRoot()->getRight()->getValue() == 30);
    assert(avl.getHeight() == 2);
    assert(avl.getLastRotations().size() == 1);
    assert(avl.getLastRotations()[0].type == "RL");
    std::cout << GREEN << "✓ AVL RL Rotation verified: " << avl.getLastRotations()[0].description << RESET << std::endl;
}

// Helper to check AVL balance invariant across all nodes
bool verifyAVLBalanceInvariant(const Node* node) {
    if (node == nullptr) return true;
    int bf = node->getBalanceFactor();
    if (bf < -1 || bf > 1) return false;
    return verifyAVLBalanceInvariant(node->getLeft()) && verifyAVLBalanceInvariant(node->getRight());
}

void testAVLComplexSequenceAndDeletion() {
    std::cout << CYAN << "Running Test: AVL Complex Sequences & Rebalancing on Deletion..." << RESET << std::endl;
    AVLTree avl;
    std::vector<int> data = {50, 25, 75, 10, 30, 60, 80, 5, 15, 27, 35};
    for (int v : data) {
        avl.insert(v);
        assert(verifyAVLBalanceInvariant(avl.getRoot()));
    }
    assert(avl.getNodeCount() == 11);
    assert(verifyAVLBalanceInvariant(avl.getRoot()));

    // Delete nodes causing rebalancing
    avl.remove(5);
    assert(verifyAVLBalanceInvariant(avl.getRoot()));
    avl.remove(10);
    assert(verifyAVLBalanceInvariant(avl.getRoot()));
    avl.remove(50);
    assert(verifyAVLBalanceInvariant(avl.getRoot()));
    assert(!avl.search(50));
    assert(avl.getNodeCount() == 8);

    // JSON export test
    nlohmann::json treeJson = avl.toJson();
    assert(treeJson["treeType"] == "AVL");
    assert(treeJson["nodeCount"] == 8);
    assert(treeJson.contains("root"));
    assert(treeJson.contains("lastRotations"));

    std::cout << GREEN << "✓ AVL Complex Sequence & Deletion passed." << RESET << std::endl;
}

void testPolymorphismAndAbstraction() {
    std::cout << CYAN << "Running Test: Polymorphism & Abstraction via Base Pointer..." << RESET << std::endl;
    // Polymorphic array of base class pointers
    std::vector<std::unique_ptr<BinarySearchTree>> trees;
    trees.push_back(std::make_unique<BST>());
    trees.push_back(std::make_unique<AVLTree>());

    std::vector<int> testData = {10, 20, 30, 40, 50};
    for (auto& tree : trees) {
        for (int v : testData) {
            tree->insert(v);
        }
    }

    // BST will have degraded into a linear chain (height = 5)
    assert(trees[0]->getTreeType() == "BST");
    assert(trees[0]->getHeight() == 5);

    // AVL will have self-balanced (height = 3)
    assert(trees[1]->getTreeType() == "AVL");
    assert(trees[1]->getHeight() == 3);

    // Inorder must be identical for both (sorted)
    assert(trees[0]->inorder() == trees[1]->inorder());

    std::cout << GREEN << "✓ Polymorphism verified: BST height = " << trees[0]->getHeight() 
              << ", AVL height = " << trees[1]->getHeight() << RESET << std::endl;
}

int main() {
    std::cout << YELLOW << "========================================" << RESET << std::endl;
    std::cout << YELLOW << "   BST & AVL TREE OOP CORE TEST SUITE   " << RESET << std::endl;
    std::cout << YELLOW << "========================================" << RESET << std::endl;

    try {
        testNodeEncapsulation();
        testBSTOperations();
        testAVLLRotations();
        testAVLRRRotations();
        testAVLLRRotations();
        testAVLRLRotations();
        testAVLComplexSequenceAndDeletion();
        testPolymorphismAndAbstraction();

        std::cout << YELLOW << "========================================" << RESET << std::endl;
        std::cout << GREEN << " ALL OOP UNIT TESTS PASSED SUCCESSFULLY! " << RESET << std::endl;
        std::cout << YELLOW << "========================================" << RESET << std::endl;
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << RED << "Test failed with exception: " << ex.what() << RESET << std::endl;
        return 1;
    }
}


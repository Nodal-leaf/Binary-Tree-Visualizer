#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include "Node.h"
#include <vector>
#include <string>
#include <nlohmann/json.hpp>











class BinarySearchTree {
protected:
    Node* root_;
    int nodeCount_;
    int nextNodeId_;

public:



    struct DeletionEvent {
        bool occurred = false;
        int targetValue = 0;
        bool hasTwoChildren = false;
        int successorValue = 0;
        std::string description;

        nlohmann::json toJson() const {
            return {
                {"occurred", occurred},
                {"targetValue", targetValue},
                {"hasTwoChildren", hasTwoChildren},
                {"successorValue", successorValue},
                {"description", description}
            };
        }
    };

protected:
    DeletionEvent lastDeletion_;


    void destroySubtree(Node* node);
    Node* cloneSubtree(const Node* node);
    int getNodeHeight(const Node* node) const;
    int calculateBalance(const Node* node) const;

    void inorderRecursive(const Node* node, std::vector<int>& result) const;
    void preorderRecursive(const Node* node, std::vector<int>& result) const;
    void postorderRecursive(const Node* node, std::vector<int>& result) const;

public:



    BinarySearchTree();




    virtual ~BinarySearchTree();


    BinarySearchTree(const BinarySearchTree& other);
    BinarySearchTree& operator=(const BinarySearchTree& other);
    BinarySearchTree(BinarySearchTree&& other) noexcept;
    BinarySearchTree& operator=(BinarySearchTree&& other) noexcept;







    virtual bool insert(int value) = 0;






    virtual bool remove(int value) = 0;





    virtual std::string getTreeType() const = 0;







    virtual bool search(int value) const;







    virtual std::vector<int> getSearchPath(int value) const;




    virtual std::vector<int> inorder() const;




    virtual std::vector<int> preorder() const;




    virtual std::vector<int> postorder() const;




    virtual int getHeight() const;




    virtual int getNodeCount() const;




    virtual bool isEmpty() const;




    virtual void clear();




    const DeletionEvent& getLastDeletion() const { return lastDeletion_; }




    void clearLastDeletion() { lastDeletion_ = DeletionEvent{}; }




    const Node* getRoot() const;





    virtual nlohmann::json toJson() const;
};

#endif


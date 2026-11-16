#ifndef BST_H
#define BST_H

#include "BinarySearchTree.h"









class BST : public BinarySearchTree {
private:
    Node* insertHelper(Node* current, int value, bool& inserted);
    Node* removeHelper(Node* current, int value, bool& removed);
    Node* findMin(Node* current) const;
    void updateHeight(Node* node);

public:
    BST();
    ~BST() override = default;






    bool insert(int value) override;






    bool remove(int value) override;




    std::string getTreeType() const override;
};

#endif


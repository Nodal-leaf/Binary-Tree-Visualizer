#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "BinarySearchTree.h"
#include <vector>
#include <string>










class AVLTree : public BinarySearchTree {
public:



    struct RotationStep {
        std::string type;
        int rootValue;
        int pivotValue;
        int grandchildValue = 0;
        std::string direction;
        std::string description;
        nlohmann::json treeBefore;
        nlohmann::json treeAfter;

        nlohmann::json toJson() const {
            return {
                {"type", type},
                {"rootValue", rootValue},
                {"pivotValue", pivotValue},
                {"grandchildValue", grandchildValue},
                {"direction", direction},
                {"description", description},
                {"treeBefore", treeBefore},
                {"treeAfter", treeAfter}
            };
        }
    };

private:
    std::vector<RotationStep> lastRotations_;


    Node* rotateRight(Node* y);
    Node* rotateLeft(Node* x);
    Node* rotateLeftRight(Node* node);
    Node* rotateRightLeft(Node* node);


    Node* balance(Node* node);
    Node* insertHelper(Node* current, int value, bool& inserted);
    Node* removeHelper(Node* current, int value, bool& removed);
    Node* findMin(Node* current) const;
    void updateHeight(Node* node);

public:
    AVLTree();
    ~AVLTree() override = default;






    bool insert(int value) override;






    bool remove(int value) override;




    std::string getTreeType() const override;




    const std::vector<RotationStep>& getLastRotations() const;




    void clearLastRotations();




    nlohmann::json toJson() const override;
};

#endif


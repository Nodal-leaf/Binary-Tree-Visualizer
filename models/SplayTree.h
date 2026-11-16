#ifndef SPLAY_TREE_H
#define SPLAY_TREE_H

#include "BinarySearchTree.h"
#include <vector>
#include <string>














class SplayTree : public BinarySearchTree {
public:



    struct SplayStep {
        std::string type;
        int accessedValue;
        int newRootValue;
        std::string description;

        nlohmann::json toJson() const {
            return {
                {"type",          type},
                {"accessedValue", accessedValue},
                {"newRootValue",  newRootValue},
                {"description",   description}
            };
        }
    };

private:
    mutable std::vector<SplayStep> lastSplaySteps_;


    Node* rotateRight(Node* y);
    Node* rotateLeft(Node* x);









    Node* splay(Node* root, int key);


    Node* findMax(Node* node) const;


    void updateHeight(Node* node);

public:
    SplayTree();
    ~SplayTree() override = default;





    bool insert(int value) override;







    bool remove(int value) override;






    bool search(int value) const override;




    std::string getTreeType() const override;




    const std::vector<SplayStep>& getLastSplaySteps() const;




    void clearLastSplaySteps();




    nlohmann::json toJson() const override;
};

#endif

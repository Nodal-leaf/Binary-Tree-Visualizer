#ifndef NODE_H
#define NODE_H
#include <nlohmann/json.hpp>
#include <algorithm>









class Node {
private:
    int value_;
    int height_;
    int id_;
    Node* left_;
    Node* right_;
public:





    explicit Node(int val, int id = 0);




    ~Node() = default;


    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;


    int getValue() const;
    int getHeight() const;
    int getId() const;
    Node* getLeft() const;
    Node* getRight() const;


    void setValue(int val);
    void setHeight(int height);
    void setId(int id);
    void setLeft(Node* left);
    void setRight(Node* right);





    int getBalanceFactor() const;




    bool isLeaf() const;
    bool hasLeft() const;
    bool hasRight() const;
    bool hasBothChildren() const;





    nlohmann::json toJson() const;
};
#endif


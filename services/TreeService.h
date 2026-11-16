#ifndef TREE_SERVICE_H
#define TREE_SERVICE_H

#include "../models/BinarySearchTree.h"
#include "../models/BST.h"
#include "../models/AVLTree.h"
#include "../models/SplayTree.h"
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>










class TreeService {
private:
    std::unique_ptr<BinarySearchTree> currentTree_;
    mutable std::mutex mutex_;

    void rebuildAsType(const std::string& targetType);

public:
    explicit TreeService(const std::string& initialType = "AVL");
    ~TreeService() = default;


    TreeService(const TreeService&) = delete;
    TreeService& operator=(const TreeService&) = delete;








    nlohmann::json switchTreeType(const std::string& treeType, bool preserveData = true);






    nlohmann::json insertValue(int value);






    nlohmann::json removeValue(int value);






    nlohmann::json searchValue(int value) const;





    nlohmann::json clearTree();






    nlohmann::json batchInsert(const std::vector<int>& values);





    nlohmann::json getTreeState() const;




    nlohmann::json getTraversal(const std::string& type) const;




    nlohmann::json loadSample();
};

#endif


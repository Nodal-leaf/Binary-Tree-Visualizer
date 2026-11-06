#include "TreeService.h"
#include <iostream>

TreeService::TreeService(const std::string& initialType) {
    if (initialType == "BST") {
        currentTree_ = std::make_unique<BST>();
    } else {
        currentTree_ = std::make_unique<AVLTree>();
    }
}

void TreeService::rebuildAsType(const std::string& targetType) {
    std::vector<int> existingValues;
    if (currentTree_) {
        // Use preorder traversal to preserve natural insertion order
        existingValues = currentTree_->preorder();
    }

    if (targetType == "BST") {
        currentTree_ = std::make_unique<BST>();
    } else {
        currentTree_ = std::make_unique<AVLTree>();
    }

    for (int val : existingValues) {
        currentTree_->insert(val);
    }
}

nlohmann::json TreeService::switchTreeType(const std::string& treeType, bool preserveData) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::string normalized = (treeType == "BST" || treeType == "bst") ? "BST" : "AVL";
    if (preserveData) {
        rebuildAsType(normalized);
    } else {
        if (normalized == "BST") {
            currentTree_ = std::make_unique<BST>();
        } else {
            currentTree_ = std::make_unique<AVLTree>();
        }
    }

    nlohmann::json res;
    res["success"] = true;
    res["message"] = "Switched tree mode to " + normalized;
    res["tree"] = currentTree_->toJson();
    return res;
}

nlohmann::json TreeService::insertValue(int value) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Capture comparison path for GUI animation
    std::vector<int> path = currentTree_->getSearchPath(value);

    bool inserted = currentTree_->insert(value);

    nlohmann::json res;
    res["success"] = inserted;
    res["operation"] = "insert";
    res["value"] = value;
    res["searchPath"] = path;
    if (inserted) {
        res["message"] = "Successfully inserted " + std::to_string(value);
    } else {
        res["message"] = "Value " + std::to_string(value) + " already exists in the tree.";
    }
    res["tree"] = currentTree_->toJson();
    return res;
}

nlohmann::json TreeService::removeValue(int value) {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<int> path = currentTree_->getSearchPath(value);
    bool removed = currentTree_->remove(value);

    nlohmann::json res;
    res["success"] = removed;
    res["operation"] = "delete";
    res["value"] = value;
    res["searchPath"] = path;
    if (removed) {
        res["message"] = "Successfully deleted " + std::to_string(value);
    } else {
        res["message"] = "Value " + std::to_string(value) + " not found in the tree.";
    }
    res["tree"] = currentTree_->toJson();
    return res;
}

nlohmann::json TreeService::searchValue(int value) const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<int> path = currentTree_->getSearchPath(value);
    bool found = currentTree_->search(value);

    nlohmann::json res;
    res["success"] = true;
    res["operation"] = "search";
    res["value"] = value;
    res["found"] = found;
    res["path"] = path;
    if (found) {
        res["message"] = "Value " + std::to_string(value) + " found in tree.";
    } else {
        res["message"] = "Value " + std::to_string(value) + " not found.";
    }
    return res;
}

nlohmann::json TreeService::clearTree() {
    std::lock_guard<std::mutex> lock(mutex_);
    currentTree_->clear();

    nlohmann::json res;
    res["success"] = true;
    res["operation"] = "clear";
    res["message"] = "Tree cleared successfully.";
    res["tree"] = currentTree_->toJson();
    return res;
}

nlohmann::json TreeService::batchInsert(const std::vector<int>& values) {
    std::lock_guard<std::mutex> lock(mutex_);
    int insertedCount = 0;
    for (int v : values) {
        if (currentTree_->insert(v)) {
            insertedCount++;
        }
    }

    nlohmann::json res;
    res["success"] = true;
    res["operation"] = "batch_insert";
    res["insertedCount"] = insertedCount;
    res["message"] = "Inserted " + std::to_string(insertedCount) + " out of " + std::to_string(values.size()) + " values.";
    res["tree"] = currentTree_->toJson();
    return res;
}

nlohmann::json TreeService::getTreeState() const {
    std::lock_guard<std::mutex> lock(mutex_);
    nlohmann::json res;
    res["success"] = true;
    res["tree"] = currentTree_->toJson();
    return res;
}

nlohmann::json TreeService::getTraversal(const std::string& type) const {
    std::lock_guard<std::mutex> lock(mutex_);
    nlohmann::json res;
    res["success"] = true;
    res["type"] = type;

    if (type == "inorder") {
        res["sequence"] = currentTree_->inorder();
    } else if (type == "preorder") {
        res["sequence"] = currentTree_->preorder();
    } else if (type == "postorder") {
        res["sequence"] = currentTree_->postorder();
    } else {
        res["success"] = false;
        res["message"] = "Invalid traversal type: " + type;
        return res;
    }

    return res;
}

nlohmann::json TreeService::loadSample() {
    std::lock_guard<std::mutex> lock(mutex_);
    currentTree_->clear();
    std::vector<int> sampleValues = {50, 25, 75, 15, 35, 65, 90, 30, 40};
    for (int v : sampleValues) {
        currentTree_->insert(v);
    }

    nlohmann::json res;
    res["success"] = true;
    res["operation"] = "sample";
    res["message"] = "Sample tree populated.";
    res["tree"] = currentTree_->toJson();
    return res;
}


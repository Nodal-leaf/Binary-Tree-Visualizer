#include "TreeController.h"
#include <iostream>
#include <nlohmann/json.hpp>

TreeController::TreeController(std::shared_ptr<TreeService> service)
    : treeService_(std::move(service)) {}

void TreeController::setupCors(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
}

int TreeController::parseValueFromJson(const std::string& body, bool& ok) {
    ok = false;
    try {
        auto json = nlohmann::json::parse(body);
        if (json.contains("value") && json["value"].is_number_integer()) {
            ok = true;
            return json["value"].get<int>();
        }
    } catch (...) {
        ok = false;
    }
    return 0;
}

void TreeController::registerRoutes(httplib::Server& svr) {
    // Global CORS preflight handler
    svr.Options(".*", [this](const httplib::Request&, httplib::Response& res) {
        setupCors(res);
        res.status = 204;
    });

    // GET /api/tree - Get full tree state
    svr.Get("/api/tree", [this](const httplib::Request&, httplib::Response& res) {
        setupCors(res);
        nlohmann::json state = treeService_->getTreeState();
        res.set_content(state.dump(2), "application/json");
    });

    // POST /api/tree/type - Switch between BST and AVL
    svr.Post("/api/tree/type", [this](const httplib::Request& req, httplib::Response& res) {
        setupCors(res);
        std::string type = "AVL";
        bool preserve = true;
        try {
            auto j = nlohmann::json::parse(req.body);
            if (j.contains("type") && j["type"].is_string()) {
                type = j["type"].get<std::string>();
            }
            if (j.contains("preserve") && j["preserve"].is_boolean()) {
                preserve = j["preserve"].get<bool>();
            }
        } catch (...) {}

        nlohmann::json result = treeService_->switchTreeType(type, preserve);
        res.set_content(result.dump(2), "application/json");
    });

    // POST /api/insert - Insert a value
    svr.Post("/api/insert", [this](const httplib::Request& req, httplib::Response& res) {
        setupCors(res);
        bool ok = false;
        int value = parseValueFromJson(req.body, ok);
        if (!ok) {
            nlohmann::json err = {
                {"success", false},
                {"message", "Invalid JSON payload. Expected {\"value\": integer}"}
            };
            res.status = 400;
            res.set_content(err.dump(2), "application/json");
            return;
        }

        nlohmann::json result = treeService_->insertValue(value);
        res.set_content(result.dump(2), "application/json");
    });

    // POST /api/delete - Delete a value
    svr.Post("/api/delete", [this](const httplib::Request& req, httplib::Response& res) {
        setupCors(res);
        bool ok = false;
        int value = parseValueFromJson(req.body, ok);
        if (!ok) {
            nlohmann::json err = {
                {"success", false},
                {"message", "Invalid JSON payload. Expected {\"value\": integer}"}
            };
            res.status = 400;
            res.set_content(err.dump(2), "application/json");
            return;
        }

        nlohmann::json result = treeService_->removeValue(value);
        res.set_content(result.dump(2), "application/json");
    });

    // POST /api/search - Search for a value and return animated search path
    svr.Post("/api/search", [this](const httplib::Request& req, httplib::Response& res) {
        setupCors(res);
        bool ok = false;
        int value = parseValueFromJson(req.body, ok);
        if (!ok) {
            nlohmann::json err = {
                {"success", false},
                {"message", "Invalid JSON payload. Expected {\"value\": integer}"}
            };
            res.status = 400;
            res.set_content(err.dump(2), "application/json");
            return;
        }

        nlohmann::json result = treeService_->searchValue(value);
        res.set_content(result.dump(2), "application/json");
    });

    // POST /api/clear - Clear the tree
    svr.Post("/api/clear", [this](const httplib::Request&, httplib::Response& res) {
        setupCors(res);
        nlohmann::json result = treeService_->clearTree();
        res.set_content(result.dump(2), "application/json");
    });

    // POST /api/batch - Insert multiple numbers at once
    svr.Post("/api/batch", [this](const httplib::Request& req, httplib::Response& res) {
        setupCors(res);
        std::vector<int> values;
        try {
            auto j = nlohmann::json::parse(req.body);
            if (j.contains("values") && j["values"].is_array()) {
                for (const auto& item : j["values"]) {
                    if (item.is_number_integer()) {
                        values.push_back(item.get<int>());
                    }
                }
            }
        } catch (...) {
            nlohmann::json err = {
                {"success", false},
                {"message", "Invalid JSON payload. Expected {\"values\": [integer, ...]}"}
            };
            res.status = 400;
            res.set_content(err.dump(2), "application/json");
            return;
        }

        nlohmann::json result = treeService_->batchInsert(values);
        res.set_content(result.dump(2), "application/json");
    });

    // POST /api/sample - Load standard demo tree
    svr.Post("/api/sample", [this](const httplib::Request&, httplib::Response& res) {
        setupCors(res);
        nlohmann::json result = treeService_->loadSample();
        res.set_content(result.dump(2), "application/json");
    });

    // GET /api/traversal/:type - Get traversal order (inorder, preorder, postorder)
    svr.Get(R"(/api/traversal/(\w+))", [this](const httplib::Request& req, httplib::Response& res) {
        setupCors(res);
        std::string type = req.matches[1];
        nlohmann::json result = treeService_->getTraversal(type);
        if (!result["success"].get<bool>()) {
            res.status = 400;
        }
        res.set_content(result.dump(2), "application/json");
    });

    // Serve static files from the frontend directory
    svr.set_mount_point("/", "./frontend");
    svr.set_file_extension_and_mimetype_mapping("svg", "image/svg+xml");
    svr.set_file_extension_and_mimetype_mapping("js", "application/javascript");
    svr.set_file_extension_and_mimetype_mapping("css", "text/css");
    svr.set_file_extension_and_mimetype_mapping("html", "text/html");
}

void TreeController::start(httplib::Server& svr, const std::string& host, int port) {
    registerRoutes(svr);
    std::cout << "\n========================================================" << std::endl;
    std::cout << "  BST & AVL Tree Visualizer Server Started Successfully " << std::endl;
    std::cout << "  Server URL: http://" << host << ":" << port << std::endl;
    std::cout << "  REST API base: http://" << host << ":" << port << "/api" << std::endl;
    std::cout << "========================================================\n" << std::endl;
    svr.listen(host, port);
}


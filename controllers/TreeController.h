#ifndef TREE_CONTROLLER_H
#define TREE_CONTROLLER_H

#include "../services/TreeService.h"
#include "../include/httplib.h"
#include <memory>
#include <string>

/**
 * @file TreeController.h
 * @brief REST API Controller layer handling HTTP requests and delegating to TreeService.
 * 
 * Demonstrates:
 * - Separation of Concerns: Handles network protocols, JSON parsing, error validation,
 *   and HTTP status codes, completely isolated from data structure logic.
 * - Dependency Injection: Receives a shared reference to the TreeService.
 */
class TreeController {
private:
    std::shared_ptr<TreeService> treeService_;

    void setupCors(httplib::Response& res);
    int parseValueFromJson(const std::string& body, bool& ok);

public:
    explicit TreeController(std::shared_ptr<TreeService> service);
    ~TreeController() = default;

    /**
     * @brief Registers all REST API endpoints and static file mounts with the HTTP server.
     * @param svr Reference to httplib::Server.
     */
    void registerRoutes(httplib::Server& svr);

    /**
     * @brief Starts the web server listening on host and port.
     * @param svr Reference to httplib::Server.
     * @param host IP address to bind (e.g., "127.0.0.1" or "0.0.0.0").
     * @param port Network port (e.g., 8080).
     */
    void start(httplib::Server& svr, const std::string& host = "127.0.0.1", int port = 8080);
};

#endif // TREE_CONTROLLER_H


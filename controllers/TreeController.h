#ifndef TREE_CONTROLLER_H
#define TREE_CONTROLLER_H
#include "../services/TreeService.h"
#include "../include/httplib.h"
#include <memory>
#include <string>










class TreeController {
private:
    std::shared_ptr<TreeService> treeService_;
    void setupCors(httplib::Response& res);
    int parseValueFromJson(const std::string& body, bool& ok);
public:
    explicit TreeController(std::shared_ptr<TreeService> service);
    ~TreeController() = default;





    void registerRoutes(httplib::Server& svr);







    void start(httplib::Server& svr, const std::string& host = "127.0.0.1", int port = 8080);
};
#endif


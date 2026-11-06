#include "services/TreeService.h"
#include "controllers/TreeController.h"
#include "include/httplib.h"
#include <iostream>
#include <memory>
#include <csignal>

httplib::Server svr;

void signalHandler(int signum) {
    std::cout << "\n[Server] Interrupt signal (" << signum << ") received. Shutting down gracefully..." << std::endl;
    svr.stop();
}

int main(int argc, char* argv[]) {
    // Register signal handlers for clean exit
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    int port = 8080;
    std::string host = "127.0.0.1";

    if (argc >= 2) {
        try {
            port = std::stoi(argv[1]);
        } catch (...) {
            std::cerr << "Invalid port argument, defaulting to 8080" << std::endl;
        }
    }

    std::cout << "Initializing Tree Backend Services..." << std::endl;
    auto service = std::make_shared<TreeService>("AVL");
    // Pre-populate with a balanced sample tree for immediate demonstration
    service->loadSample();

    TreeController controller(service);

    std::cout << "Starting HTTP server on " << host << ":" << port << "..." << std::endl;
    controller.start(svr, host, port);

    std::cout << "Server stopped." << std::endl;
    return 0;
}


cmake_minimum_required(VERSION 3.20)
project(TreeSimulation VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find or include nlohmann/json
find_package(nlohmann_json 3.2.0 QUIET)

# Core tree models library
add_library(TreeModels STATIC
    models/Node.cpp
    models/BinarySearchTree.cpp
    models/BST.cpp
    models/AVLTree.cpp
)

target_include_directories(TreeModels PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/models
)

if (TARGET nlohmann_json::nlohmann_json)
    target_link_libraries(TreeModels PUBLIC nlohmann_json::nlohmann_json)
endif()

# Unit tests executable
add_executable(test_trees
    tests/test_trees.cpp
)

target_link_libraries(test_trees PRIVATE TreeModels)
if (TARGET nlohmann_json::nlohmann_json)
    target_link_libraries(test_trees PRIVATE nlohmann_json::nlohmann_json)
endif()


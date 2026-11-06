## Demo

https://github.com/user-attachments/assets/4c70d301-0dd9-401d-95e0-677c35d3f1f9

# BST and AVL Tree GUI Simulation

This project is an interactive GUI simulation of Binary Search Trees (BST) and AVL Trees (a self-balancing binary search tree where the heights of any node's two child subtrees differ by at most one) built with a C++ backend and an HTML, CSS, and JavaScript frontend connected through a Crow-powered REST API. The backend strictly follows Object-Oriented Programming (OOP) principles: abstraction (exposing essential tree operations while hiding complex recursive details), encapsulation (keeping internal node data private with controlled public accessors), inheritance (allowing derived classes to reuse and extend base tree structures), polymorphism (enabling dynamic method dispatch for specialized operations through a shared base interface), and separation of concerns (dividing models, controllers, services, and frontend presentation into dedicated layers). At the core lies an abstract `BinarySearchTree` base class (a parent class defining a standardized contract of pure virtual methods without direct instantiation) alongside concrete derived `BST` and `AVLTree` classes operating on encapsulated `Node` objects. Both tree variants support insertion, deletion, searching, height calculation, node counting, and traversals (systematically visiting every node: inorder for sorted order, preorder, and postorder). The `AVLTree` class overrides insertion and deletion to perform balance-factor calculation (the height difference between left and right subtrees, calculated as height(left) - height(right)) and automatically triggers LL, RR, LR, and RL rotations (constant-time pointer restructurings that restore AVL balance without violating search order invariants), while also logging rotation metadata. On the frontend, JavaScript uses SVG to render nodes, calculate layout coordinates, draw dynamic connecting edges, and choreograph step-by-step animations for traversals, search paths, and rotations based on JSON payloads returned by the C++ server. Organized across `models/`, `controllers/`, `services/`, and `frontend/` directories and built using CMake with `nlohmann/json`, the project enforces a strict architectural boundary: all tree algorithms, balancing, and state transitions execute entirely in C++, while the frontend is solely responsible for rendering, animation, and user interaction.

---

## Getting Started

Follow these simple steps to clone, build, and launch the entire application with one click:

### 1. Clone the Repository
Open your terminal or command prompt and clone the repository:
```bash
git clone https://github.com/Nodal-leaf/Binary-Tree-Visualizer.git
cd Binary-Tree-Visualizer
```

### 2. Start the Server and GUI (One-Click Launch)
Run the automated Windows batch script by double-clicking **`run.bat`** in Windows File Explorer, or executing it from your terminal:
```cmd
run.bat
```

Alternatively, if you prefer PowerShell or Python:
- **PowerShell**: `.\run.ps1`
- **Python**: `python main.py`

### 3. What Happens Automatically
1. **Compilation Check**: The script automatically checks if the C++ backend binary (`build/tree_server.exe`) exists. If missing, it builds the project using CMake and Ninja.
2. **Server Launch**: Starts the high-performance C++ REST server on `http://127.0.0.1:8080`.
3. **Browser Automation**: Automatically opens your default web browser directly to the Interstellar Cosmos Tree Visualizer interface.
4. **Shutdown**: Press `Ctrl+C` in the terminal to stop the server cleanly and free the port.

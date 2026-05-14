# The Iron Nexus — Railway Management System

A comprehensive, terminal-based railway management system built in **C++** using advanced data structures. Manages train registration, coach configuration, railway networks, seat bookings, and operation logging — with full undo/redo support.

## Project Overview

The Iron Nexus is a complete railway management solution demonstrating sophisticated data structure implementations **without relying on STL containers** — all core structures are built from scratch using manual pointer-based memory management.

### Key Features

- **Train Registry Management** — AVL tree-based train organization for O(log n) operations
- **Coach Management** — Doubly-linked list for flexible coach manipulation
- **Railway Network** — Graph-based track and station system with pathfinding
- **Seating System** — Hybrid hash table + BST for instant seat lookups and sorted display
- **Operation Logging** — Stack-based logging with chronological history
- **Undo/Redo Support** — Complete action history with rollback capability
- **Persistent Storage** — Save/load system with full structure preservation
- **Colored Terminal UI** — Rich, user-friendly command-line interface

---

## System Architecture

### Module 1: Train Registry (`TrainRegistry.h`)
**Data Structure:** AVL Tree  
**Purpose:** Self-balancing binary search tree for O(log n) train lookups by ID  
**Operations:** Insert, delete, search, in-order/pre-order/post-order traversal

### Module 2: Coach Management (`CoachList.h`)
**Data Structure:** Doubly-Linked List  
**Purpose:** Flexible coach ordering with bidirectional traversal  
**Operations:** Add front/end/middle, remove, reverse, positional access

### Module 3: Railway Network (`RailwayNetwork.h`)
**Data Structure:** Adjacency List Graph  
**Purpose:** Model stations and tracks with distance optimization  
**Operations:** Add/remove stations, add/remove tracks, pathfinding

### Module 4: Seating Chart (`Seatingchart.h` & `Seatingchart.cpp`)
**Data Structure:** Hybrid Hash Table + Binary Search Tree
- **Layer 1 (Hash Table):** O(1) seat lookups via hashing with chaining collision handling
- **Layer 2 (BST):** Maintains ordered seat traversal and organized display

**Operations:** Book seat, cancel seat, check availability, display sorted/booked/available

### Module 5: Operation Log (`OperationLog.h`)
**Data Structure:** Stack  
**Purpose:** LIFO action recording with chronological access  
**Operations:** Push, pop, peek, display recent/all logs

### Module 6: Undo/Redo (`UndoRedo.h`, `UndoRedoImp.h`)
**Data Structure:** Two Action Stacks  
**Purpose:** Complete action history for reversible operations  
**Supported Actions:** Train / coach / station / track / seat operations, log clearing

---

## Project Structure

```
The-Iron-Nexus/
├── main.cpp              # Entry point & interactive CLI menu system
├── types.h               # Core struct definitions (CoachNode, TrainNode, LogEntry, StationNode)
├── TrainRegistry.h       # AVL tree implementation
├── CoachList.h           # Doubly-linked list implementation
├── RailwayNetwork.h      # Graph-based station & track network
├── Seatingchart.h        # Seating chart interface
├── Seatingchart.cpp      # Seating chart implementation (BST + hash table)
├── OperationLog.h        # Stack-based audit log
├── UndoRedo.h            # Undo/redo framework & action record definitions
├── UndoRedoImp.h         # Undo/redo logic implementation
├── .gitignore            # Git ignore rules
└── README.md             # This file
```

---

## Getting Started

### Requirements
Before running the project, make sure you have:

- Visual Studio 2022 (recommended)  
OR
- A C++11 compatible compiler:
  - `g++`
  - `clang++`
  - MSVC

Supported platforms:
- Windows
- Linux
- macOS

---

## Running the Project in Visual Studio (Recommended)

1. Clone or download the repository

```bash
git clone https://github.com/rameenfatima325/Railway-Managemnt-System.git
```
2. Open the project folder
3. Double-click: data proj.sln
4. Visual Studio will open the solution automatically
5. Select: Debug OR x64 (or x86 depending on your setup)
6. Press: Ctrl + F5 OR click Local Windows Debugger

Compilation Using g++

```bash
g++ -std=c++11 -o IronNexus main.cpp Seatingchart.cpp
```
Run

Linux / macOS

```bash
./IronNexus
```

Windows
```bash
IronNexus.exe
```

---

## Data Persistence

Data is automatically loaded on startup and can be saved from the main menu. The following files are created in the working directory:

| File | Contents |
|------|----------|
| `trains.txt` | Train records |
| `trains_structure.txt` | AVL tree structure (preorder) |
| `coaches_<ID>.txt` | Coach list per train |
| `coaches_<ID>_coach_<N>.txt` | Seating data per coach |
| `network.txt` | Station and track data |
| `operations.txt` | Full operation log |

These files are listed in `.gitignore` and are not committed to the repository.

---

## Undo/Redo Functionality

Every mutating operation is recorded and reversible:

- Add/delete trains — with automatic coach preservation
- Add/remove coaches — with automatic seat preservation
- Add/remove stations — with track restoration
- Add/remove tracks
- Book/cancel seats — with passenger name restoration
- Clear operation log — with full history restoration

---

## Data Structure Summary

| Module | Structure | Lookup | Insert/Delete | Rationale |
|--------|-----------|--------|---------------|-----------|
| Train Registry | AVL Tree | O(log n) | O(log n) | Balanced searches for large fleet |
| Coaches | Doubly-Linked List | O(n) | O(1) at ends | Flexible ordering, bidirectional traversal |
| Network | Adjacency List Graph | O(V+E) | O(1) | Sparse graph, efficient traversal |
| Seating | Hash Table + BST | O(1) / O(log n) | O(log n) | Fast lookup + sorted display |
| Operation Log | Stack | O(1) | O(1) | LIFO ordering, recent-first access |
| Undo/Redo | Dual Stacks | O(1) | O(1) | Stateful action history |

---

## Academic Notes

This project implements all data structures from scratch:
- No `<vector>`, `<list>`, `<stack>`, `<map>`, or other STL containers used for core logic
- Manual memory management with `new` and `delete`
- Custom pointer-based node structures
- Proper destructor chains for clean memory deallocation

---

## Contributors

This project was developed collaboratively by:

- **Rameen Fatima**
- **Ayan Ahmad**
- **Aizah Atif**

---

*Built for the Data Structures course — Spring 2026.*

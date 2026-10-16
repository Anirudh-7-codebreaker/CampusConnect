# SkillSwap 🎓 — Peer-to-Peer Skill Exchange Platform

> **Academic Milestone:** 40% Development Prototype  
> **Language / Standard:** C++17 (Standard Library only)  
> **Interface:** Terminal User Interface (TUI)  
> **Persistence:** Flat-file persistence (`ifstream` / `ofstream` / `getline`)  

---

## 1. Project Overview

**SkillSwap** is an Object-Oriented peer-to-peer skill exchange and mutual mentorship platform designed for university students. 

In university environments, students often excel in particular disciplines (e.g., C++ programming, UI/UX design, CAD modeling, machine learning) while wanting to learn complementary skills from peers. SkillSwap eliminates one-way transactional tutoring by algorithmically matching students based on **mutual skill reciprocity**: identifying pairs where Student A can teach what Student B wants to learn, and Student B can teach what Student A wants to learn.

This terminal-based C++17 prototype represents the **40% project milestone**. It provides a fully working, demonstrable system ready for academic code walkthroughs and viva examinations.

---

## 2. Directory Structure

```text
SkillSwap/
|
|-- .vscode/
|   |-- tasks.json               # Custom VS Code build task for all .cpp files
|
|-- include/
|   |-- skill.h                  # Skill class definition & normalization
|   |-- user.h                   # Abstract base User class
|   |-- student.h                # Derived Student class
|   |-- connection_request.h     # ConnectionRequest model & lifecycle
|   |-- match.h                  # Match struct & MatchComparator for priority_queue
|   |-- file_manager.h           # File persistence module interface
|   |-- skill_swap_app.h         # Main application controller
|
|-- src/
|   |-- main.cpp                 # Application entry point
|   |-- user.cpp                 # User base class implementation
|   |-- student.cpp              # Student & Skill implementations
|   |-- connection_request.cpp   # ConnectionRequest state transitions
|   |-- match_engine.cpp         # Match display & scoring presentation
|   |-- file_manager.cpp         # File I/O, parsing & seeding logic
|   |-- skill_swap_app.cpp       # UI, workflows, and data structure integration
|
|-- data/
|   |-- students.txt             # Persistent student records
|   |-- requests.txt             # Persistent connection requests
|   |-- README.txt               # Data directory documentation & schema
|
|-- docs/
|   |-- architecture.md          # Architectural patterns & OOP breakdown
|   |-- data_structures.md       # STL data structures mapping & complexities
|
|-- .gitignore                   # Ignores executables, objects, and editor files
|-- README.md                    # Project manual and build guide
```

---

## 3. How to Build and Run

### Prerequisites
- Any standard **C++17** compiler:
  - **Linux / macOS**: `g++` (GCC 7+) or `clang++`
  - **Windows**: MinGW-w64 (`g++`) or MSVC

### Linux / macOS
```bash
# Compile all source files with warnings enabled
g++ -std=c++17 -Wall -Wextra -g -I include src/*.cpp -o SkillSwap

# Run the executable
./SkillSwap
```

### Windows (PowerShell / Command Prompt)
If your Windows shell does not expand wildcards (`src/*.cpp`), specify translation units explicitly:
```powershell
g++ -std=c++17 -Wall -Wextra -g -I include src/main.cpp src/user.cpp src/student.cpp src/connection_request.cpp src/match_engine.cpp src/file_manager.cpp src/skill_swap_app.cpp -o SkillSwap.exe

# Run the executable
.\SkillSwap.exe
```

### Building in VS Code
- Open the project directory in VS Code.
- Press `Ctrl + Shift + B` (or `Cmd + Shift + B` on macOS).
- Select **C/C++: Build SkillSwap (All Files)** to trigger the build configured in `.vscode/tasks.json`.

---

## 4. Key Object-Oriented Programming (OOP) Concepts

| Concept | Implementation in SkillSwap | File Reference |
|---|---|---|
| **Classes & Objects** | Concrete instances of `Student`, `Skill`, `ConnectionRequest`, `Match`, and `SkillSwapApp`. | `include/*.h` |
| **Encapsulation** | All member variables are strictly `private`. Public getters and setters validate inputs (e.g. valid IDs, academic years 1–6, non-empty names). | `user.h`, `student.h`, `skill.h` |
| **Inheritance** | Derived class `Student` publicly inherits from abstract base class `User`. | `student.h`, `user.h` |
| **Runtime Polymorphism** | Abstract base `User` defines pure virtual `virtual void displayProfile() const = 0` and `virtual ~User()`. `Student` overrides `displayProfile()`. Demonstrable via dynamic dispatch through base class references. | `user.h`, `student.cpp`, `skill_swap_app.cpp` |
| **Function Overloading** | Overloaded `findStudent(int id)` vs. `findStudent(const string& name)`. Overloaded `addTeachableSkill(Skill)` vs. `addTeachableSkill(string, string)`. Overloaded `canTeach(...)` and `wantsToLearn(...)`. | `student.h`, `skill_swap_app.h` |
| **Operator Overloading** | `Skill::operator==` (case-insensitive normalized equality), `Skill::operator<` (strict weak ordering), and `operator<<` for clean stream output. | `skill.h`, `student.cpp`, `connection_request.cpp` |
| **Composition** | `Student` owns collections of `Skill` objects (`vector<Skill> teachableSkills`, `vector<Skill> desiredSkills`). | `student.h` |
| **Separation of Concerns** | Isolated persistence (`FileManager`), matching algorithms (`MatchEngine`), domain models, and UI controller (`SkillSwapApp`). | `src/` |

---

## 5. Required STL Data Structures and Their Real Uses

All five data structures play active, functional roles in the running application:

1. **`std::vector<Student>` — Master Profile Storage**
   - Stores all registered student profiles in contiguous memory for high CPU cache efficiency.
   - Used for iterating through all candidates during matching and writing profiles to disk.

2. **`std::unordered_map<int, size_t>` — Fast $O(1)$ ID Index**
   - Indexes `studentId -> index in vector`.
   - Eliminates expensive $O(N)$ linear scans when looking up profiles by student ID.
   - Rebuilt automatically upon data loading or profile creation to guarantee index consistency.

3. **`std::queue<int>` — FIFO Request Processing Queue**
   - Manages pending connection requests in strict arrival order (First-In, First-Out).
   - Demonstrates `push()`, `front()`, `pop()`, and `empty()`.
   - Reconstructed on startup from persistent records having `PENDING` status.

4. **`std::priority_queue<Match, vector<Match>, MatchComparator>` — Ranked Matching Engine**
   - Computes compatibility scores between the active student and potential candidates.
   - Max-heap comparator ensures that the candidate with the highest compatibility score (especially mutual skill swaps) appears first.

5. **`std::list<int>` — Active Connection Management**
   - Maintains a doubly-linked list of active connected peer IDs for each student.
   - Supports efficient insertion upon request acceptance and fast disconnection (`list::remove`).

---

## 6. Compatibility Scoring Formula

When matching a student with peers, the engine applies a transparent, documented formula:

- **+20 points** for each skill the student wants to learn that the candidate can teach.
- **+20 points** for each skill the student can teach that the candidate wants to learn.
- **+25 bonus points** for a **Mutual Exchange** (both students can learn from and teach each other).
- **+5 bonus points** if both students are in the same department (campus convenience).
- **+5 bonus points** if students are in the same or adjacent academic year (peer proximity).
- **Exclusions**: Self-matches ($A = B$) and candidates with zero skill overlap are automatically excluded.

---

## 7. Sample Walkthrough / Feature Demonstration

On launch, SkillSwap loads existing data from `data/students.txt` and `data/requests.txt`. If the data files are empty, it automatically seeds 8 realistic sample university students and 5 connection requests:

### Main Menu:
```text
========================================================================
        ____  _     _ _ _ ____                     
       / ___|| |__ (_) | / ___|_      ____ _ _ __  
       \___ \| |/ /| | | \___ \ \ /\ / / _` | '_ \ 
        ___) |   < | | | |___) \ V  V / (_| | |_) |
       |____/|_|\_|_|_|_|____/ \_/\_/ \__,_| .__/ 
                                            |_|    
           Peer-to-Peer University Skill Exchange  
                   Terminal Prototype v1.0         
========================================================================
 MAIN MENU
------------------------------------------------------------------------
  [1] View All Student Profiles        (vector iteration)
  [2] View / Inspect Student Profile   (O(1) unordered_map & Polymorphism)
  [3] Create New Student Profile       (Encapsulation & Validation)
  [4] Find Skill Matches               (priority_queue Ranked Compatibility)
  [5] Send Connection Request          (queue enqueue)
  [6] View Pending Requests            (queue FIFO pool inspection)
  [7] Process Next Pending Request     (queue FIFO front/pop & list update)
  [8] View Active Connections          (list<int> management & disconnect)
  [9] Save Data to Disk                (ifstream / ofstream persistence)
  [0] Exit Application
------------------------------------------------------------------------
```

### Try these quick actions during a demo or viva:
1. **Option 1 (View All Profiles)**: View the roster of 8 pre-seeded students across Computer Science, Data Science, Design, IT, Mechanical, and Electrical Engineering.
2. **Option 4 (Find Matches)**: Enter Student ID `101` (Anirudh Sharma: teaches C++, wants Python).
   - Notice Candidate `102` (Priya Patel: teaches Python, wants C++) ranks **#1** with an **80% Mutual Exchange score**!
   - Directly send a connection request from the prompt.
3. **Option 6 (View Pending Requests)**: Observe requests currently queued in FIFO order.
4. **Option 7 (Process Next Pending Request)**: Examine the oldest waiting request, view both sender and receiver profiles with skill exchange details, and choose **[1] Accept**.
5. **Option 8 (View Active Connections)**: Enter Student ID `101` or `102` to see their active peer network maintained in `std::list<int>`.

---

## 8. Manual Test Verification Checklist

The prototype has been tested against the following scenarios:

| Test Case | Procedure | Expected Result | Pass? |
|---|---|---|---|
| **Clean Build** | Run `g++ -std=c++17 -Wall -Wextra src/*.cpp` | Zero compilation errors and zero warnings. | **PASS** |
| **Startup Seeding** | Delete `data/*.txt` and launch application | Creates `data/` directory and seeds realistic starter data safely. | **PASS** |
| **Invalid Numeric Input** | Enter letters (e.g. `abc`) into menu choices | Recovers gracefully with warning; no infinite loop. | **PASS** |
| **Self-Request Prevention** | Send request from ID 101 to ID 101 | Rejected with message: *Students cannot send requests to themselves.* | **PASS** |
| **Duplicate Request Prevention**| Send a second request between same students | Rejected with duplicate pending request warning. | **PASS** |
| **Queue FIFO Order** | Send request A, then request B, then process | Request A is presented first, followed by Request B. | **PASS** |
| **Dynamic Dispatch** | Inspect profile by ID or name | Invokes virtual `displayProfile()` through `const User&`. | **PASS** |
| **Active Connection List** | Accept a pending request | Added to both students' `std::list<int>`. | **PASS** |
| **Active Disconnection** | Disconnect peer via Option 8 | Removed from both students' lists using `list::remove()`. | **PASS** |

---

## 9. Scope Boundaries & Future Roadmap

### Milestone 1 (Current 40% Scope) Completed:
- Full modular C++17 architecture (`include/` and `src/`).
- Complete domain models (`User`, `Student`, `Skill`, `ConnectionRequest`, `Match`).
- All 5 required STL data structures in working features.
- Algorithmic matching with max-heap priority queue.
- FIFO queue request handling.
- Active connection linked list management.
- Robust flat-file persistence.
- Complete documentation (`architecture.md`, `data_structures.md`, `README.md`).

### Planned for Subsequent Milestones (Remaining 60%):
- Real-time peer-to-peer messaging and session scheduling.
- Student rating and reputation system based on completed exchanges.
- Advanced multi-skill graph matching (e.g., 3-way circular skill trades).
- User authentication and encrypted credentials.
- Optional web frontend or graphical desktop UI.

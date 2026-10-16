# CampusConnect 🎓 — Student Networking & Peer Skill Exchange Platform


## 1. Project Overview

**CampusConnect** is a terminal-based peer-to-peer university student networking and skill exchange platform developed in standard **C++17**.

Students often possess specific skills (e.g. C++ programming, UI/UX design, CAD modeling, machine learning) while wanting to learn other disciplines from campus peers. CampusConnect automates this discovery by algorithmically calculating two-way compatibility scores and ranking prospective collaborators using a binary max-heap (`priority_queue`).

---

## 2. Directory Structure

```text
CampusConnect/
|
|-- .vscode/
|   |-- tasks.json               # Multi-file build task for VS Code (MinGW & g++)
|   |-- c_cpp_properties.json    # IntelliSense configuration for C++17
|
|-- include/
|   |-- skill.h                  # Skill class, normalization & operators
|   |-- user.h                   # Abstract base User class (polymorphism)
|   |-- student.h                # Derived Student class with overloads
|   |-- connection_request.h     # ConnectionRequest model & validated FSM
|   |-- match.h                  # Match model & MatchComparator for priority_queue
|   |-- file_manager.h           # Flat-file persistence module interface
|   |-- campus_connect_app.h     # Application controller & state coordinator
|
|-- src/
|   |-- main.cpp                 # Clean C++ main() entry point
|   |-- user.cpp                 # User constructors, virtual destructor, getters/setters
|   |-- student.cpp              # Student & Skill implementations with overloading
|   |-- connection_request.cpp   # Request lifecycle state transitions & formatting
|   |-- match_engine.cpp         # Match display card & scoring breakdown
|   |-- file_manager.cpp         # Robust parsing (ifstream, ofstream, stringstream)
|   |-- campus_connect_app.cpp   # Main menu, data structures, and terminal I/O
|
|-- data/
|   |-- students.txt             # Persistent student records (8 pre-seeded profiles)
|   |-- requests.txt             # Persistent connection requests (5 pre-seeded requests)
|   |-- README.txt               # Data directory schema and documentation
|
|-- docs/
|   |-- architecture.md          # Architecture, OOP principles & scoring breakdown
|   |-- data_structures.md       # STL data structures mapping & complexity analysis
|
|-- .gitignore                   # Excludes binaries, object files, and editor caches
|-- README.md                    # Complete project guide and editor walkthrough
```

---

## 3. How to Run in VS Code / Code Editor (Step-by-Step)

### Option A: Using VS Code's Built-in Terminal (Recommended)

1. **Open Project Folder in VS Code**:
   - Launch VS Code.
   - Go to `File` > `Open Folder...` (or press `Ctrl+K Ctrl+O` on Windows, `Cmd+O` on macOS).
   - Select the `CampusConnect` root directory.

2. **Open the Integrated Terminal**:
   - Press `Ctrl + ~` (tilde) or go to menu `Terminal` > `New Terminal`.

3. **Compile the Project**:
   - **Windows (PowerShell or Command Prompt with MinGW/g++)**:
     ```powershell
     g++ -std=c++17 -Wall -Wextra -g -I include src/main.cpp src/user.cpp src/student.cpp src/connection_request.cpp src/match_engine.cpp src/file_manager.cpp src/campus_connect_app.cpp -o CampusConnect.exe
     ```
   - **Linux / macOS (Bash / Zsh)**:
     ```bash
     g++ -std=c++17 -Wall -Wextra -g -I include src/*.cpp -o CampusConnect
     ```

4. **Run the Executable**:
   - **Windows**:
     ```powershell
     .\CampusConnect.exe
     ```
   - **Linux / macOS**:
     ```bash
     ./CampusConnect
     ```

---

### Option B: Using VS Code Automated Build Task (`tasks.json`)

The project includes a ready-to-use `.vscode/tasks.json` configuration:

1. In VS Code, press **`Ctrl + Shift + B`** (or `Cmd + Shift + B` on macOS).
2. Select **`C/C++: Build CampusConnect.exe (Windows MinGW)`** or **`C/C++: Build CampusConnect (Linux / macOS)`**.
3. VS Code will automatically compile all translation units without any manual commands.
4. Open the terminal panel and execute `.\CampusConnect.exe` (Windows) or `./CampusConnect` (Linux/macOS).

---

### Option C: Running in CLion, Code::Blocks, or Dev-C++

1. Create a new **C++ Console Application** project targeting **C++17**.
2. Add all header files from `include/` to the project headers.
3. Add all source files from `src/` to the project sources.
4. Set the include search directory to `include`.
5. Ensure the working directory contains the `data/` folder so `data/students.txt` and `data/requests.txt` are accessible.
6. Click **Build & Run** (or press `F9` / `Ctrl+F5`).

---

## 4. Key Object-Oriented Programming (OOP) Principles

| Concept | Implementation in CampusConnect | File Reference |
|---|---|---|
| **Classes & Objects** | Concrete domain objects for `Student`, `Skill`, `ConnectionRequest`, `Match`, and `CampusConnectApp`. | `include/*.h` |
| **Encapsulation** | Member variables are declared `private`. Public getters and setters enforce business rules (valid IDs, academic years 1–6, non-empty names). | `user.h`, `student.h`, `skill.h` |
| **Inheritance** | `Student` publicly derives from the abstract base class `User`. | `student.h`, `user.h` |
| **Runtime Polymorphism** | Abstract base `User` has `virtual void displayProfile() const = 0` and `virtual ~User()`. `Student` overrides `displayProfile()`. Demonstrable via dynamic dispatch through base class references (`const User&`). | `user.h`, `student.cpp`, `campus_connect_app.cpp` |
| **Function Overloading** | Overloaded `findStudent(int id)` vs. `findStudent(const string& name)`. Overloaded `addTeachableSkill(Skill)` vs. `addTeachableSkill(string, string)`. Overloaded `canTeach(...)` and `wantsToLearn(...)`. | `student.h`, `campus_connect_app.h` |
| **Operator Overloading** | `Skill::operator==` (case-insensitive normalized equality), `Skill::operator<` (strict weak ordering), and `operator<<` for clean stream output. | `skill.h`, `student.cpp`, `connection_request.cpp` |
| **Composition** | `Student` owns collections of `Skill` objects (`vector<Skill> teachableSkills`, `vector<Skill> desiredSkills`). | `student.h` |

---

## 5. STL Data Structures and Their Operational Roles

1. **`std::vector<Student>` — Master Profile Storage**
   - Stores all registered student profiles in contiguous memory for high CPU cache efficiency.
   - Used for profile iteration, disk serialization, and candidate iteration during matching.

2. **`std::unordered_map<int, size_t>` — Fast $O(1)$ ID Index**
   - Indexes `studentId -> index in vector`.
   - Eliminates expensive $O(N)$ linear scans when looking up profiles by student ID.
   - Rebuilt automatically upon data loading or new profile creation.

3. **`std::queue<int>` — FIFO Request Processing Queue**
   - Manages pending connection requests in strict chronological arrival order (First-In, First-Out).
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

## 7. Interactive Terminal Walkthrough

Upon launching `CampusConnect.exe`, the interactive terminal menu appears:

```text
========================================================================
   ____                                     ____                            _   
  / ___|__ _ _ __ ___  _ __  _   _ ___     / ___|___  _ __  _ __   ___  ___| |_ 
 | |   / _` | '_ ` _ \| '_ \| | | / __|   | |   / _ \| '_ \| '_ \ / _ \/ __| __|
 | |__| (_| | | | | | | |_) | |_| \__ \   | |__| (_) | | | | | | |  __/ (__| |_ 
  \____\__,_|_| |_| |_| .__/ \__,_|___/    \____\___/|_| |_|_| |_|\___|\___|\__|
                      |_|                                                       
           Student Networking & Peer Skill Exchange Platform
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

### Try these demo workflows:
1. **Option 1**: Inspect the 8 pre-seeded student profiles across Computer Science, Data Science, Design, IT, Mechanical, and Electrical Engineering.
2. **Option 4**: Enter Student ID `101` (Anirudh Sharma: teaches C++, wants Python).
   - Candidate `102` (Priya Patel: teaches Python, wants C++) ranks **#1** with a **90% Mutual Exchange score**!
3. **Option 6**: View pending connection requests queued in FIFO order.
4. **Option 7**: Evaluate the oldest waiting request, inspect sender and receiver skill exchange details, and choose **[1] Accept** to create an active connection.
5. **Option 8**: Enter Student ID `101` to view their active peer connection network stored in `std::list<int>`.

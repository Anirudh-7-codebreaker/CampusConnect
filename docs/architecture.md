# SkillSwap — Architecture & Object-Oriented Design

## 1. System Overview

**SkillSwap** is a peer-to-peer skill exchange and mutual mentorship platform designed for university students. It allows students to publish skills they can teach, specify skills they wish to learn, find compatible peers via an automated algorithmic matching engine, send connection requests, and manage active peer collaborations.

This document describes the software architecture, design patterns, and Object-Oriented Programming (OOP) principles implemented for the **40% Milestone Prototype** in **C++17**.

---

## 2. Module Organization & Responsibilities

The system is organized into modular components adhering to the **Separation of Concerns** principle:

```
+--------------------------------------------------------------------------+
|                              SkillSwapApp                                |
|   (Application Controller, Terminal UI, User Session, Lifecycle Logic)   |
+--------------------+---------------------+-------------------------------+
                     |                     |
     +---------------+                     +----------------+
     |                                                      |
     v                                                      v
+--------------------------+                      +------------------------+
|      Match Engine        |                      |      FileManager       |
| (Compatibility Scoring,  |                      | (Flat-file Persistence,|
|  priority_queue Ranking) |                      |  Parsing, Validation)  |
+------------+-------------+                      +------------+-----------+
             |                                                 |
             +--------------------+   +------------------------+
                                  |   |
                                  v   v
                   +----------------------------------+
                   |           Domain Models          |
                   |                                  |
                   |  - User (Abstract Base Class)    |
                   |  - Student (Derived Class)       |
                   |  - Skill (Composite Entity)      |
                   |  - ConnectionRequest (Lifecycle) |
                   |  - Match (Ranking Structure)     |
                   +----------------------------------+
```

### Module Breakdown:
1. **Domain Models (`skill.h`, `user.h`, `student.h`, `connection_request.h`, `match.h`)**:
   - Encapsulates domain logic, invariants, state transitions, and normalization.
2. **Matching Engine (`match.h`, `match_engine.cpp`)**:
   - Calculates two-way skill compatibility, scores candidates, and ranks them in a max-heap (`priority_queue`).
3. **Persistence Layer (`file_manager.h`, `file_manager.cpp`)**:
   - Handles structured serialization/deserialization to `data/students.txt` and `data/requests.txt`.
   - Protects against malformed data without crashing.
4. **Application Controller (`skill_swap_app.h`, `skill_swap_app.cpp`)**:
   - Manages state collections (`vector`, `unordered_map`, `queue`, `list`), orchestrates menu actions, and validates user input.
5. **Entry Point (`main.cpp`)**:
   - Clean application bootstrapping with top-level exception handling.

---

## 3. Object-Oriented Programming (OOP) Implementation

### A. Encapsulation & Data Hiding
- **Implementation**: In all classes (`User`, `Student`, `Skill`, `ConnectionRequest`), internal state variables are declared strictly `private`.
- **Interface**: External access is granted exclusively through validated public getters and setters (e.g., `setId()`, `setName()`, `setAcademicYear()`, `updateStatus()`).
- **Validation**: Mutators validate constraints (e.g., positive user IDs, academic years bounded between 1 and 6, non-empty names).

### B. Inheritance Hierarchy
- **Base Class**: `User` is an abstract base class (`user.h`) modeling common user identities:
  ```cpp
  class User {
  private:
      int id;
      std::string name;
  public:
      User(int id, const std::string& name);
      virtual ~User();
      virtual void displayProfile() const = 0; // Pure virtual
  };
  ```
- **Derived Class**: `Student` inherits publicly from `User` (`student.h`):
  ```cpp
  class Student : public User {
  private:
      std::string department;
      int academicYear;
      std::vector<Skill> teachableSkills;
      std::vector<Skill> desiredSkills;
      std::vector<std::string> interests;
      ...
  };
  ```
- **Rationale**: Extensible for future user types (e.g., `FacultyMentor`, `AlumniAdvisor`) without modifying core user identification logic.

### C. Runtime Polymorphism (Dynamic Dispatch)
- **Virtual Destructor**: `virtual ~User()` guarantees that derived `Student` instances are cleanly deallocated without memory leaks when deleted through a base class pointer.
- **Pure Virtual Function**: `virtual void displayProfile() const = 0;` establishes an abstract interface.
- **Dynamic Dispatch in Action**: Demonstrated via `renderUserProfilePolymorphic(const User& user)`:
  ```cpp
  static void renderUserProfilePolymorphic(const User& user) {
      user.displayProfile(); // Dynamic dispatch invokes Student::displayProfile()
  }
  ```

### D. Function Overloading
Multiple methods provide compile-time polymorphic variants for caller convenience:
1. **Profile Searching**:
   - `Student* findStudent(int id);` (fast lookup by integer ID via `unordered_map`)
   - `const Student* findStudent(const std::string& name);` (lookup by normalized name/substring)
2. **Adding Teachable Skills**:
   - `bool addTeachableSkill(const Skill& skill);`
   - `bool addTeachableSkill(const std::string& name, const std::string& category = "General");`
3. **Adding Desired Skills**:
   - `bool addDesiredSkill(const Skill& skill);`
   - `bool addDesiredSkill(const std::string& name, const std::string& category = "General");`
4. **Querying Skill Capabilities**:
   - `bool canTeach(const Skill& skill) const;`
   - `bool canTeach(const std::string& skillName) const;`
   - `bool wantsToLearn(const Skill& skill) const;`
   - `bool wantsToLearn(const std::string& skillName) const;`

### E. Operator Overloading
Operator overloads are implemented strictly where they improve semantic clarity and readability:
1. `bool Skill::operator==(const Skill& other) const`:
   - Case-insensitive, whitespace-normalized equality comparison (e.g., `"C++"` == `"c++ "` == `"C++"`).
2. `bool Skill::operator<(const Skill& other) const`:
   - Lexicographical ordering for sorted collections and associative containers.
3. `std::ostream& operator<<(std::ostream& os, const Skill& skill)`:
   - Stream insertion operator for intuitive formatted printing (`std::cout << skill`).
4. `bool ConnectionRequest::operator==(const ConnectionRequest& other) const`:
   - Equality based on unique request IDs.
5. `std::ostream& operator<<(std::ostream& os, const ConnectionRequest& req)`:
   - Formatted stream printing for request records.

### F. Composition & Object Relationships
- **Composite Ownership**: `Student` owns collections of `Skill` objects:
  - `std::vector<Skill> teachableSkills;`
  - `std::vector<Skill> desiredSkills;`
- **Normalization**: `Skill::normalize()` trims leading/trailing spaces and lowercases characters so that discrepancies in formatting do not produce false mismatch errors.

---

## 4. Matching Algorithm & Compatibility Scoring Formula

The matching engine computes a compatibility score $S \in [0, 100]$ between a selected student $A$ and every candidate $B \neq A$:

$$\text{Score} = (20 \times |\text{SkillsToLearn}|) + (20 \times |\text{SkillsToTeach}|) + \text{MutualBonus} + \text{DeptBonus} + \text{YearBonus}$$

Where:
1. **Skills To Learn (+20 pts each)**:
   Number of skills that student $A$ wants to learn that candidate $B$ can teach.
2. **Skills To Teach (+20 pts each)**:
   Number of skills that candidate $B$ wants to learn that student $A$ can teach.
3. **Mutual Reciprocity Bonus (+25 pts)**:
   Awarded if both $|\text{SkillsToLearn}| > 0$ and $|\text{SkillsToTeach}| > 0$. Represents a mutually beneficial exchange where both parties gain value.
4. **Department Affinity (+5 pts)**:
   Awarded if $A$ and $B$ belong to the same academic department (convenient for in-person collaboration on campus).
5. **Academic Proximity (+5 pts)**:
   Awarded if $|Year_A - Year_B| \le 1$ (peer-level proximity).
6. **Filtering**:
   Candidates with zero skill overlap ($|\text{SkillsToLearn}| = 0$ and $|\text{SkillsToTeach}| = 0$) and self-matches ($A = B$) are excluded. Scores are normalized to a maximum of 100%.

---

## 5. Connection Request State Machine

Connection requests follow a strictly validated finite state machine:

```
           +-------------------------+
           |     PENDING REQUEST     |
           +------------+------------+
                        |
            +-----------+-----------+
            |                       |
            v                       v
+-----------------------+   +-----------------------+
|   ACCEPTED REQUEST    |   |   REJECTED REQUEST    |
| (Active Connection)   |   |   (Terminal State)    |
+-----------------------+   +-----------------------+
```

- **Transitions Allowed**:
  - `PENDING -> ACCEPTED`
  - `PENDING -> REJECTED`
- **Transitions Forbidden**:
  - `ACCEPTED -> any` (terminal state)
  - `REJECTED -> any` (terminal state)
  - `PENDING -> PENDING` (no-op)
- **Side Effects**:
  - Transition to `ACCEPTED` updates both students' `std::list<int>` active connections.
  - Changes are immediately persisted to `data/requests.txt`.

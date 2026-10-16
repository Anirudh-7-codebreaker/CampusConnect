# SkillSwap — Data Structures & Algorithmic Design

This document details the exact mapping of all five required C++ Standard Template Library (STL) data structures to functional features in **SkillSwap**, including their time/space complexities, operational rationale, and persistence behavior.

---

## 1. Summary Matrix

| Data Structure | Feature Implemented | Primary Variable | Time Complexity (Primary Ops) | Role in System |
|---|---|---|---|---|
| **`std::vector`** | Primary Profile Storage | `std::vector<Student> students` | Access: $O(1)$<br>Append: amortized $O(1)$<br>Linear Scan: $O(N)$ | Master in-memory collection of all student records. |
| **`std::unordered_map`** | Fast Student ID Lookup | `std::unordered_map<int, size_t> studentIndex` | Search: average $O(1)$, worst $O(N)$<br>Insert: average $O(1)$ | Direct index from Student ID to vector index. |
| **`std::queue`** | FIFO Request Processing | `std::queue<int> pendingRequestIds` | Push: $O(1)$<br>Front: $O(1)$<br>Pop: $O(1)$ | Manages incoming connection requests in first-in-first-out order. |
| **`std::priority_queue`** | Ranked Compatibility Matching | `std::priority_queue<Match, vector<Match>, MatchComparator>` | Push: $O(\log K)$<br>Top: $O(1)$<br>Pop: $O(\log K)$ | Max-heap displaying highest-scoring exchange candidates first. |
| **`std::list`** | Active Connection Management | `std::unordered_map<int, std::list<int>> connectionsMap` | Insert: $O(1)$<br>Remove: $O(K)$ by value<br>Iteration: $O(K)$ | Manages bilateral peer networks and handles connection disconnects. |

---

## 2. In-Memory State vs. File Persistence

A fundamental architectural distinction in SkillSwap is between **temporary in-memory state** and **persistent disk storage**:

```
+--------------------------------------------------------------+
|                    PERSISTENT DISK LAYER                     |
|           data/students.txt    &    data/requests.txt        |
+------------------------------+-------------------------------+
                               |
               Boot / Load     |     Save / Mutate
                               v
+--------------------------------------------------------------+
|                     IN-MEMORY STL STATE                      |
|                                                              |
|  - vector<Student> students          (All Profiles)          |
|  - unordered_map<int, size_t> index  (O(1) ID Map)           |
|  - queue<int> pendingRequestIds      (Transient FIFO State)  |
|  - list<int> activeConnections       (Active Collaborators)  |
|  - priority_queue<Match> (transient) (Ranked Matches)        |
+--------------------------------------------------------------+
```

1. **Why In-Memory Collections Exist**:
   - Reading files from disk during every menu click or search operation would introduce severe disk I/O latency ($O(\text{disk access})$).
   - In-memory data structures provide microsecond-level query speed ($O(1)$ map lookups, $O(\log N)$ heap pops).
2. **Reconstruction on Startup**:
   - When SkillSwap launches, `FileManager::loadStudents()` populates `std::vector<Student>`.
   - `studentIndex` is indexed from the vector elements.
   - `pendingRequestIds` queue is reconstructed exclusively from request records whose status is `PENDING`.
   - `connectionsMap` lists are reconstructed exclusively from request records whose status is `ACCEPTED`.
3. **Synchronization**:
   - Whenever a profile is added, a request is sent, or a decision is made, in-memory state is updated and immediately synchronized back to disk via `FileManager::saveStudents()` or `FileManager::saveRequests()`.

---

## 3. Deep Dive into Data Structures

### A. `std::vector<Student>` — Master Profile Storage
- **Feature**:
  - Serves as the primary container storing all loaded student profiles.
  - Used for profile enumeration, disk serialization, and candidate iteration during skill matching.
- **Why Chosen**:
  - Elements are stored contiguously in memory, yielding excellent CPU cache locality during matching loops.
  - Random access is $O(1)$.
- **Complexity**:
  - Iteration: $O(N)$ where $N$ is total number of students.
  - Push back: Amortized $O(1)$.
- **Safe Index Handling**:
  - To prevent raw pointer invalidation caused by vector reallocations during `push_back()`, the application maps IDs to `size_t` indices in an `unordered_map`, rather than storing raw pointers.

---

### B. `std::unordered_map<int, size_t>` — Fast ID Lookup
- **Feature**:
  - Maps `Student.id -> vector index`.
  - Used in profile lookup, connection request dispatch, and match target verification.
- **Why Chosen**:
  - Avoids costly $O(N)$ linear scans when looking up students by ID.
  - Implemented via a hash table with bucket chaining.
- **Complexity**:
  - Lookup / Insert / Delete: Average $O(1)$, worst-case $O(N)$ (rare hash collisions).
- **Index Invalidation Protection**:
  - Whenever students are added, `studentIndex[newId] = students.size() - 1` is updated.
  - A full rebuild `rebuildStudentIndex()` runs whenever bulk records are reloaded.

---

### C. `std::queue<int>` — FIFO Request Processing
- **Feature**:
  - Manages the incoming pool of pending connection requests.
  - Processes requests in strict chronological arrival order (First-In, First-Out).
- **Why Chosen**:
  - Fair request scheduling: Earlier connection requests are evaluated by prospective mentors before newer ones.
- **Operations Demonstrated**:
  - `pendingRequestIds.push(newReqId)`: Enqueue newly sent request.
  - `pendingRequestIds.front()`: Inspect the oldest waiting request without popping.
  - `pendingRequestIds.pop()`: Remove the processed request from the front of the queue.
  - `pendingRequestIds.empty()`: Check if requests remain in pool.
- **Complexity**:
  - All operations are strictly $O(1)$.
- **Key Design Decision**:
  - The queue stores **request IDs (`int`)**, not duplicate `ConnectionRequest` objects. This prevents state divergence between the queue copy and the persistent vector/file records.

---

### D. `std::priority_queue<Match, vector<Match>, MatchComparator>` — Ranked Skill Matching
- **Feature**:
  - Evaluates all potential peers for a given student and ranks them in descending order of compatibility score.
- **Why Chosen**:
  - A binary max-heap ensures that candidate peers with the highest compatibility score (especially mutual skill swaps) are always retrieved first at $O(1)$ from `top()`.
- **Comparator Design**:
  ```cpp
  struct MatchComparator {
      bool operator()(const Match& a, const Match& b) const {
          if (a.compatibilityScore != b.compatibilityScore) {
              return a.compatibilityScore < b.compatibilityScore; // Max-heap: higher score prioritized
          }
          return a.candidateId > b.candidateId; // Deterministic tie-breaker
      }
  };
  ```
- **Complexity**:
  - Insertion: $O(\log K)$ where $K$ is number of compatible candidates.
  - Pop highest: $O(\log K)$.
  - Top examination: $O(1)$.
- **Behavioral Guarantee**:
  - The max-heap correctly surfaces candidates offering mutual skill exchanges (e.g. C++ $\leftrightarrow$ Python) at the very top of the recommendation list.

---

### E. `std::list<int>` — Active Connection Management
- **Feature**:
  - Maintains the collection of active peer student IDs connected to each student (`connectionsMap[studentId]`).
  - Supports peer listing and on-demand disconnection.
- **Why Chosen**:
  - Implemented as a doubly-linked list.
  - Insertion (`push_back`) and removal by iterator or value (`remove`) are fast and do not require shifting contiguous blocks of memory (unlike vectors).
  - Iterators to remaining elements remain valid when an active connection is removed.
- **Trade-offs & Limitations**:
  - *Lookup is $O(K)$*: Unlike an `unordered_map` or `unordered_set`, searching for a peer in a `std::list` requires traversing pointers ($O(K)$ where $K$ is the number of connections).
  - *Extra Memory Overhead*: Each node incurs memory overhead for two pointers (`prev` and `next`).
  - *Cache Performance*: Nodes may be scattered across heap memory, resulting in higher cache-miss rates than contiguous arrays.
  - For university students whose active peer network typically numbers between 2 and 20 peers, the small constant factor makes `std::list` completely suitable and demonstrative of linked-list manipulation.
- **Operations Demonstrated**:
  - `list::push_back(peerId)`: Adding newly accepted peer.
  - `list::remove(peerId)`: Disconnecting an active peer.
  - Iteration via `list::begin()` and `list::end()` for displaying active peer synergies.

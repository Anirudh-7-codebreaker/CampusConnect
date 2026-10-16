#ifndef SKILL_SWAP_APP_H
#define SKILL_SWAP_APP_H

#include "student.h"
#include "connection_request.h"
#include "match.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <list>
#include <string>

/**
 * @brief Core controller class managing the SkillSwap application lifecycle.
 *
 * Demonstrates:
 * - vector<Student>: primary in-memory storage of all loaded student profiles.
 * - unordered_map<int, size_t>: O(1) average lookup indexing student ID -> vector index.
 * - queue<int>: FIFO queue storing IDs of pending connection requests for sequential processing.
 * - priority_queue<Match, vector<Match>, MatchComparator>: max-heap scoring and ranking candidates.
 * - list<int>: active connection list for students, supporting efficient insertions and deletions.
 */
class SkillSwapApp {
private:
    // Core Data Structures
    std::vector<Student> students;                          // Data structure 1: Vector
    std::unordered_map<int, size_t> studentIndex;           // Data structure 2: Unordered Map
    std::vector<ConnectionRequest> requests;                // Complete history of requests
    std::queue<int> pendingRequestIds;                      // Data structure 3: Queue (FIFO)
    std::unordered_map<int, std::list<int>> connectionsMap; // Data structure 5: List (per student)

    // Configuration & File Paths
    std::string studentsFilePath;
    std::string requestsFilePath;

    // Active session state (optional currently active student for seamless menu interaction)
    int currentSessionStudentId;

    // Internal Helper Methods
    void rebuildStudentIndex();
    void rebuildPendingQueue();
    void rebuildActiveConnections();
    int getNextRequestId() const;
    int getNextStudentId() const;

    // Terminal UI Helpers
    void printHeader(const std::string& title) const;
    void printBanner() const;
    void printDivider() const;
    void pauseForUser() const;
    int readInt(const std::string& prompt, int minVal = -999999, int maxVal = 999999) const;
    std::string readLine(const std::string& prompt) const;

public:
    // Constructors & Destructor
    SkillSwapApp(const std::string& studentFile = "data/students.txt",
                 const std::string& requestFile = "data/requests.txt");
    ~SkillSwapApp();

    // Initialization & Lifecycle
    void init();
    void run();

    // Student Management
    void viewAllProfiles() const;
    void viewProfileById();
    void createProfile();

    // Function Overloading demonstrations
    Student* findStudent(int id);
    const Student* findStudent(int id) const;
    const Student* findStudent(const std::string& name) const; // Overload by normalized name

    // Skill Matching Engine (Data structure 4: Priority Queue)
    std::priority_queue<Match, std::vector<Match>, MatchComparator>
    calculateMatches(int studentId) const;
    void findSkillMatches();

    // Connection Request Management (Queue & List)
    void sendConnectionRequest();
    void viewPendingRequests() const;
    void processNextPendingRequest();
    void viewActiveConnections();
    void disconnectConnection();

    // Persistence
    void saveData();
    void reloadData();
};

#endif // SKILL_SWAP_APP_H

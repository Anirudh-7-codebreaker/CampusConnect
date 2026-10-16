#ifndef CAMPUS_CONNECT_APP_H
#define CAMPUS_CONNECT_APP_H

#include "student.h"
#include "connection_request.h"
#include "match.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <list>
#include <string>

/**
 * @brief CampusConnect Application Controller.
 *
 * Terminal-based student networking and peer skill exchange platform.
 * Core Data Structures:
 * - std::vector<Student>: Main in-memory profile collection.
 * - std::unordered_map<int, size_t>: O(1) student ID lookup index.
 * - std::queue<int>: FIFO queue for incoming connection requests.
 * - std::priority_queue<Match, vector<Match>, MatchComparator>: Compatibility ranking.
 * - std::list<int>: Active peer connections list per student.
 */
class CampusConnectApp {
private:
    // Core Data Structures
    std::vector<Student> students;                          // Data structure 1: Vector
    std::unordered_map<int, size_t> studentIndex;           // Data structure 2: Unordered Map
    std::vector<ConnectionRequest> requests;                // Complete request history
    std::queue<int> pendingRequestIds;                      // Data structure 3: Queue (FIFO)
    std::unordered_map<int, std::list<int>> connectionsMap; // Data structure 5: List (per student)

    // Configuration & Paths
    std::string studentsFilePath;
    std::string requestsFilePath;

    // Helper routines
    void rebuildStudentIndex();
    void rebuildPendingQueue();
    void rebuildActiveConnections();
    int getNextRequestId() const;
    int getNextStudentId() const;

    // Clean Terminal UI Helpers
    void printBanner() const;
    void printHeader(const std::string& title) const;
    void printDivider() const;
    void pauseForUser() const;
    int readInt(const std::string& prompt, int minVal = -999999, int maxVal = 999999) const;
    std::string readLine(const std::string& prompt) const;

public:
    CampusConnectApp(const std::string& studentFile = "data/students.txt",
                     const std::string& requestFile = "data/requests.txt");
    ~CampusConnectApp();

    void init();
    void run();

    // Profile Management
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

    // Flat-file persistence
    void saveData();
    void reloadData();
};

#endif // CAMPUS_CONNECT_APP_H

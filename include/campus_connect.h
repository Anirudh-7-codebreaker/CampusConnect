#ifndef CAMPUS_CONNECT_H
#define CAMPUS_CONNECT_H

#include "student.h"
#include <cstddef>
#include <list>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

struct Match {
    int candidateStudentId;
    std::string candidateName;
    int score;
    int learningMatches;
    int exchangeMatches;
    int mutualExchangeBonus;
    int sharedInterests;
    std::string explanation;
};

struct MatchComparator {
    bool operator()(const Match& left, const Match& right) const;
};

class CampusConnect {
public:
    bool registerStudent(int studentId, const std::string& name,
                         const std::string& department, int year,
                         const std::vector<std::string>& teachableSkills,
                         const std::vector<std::string>& desiredSkills,
                         const std::vector<std::string>& interests);
    bool addStudent(const Student& student);
    void displayAllProfiles() const;
    bool findStudentById(int studentId, Student& result) const;
    std::vector<Student> searchByInterest(const std::string& interest) const;
    std::vector<Student> searchByTeachableSkill(
        const std::string& skill) const;
    bool addDemoProfiles();
    std::priority_queue<Match, std::vector<Match>, MatchComparator>
    findMatches(int currentStudentId) const;
    void displayMatches(int currentStudentId) const;

    bool sendConnectionRequest(int senderId, int receiverId);
    void displayPendingRequests() const;
    bool processNextRequest(bool acceptRequest);
    void displayActiveConnections() const;

    const std::vector<Student>& getStudents() const;

private:
    static std::string normalizeSearchTerm(const std::string& value);
    static bool hasValidNames(const std::vector<std::string>& names);
    void rebuildIdIndex();
    bool areStudentsConnected(int firstStudentId, int secondStudentId) const;
    bool hasPendingRequest(int senderId, int receiverId) const;

    std::vector<Student> students_;
    std::unordered_map<int, std::size_t> studentIdIndex_;
    std::unordered_map<int, ConnectionRequest> requests_;
    std::queue<int> pendingRequestIds_;
    std::list<int> activeConnectionRequestIds_;
    int nextRequestId_ = 1;
};

#endif

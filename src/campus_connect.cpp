#include "campus_connect.h"
#include "file_handler.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <stdexcept>

bool MatchComparator::operator()(const Match& left,
                                 const Match& right) const {
    if (left.score != right.score) {
        return left.score < right.score;
    }
    return left.candidateStudentId > right.candidateStudentId;
}

bool CampusConnect::registerStudent(
    int studentId, const std::string& name, const std::string& department,
    int year, const std::vector<std::string>& teachableSkills,
    const std::vector<std::string>& interests) {
    if (studentId <= 0 || normalizeSearchTerm(name).empty() ||
        normalizeSearchTerm(department).empty() || year <= 0 ||
        !hasValidNames(teachableSkills) || !hasValidNames(interests)) {
        return false;
    }

    Student student(std::to_string(studentId), name, department, year);
    for (const std::string& skill : teachableSkills) {
        student.addTeachableSkill(skill);
    }
    for (const std::string& interest : interests) {
        student.addInterest(interest);
    }
    if (addStudent(student)) {
        saveNewStudentName(name);
        return true;
    }
    return false;
}

bool CampusConnect::addStudent(const Student& student) {
    int studentId = 0;
    try {
        std::size_t charactersRead = 0;
        studentId = std::stoi(student.getId(), &charactersRead);
        if (charactersRead != student.getId().size()) {
            return false;
        }
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }

    if (studentId <= 0 || normalizeSearchTerm(student.getName()).empty() ||
        normalizeSearchTerm(student.getDepartment()).empty() ||
        student.getYear() <= 0 ||
        !hasValidNames(student.getTeachableSkills()) ||
        !hasValidNames(student.getInterests())) {
        return false;
    }

    if (studentIdIndex_.count(studentId) != 0) {
        return false;
    }

    studentIdIndex_[studentId] = students_.size();
    students_.push_back(student);
    return true;
}

void CampusConnect::displayAllProfiles() const {
    for (const Student& student : students_) {
        student.displayProfile();
        std::cout << '\n';
    }
}

bool CampusConnect::findStudentById(int studentId, Student& result) const {
    const auto index = studentIdIndex_.find(studentId);
    if (index == studentIdIndex_.end()) {
        return false;
    }

    result = students_[index->second];
    return true;
}

std::vector<Student> CampusConnect::searchByInterest(
    const std::string& interest) const {
    const std::string normalizedInterest = normalizeSearchTerm(interest);
    std::vector<Student> matches;

    if (normalizedInterest.empty()) {
        return matches;
    }

    for (const Student& student : students_) {
        for (const std::string& studentInterest : student.getInterests()) {
            if (normalizeSearchTerm(studentInterest) == normalizedInterest) {
                matches.push_back(student);
                break;
            }
        }
    }
    return matches;
}

std::vector<Student> CampusConnect::searchByTeachableSkill(
    const std::string& skill) const {
    const std::string normalizedSkill = normalizeSearchTerm(skill);
    std::vector<Student> matches;

    if (normalizedSkill.empty()) {
        return matches;
    }

    for (const Student& student : students_) {
        for (const std::string& studentSkill : student.getTeachableSkills()) {
            if (normalizeSearchTerm(studentSkill) == normalizedSkill) {
                matches.push_back(student);
                break;
            }
        }
    }
    return matches;
}

bool CampusConnect::addDemoProfiles() {
    const std::size_t originalSize = students_.size();
    const bool addedAll =
        registerStudent(1001, "Aarav Mehta", "Computer Science", 2,
                        {"C++", "Algorithms"},
                        {"Robotics", "Photography"}) &&
        registerStudent(1002, "Diya Nair", "Information Technology", 3,
                        {"UI Design", "Figma"},
                        {"Reading", "Robotics"}) &&
        registerStudent(1003, "Rohan Iyer", "Electrical Engineering", 1,
                        {"Arduino", "Embedded Systems"},
                        {"Music", "Cycling"}) &&
        registerStudent(1004, "Ananya Das", "Mechanical Engineering", 4,
                        {"3D Modeling", "CAD"},
                        {"Robotics", "Volunteering"}) &&
        registerStudent(1005, "Kabir Shah", "Business Administration", 2,
                        {"Public Speaking", "Marketing"},
                        {"Entrepreneurship", "Photography"});

    if (!addedAll) {
        students_.resize(originalSize);
        rebuildIdIndex();
    }
    return addedAll;
}

std::priority_queue<Match, std::vector<Match>, MatchComparator>
CampusConnect::findMatches(int currentStudentId) const {
    std::priority_queue<Match, std::vector<Match>, MatchComparator> matches;
    Student currentStudent;
    if (!findStudentById(currentStudentId, currentStudent)) {
        return matches;
    }

    for (const Student& candidate : students_) {
        int candidateId = 0;
        try {
            std::size_t charactersRead = 0;
            candidateId = std::stoi(candidate.getId(), &charactersRead);
            if (charactersRead != candidate.getId().size()) {
                continue;
            }
        } catch (const std::invalid_argument&) {
            continue;
        } catch (const std::out_of_range&) {
            continue;
        }

        if (candidateId == currentStudentId) {
            continue;
        }

        int sharedSkills = 0;
        for (const std::string& teachableSkill : currentStudent.getTeachableSkills()) {
            for (const std::string& candidateSkill : candidate.getTeachableSkills()) {
                if (normalizeSearchTerm(teachableSkill) ==
                    normalizeSearchTerm(candidateSkill)) {
                    ++sharedSkills;
                    break;
                }
            }
        }

        int sharedInterests = 0;
        for (const std::string& currentInterest : currentStudent.getInterests()) {
            for (const std::string& candidateInterest : candidate.getInterests()) {
                if (normalizeSearchTerm(currentInterest) ==
                    normalizeSearchTerm(candidateInterest)) {
                    ++sharedInterests;
                    break;
                }
            }
        }

        if (sharedSkills == 0) {
            continue;
        }

        Match match;
        match.candidateStudentId = candidateId;
        match.candidateName = candidate.getName();
        match.exchangeMatches = sharedSkills;
        match.sharedInterests = sharedInterests;
        match.score = 15 * sharedSkills + 2 * sharedInterests;

        std::ostringstream explanation;
        explanation << sharedSkills
                    << " shared teachable skill(s), "
                    << sharedInterests << " shared interest(s)";
        match.explanation = explanation.str();
        matches.push(match);
    }

    return matches;
}

void CampusConnect::displayMatches(int currentStudentId) const {
    std::priority_queue<Match, std::vector<Match>, MatchComparator> matches =
        findMatches(currentStudentId);
    if (matches.empty()) {
        std::cout << "No skill matches found.\n";
        return;
    }

    std::cout << "Ranked matches for student " << currentStudentId << ":\n";
    int rank = 1;
    while (!matches.empty()) {
        const Match match = matches.top();
        matches.pop();
        std::cout << rank++ << ". " << match.candidateName << " (ID "
                  << match.candidateStudentId << ")\n"
                  << "   Score: " << match.score << '\n'
                  << "   Exchange matches: " << match.exchangeMatches << '\n'
                  << "   Shared interests: " << match.sharedInterests << '\n'
                  << "   Explanation: " << match.explanation << '\n';
    }
}

bool CampusConnect::sendConnectionRequest(int senderId, int receiverId) {
    if (senderId == receiverId || studentIdIndex_.count(senderId) == 0 ||
        studentIdIndex_.count(receiverId) == 0 ||
        hasPendingRequest(senderId, receiverId) ||
        areStudentsConnected(senderId, receiverId)) {
        return false;
    }

    const int requestId = nextRequestId_++;
    requests_.emplace(requestId,
                       ConnectionRequest(std::to_string(requestId),
                                         std::to_string(senderId),
                                         std::to_string(receiverId)));
    pendingRequestIds_.push(requestId);
    return true;
}

void CampusConnect::displayPendingRequests() const {
    if (pendingRequestIds_.empty()) {
        std::cout << "No pending connection requests.\n";
        return;
    }

    std::queue<int> pendingIds = pendingRequestIds_;
    while (!pendingIds.empty()) {
        const int requestId = pendingIds.front();
        pendingIds.pop();

        const auto request = requests_.find(requestId);
        if (request == requests_.end()) {
            continue;
        }

        std::cout << "Request " << requestId << ": Student "
                  << request->second.getSenderId() << " -> Student "
                  << request->second.getReceiverId() << '\n';
    }
}

bool CampusConnect::processNextRequest(bool acceptRequest) {
    if (pendingRequestIds_.empty()) {
        return false;
    }

    const int requestId = pendingRequestIds_.front();
    pendingRequestIds_.pop();

    const auto request = requests_.find(requestId);
    if (request == requests_.end()) {
        return false;
    }

    if (acceptRequest) {
        request->second.accept();
        activeConnectionRequestIds_.push_back(requestId);
    } else {
        request->second.reject();
    }
    return true;
}

void CampusConnect::displayActiveConnections() const {
    if (activeConnectionRequestIds_.empty()) {
        std::cout << "No active connections.\n";
        return;
    }

    for (const int requestId : activeConnectionRequestIds_) {
        const auto request = requests_.find(requestId);
        if (request != requests_.end()) {
            std::cout << "Student " << request->second.getSenderId()
                      << " <-> Student " << request->second.getReceiverId()
                      << '\n';
        }
    }
}

const std::vector<Student>& CampusConnect::getStudents() const {
    return students_;
}

std::string CampusConnect::normalizeSearchTerm(const std::string& value) {
    std::string normalized;
    bool inWhitespace = false;

    for (const unsigned char ch : value) {
        if (std::isspace(ch)) {
            if (!normalized.empty() && !inWhitespace) {
                normalized.push_back(' ');
                inWhitespace = true;
            }
            continue;
        }

        normalized.push_back(static_cast<char>(ch));
        inWhitespace = false;
    }

    while (!normalized.empty() && normalized.front() == ' ') {
        normalized.erase(normalized.begin());
    }
    while (!normalized.empty() && normalized.back() == ' ') {
        normalized.pop_back();
    }

    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });
    return normalized;
}

bool CampusConnect::hasValidNames(
    const std::vector<std::string>& names) {
    for (std::size_t index = 0; index < names.size(); ++index) {
        const std::string normalizedName = normalizeSearchTerm(names[index]);
        if (normalizedName.empty()) {
            return false;
        }
        for (std::size_t previous = 0; previous < index; ++previous) {
            if (normalizeSearchTerm(names[previous]) == normalizedName) {
                return false;
            }
        }
    }
    return true;
}

void CampusConnect::rebuildIdIndex() {
    studentIdIndex_.clear();
    for (std::size_t position = 0; position < students_.size(); ++position) {
        studentIdIndex_[std::stoi(students_[position].getId())] = position;
    }
}

bool CampusConnect::areStudentsConnected(int firstStudentId,
                                         int secondStudentId) const {
    for (const int requestId : activeConnectionRequestIds_) {
        const auto request = requests_.find(requestId);
        if (request == requests_.end()) {
            continue;
        }

        const int senderId = std::stoi(request->second.getSenderId());
        const int receiverId = std::stoi(request->second.getReceiverId());
        if ((senderId == firstStudentId && receiverId == secondStudentId) ||
            (senderId == secondStudentId && receiverId == firstStudentId)) {
            return true;
        }
    }
    return false;
}

bool CampusConnect::hasPendingRequest(int senderId, int receiverId) const {
    for (const auto& requestEntry : requests_) {
        const ConnectionRequest& request = requestEntry.second;
        if (request.getStatus() != ConnectionRequest::Status::Pending) {
            continue;
        }

        const int existingSenderId = std::stoi(request.getSenderId());
        const int existingReceiverId = std::stoi(request.getReceiverId());
        if ((existingSenderId == senderId && existingReceiverId == receiverId) ||
            (existingSenderId == receiverId && existingReceiverId == senderId)) {
            return true;
        }
    }
    return false;
}

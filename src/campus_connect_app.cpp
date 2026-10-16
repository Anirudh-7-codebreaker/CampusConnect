#include "campus_connect_app.h"
#include "file_manager.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <cmath>
#include <algorithm>

// Standalone function demonstrating Runtime Polymorphism via base class User reference
static void renderUserProfilePolymorphic(const User& user) {
    user.displayProfile(); // Dynamic dispatch invokes Student::displayProfile()
}

CampusConnectApp::CampusConnectApp(const std::string& studentFile, const std::string& requestFile)
    : studentsFilePath(studentFile), requestsFilePath(requestFile) {}

CampusConnectApp::~CampusConnectApp() {}

void CampusConnectApp::init() {
    FileManager::ensureDirectoryExists("data");
    FileManager::seedSampleDataIfEmpty(studentsFilePath, requestsFilePath);
    reloadData();
}

void CampusConnectApp::reloadData() {
    students.clear();
    requests.clear();

    FileManager::loadStudents(studentsFilePath, students);
    FileManager::loadRequests(requestsFilePath, requests);

    rebuildStudentIndex();
    rebuildPendingQueue();
    rebuildActiveConnections();
}

void CampusConnectApp::rebuildStudentIndex() {
    studentIndex.clear();
    for (size_t i = 0; i < students.size(); ++i) {
        studentIndex[students[i].getId()] = i;
    }
}

void CampusConnectApp::rebuildPendingQueue() {
    std::queue<int> emptyQueue;
    std::swap(pendingRequestIds, emptyQueue);

    for (const auto& req : requests) {
        if (req.getStatus() == RequestStatus::PENDING) {
            pendingRequestIds.push(req.getRequestId());
        }
    }
}

void CampusConnectApp::rebuildActiveConnections() {
    connectionsMap.clear();

    for (const auto& req : requests) {
        if (req.getStatus() == RequestStatus::ACCEPTED) {
            int s = req.getSenderId();
            int r = req.getReceiverId();

            auto& sList = connectionsMap[s];
            if (std::find(sList.begin(), sList.end(), r) == sList.end()) {
                sList.push_back(r);
            }

            auto& rList = connectionsMap[r];
            if (std::find(rList.begin(), rList.end(), s) == rList.end()) {
                rList.push_back(s);
            }
        }
    }
}

int CampusConnectApp::getNextRequestId() const {
    int maxId = 0;
    for (const auto& r : requests) {
        if (r.getRequestId() > maxId) maxId = r.getRequestId();
    }
    return maxId + 1;
}

int CampusConnectApp::getNextStudentId() const {
    int maxId = 100;
    for (const auto& s : students) {
        if (s.getId() > maxId) maxId = s.getId();
    }
    return maxId + 1;
}

// Function Overload 1: findStudent by ID (non-const) using unordered_map
Student* CampusConnectApp::findStudent(int id) {
    auto it = studentIndex.find(id);
    if (it != studentIndex.end() && it->second < students.size()) {
        return &students[it->second];
    }
    return nullptr;
}

// Function Overload 2: findStudent by ID (const) using unordered_map
const Student* CampusConnectApp::findStudent(int id) const {
    auto it = studentIndex.find(id);
    if (it != studentIndex.end() && it->second < students.size()) {
        return &students[it->second];
    }
    return nullptr;
}

// Function Overload 3: findStudent by Name (const) using normalized string search
const Student* CampusConnectApp::findStudent(const std::string& name) const {
    std::string normTarget = Skill::normalize(name);
    for (const auto& s : students) {
        std::string normName = Skill::normalize(s.getName());
        if (normName == normTarget || normName.find(normTarget) != std::string::npos) {
            return &s;
        }
    }
    return nullptr;
}

// Terminal UI Helpers
void CampusConnectApp::printBanner() const {
    std::cout << "\n";
    std::cout << "========================================================================\n";
    std::cout << "   ____                                     ____                            _   \n";
    std::cout << "  / ___|__ _ _ __ ___  _ __  _   _ ___     / ___|___  _ __  _ __   ___  ___| |_ \n";
    std::cout << " | |   / _` | '_ ` _ \\| '_ \\| | | / __|   | |   / _ \\| '_ \\| '_ \\ / _ \\/ __| __|\n";
    std::cout << " | |__| (_| | | | | | | |_) | |_| \\__ \\   | |__| (_) | | | | | | |  __/ (__| |_ \n";
    std::cout << "  \\____\\__,_|_| |_| |_| .__/ \\__,_|___/    \\____\\___/|_| |_|_| |_|\\___|\\___|\\__|\n";
    std::cout << "                      |_|                                                       \n";
    std::cout << "           Student Networking & Peer Skill Exchange Platform\n";
    std::cout << "========================================================================\n";
}

void CampusConnectApp::printHeader(const std::string& title) const {
    std::cout << "\n+----------------------------------------------------------------------+\n";
    std::cout << "| " << std::left << std::setw(68) << title << " |\n";
    std::cout << "+----------------------------------------------------------------------+\n";
}

void CampusConnectApp::printDivider() const {
    std::cout << "------------------------------------------------------------------------\n";
}

void CampusConnectApp::pauseForUser() const {
    std::cout << "\n[Press Enter to continue...]";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int CampusConnectApp::readInt(const std::string& prompt, int minVal, int maxVal) const {
    int val = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            if (val >= minVal && val <= maxVal) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return val;
            } else {
                std::cout << "  [!] Please enter a number between " << minVal << " and " << maxVal << ".\n";
            }
        } else {
            std::cout << "  [!] Invalid numeric input. Please try again.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::string CampusConnectApp::readLine(const std::string& prompt) const {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    size_t first = line.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = line.find_last_not_of(" \t\r\n");
    return line.substr(first, (last - first + 1));
}

// 1. View Profiles
void CampusConnectApp::viewAllProfiles() const {
    printHeader("REGISTERED STUDENT PROFILES (vector<Student> iteration)");
    std::cout << "Total Profiles: " << students.size() << "\n\n";

    if (students.empty()) {
        std::cout << "No student profiles found. Choose option 3 to create one.\n";
        return;
    }

    std::cout << std::left << std::setw(6) << "ID"
              << std::setw(20) << "Name"
              << std::setw(24) << "Department"
              << std::setw(8)  << "Year"
              << std::setw(25) << "Teaches (Sample)" << "\n";
    printDivider();

    for (const auto& s : students) {
        std::string sampleTeach = "(None)";
        if (!s.getTeachableSkills().empty()) {
            sampleTeach = s.getTeachableSkills().front().getName();
            if (s.getTeachableSkills().size() > 1) {
                sampleTeach += " (+" + std::to_string(s.getTeachableSkills().size() - 1) + " more)";
            }
        }

        std::cout << std::left << std::setw(6) << s.getId()
                  << std::setw(20) << s.getName()
                  << std::setw(24) << s.getDepartment()
                  << "Year " << std::setw(3) << s.getAcademicYear()
                  << std::setw(25) << sampleTeach << "\n";
    }
    printDivider();
}

// 2. View/Inspect Profile
void CampusConnectApp::viewProfileById() {
    printHeader("INSPECT STUDENT PROFILE (Polymorphism & Overloaded Lookup)");
    std::cout << "  [1] Search by Student ID (O(1) unordered_map search)\n";
    std::cout << "  [2] Search by Student Name (Normalized string search)\n";
    int choice = readInt("Enter choice (1-2): ", 1, 2);

    const Student* student = nullptr;
    if (choice == 1) {
        int id = readInt("Enter Student ID: ", 1, 999999);
        student = findStudent(id); // Calls findStudent(int)
    } else {
        std::string query = readLine("Enter Student Name: ");
        student = findStudent(query); // Calls findStudent(const string&)
    }

    if (!student) {
        std::cout << "\n[!] Error: Student not found in CampusConnect directory.\n";
        return;
    }

    std::cout << "\n[Dynamic Dispatch: Calling pure virtual displayProfile() through base User reference]\n";
    renderUserProfilePolymorphic(*student);

    auto it = connectionsMap.find(student->getId());
    std::cout << "  Active Connections: ";
    if (it == connectionsMap.end() || it->second.empty()) {
        std::cout << "None yet.\n";
    } else {
        std::cout << it->second.size() << " peer(s) [IDs: ";
        for (auto peerIt = it->second.begin(); peerIt != it->second.end(); ++peerIt) {
            std::cout << *peerIt;
            if (std::next(peerIt) != it->second.end()) std::cout << ", ";
        }
        std::cout << "]\n";
    }
    std::cout << "+----------------------------------------------------------------------+\n";
}

// 3. Create Profile
void CampusConnectApp::createProfile() {
    printHeader("CREATE NEW STUDENT PROFILE (Encapsulation & Validation)");

    int newId = getNextStudentId();
    std::cout << "Generated Student ID: #" << newId << "\n";

    std::string name;
    while (name.empty()) {
        name = readLine("Enter Full Name: ");
        if (name.empty()) std::cout << "  [!] Name cannot be blank.\n";
    }

    std::string dept = readLine("Enter Department (e.g., Computer Science, Design, IT): ");
    if (dept.empty()) dept = "General Engineering";

    int year = readInt("Enter Academic Year (1-6): ", 1, 6);

    Student newStudent(newId, name, dept, year);

    std::cout << "\n--- Teachable Skills (Skills you can share with peers) ---\n";
    while (true) {
        std::string skillName = readLine("Skill Name (leave empty to finish): ");
        if (skillName.empty()) break;
        std::string category = readLine("Category [default: General]: ");
        if (category.empty()) category = "General";

        if (newStudent.addTeachableSkill(skillName, category)) {
            std::cout << "  [+] Added: " << skillName << "\n";
        } else {
            std::cout << "  [!] Duplicate skill skipped.\n";
        }
    }

    std::cout << "\n--- Desired Skills (Skills you want to learn) ---\n";
    while (true) {
        std::string skillName = readLine("Skill Name (leave empty to finish): ");
        if (skillName.empty()) break;
        std::string category = readLine("Category [default: General]: ");
        if (category.empty()) category = "General";

        if (newStudent.addDesiredSkill(skillName, category)) {
            std::cout << "  [+] Added: " << skillName << "\n";
        } else {
            std::cout << "  [!] Duplicate skill skipped.\n";
        }
    }

    std::cout << "\n--- Extracurricular Interests ---\n";
    std::string interestsLine = readLine("Enter comma-separated interests (or empty to skip): ");
    if (!interestsLine.empty()) {
        std::stringstream ss(interestsLine);
        std::string item;
        while (std::getline(ss, item, ',')) {
            size_t f = item.find_first_not_of(" ");
            size_t l = item.find_last_not_of(" ");
            if (f != std::string::npos) {
                newStudent.addInterest(item.substr(f, l - f + 1));
            }
        }
    }

    students.push_back(newStudent);
    studentIndex[newId] = students.size() - 1;

    FileManager::saveStudents(studentsFilePath, students);
    std::cout << "\n[✓] Student profile created and saved to " << studentsFilePath << "!\n";
    renderUserProfilePolymorphic(newStudent);
}

// 4. Priority Queue Skill Matching Engine
std::priority_queue<Match, std::vector<Match>, MatchComparator>
CampusConnectApp::calculateMatches(int studentId) const {
    std::priority_queue<Match, std::vector<Match>, MatchComparator> pq;

    const Student* current = findStudent(studentId);
    if (!current) return pq;

    for (const auto& candidate : students) {
        if (candidate.getId() == studentId) continue;

        std::vector<Skill> toLearn = current->getMatchedSkillsToLearn(candidate);
        std::vector<Skill> toTeach = current->getMatchedSkillsToTeach(candidate);

        if (toLearn.empty() && toTeach.empty()) {
            continue;
        }

        int score = 0;
        score += static_cast<int>(toLearn.size()) * 20;
        score += static_cast<int>(toTeach.size()) * 20;

        bool isMutual = (!toLearn.empty() && !toTeach.empty());
        if (isMutual) score += 25; // Bonus for mutual reciprocity

        if (current->getDepartment() == candidate.getDepartment()) score += 5;
        if (std::abs(current->getAcademicYear() - candidate.getAcademicYear()) <= 1) score += 5;

        if (score > 100) score = 100;

        std::vector<std::string> toLearnNames;
        for (const auto& s : toLearn) toLearnNames.push_back(s.getName());
        std::vector<std::string> toTeachNames;
        for (const auto& s : toTeach) toTeachNames.push_back(s.getName());

        std::string explanation;
        if (isMutual) {
            explanation = "Mutual Exchange (+25 pts bonus): You learn [";
            for (size_t i = 0; i < toLearnNames.size(); ++i) {
                explanation += toLearnNames[i] + (i + 1 < toLearnNames.size() ? ", " : "");
            }
            explanation += "]; " + candidate.getName() + " learns [";
            for (size_t i = 0; i < toTeachNames.size(); ++i) {
                explanation += toTeachNames[i] + (i + 1 < toTeachNames.size() ? ", " : "");
            }
            explanation += "].";
        } else if (!toLearnNames.empty()) {
            explanation = "Direct Learning: " + candidate.getName() + " can teach you [";
            for (size_t i = 0; i < toLearnNames.size(); ++i) {
                explanation += toLearnNames[i] + (i + 1 < toLearnNames.size() ? ", " : "");
            }
            explanation += "].";
        } else {
            explanation = "Direct Mentoring: You can teach " + candidate.getName() + " [";
            for (size_t i = 0; i < toTeachNames.size(); ++i) {
                explanation += toTeachNames[i] + (i + 1 < toTeachNames.size() ? ", " : "");
            }
            explanation += "].";
        }

        Match match(candidate.getId(), candidate.getName(), candidate.getDepartment(),
                    candidate.getAcademicYear(), score, explanation,
                    toLearnNames, toTeachNames, isMutual);
        pq.push(match);
    }

    return pq;
}

void CampusConnectApp::findSkillMatches() {
    printHeader("SKILL MATCHING ENGINE (priority_queue Ranked Compatibility)");

    int studentId = readInt("Enter your Student ID: ", 1, 999999);
    const Student* current = findStudent(studentId);

    if (!current) {
        std::cout << "[!] Student ID #" << studentId << " not found.\n";
        return;
    }

    std::cout << "\nSearching matches for " << current->getName()
              << " (" << current->getDepartment() << ", Year " << current->getAcademicYear() << ")...\n";

    auto rankedMatches = calculateMatches(studentId);

    if (rankedMatches.empty()) {
        std::cout << "\nNo compatible matches found for your current skills.\n";
        return;
    }

    std::cout << "\nFound " << rankedMatches.size()
              << " compatible candidates (Ranked highest compatibility first by max-heap):\n\n";

    std::vector<Match> displayList;
    int rank = 1;
    while (!rankedMatches.empty()) {
        Match topMatch = rankedMatches.top();
        rankedMatches.pop();

        std::cout << "RANK #" << rank++ << "\n";
        topMatch.display();
        std::cout << "\n";

        displayList.push_back(topMatch);
    }

    std::cout << "Send a connection request to a candidate?\n";
    std::cout << "  [1] Yes, send request\n";
    std::cout << "  [0] Return to main menu\n";
    int choice = readInt("Enter choice: ", 0, 1);

    if (choice == 1) {
        int receiverId = readInt("Enter Candidate ID to connect with: ", 1, 999999);
        bool isValidCandidate = false;
        for (const auto& m : displayList) {
            if (m.candidateId == receiverId) {
                isValidCandidate = true;
                break;
            }
        }

        if (!isValidCandidate) {
            std::cout << "[!] That ID was not among the ranked candidates.\n";
            return;
        }

        for (const auto& req : requests) {
            if (req.getSenderId() == studentId && req.getReceiverId() == receiverId &&
                req.getStatus() == RequestStatus::PENDING) {
                std::cout << "[!] A pending connection request already exists.\n";
                return;
            }
        }

        auto it = connectionsMap.find(studentId);
        if (it != connectionsMap.end()) {
            if (std::find(it->second.begin(), it->second.end(), receiverId) != it->second.end()) {
                std::cout << "[!] You are already actively connected with Student #" << receiverId << "!\n";
                return;
            }
        }

        int newReqId = getNextRequestId();
        ConnectionRequest newReq(newReqId, studentId, receiverId, RequestStatus::PENDING);
        requests.push_back(newReq);
        pendingRequestIds.push(newReqId);

        FileManager::saveRequests(requestsFilePath, requests);
        std::cout << "\n[✓] Connection request #" << newReqId << " sent to Student #" << receiverId << "!\n";
    }
}

// 5. Send Connection Request
void CampusConnectApp::sendConnectionRequest() {
    printHeader("SEND CONNECTION REQUEST");

    int senderId = readInt("Enter Sender Student ID: ", 1, 999999);
    const Student* sender = findStudent(senderId);
    if (!sender) {
        std::cout << "[!] Error: Sender Student ID #" << senderId << " not found.\n";
        return;
    }

    int receiverId = readInt("Enter Receiver Student ID: ", 1, 999999);
    const Student* receiver = findStudent(receiverId);
    if (!receiver) {
        std::cout << "[!] Error: Receiver Student ID #" << receiverId << " not found.\n";
        return;
    }

    if (senderId == receiverId) {
        std::cout << "[!] Validation Error: Students cannot send connection requests to themselves.\n";
        return;
    }

    for (const auto& req : requests) {
        if (req.getSenderId() == senderId && req.getReceiverId() == receiverId &&
            req.getStatus() == RequestStatus::PENDING) {
            std::cout << "[!] Duplicate Request: A pending request already exists between these students.\n";
            return;
        }
    }

    auto it = connectionsMap.find(senderId);
    if (it != connectionsMap.end()) {
        if (std::find(it->second.begin(), it->second.end(), receiverId) != it->second.end()) {
            std::cout << "[!] Notice: Students are already actively connected!\n";
            return;
        }
    }

    int newReqId = getNextRequestId();
    ConnectionRequest newReq(newReqId, senderId, receiverId, RequestStatus::PENDING);

    requests.push_back(newReq);
    pendingRequestIds.push(newReqId);

    FileManager::saveRequests(requestsFilePath, requests);

    std::cout << "\n[✓] Connection request #" << newReqId << " created and added to FIFO queue!\n";
    newReq.display();
}

// 6. View Pending Requests
void CampusConnectApp::viewPendingRequests() const {
    printHeader("PENDING CONNECTION REQUESTS (queue<int> FIFO pool)");

    if (pendingRequestIds.empty()) {
        std::cout << "The pending request queue is currently EMPTY.\n";
        return;
    }

    std::cout << "Queue Size: " << pendingRequestIds.size() << " request(s) waiting in FIFO order.\n\n";

    std::queue<int> tempQueue = pendingRequestIds;
    int order = 1;

    while (!tempQueue.empty()) {
        int reqId = tempQueue.front();
        tempQueue.pop();

        for (const auto& req : requests) {
            if (req.getRequestId() == reqId) {
                std::cout << "  FIFO Position #" << order++ << ": ";
                req.display();

                const Student* s = findStudent(req.getSenderId());
                const Student* r = findStudent(req.getReceiverId());
                if (s && r) {
                    std::cout << "    [Sender: " << s->getName() << " -> Receiver: " << r->getName() << "]\n";
                }
                break;
            }
        }
    }
}

// 7. Process Next Pending Request
void CampusConnectApp::processNextPendingRequest() {
    printHeader("PROCESS NEXT PENDING REQUEST (FIFO queue::front & pop)");

    if (pendingRequestIds.empty()) {
        std::cout << "[!] The pending request queue is currently EMPTY.\n";
        return;
    }

    int nextReqId = pendingRequestIds.front();

    ConnectionRequest* targetReq = nullptr;
    for (auto& req : requests) {
        if (req.getRequestId() == nextReqId) {
            targetReq = &req;
            break;
        }
    }

    if (!targetReq || targetReq->getStatus() != RequestStatus::PENDING) {
        pendingRequestIds.pop();
        std::cout << "[!] Request was already resolved. Removed from queue.\n";
        return;
    }

    const Student* sender = findStudent(targetReq->getSenderId());
    const Student* receiver = findStudent(targetReq->getReceiverId());

    std::cout << "Processing Request #" << targetReq->getRequestId() << ":\n";
    targetReq->display();

    if (sender && receiver) {
        std::cout << "\n  Sender   : " << sender->getName() << " (" << sender->getDepartment() << ", Year " << sender->getAcademicYear() << ")\n";
        std::cout << "  Receiver : " << receiver->getName() << " (" << receiver->getDepartment() << ", Year " << receiver->getAcademicYear() << ")\n";

        auto toLearn = receiver->getMatchedSkillsToLearn(*sender);
        auto toTeach = receiver->getMatchedSkillsToTeach(*sender);

        if (!toLearn.empty()) {
            std::cout << "  Receiver Learns  : ";
            for (size_t i = 0; i < toLearn.size(); ++i) {
                std::cout << toLearn[i] << (i + 1 < toLearn.size() ? ", " : "");
            }
            std::cout << "\n";
        }
        if (!toTeach.empty()) {
            std::cout << "  Receiver Teaches : ";
            for (size_t i = 0; i < toTeach.size(); ++i) {
                std::cout << toTeach[i] << (i + 1 < toTeach.size() ? ", " : "");
            }
            std::cout << "\n";
        }
    }

    std::cout << "\nDecision for Receiver:\n";
    std::cout << "  [1] Accept Request (Add to Active Connections list)\n";
    std::cout << "  [2] Reject Request\n";
    std::cout << "  [3] Postpone (Keep in queue)\n";
    int decision = readInt("Enter decision (1-3): ", 1, 3);

    if (decision == 1) {
        pendingRequestIds.pop();
        targetReq->updateStatus(RequestStatus::ACCEPTED);

        int sId = targetReq->getSenderId();
        int rId = targetReq->getReceiverId();

        auto& sList = connectionsMap[sId];
        if (std::find(sList.begin(), sList.end(), rId) == sList.end()) {
            sList.push_back(rId);
        }

        auto& rList = connectionsMap[rId];
        if (std::find(rList.begin(), rList.end(), sId) == rList.end()) {
            rList.push_back(sId);
        }

        FileManager::saveRequests(requestsFilePath, requests);
        std::cout << "\n[✓] Request #" << nextReqId << " ACCEPTED!\n";
        std::cout << "  Active connection established between Student #" << sId << " and #" << rId << ".\n";
    } else if (decision == 2) {
        pendingRequestIds.pop();
        targetReq->updateStatus(RequestStatus::REJECTED);
        FileManager::saveRequests(requestsFilePath, requests);
        std::cout << "\n[✓] Request #" << nextReqId << " REJECTED.\n";
    } else {
        std::cout << "\n[-] Request postponed in queue.\n";
    }
}

// 8. Active Connections
void CampusConnectApp::viewActiveConnections() {
    printHeader("ACTIVE CONNECTIONS (std::list<int> per student)");

    int studentId = readInt("Enter Student ID to view their active connections: ", 1, 999999);
    const Student* student = findStudent(studentId);

    if (!student) {
        std::cout << "[!] Student ID #" << studentId << " not found.\n";
        return;
    }

    std::cout << "\nActive connections for " << student->getName() << " (#" << student->getId() << "):\n";

    auto it = connectionsMap.find(studentId);
    if (it == connectionsMap.end() || it->second.empty()) {
        std::cout << "No active connections established yet.\n";
        return;
    }

    const std::list<int>& connList = it->second;
    std::cout << "Total Active Peers: " << connList.size() << "\n";
    printDivider();

    int index = 1;
    for (auto listIt = connList.begin(); listIt != connList.end(); ++listIt) {
        int peerId = *listIt;
        const Student* peer = findStudent(peerId);

        std::cout << "  [" << index++ << "] Connected Student #" << peerId;
        if (peer) {
            std::cout << " : " << peer->getName() << " (" << peer->getDepartment()
                      << ", Year " << peer->getAcademicYear() << ")\n";
        } else {
            std::cout << "\n";
        }
    }
    printDivider();

    std::cout << "\nOptions:\n";
    std::cout << "  [1] Disconnect an active peer (Demonstrates list::remove)\n";
    std::cout << "  [0] Return to main menu\n";
    int choice = readInt("Enter choice: ", 0, 1);

    if (choice == 1) {
        int peerToDisconnect = readInt("Enter Peer Student ID to disconnect: ", 1, 999999);

        auto& currentList = connectionsMap[studentId];
        auto found = std::find(currentList.begin(), currentList.end(), peerToDisconnect);

        if (found == currentList.end()) {
            std::cout << "[!] Student #" << peerToDisconnect << " is not in the active connections list.\n";
            return;
        }

        currentList.remove(peerToDisconnect);
        connectionsMap[peerToDisconnect].remove(studentId);

        for (auto& req : requests) {
            if ((req.getSenderId() == studentId && req.getReceiverId() == peerToDisconnect) ||
                (req.getSenderId() == peerToDisconnect && req.getReceiverId() == studentId)) {
                if (req.getStatus() == RequestStatus::ACCEPTED) {
                    req.updateStatus(RequestStatus::REJECTED);
                }
            }
        }

        FileManager::saveRequests(requestsFilePath, requests);
        std::cout << "\n[✓] Connection disconnected between Student #" << studentId
                  << " and #" << peerToDisconnect << ".\n";
    }
}

// 9. Save Data
void CampusConnectApp::saveData() {
    printHeader("PERSIST DATA TO DISK");

    bool sOk = FileManager::saveStudents(studentsFilePath, students);
    bool rOk = FileManager::saveRequests(requestsFilePath, requests);

    if (sOk && rOk) {
        std::cout << "[✓] Data successfully saved to:\n";
        std::cout << "    - " << studentsFilePath << " (" << students.size() << " student records)\n";
        std::cout << "    - " << requestsFilePath << " (" << requests.size() << " requests)\n";
    } else {
        std::cout << "[!] Error writing data files.\n";
    }
}

// Main Interactive Terminal Loop
void CampusConnectApp::run() {
    init();

    while (true) {
        printBanner();
        std::cout << " MAIN MENU\n";
        printDivider();
        std::cout << "  [1] View All Student Profiles        (vector iteration)\n";
        std::cout << "  [2] View / Inspect Student Profile   (O(1) unordered_map & Polymorphism)\n";
        std::cout << "  [3] Create New Student Profile       (Encapsulation & Validation)\n";
        std::cout << "  [4] Find Skill Matches               (priority_queue Ranked Compatibility)\n";
        std::cout << "  [5] Send Connection Request          (queue enqueue)\n";
        std::cout << "  [6] View Pending Requests            (queue FIFO pool inspection)\n";
        std::cout << "  [7] Process Next Pending Request     (queue FIFO front/pop & list update)\n";
        std::cout << "  [8] View Active Connections          (list<int> management & disconnect)\n";
        std::cout << "  [9] Save Data to Disk                (ifstream / ofstream persistence)\n";
        std::cout << "  [0] Exit Application\n";
        printDivider();

        int choice = readInt("Select an option (0-9): ", 0, 9);

        switch (choice) {
            case 1:
                viewAllProfiles();
                pauseForUser();
                break;
            case 2:
                viewProfileById();
                pauseForUser();
                break;
            case 3:
                createProfile();
                pauseForUser();
                break;
            case 4:
                findSkillMatches();
                pauseForUser();
                break;
            case 5:
                sendConnectionRequest();
                pauseForUser();
                break;
            case 6:
                viewPendingRequests();
                pauseForUser();
                break;
            case 7:
                processNextPendingRequest();
                pauseForUser();
                break;
            case 8:
                viewActiveConnections();
                pauseForUser();
                break;
            case 9:
                saveData();
                pauseForUser();
                break;
            case 0:
                printHeader("THANK YOU FOR USING CAMPUSCONNECT");
                std::cout << "Saving changes before exit...\n";
                saveData();
                std::cout << "Goodbye!\n\n";
                return;
            default:
                std::cout << "[!] Unknown option.\n";
                pauseForUser();
                break;
        }
    }
}

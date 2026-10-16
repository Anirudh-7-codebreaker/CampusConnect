#include "skill_swap_app.h"
#include "file_manager.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <cmath>
#include <algorithm>

// Standalone function demonstrating Runtime Polymorphism via base class reference
static void renderUserProfilePolymorphic(const User& user) {
    // Dynamic dispatch resolves to Student::displayProfile() at runtime
    user.displayProfile();
}

SkillSwapApp::SkillSwapApp(const std::string& studentFile, const std::string& requestFile)
    : studentsFilePath(studentFile), requestsFilePath(requestFile), currentSessionStudentId(101) {}

SkillSwapApp::~SkillSwapApp() {
    // Clean-up handled by STL RAII
}

void SkillSwapApp::init() {
    FileManager::ensureDirectoryExists("data");
    FileManager::seedSampleDataIfEmpty(studentsFilePath, requestsFilePath);

    reloadData();
}

void SkillSwapApp::reloadData() {
    students.clear();
    requests.clear();

    FileManager::loadStudents(studentsFilePath, students);
    FileManager::loadRequests(requestsFilePath, requests);

    rebuildStudentIndex();
    rebuildPendingQueue();
    rebuildActiveConnections();
}

void SkillSwapApp::rebuildStudentIndex() {
    studentIndex.clear();
    for (size_t i = 0; i < students.size(); ++i) {
        studentIndex[students[i].getId()] = i;
    }
}

void SkillSwapApp::rebuildPendingQueue() {
    // Clear queue (re-instantiate)
    std::queue<int> emptyQueue;
    std::swap(pendingRequestIds, emptyQueue);

    // Reconstruct FIFO queue only from PENDING requests
    for (const auto& req : requests) {
        if (req.getStatus() == RequestStatus::PENDING) {
            pendingRequestIds.push(req.getRequestId());
        }
    }
}

void SkillSwapApp::rebuildActiveConnections() {
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

int SkillSwapApp::getNextRequestId() const {
    int maxId = 0;
    for (const auto& r : requests) {
        if (r.getRequestId() > maxId) maxId = r.getRequestId();
    }
    return maxId + 1;
}

int SkillSwapApp::getNextStudentId() const {
    int maxId = 100;
    for (const auto& s : students) {
        if (s.getId() > maxId) maxId = s.getId();
    }
    return maxId + 1;
}

// Function Overload 1: findStudent by ID (non-const) using unordered_map index
Student* SkillSwapApp::findStudent(int id) {
    auto it = studentIndex.find(id);
    if (it != studentIndex.end() && it->second < students.size()) {
        return &students[it->second];
    }
    return nullptr;
}

// Function Overload 2: findStudent by ID (const) using unordered_map index
const Student* SkillSwapApp::findStudent(int id) const {
    auto it = studentIndex.find(id);
    if (it != studentIndex.end() && it->second < students.size()) {
        return &students[it->second];
    }
    return nullptr;
}

// Function Overload 3: findStudent by Name (const) using normalized string search
const Student* SkillSwapApp::findStudent(const std::string& name) const {
    std::string normTarget = Skill::normalize(name);
    for (const auto& s : students) {
        std::string normName = Skill::normalize(s.getName());
        if (normName == normTarget || normName.find(normTarget) != std::string::npos) {
            return &s;
        }
    }
    return nullptr;
}

// UI Formatting Helpers
void SkillSwapApp::printBanner() const {
    std::cout << "\n";
    std::cout << "========================================================================\n";
    std::cout << "        ____  _     _ _ _ ____                     \n";
    std::cout << "       / ___|| |__ (_) | / ___|_      ____ _ _ __  \n";
    std::cout << "       \\___ \\| |/ /| | | \\___ \\ \\ /\\ / / _` | '_ \\ \n";
    std::cout << "        ___) |   < | | | |___) \\ V  V / (_| | |_) |\n";
    std::cout << "       |____/|_|\\_\\|_|_|_|____/ \\_/\\_/ \\__,_| .__/ \n";
    std::cout << "                                            |_|    \n";
    std::cout << "           Peer-to-Peer University Skill Exchange  \n";
    std::cout << "                   Terminal Prototype v1.0         \n";
    std::cout << "========================================================================\n";
}

void SkillSwapApp::printHeader(const std::string& title) const {
    std::cout << "\n+----------------------------------------------------------------------+\n";
    std::cout << "| " << std::left << std::setw(68) << title << " |\n";
    std::cout << "+----------------------------------------------------------------------+\n";
}

void SkillSwapApp::printDivider() const {
    std::cout << "------------------------------------------------------------------------\n";
}

void SkillSwapApp::pauseForUser() const {
    std::cout << "\n[Press Enter to continue...]";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int SkillSwapApp::readInt(const std::string& prompt, int minVal, int maxVal) const {
    int val = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            if (val >= minVal && val <= maxVal) {
                // Clear any leftover characters up to newline
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return val;
            } else {
                std::cout << "  [!] Please enter a value between " << minVal << " and " << maxVal << ".\n";
            }
        } else {
            std::cout << "  [!] Invalid numeric input. Please try again.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::string SkillSwapApp::readLine(const std::string& prompt) const {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    // Trim leading and trailing spaces
    size_t first = line.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = line.find_last_not_of(" \t\r\n");
    return line.substr(first, (last - first + 1));
}

// Feature 1: View Profiles
void SkillSwapApp::viewAllProfiles() const {
    printHeader("REGISTERED STUDENT PROFILES (vector<Student> iteration)");
    std::cout << "Total Profiles Loaded: " << students.size() << "\n\n";

    if (students.empty()) {
        std::cout << "No student profiles found. Choose option 2 to create one.\n";
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

    std::cout << "\nTip: Select option 2 in main menu to inspect full details for any student.\n";
}

void SkillSwapApp::viewProfileById() {
    printHeader("INSPECT INDIVIDUAL STUDENT PROFILE");
    std::cout << "Lookup methods available:\n";
    std::cout << "  [1] Lookup by Student ID (O(1) unordered_map search)\n";
    std::cout << "  [2] Lookup by Student Name (Normalized substring search)\n";
    int choice = readInt("Enter choice (1-2): ", 1, 2);

    const Student* student = nullptr;
    if (choice == 1) {
        int id = readInt("Enter Student ID: ", 1, 999999);
        student = findStudent(id); // Calls findStudent(int)
    } else {
        std::string query = readLine("Enter Student Name: ");
        student = findStudent(query); // Calls findStudent(const string&) - Function Overloading
    }

    if (!student) {
        std::cout << "\n[!] Error: Student not found in directory.\n";
        return;
    }

    std::cout << "\n[Demonstrating Polymorphism via base class User reference:]\n";
    renderUserProfilePolymorphic(*student);

    // Also display connection stats
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

// Feature 2: Create Profile
void SkillSwapApp::createProfile() {
    printHeader("CREATE NEW STUDENT PROFILE");

    int newId = getNextStudentId();
    std::cout << "Assigned Student ID: " << newId << "\n";

    std::string name;
    while (name.empty()) {
        name = readLine("Enter Full Name: ");
        if (name.empty()) std::cout << "  [!] Name cannot be blank.\n";
    }

    std::string dept = readLine("Enter Department (e.g. Computer Science, IT, Design): ");
    if (dept.empty()) dept = "General Engineering";

    int year = readInt("Enter Academic Year (1-6): ", 1, 6);

    Student newStudent(newId, name, dept, year);

    // Add teachable skills
    std::cout << "\n--- Teachable Skills (Skills you can share/teach) ---\n";
    std::cout << "Enter skills one by one. Press Enter with an empty name when finished.\n";
    while (true) {
        std::string skillName = readLine("Skill Name (or empty to finish): ");
        if (skillName.empty()) break;
        std::string category = readLine("Skill Category (e.g., Programming, Web, Design) [default: General]: ");
        if (category.empty()) category = "General";

        // Demonstrating overloaded method addTeachableSkill(name, category)
        if (newStudent.addTeachableSkill(skillName, category)) {
            std::cout << "  [+] Added teachable skill: " << skillName << "\n";
        } else {
            std::cout << "  [!] Skill already in your list.\n";
        }
    }

    // Add desired skills
    std::cout << "\n--- Desired Skills (Skills you want to learn) ---\n";
    std::cout << "Enter skills one by one. Press Enter with an empty name when finished.\n";
    while (true) {
        std::string skillName = readLine("Skill Name (or empty to finish): ");
        if (skillName.empty()) break;
        std::string category = readLine("Skill Category [default: General]: ");
        if (category.empty()) category = "General";

        // Demonstrating overloaded method addDesiredSkill(name, category)
        if (newStudent.addDesiredSkill(skillName, category)) {
            std::cout << "  [+] Added desired skill: " << skillName << "\n";
        } else {
            std::cout << "  [!] Skill already in your list.\n";
        }
    }

    // Add interests
    std::cout << "\n--- General Interests / Hobbies ---\n";
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

    // Insert into vector
    students.push_back(newStudent);

    // Update ID index
    studentIndex[newId] = students.size() - 1;

    // Persist immediately
    FileManager::saveStudents(studentsFilePath, students);

    std::cout << "\n[✓] Student profile successfully created and saved to disk!\n";
    renderUserProfilePolymorphic(newStudent);
}

// Feature 3: Priority Queue Skill Matching Engine
std::priority_queue<Match, std::vector<Match>, MatchComparator>
SkillSwapApp::calculateMatches(int studentId) const {
    std::priority_queue<Match, std::vector<Match>, MatchComparator> pq;

    const Student* current = findStudent(studentId);
    if (!current) return pq;

    for (const auto& candidate : students) {
        // Exclude current student from self-match
        if (candidate.getId() == studentId) continue;

        // Skills candidate can teach that current student wants to learn
        std::vector<Skill> toLearn = current->getMatchedSkillsToLearn(candidate);
        // Skills current student can teach that candidate wants to learn
        std::vector<Skill> toTeach = current->getMatchedSkillsToTeach(candidate);

        // Exclude candidates with zero skill overlap
        if (toLearn.empty() && toTeach.empty()) {
            continue;
        }

        // Calculate Compatibility Score according to documented formula:
        // - +20 points per desired skill candidate can teach
        // - +20 points per teachable skill candidate wants to learn
        // - +25 bonus points for mutual skill exchange (two-way win)
        // - +5 bonus points if students are in the same department
        // - +5 bonus points if academic years are adjacent (peer proximity)
        int score = 0;
        score += static_cast<int>(toLearn.size()) * 20;
        score += static_cast<int>(toTeach.size()) * 20;

        bool isMutual = (!toLearn.empty() && !toTeach.empty());
        if (isMutual) {
            score += 25; // High value on mutual reciprocity
        }

        if (current->getDepartment() == candidate.getDepartment()) {
            score += 5;
        }

        if (std::abs(current->getAcademicYear() - candidate.getAcademicYear()) <= 1) {
            score += 5;
        }

        if (score > 100) score = 100; // Cap at 100%

        // Build human-readable explanation
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

void SkillSwapApp::findSkillMatches() {
    printHeader("SKILL MATCHING ENGINE (priority_queue Ranked Compatibility)");

    int studentId = readInt("Enter your Student ID to calculate matches: ", 1, 999999);
    const Student* current = findStudent(studentId);

    if (!current) {
        std::cout << "[!] Student ID #" << studentId << " not found.\n";
        return;
    }

    std::cout << "\nCalculating compatibility for: " << current->getName()
              << " (" << current->getDepartment() << ", Year " << current->getAcademicYear() << ")...\n";

    auto rankedMatches = calculateMatches(studentId);

    if (rankedMatches.empty()) {
        std::cout << "\nNo compatible matches found for your current skills.\n";
        std::cout << "Try adding more teachable or desired skills to your profile!\n";
        return;
    }

    std::cout << "\nFound " << rankedMatches.size()
              << " compatible candidates (Ranked highest compatibility first by max-heap):\n\n";

    // Keep ranked list in a vector for immediate connection request dispatch
    std::vector<Match> displayList;

    int rank = 1;
    while (!rankedMatches.empty()) {
        Match topMatch = rankedMatches.top();
        rankedMatches.pop(); // Demonstrating priority_queue::top() and pop()

        std::cout << "RANK #" << rank++ << "\n";
        topMatch.display();
        std::cout << "\n";

        displayList.push_back(topMatch);
    }

    // Interactive action: allow sending request directly to a recommended match
    std::cout << "Would you like to send a connection request to one of these candidates?\n";
    std::cout << "  [1] Yes, send request\n";
    std::cout << "  [0] Return to main menu\n";
    int choice = readInt("Enter choice: ", 0, 1);

    if (choice == 1) {
        int receiverId = readInt("Enter Candidate ID to connect with: ", 1, 999999);
        // Verify valid candidate
        bool isValidCandidate = false;
        for (const auto& m : displayList) {
            if (m.candidateId == receiverId) {
                isValidCandidate = true;
                break;
            }
        }

        if (!isValidCandidate) {
            std::cout << "[!] That ID was not among the recommended matches.\n";
            return;
        }

        // Check if duplicate pending or already connected
        for (const auto& req : requests) {
            if (req.getSenderId() == studentId && req.getReceiverId() == receiverId &&
                req.getStatus() == RequestStatus::PENDING) {
                std::cout << "[!] A pending connection request already exists between you and student #" << receiverId << ".\n";
                return;
            }
        }

        auto it = connectionsMap.find(studentId);
        if (it != connectionsMap.end()) {
            if (std::find(it->second.begin(), it->second.end(), receiverId) != it->second.end()) {
                std::cout << "[!] You are already actively connected with student #" << receiverId << "!\n";
                return;
            }
        }

        int newReqId = getNextRequestId();
        ConnectionRequest newReq(newReqId, studentId, receiverId, RequestStatus::PENDING);
        requests.push_back(newReq);
        pendingRequestIds.push(newReqId); // queue::push()

        FileManager::saveRequests(requestsFilePath, requests);
        std::cout << "\n[✓] Connection request #" << newReqId << " sent to Student #" << receiverId << " and queued for processing!\n";
    }
}

// Feature 4: Send Connection Request
void SkillSwapApp::sendConnectionRequest() {
    printHeader("SEND CONNECTION REQUEST");

    int senderId = readInt("Enter Sender Student ID: ", 1, 999999);
    const Student* sender = findStudent(senderId);
    if (!sender) {
        std::cout << "[!] Error: Sender Student ID #" << senderId << " does not exist.\n";
        return;
    }

    int receiverId = readInt("Enter Receiver Student ID: ", 1, 999999);
    const Student* receiver = findStudent(receiverId);
    if (!receiver) {
        std::cout << "[!] Error: Receiver Student ID #" << receiverId << " does not exist.\n";
        return;
    }

    // Business Logic Validation 1: Prevent self-requests
    if (senderId == receiverId) {
        std::cout << "[!] Validation Error: Students cannot send connection requests to themselves.\n";
        return;
    }

    // Business Logic Validation 2: Prevent duplicate pending requests
    for (const auto& req : requests) {
        if (req.getSenderId() == senderId && req.getReceiverId() == receiverId &&
            req.getStatus() == RequestStatus::PENDING) {
            std::cout << "[!] Duplicate Request: There is already a PENDING request (#"
                      << req.getRequestId() << ") from Student #" << senderId
                      << " to Student #" << receiverId << ".\n";
            return;
        }
    }

    // Business Logic Validation 3: Check if already active connections
    auto it = connectionsMap.find(senderId);
    if (it != connectionsMap.end()) {
        if (std::find(it->second.begin(), it->second.end(), receiverId) != it->second.end()) {
            std::cout << "[!] Notice: Students #" << senderId << " and #" << receiverId
                      << " are already actively connected!\n";
            return;
        }
    }

    int newReqId = getNextRequestId();
    ConnectionRequest newReq(newReqId, senderId, receiverId, RequestStatus::PENDING);

    requests.push_back(newReq);
    pendingRequestIds.push(newReqId); // Queue FIFO demonstration: push()

    FileManager::saveRequests(requestsFilePath, requests);

    std::cout << "\n[✓] Connection request successfully created!\n";
    newReq.display();
    std::cout << "  Enqueued Request ID #" << newReqId << " for FIFO processing.\n";
    std::cout << "  Pending Queue Size: " << pendingRequestIds.size() << "\n";
}

// Feature 5: View Pending Requests
void SkillSwapApp::viewPendingRequests() const {
    printHeader("PENDING CONNECTION REQUESTS (queue<int> processing pool)");

    if (pendingRequestIds.empty()) {
        std::cout << "The pending request queue is currently EMPTY.\n";
        std::cout << "Send connection requests via option 4 to populate the queue.\n";
        return;
    }

    std::cout << "Current Queue Size: " << pendingRequestIds.size()
              << " request(s) waiting in FIFO order.\n\n";

    // Inspect copy of the queue without mutating application state
    std::queue<int> tempQueue = pendingRequestIds;
    int order = 1;

    while (!tempQueue.empty()) {
        int reqId = tempQueue.front(); // Demonstrating queue::front()
        tempQueue.pop();               // Demonstrating queue::pop()

        // Locate request in vector
        for (const auto& req : requests) {
            if (req.getRequestId() == reqId) {
                std::cout << "  FIFO Position #" << order++ << ": ";
                req.display();

                const Student* s = findStudent(req.getSenderId());
                const Student* r = findStudent(req.getReceiverId());
                if (s && r) {
                    std::cout << "    [Sender: " << s->getName() << " (" << s->getDepartment() << ") -> "
                              << "Receiver: " << r->getName() << " (" << r->getDepartment() << ")]\n";
                }
                break;
            }
        }
    }
}

// Feature 6: Process Next Pending Request
void SkillSwapApp::processNextPendingRequest() {
    printHeader("PROCESS NEXT PENDING REQUEST (FIFO queue::front & pop)");

    if (pendingRequestIds.empty()) { // Demonstrating queue::empty()
        std::cout << "[!] The pending request queue is currently EMPTY.\n";
        std::cout << "No requests waiting to be processed.\n";
        return;
    }

    int nextReqId = pendingRequestIds.front(); // queue::front()

    // Find the request record
    ConnectionRequest* targetReq = nullptr;
    for (auto& req : requests) {
        if (req.getRequestId() == nextReqId) {
            targetReq = &req;
            break;
        }
    }

    if (!targetReq || targetReq->getStatus() != RequestStatus::PENDING) {
        // If stale ID in queue, pop and retry
        pendingRequestIds.pop();
        std::cout << "[!] Request record was no longer pending. Popped from queue.\n";
        return;
    }

    const Student* sender = findStudent(targetReq->getSenderId());
    const Student* receiver = findStudent(targetReq->getReceiverId());

    std::cout << "Now processing Request #" << targetReq->getRequestId() << " (Next in FIFO queue):\n";
    targetReq->display();

    if (sender && receiver) {
        std::cout << "\n  Sender Details   : " << sender->getName() << " (" << sender->getDepartment()
                  << ", Year " << sender->getAcademicYear() << ")\n";
        std::cout << "  Receiver Details : " << receiver->getName() << " (" << receiver->getDepartment()
                  << ", Year " << receiver->getAcademicYear() << ")\n";

        // Display shared skill synergy
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
    std::cout << "  [2] Reject Request (Mark as Rejected)\n";
    std::cout << "  [3] Postpone (Keep in Queue for later)\n";
    int decision = readInt("Enter decision (1-3): ", 1, 3);

    if (decision == 1) {
        pendingRequestIds.pop(); // queue::pop()
        targetReq->updateStatus(RequestStatus::ACCEPTED);

        // Update active connections in list<int>
        int sId = targetReq->getSenderId();
        int rId = targetReq->getReceiverId();

        auto& sList = connectionsMap[sId];
        if (std::find(sList.begin(), sList.end(), rId) == sList.end()) {
            sList.push_back(rId); // list::push_back()
        }

        auto& rList = connectionsMap[rId];
        if (std::find(rList.begin(), rList.end(), sId) == rList.end()) {
            rList.push_back(sId); // list::push_back()
        }

        FileManager::saveRequests(requestsFilePath, requests);
        std::cout << "\n[✓] Request #" << nextReqId << " ACCEPTED!\n";
        std::cout << "  Active connection established between Student #" << sId
                  << " and Student #" << rId << ".\n";
    } else if (decision == 2) {
        pendingRequestIds.pop(); // queue::pop()
        targetReq->updateStatus(RequestStatus::REJECTED);

        FileManager::saveRequests(requestsFilePath, requests);
        std::cout << "\n[✓] Request #" << nextReqId << " REJECTED.\n";
    } else {
        std::cout << "\n[-] Request postponed. Retained in queue.\n";
    }

    std::cout << "Remaining in pending queue: " << pendingRequestIds.size() << "\n";
}

// Feature 7: View Active Connections (list<int>)
void SkillSwapApp::viewActiveConnections() {
    printHeader("ACTIVE CONNECTIONS (list<int> per student)");

    int studentId = readInt("Enter Student ID to view their active connections: ", 1, 999999);
    const Student* student = findStudent(studentId);

    if (!student) {
        std::cout << "[!] Student ID #" << studentId << " does not exist.\n";
        return;
    }

    std::cout << "\nActive connections for: " << student->getName()
              << " (#" << student->getId() << ")\n";

    auto it = connectionsMap.find(studentId);
    if (it == connectionsMap.end() || it->second.empty()) {
        std::cout << "No active skill exchange connections currently established.\n";
        return;
    }

    const std::list<int>& connList = it->second;
    std::cout << "Total Active Connections: " << connList.size() << "\n";
    printDivider();

    // Iterate through std::list<int>
    int index = 1;
    for (auto listIt = connList.begin(); listIt != connList.end(); ++listIt) {
        int peerId = *listIt;
        const Student* peer = findStudent(peerId);

        std::cout << "  [" << index++ << "] Connected Student #" << peerId;
        if (peer) {
            std::cout << " : " << peer->getName() << " (" << peer->getDepartment()
                      << ", Year " << peer->getAcademicYear() << ")\n";

            // Show skills being exchanged
            auto learnFromPeer = student->getMatchedSkillsToLearn(*peer);
            auto teachToPeer = student->getMatchedSkillsToTeach(*peer);

            if (!learnFromPeer.empty()) {
                std::cout << "      Learning from peer: ";
                for (size_t i = 0; i < learnFromPeer.size(); ++i) {
                    std::cout << learnFromPeer[i] << (i + 1 < learnFromPeer.size() ? ", " : "");
                }
                std::cout << "\n";
            }
            if (!teachToPeer.empty()) {
                std::cout << "      Teaching to peer  : ";
                for (size_t i = 0; i < teachToPeer.size(); ++i) {
                    std::cout << teachToPeer[i] << (i + 1 < teachToPeer.size() ? ", " : "");
                }
                std::cout << "\n";
            }
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

        // Check if actually in connection list
        auto& currentList = connectionsMap[studentId];
        auto found = std::find(currentList.begin(), currentList.end(), peerToDisconnect);

        if (found == currentList.end()) {
            std::cout << "[!] Student #" << peerToDisconnect << " is not in the active connections list.\n";
            return;
        }

        // Demonstrate std::list::remove()
        currentList.remove(peerToDisconnect);

        // Remove symmetrical connection from peer's list
        connectionsMap[peerToDisconnect].remove(studentId);

        // Update corresponding request in requests history if exists
        for (auto& req : requests) {
            if ((req.getSenderId() == studentId && req.getReceiverId() == peerToDisconnect) ||
                (req.getSenderId() == peerToDisconnect && req.getReceiverId() == studentId)) {
                if (req.getStatus() == RequestStatus::ACCEPTED) {
                    req.updateStatus(RequestStatus::REJECTED); // Mark closed
                }
            }
        }

        FileManager::saveRequests(requestsFilePath, requests);
        std::cout << "\n[✓] Connection between Student #" << studentId
                  << " and Student #" << peerToDisconnect << " has been removed from active list.\n";
    }
}

// Feature 8: Save Data
void SkillSwapApp::saveData() {
    printHeader("PERSIST DATA TO DISK");

    bool sOk = FileManager::saveStudents(studentsFilePath, students);
    bool rOk = FileManager::saveRequests(requestsFilePath, requests);

    if (sOk && rOk) {
        std::cout << "[✓] Data successfully saved to:\n";
        std::cout << "    - " << studentsFilePath << " (" << students.size() << " student records)\n";
        std::cout << "    - " << requestsFilePath << " (" << requests.size() << " connection requests)\n";
    } else {
        std::cout << "[!] Error occurred while writing data files.\n";
    }
}

// Main Interactive Loop
void SkillSwapApp::run() {
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
                printHeader("THANK YOU FOR USING SKILLSWAP");
                std::cout << "Saving changes before exit...\n";
                saveData();
                std::cout << "Exiting SkillSwap. Happy Skill Exchanging!\n\n";
                return;
            default:
                std::cout << "[!] Unknown menu option.\n";
                pauseForUser();
                break;
        }
    }
}

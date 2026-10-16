#include "file_manager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#define MKDIR(dir) mkdir(dir, 0755)
#endif

bool FileManager::ensureDirectoryExists(const std::string& dirPath) {
    struct stat info;
    if (stat(dirPath.c_str(), &info) != 0) {
        return (MKDIR(dirPath.c_str()) == 0);
    } else if (info.st_mode & S_IFDIR) {
        return true;
    }
    return false;
}

static std::vector<Skill> parseSkillString(const std::string& str) {
    std::vector<Skill> skills;
    if (str.empty()) return skills;

    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, ';')) {
        if (token.empty()) continue;
        size_t colonPos = token.find(':');
        if (colonPos != std::string::npos) {
            std::string sName = token.substr(0, colonPos);
            std::string sCat = token.substr(colonPos + 1);
            if (!sName.empty()) {
                skills.emplace_back(sName, sCat);
            }
        } else {
            skills.emplace_back(token, "General");
        }
    }
    return skills;
}

static std::string serializeSkillList(const std::vector<Skill>& skills) {
    std::string result;
    for (size_t i = 0; i < skills.size(); ++i) {
        result += skills[i].getName() + ":" + skills[i].getCategory();
        if (i + 1 < skills.size()) {
            result += ";";
        }
    }
    return result;
}

static std::vector<std::string> parseInterestsString(const std::string& str) {
    std::vector<std::string> interests;
    if (str.empty()) return interests;

    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (!item.empty()) {
            interests.push_back(item);
        }
    }
    return interests;
}

static std::string serializeInterests(const std::vector<std::string>& interests) {
    std::string result;
    for (size_t i = 0; i < interests.size(); ++i) {
        result += interests[i];
        if (i + 1 < interests.size()) {
            result += ",";
        }
    }
    return result;
}

bool FileManager::loadStudents(const std::string& filePath, std::vector<Student>& students) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        std::cerr << "[FileManager] Notice: File " << filePath << " not found (will be created).\n";
        return false;
    }

    students.clear();
    std::string line;
    int lineNum = 0;

    while (std::getline(inFile, line)) {
        lineNum++;
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string idStr, name, dept, yearStr, teachStr, wantStr, interestStr;

        if (std::getline(ss, idStr, '|') &&
            std::getline(ss, name, '|') &&
            std::getline(ss, dept, '|') &&
            std::getline(ss, yearStr, '|') &&
            std::getline(ss, teachStr, '|') &&
            std::getline(ss, wantStr, '|')) {

            std::getline(ss, interestStr, '|');

            try {
                int id = std::stoi(idStr);
                int year = std::stoi(yearStr);

                if (id <= 0 || year < 1 || year > 6) {
                    continue;
                }

                Student student(id, name, dept, year);

                std::vector<Skill> teachSkills = parseSkillString(teachStr);
                for (const auto& s : teachSkills) {
                    student.addTeachableSkill(s);
                }

                std::vector<Skill> wantSkills = parseSkillString(wantStr);
                for (const auto& s : wantSkills) {
                    student.addDesiredSkill(s);
                }

                std::vector<std::string> interests = parseInterestsString(interestStr);
                for (const auto& intr : interests) {
                    student.addInterest(intr);
                }

                students.push_back(student);
            } catch (...) {
                continue;
            }
        }
    }

    inFile.close();
    return true;
}

bool FileManager::saveStudents(const std::string& filePath, const std::vector<Student>& students) {
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        ensureDirectoryExists(filePath.substr(0, lastSlash));
    }

    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "[FileManager] Error: Could not write to " << filePath << "\n";
        return false;
    }

    outFile << "# CampusConnect Students Record File\n";
    outFile << "# Format: ID|Name|Department|AcademicYear|TeachableSkills|DesiredSkills|Interests\n";

    for (const auto& s : students) {
        outFile << s.getId() << "|"
                << s.getName() << "|"
                << s.getDepartment() << "|"
                << s.getAcademicYear() << "|"
                << serializeSkillList(s.getTeachableSkills()) << "|"
                << serializeSkillList(s.getDesiredSkills()) << "|"
                << serializeInterests(s.getInterests()) << "\n";
    }

    outFile.close();
    return true;
}

bool FileManager::loadRequests(const std::string& filePath, std::vector<ConnectionRequest>& requests) {
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        return false;
    }

    requests.clear();
    std::string line;

    while (std::getline(inFile, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string reqIdStr, senderIdStr, receiverIdStr, statusStr, timestamp;

        if (std::getline(ss, reqIdStr, '|') &&
            std::getline(ss, senderIdStr, '|') &&
            std::getline(ss, receiverIdStr, '|') &&
            std::getline(ss, statusStr, '|')) {

            if (!std::getline(ss, timestamp, '|')) {
                timestamp = "2026-10-16 10:00";
            }

            try {
                int reqId = std::stoi(reqIdStr);
                int senderId = std::stoi(senderIdStr);
                int receiverId = std::stoi(receiverIdStr);
                RequestStatus status = stringToRequestStatus(statusStr);

                if (reqId > 0 && senderId > 0 && receiverId > 0 && senderId != receiverId) {
                    requests.emplace_back(reqId, senderId, receiverId, status, timestamp);
                }
            } catch (...) {
                continue;
            }
        }
    }

    inFile.close();
    return true;
}

bool FileManager::saveRequests(const std::string& filePath, const std::vector<ConnectionRequest>& requests) {
    size_t lastSlash = filePath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        ensureDirectoryExists(filePath.substr(0, lastSlash));
    }

    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "[FileManager] Error: Could not write to " << filePath << "\n";
        return false;
    }

    outFile << "# CampusConnect Connection Requests File\n";
    outFile << "# Format: RequestID|SenderStudentID|ReceiverStudentID|Status|Timestamp\n";

    for (const auto& req : requests) {
        outFile << req.getRequestId() << "|"
                << req.getSenderId() << "|"
                << req.getReceiverId() << "|"
                << req.getStatusString() << "|"
                << req.getTimestamp() << "\n";
    }

    outFile.close();
    return true;
}

void FileManager::seedSampleDataIfEmpty(const std::string& studentPath, const std::string& requestPath) {
    std::ifstream check(studentPath);
    bool needsSeeding = false;
    if (!check.is_open()) {
        needsSeeding = true;
    } else {
        std::string firstLine;
        bool hasRecords = false;
        while (std::getline(check, firstLine)) {
            if (!firstLine.empty() && firstLine[0] != '#') {
                hasRecords = true;
                break;
            }
        }
        if (!hasRecords) needsSeeding = true;
        check.close();
    }

    if (!needsSeeding) return;

    std::cout << "[FileManager] Seeding starter university student records for CampusConnect...\n";

    std::vector<Student> sampleStudents;

    // Student 101: Anirudh Sharma (Computer Science Year 3)
    Student s1(101, "Anirudh Sharma", "Computer Science", 3);
    s1.addTeachableSkill("C++", "Programming");
    s1.addTeachableSkill("Data Structures", "Computer Science");
    s1.addTeachableSkill("Git", "DevOps");
    s1.addDesiredSkill("Python", "Programming");
    s1.addDesiredSkill("Machine Learning", "Artificial Intelligence");
    s1.addInterest("Competitive Programming");
    s1.addInterest("Open Source");
    sampleStudents.push_back(s1);

    // Student 102: Priya Patel (Data Science Year 3) - Mutual match with Anirudh
    Student s2(102, "Priya Patel", "Data Science", 3);
    s2.addTeachableSkill("Python", "Programming");
    s2.addTeachableSkill("Machine Learning", "Artificial Intelligence");
    s2.addTeachableSkill("SQL", "Database");
    s2.addDesiredSkill("C++", "Programming");
    s2.addDesiredSkill("Competitive Programming", "Algorithms");
    s2.addInterest("Kaggle Competitions");
    s2.addInterest("Hackathons");
    sampleStudents.push_back(s2);

    // Student 103: Rohan Verma (Design Year 2)
    Student s3(103, "Rohan Verma", "Interaction Design", 2);
    s3.addTeachableSkill("UI/UX Design", "Design");
    s3.addTeachableSkill("Figma", "Design Tools");
    s3.addTeachableSkill("Blender", "3D Modeling");
    s3.addDesiredSkill("HTML/CSS", "Web Development");
    s3.addDesiredSkill("JavaScript", "Web Development");
    s3.addInterest("Digital Illustration");
    sampleStudents.push_back(s3);

    // Student 104: Sneha Kulkarni (IT Year 2) - Mutual match with Rohan
    Student s4(104, "Sneha Kulkarni", "Information Technology", 2);
    s4.addTeachableSkill("HTML/CSS", "Web Development");
    s4.addTeachableSkill("JavaScript", "Web Development");
    s4.addTeachableSkill("React", "Web Frameworks");
    s4.addDesiredSkill("UI/UX Design", "Design");
    s4.addDesiredSkill("Figma", "Design Tools");
    s4.addInterest("Frontend Architecture");
    sampleStudents.push_back(s4);

    // Student 105: Vikram Joshi (Mechanical Eng Year 4)
    Student s5(105, "Vikram Joshi", "Mechanical Engineering", 4);
    s5.addTeachableSkill("CAD Modeling", "Engineering");
    s5.addTeachableSkill("MATLAB", "Simulation");
    s5.addTeachableSkill("Arduino", "Embedded Systems");
    s5.addDesiredSkill("Python", "Programming");
    s5.addDesiredSkill("Data Structures", "Computer Science");
    s5.addInterest("Robotics");
    sampleStudents.push_back(s5);

    // Student 106: Maya Nair (Electrical Eng Year 3)
    Student s6(106, "Maya Nair", "Electrical Engineering", 3);
    s6.addTeachableSkill("Digital Circuit Design", "Electronics");
    s6.addTeachableSkill("Embedded C", "Embedded Systems");
    s6.addDesiredSkill("Arduino", "Embedded Systems");
    s6.addDesiredSkill("Git", "DevOps");
    s6.addInterest("IoT Projects");
    sampleStudents.push_back(s6);

    saveStudents(studentPath, sampleStudents);

    std::vector<ConnectionRequest> sampleRequests;
    sampleRequests.emplace_back(1, 101, 102, RequestStatus::PENDING, "2026-10-16 08:30");
    sampleRequests.emplace_back(2, 103, 104, RequestStatus::ACCEPTED, "2026-10-15 14:15");
    sampleRequests.emplace_back(3, 105, 101, RequestStatus::PENDING, "2026-10-16 09:10");

    saveRequests(requestPath, sampleRequests);
}

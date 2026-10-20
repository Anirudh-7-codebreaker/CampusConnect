#include "campus_connect.h"

#include <iostream>
#include <sstream>

namespace {

void printHeading(const std::string& title) {
    std::cout << "\n========================================\n"
              << title << '\n'
              << "========================================\n";
}

bool readInteger(const std::string& prompt, int& value) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            return false;
        }

        std::stringstream input(line);
        char extraCharacter;
        if (input >> value && !(input >> extraCharacter)) {
            return true;
        }
        std::cout << "Please enter a whole number.\n";
    }
}

bool readText(const std::string& prompt, std::string& value) {
    std::cout << prompt;
    if (!std::getline(std::cin, value)) {
        return false;
    }
    return true;
}

std::vector<std::string> splitList(const std::string& line) {
    std::vector<std::string> values;
    std::stringstream input(line);
    std::string value;
    while (std::getline(input, value, ',')) {
        values.push_back(value);
    }
    return values;
}

void displayStudents(const std::vector<Student>& students) {
    if (students.empty()) {
        std::cout << "No students found.\n";
        return;
    }

    for (const Student& student : students) {
        student.displayProfile();
        std::cout << "----------------------------------------\n";
    }
}

void displayMenu() {
    std::cout << "\n1. View All Students\n"
              << "2. Register Student\n"
              << "3. Search by Interest\n"
              << "4. Search by Skill\n"
              << "5. Find Skill Matches\n"
              << "6. Send Connection Request\n"
              << "7. View Pending Requests\n"
              << "8. Process Next Request\n"
              << "9. View Active Connections\n"
              << "10. Exit\n";
}

bool registerStudentFromMenu(CampusConnect& campus) {
    int studentId;
    int year;
    std::string name;
    std::string department;
    std::string teachableSkills;
    std::string desiredSkills;
    std::string interests;

    if (!readInteger("Student ID: ", studentId) ||
        !readText("Name: ", name) ||
        !readText("Department: ", department) ||
        !readInteger("Year: ", year) ||
        !readText("Teachable skills (comma-separated): ", teachableSkills) ||
        !readText("Desired skills (comma-separated): ", desiredSkills) ||
        !readText("Interests (comma-separated): ", interests)) {
        return false;
    }

    if (campus.registerStudent(studentId, name, department, year,
                               splitList(teachableSkills),
                               splitList(desiredSkills), splitList(interests))) {
        std::cout << "Student registered successfully.\n";
    } else {
        std::cout << "Student could not be registered. Check the values and "
                     "make sure the ID is unique.\n";
    }
    return true;
}

}  // namespace

int main() {
    CampusConnect campus;
    campus.addDemoProfiles();

    std::cout << "Welcome to Campus Connect!\n"
              << "Profiles and connection requests last only for this "
                 "program session.\n";

    while (true) {
        displayMenu();

        int choice;
        if (!readInteger("\nChoose an option: ", choice)) {
            std::cout << "\nInput ended. Goodbye!\n";
            return 0;
        }

        if (choice == 1) {
            printHeading("All Students");
            displayStudents(campus.getStudents());
        } else if (choice == 2) {
            printHeading("Register Student");
            if (!registerStudentFromMenu(campus)) {
                std::cout << "Input ended. Goodbye!\n";
                return 0;
            }
        } else if (choice == 3) {
            std::string interest;
            printHeading("Search by Interest");
            if (!readText("Interest: ", interest)) {
                return 0;
            }
            displayStudents(campus.searchByInterest(interest));
        } else if (choice == 4) {
            std::string skill;
            printHeading("Search by Skill");
            if (!readText("Skill: ", skill)) {
                return 0;
            }
            displayStudents(campus.searchByTeachableSkill(skill));
        } else if (choice == 5) {
            int studentId;
            printHeading("Find Skill Matches");
            if (!readInteger("Student ID: ", studentId)) {
                return 0;
            }
            campus.displayMatches(studentId);
        } else if (choice == 6) {
            int senderId;
            int receiverId;
            printHeading("Send Connection Request");
            if (!readInteger("Your student ID: ", senderId) ||
                !readInteger("Receiver student ID: ", receiverId)) {
                return 0;
            }
            if (campus.sendConnectionRequest(senderId, receiverId)) {
                std::cout << "Connection request sent.\n";
            } else {
                std::cout << "Request rejected. Check the IDs and make sure "
                             "there is no duplicate or existing connection.\n";
            }
        } else if (choice == 7) {
            printHeading("Pending Requests");
            campus.displayPendingRequests();
        } else if (choice == 8) {
            int decision;
            printHeading("Process Next Request");
            if (!readInteger("Enter 1 to accept or 2 to reject: ", decision)) {
                return 0;
            }
            if (decision != 1 && decision != 2) {
                std::cout << "Please choose 1 or 2.\n";
            } else if (campus.processNextRequest(decision == 1)) {
                std::cout << "The oldest pending request was "
                          << (decision == 1 ? "accepted." : "rejected.")
                          << '\n';
            } else {
                std::cout << "There are no pending requests.\n";
            }
        } else if (choice == 9) {
            printHeading("Active Connections");
            campus.displayActiveConnections();
        } else if (choice == 10) {
            std::cout << "Goodbye!\n";
            return 0;
        } else {
            std::cout << "Please choose a number from 1 to 10.\n";
        }
    }
}

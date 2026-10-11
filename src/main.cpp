
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include "student.h"

using namespace std;

struct ConnectionRequest {
    int senderId;
    int recipientId;
    string status;
};

string toLower(string text) {
    transform(text.begin(), text.end(), text.begin(),
        [](unsigned char c) {
            return static_cast<char>(tolower(c));
        });
    return text;
}

void displayStudent(const Student& s) {
    cout << "\nID: " << s.id
         << "\nName: " << s.name
         << "\nDepartment: " << s.department
         << "\nYear: " << s.year
         << "\nInterests: ";

    for (size_t i = 0; i < s.interests.size(); i++) {
        if (i > 0) cout << ", ";
        cout << s.interests[i];
    }

    cout << "\n-------------------------\n";
}

void showAllStudents(const vector<Student>& students, int currentUserId) {
    for (const Student& s : students) {
        if (s.id != currentUserId) {
            displayStudent(s);
        }
    }
}

void searchByInterest(const vector<Student>& students, int currentUserId) {
    string interest;
    cout << "Enter an interest: ";
    getline(cin, interest);

    string target = toLower(interest);
    bool found = false;

    for (const Student& s : students) {
        if (s.id == currentUserId) continue;

        for (const string& item : s.interests) {
            if (toLower(item).find(target) != string::npos) {
                displayStudent(s);
                found = true;
                break;
            }
        }
    }

    if (!found) {
        cout << "No matching students found.\n";
    }
}

void sendRequest(const vector<Student>& students,
                 vector<ConnectionRequest>& requests,
                 int currentUserId) {
    int recipientId;
    cout << "Enter the student ID to connect with: ";

    if (!(cin >> recipientId)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid ID.\n";
        return;
    }
    cin.ignore(10000, '\n');

    if (recipientId == currentUserId) {
        cout << "You cannot send a request to yourself.\n";
        return;
    }

    auto it = find_if(students.begin(), students.end(),
        [recipientId](const Student& s) {
            return s.id == recipientId;
        });

    if (it == students.end()) {
        cout << "Student ID not found.\n";
        return;
    }

    auto existing = find_if(requests.begin(), requests.end(),
        [currentUserId, recipientId](const ConnectionRequest& r) {
            return r.senderId == currentUserId &&
                   r.recipientId == recipientId;
        });

    if (existing != requests.end()) {
        cout << "You have already sent a request to this student.\n";
        return;
    }

    requests.push_back({currentUserId, recipientId, "Pending"});
    cout << "Connection request sent to " << it->name << "!\n";
}

void showRequests(const vector<ConnectionRequest>& requests,
                  const vector<Student>& students) {
    if (requests.empty()) {
        cout << "No connection requests yet.\n";
        return;
    }

    for (const ConnectionRequest& r : requests) {
        string senderName = "Unknown";
        string recipientName = "Unknown";

        for (const Student& s : students) {
            if (s.id == r.senderId) senderName = s.name;
            if (s.id == r.recipientId) recipientName = s.name;
        }

        cout << senderName << " -> " << recipientName
             << " | Status: " << r.status << '\n';
    }
}

int main() {
    vector<Student> students = {
        Student(101, "Anirudh", "CSE", 2),
        Student(102, "Rahul", "CSE", 2),
        Student(103, "Priya", "ECE", 1),
        Student(104, "Neha", "CSE", 2)
    };

    students[0].interests = {"AI", "C++"};
    students[1].interests = {"Web Development", "Startups"};
    students[2].interests = {"AI", "Robotics"};
    students[3].interests = {"AI", "Web Development"};

    vector<ConnectionRequest> requests;
    const int currentUserId = 101;

    int choice;

    do {
        cout << "\n===== CAMPUS CONNECT =====\n"
             << "1. View students\n"
             << "2. Search by interest\n"
             << "3. Send connection request\n"
             << "4. View connection requests\n"
             << "5. Exit\n"
             << "Choose an option: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Please enter a number from 1 to 5.\n";
            continue;
        }
        cin.ignore(10000, '\n');

        switch (choice) {
            case 1:
                showAllStudents(students, currentUserId);
                break;

            case 2:
                searchByInterest(students, currentUserId);
                break;

            case 3:
                sendRequest(students, requests, currentUserId);
                break;

            case 4:
                showRequests(requests, students);
                break;

            case 5:
                cout << "Thanks for using Campus Connect!\n";
                break;

            default:
                cout << "Invalid option. Choose 1-5.\n";
        }

    } while (choice != 5);

    return 0;
}


#include <iostream>
#include <vector>
#include <string>
#include "student.h"

using namespace std;

struct ConnectionRequest {
    int senderId;
    int recipientId;
    string status;
};

int main() {
    vector<Student> students;

    // Create student profiles
    Student s1(101, "Anirudh", "CSE", 2);
    Student s2(102, "Rahul", "CSE", 2);
    Student s3(103, "Priya", "ECE", 1);
    Student s4(104, "Neha", "CSE", 2);

    s1.interests = {"AI", "C++"};
    s2.interests = {"Web Development", "Startups"};
    s3.interests = {"AI", "Robotics"};
    s4.interests = {"AI", "Web Development"};

    students.push_back(s1);
    students.push_back(s2);
    students.push_back(s3);
    students.push_back(s4);

    // ID of the currently logged-in student
    int currentUserId = 101;

    // Store connection requests in memory
    vector<ConnectionRequest> requests;

    cout << "===== CAMPUS CONNECT =====\n";
    cout << "Welcome, Anirudh!\n\n";

    // Search by interest
    string searchInterest;
    cout << "Enter an interest to discover students: ";
    getline(cin, searchInterest);

    vector<int> matchingIds;

    cout << "\nMatching students:\n\n";

    for (const Student& student : students) {
        // Do not show the current user
        if (student.id == currentUserId) {
            continue;
        }

        for (const string& interest : student.interests) {
            if (interest == searchInterest) {
                cout << "ID: " << student.id << '\n';
                cout << "Name: " << student.name << '\n';
                cout << "Department: " << student.department << '\n';
                cout << "Year: " << student.year << '\n';
                cout << "----------------------\n";

                matchingIds.push_back(student.id);
                break;
            }
        }
    }

    if (matchingIds.empty()) {
        cout << "No other students found with that interest.\n";
        return 0;
    }

    // Choose a student
    int selectedId;
    cout << "\nEnter the ID you want to connect with: ";
    cin >> selectedId;

    bool validChoice = false;

    for (int id : matchingIds) {
        if (id == selectedId) {
            validChoice = true;
            break;
        }
    }

    if (!validChoice) {
        cout << "Invalid selection. Please choose a displayed ID.\n";
        return 0;
    }

    // Create a new connection request
    ConnectionRequest request;
    request.senderId = currentUserId;
    request.recipientId = selectedId;
    request.status = "Pending";

    requests.push_back(request);

    cout << "\nConnection request sent to ";

    for (const Student& student : students) {
        if (student.id == selectedId) {
            cout << student.name << "!\n";
            break;
        }
    }

    cout << "Request status: " << requests.back().status << '\n';

    // Simulate the recipient responding
    int response;
    cout << "\nSimulate recipient response:\n";
    cout << "1. Accept request\n";
    cout << "2. Reject request\n";
    cout << "Enter choice: ";
    cin >> response;

    if (response == 1) {
        requests.back().status = "Accepted";
    } else if (response == 2) {
        requests.back().status = "Rejected";
    } else {
        cout << "Invalid response. Request remains pending.\n";
    }

    cout << "\n===== REQUEST SUMMARY =====\n";
    cout << "Sender ID: " << requests.back().senderId << '\n';
    cout << "Recipient ID: " << requests.back().recipientId << '\n';
    cout << "Final status: " << requests.back().status << '\n';

    return 0;
}
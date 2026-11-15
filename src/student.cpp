#include "student.h"

#include <algorithm>
#include <iostream>

User::User(std::string userId, std::string name)
    : id_(userId), name_(name) {}

const std::string& User::getId() const {
    return id_;
}

const std::string& User::getName() const {
    return name_;
}

Student::Student()
    : User("", "") {}

Student::Student(std::string studentId, std::string name,
                 std::string department, int year)
        : User(studentId, name),
            department_(department),
      year_(year) {}

Student::Student(std::string studentId, std::string name,
                 std::vector<std::string> skills)
    : Student(studentId, name, "", 0) {
    for (const std::string& skill : skills) {
        addTeachableSkill(skill);
    }
}

const std::string& Student::getDepartment() const {
    return department_;
}

int Student::getYear() const {
    return year_;
}

const std::vector<std::string>& Student::getSkills() const {
    return teachableSkills_;
}

const std::vector<std::string>& Student::getTeachableSkills() const {
    return teachableSkills_;
}

const std::vector<std::string>& Student::getInterests() const {
    return interests_;
}

bool Student::addTeachableSkill(const std::string& skill) {
    if (skill.empty() ||
        std::find(teachableSkills_.begin(), teachableSkills_.end(), skill) !=
            teachableSkills_.end()) {
        return false;
    }

    teachableSkills_.push_back(skill);
    return true;
}

bool Student::addInterest(const std::string& interest) {
    if (interest.empty() ||
        std::find(interests_.begin(), interests_.end(), interest) !=
            interests_.end()) {
        return false;
    }

    interests_.push_back(interest);
    return true;
}

bool Student::canTeachSkill(const std::string& skill) const {
    return std::find(teachableSkills_.begin(), teachableSkills_.end(), skill) !=
           teachableSkills_.end();
}

void Student::displayProfile() const {
    std::cout << "Name: " << getName() << '\n'
              << "ID: " << getId() << '\n'
              << "Department: " << department_ << '\n'
              << "Year: " << year_ << '\n'
              << "Teachable skills: ";

    for (std::size_t index = 0; index < teachableSkills_.size(); ++index) {
        if (index > 0) {
            std::cout << ", ";
        }
        std::cout << teachableSkills_[index];
    }

    std::cout << "\nInterests: ";
    for (std::size_t index = 0; index < interests_.size(); ++index) {
        if (index > 0) {
            std::cout << ", ";
        }
        std::cout << interests_[index];
    }
    std::cout << '\n';
}

ConnectionRequest::ConnectionRequest(std::string requestId,
                                     std::string senderId,
                                     std::string receiverId)
        : requestId_(requestId),
            senderId_(senderId),
            receiverId_(receiverId) {}

const std::string& ConnectionRequest::getRequestId() const {
    return requestId_;
}

const std::string& ConnectionRequest::getSenderId() const {
    return senderId_;
}

const std::string& ConnectionRequest::getReceiverId() const {
    return receiverId_;
}

ConnectionRequest::Status ConnectionRequest::getStatus() const {
    return status_;
}

bool ConnectionRequest::accept() {
    if (status_ != Status::Pending) {
        return false;
    }

    status_ = Status::Accepted;
    return true;
}

bool ConnectionRequest::reject() {
    if (status_ != Status::Pending) {
        return false;
    }

    status_ = Status::Rejected;
    return true;
}

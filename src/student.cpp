#include "student.h"

#include <algorithm>
#include <cctype>
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
                 std::string department, int year,
                 const std::vector<std::string>& teachableSkills,
                 const std::vector<std::string>& desiredSkills,
                 const std::vector<std::string>& interests)
                : Student(studentId, name, department, year) {
    for (const std::string& skill : teachableSkills) {
        addTeachableSkill(skill);
    }
    for (const std::string& skill : desiredSkills) {
        addDesiredSkill(skill);
    }
    for (const std::string& interest : interests) {
        addInterest(interest);
    }
}

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

const std::vector<std::string>& Student::getDesiredSkills() const {
    return desiredSkills_;
}

const std::vector<std::string>& Student::getInterests() const {
    return interests_;
}

bool Student::addTeachableSkill(const std::string& skill) {
    return addUniqueName(teachableSkills_, skill);
}

bool Student::addDesiredSkill(const std::string& skill) {
    return addUniqueName(desiredSkills_, skill);
}

bool Student::addInterest(const std::string& interest) {
    return addUniqueName(interests_, interest);
}

bool Student::canTeachSkill(const std::string& skill) const {
    const std::string normalizedSkill = normalizeName(skill);
    if (normalizedSkill.empty()) {
        return false;
    }

    return std::any_of(teachableSkills_.begin(), teachableSkills_.end(),
                       [&normalizedSkill](const std::string& teachableSkill) {
                           return normalizeName(teachableSkill) ==
                                  normalizedSkill;
                       });
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

    std::cout << "\nDesired skills: ";
    for (std::size_t index = 0; index < desiredSkills_.size(); ++index) {
        if (index > 0) {
            std::cout << ", ";
        }
        std::cout << desiredSkills_[index];
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

std::string Student::normalizeName(const std::string& name) {
    std::size_t first = 0;
    while (first < name.size() &&
           std::isspace(static_cast<unsigned char>(name[first]))) {
        ++first;
    }

    std::size_t last = name.size();
    while (last > first &&
           std::isspace(static_cast<unsigned char>(name[last - 1]))) {
        --last;
    }

    std::string normalized = name.substr(first, last - first);
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char character) {
                       return static_cast<char>(std::tolower(character));
                   });
    return normalized;
}

bool Student::addUniqueName(std::vector<std::string>& names,
                            const std::string& name) {
    const std::string normalizedName = normalizeName(name);
    if (normalizedName.empty()) {
        return false;
    }

    const bool alreadyExists =
        std::any_of(names.begin(), names.end(),
                    [&normalizedName](const std::string& existingName) {
                        return normalizeName(existingName) == normalizedName;
                    });
    if (alreadyExists) {
        return false;
    }

    names.push_back(normalizedName);
    return true;
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

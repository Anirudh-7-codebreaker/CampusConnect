#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>

class User {
public:
    User(std::string userId, std::string name);
    virtual ~User() = default;

    const std::string& getId() const;
    const std::string& getName() const;

    virtual void displayProfile() const = 0;

private:
    std::string id_;
    std::string name_;
};

class Student : public User {
public:
    // The overloads are retained for compatibility with existing callers.
    Student();
    Student(std::string studentId, std::string name,
            std::string department, int year);
    Student(std::string studentId, std::string name,
            std::string department, int year,
            const std::vector<std::string>& teachableSkills,
            const std::vector<std::string>& desiredSkills,
            const std::vector<std::string>& interests);

    Student(std::string studentId, std::string name,
            std::vector<std::string> skills);

    const std::string& getDepartment() const;
    int getYear() const;
    const std::vector<std::string>& getSkills() const;
    const std::vector<std::string>& getTeachableSkills() const;
    const std::vector<std::string>& getDesiredSkills() const;
    const std::vector<std::string>& getInterests() const;

    bool addTeachableSkill(const std::string& skill);
    bool addDesiredSkill(const std::string& skill);
    bool addInterest(const std::string& interest);
    bool canTeachSkill(const std::string& skill) const;

    void displayProfile() const override;

private:
    static std::string normalizeName(const std::string& name);
    static bool addUniqueName(std::vector<std::string>& names,
                              const std::string& name);

    std::string department_;
    int year_ = 0;
    std::vector<std::string> teachableSkills_;
    std::vector<std::string> desiredSkills_;
    std::vector<std::string> interests_;
};

class ConnectionRequest {
public:
    enum class Status {
        Pending,
        Accepted,
        Rejected
    };

    ConnectionRequest(std::string requestId, std::string senderId,
                      std::string receiverId);

    const std::string& getRequestId() const;
    const std::string& getSenderId() const;
    const std::string& getReceiverId() const;
    Status getStatus() const;

    bool accept();
    bool reject();

private:
    std::string requestId_;
    std::string senderId_;
    std::string receiverId_;
    Status status_ = Status::Pending;
};

#endif

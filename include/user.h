#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>

/**
 * @brief Abstract base class representing a general system user.
 *
 * Demonstrates:
 * - Data Encapsulation: private attributes with public getters/setters and validation.
 * - Polymorphism: pure virtual displayProfile() function and a virtual destructor.
 */
class User {
private:
    int id;
    std::string name;

public:
    // Constructors & Destructor
    User();
    User(int id, const std::string& name);
    virtual ~User(); // Ensures correct cleanup of derived objects

    // Getters
    int getId() const;
    std::string getName() const;

    // Setters with validation
    bool setId(int newId);
    bool setName(const std::string& newName);

    // Pure virtual function enforcing polymorphic implementation in derived classes
    virtual void displayProfile() const = 0;
};

#endif // USER_H

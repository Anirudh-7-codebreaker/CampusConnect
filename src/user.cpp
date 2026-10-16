#include "user.h"
#include <iostream>

// Default constructor
User::User() : id(0), name("Unknown User") {}

// Parameterized constructor
User::User(int id, const std::string& name) : id(id), name(name) {
    if (id <= 0) {
        this->id = 1;
    }
    if (name.empty()) {
        this->name = "Unnamed User";
    }
}

// Virtual destructor ensures proper cleanup of derived objects in dynamic dispatch
User::~User() {
    // Empty virtual destructor body
}

// Getters
int User::getId() const {
    return id;
}

std::string User::getName() const {
    return name;
}

// Setters with validation
bool User::setId(int newId) {
    if (newId <= 0) {
        return false;
    }
    this->id = newId;
    return true;
}

bool User::setName(const std::string& newName) {
    if (newName.empty()) {
        return false;
    }
    this->name = newName;
    return true;
}

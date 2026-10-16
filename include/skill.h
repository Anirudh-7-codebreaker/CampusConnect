#ifndef SKILL_H
#define SKILL_H

#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>

/**
 * @brief Represents an individual skill that can be taught or learned.
 *
 * Encapsulates skill name and category with built-in string normalization
 * so comparisons like "C++", "c++", and "C++ " evaluate as equal.
 */
class Skill {
private:
    std::string name;
    std::string category;

public:
    // Constructors
    Skill();
    Skill(const std::string& name, const std::string& category = "General");

    // Getters & Setters (Encapsulation)
    std::string getName() const;
    std::string getCategory() const;
    std::string getNormalizedName() const;

    void setName(const std::string& newName);
    void setCategory(const std::string& newCategory);

    // Normalization utility (static helper)
    static std::string normalize(const std::string& raw);

    // Member function to display skill
    void display() const;

    // Operator Overloading
    // Equality comparison based on normalized name
    bool operator==(const Skill& other) const;
    bool operator!=(const Skill& other) const;
    // Strict weak ordering based on normalized name
    bool operator<(const Skill& other) const;

    // Stream insertion operator for clean printing
    friend std::ostream& operator<<(std::ostream& os, const Skill& skill);
};

#endif // SKILL_H

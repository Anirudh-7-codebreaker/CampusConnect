#include "student.h"
#include "skill.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>

// ============================================================================
// Skill Implementation
// ============================================================================

Skill::Skill() : name(""), category("General") {}

Skill::Skill(const std::string& name, const std::string& category)
    : name(name), category(category.empty() ? "General" : category) {}

std::string Skill::getName() const {
    return name;
}

std::string Skill::getCategory() const {
    return category;
}

std::string Skill::getNormalizedName() const {
    return normalize(name);
}

void Skill::setName(const std::string& newName) {
    name = newName;
}

void Skill::setCategory(const std::string& newCategory) {
    category = newCategory.empty() ? "General" : newCategory;
}

std::string Skill::normalize(const std::string& raw) {
    std::string result;
    bool inWhitespace = false;

    // Trim leading whitespace
    size_t start = 0;
    while (start < raw.size() && std::isspace(static_cast<unsigned char>(raw[start]))) {
        start++;
    }

    // Trim trailing whitespace
    size_t end = raw.size();
    while (end > start && std::isspace(static_cast<unsigned char>(raw[end - 1]))) {
        end--;
    }

    for (size_t i = start; i < end; ++i) {
        char ch = raw[i];
        if (std::isspace(static_cast<unsigned char>(ch))) {
            if (!inWhitespace) {
                result.push_back(' ');
                inWhitespace = true;
            }
        } else {
            result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
            inWhitespace = false;
        }
    }
    return result;
}

void Skill::display() const {
    std::cout << name << " (" << category << ")";
}

bool Skill::operator==(const Skill& other) const {
    return getNormalizedName() == other.getNormalizedName();
}

bool Skill::operator!=(const Skill& other) const {
    return !(*this == other);
}

bool Skill::operator<(const Skill& other) const {
    return getNormalizedName() < other.getNormalizedName();
}

std::ostream& operator<<(std::ostream& os, const Skill& skill) {
    os << skill.name;
    if (!skill.category.empty() && skill.category != "General") {
        os << " [" << skill.category << "]";
    }
    return os;
}

// ============================================================================
// Student Implementation
// ============================================================================

Student::Student()
    : User(0, "Unnamed"), department("General"), academicYear(1) {}

Student::Student(int id, const std::string& name, const std::string& department, int academicYear)
    : User(id, name),
      department(department.empty() ? "General" : department),
      academicYear((academicYear >= 1 && academicYear <= 6) ? academicYear : 1) {}

std::string Student::getDepartment() const {
    return department;
}

bool Student::setDepartment(const std::string& dept) {
    if (dept.empty()) return false;
    department = dept;
    return true;
}

int Student::getAcademicYear() const {
    return academicYear;
}

bool Student::setAcademicYear(int year) {
    if (year >= 1 && year <= 6) {
        academicYear = year;
        return true;
    }
    return false;
}

const std::vector<Skill>& Student::getTeachableSkills() const {
    return teachableSkills;
}

const std::vector<Skill>& Student::getDesiredSkills() const {
    return desiredSkills;
}

const std::vector<std::string>& Student::getInterests() const {
    return interests;
}

// Function Overload 1: addTeachableSkill with Skill object
bool Student::addTeachableSkill(const Skill& skill) {
    if (skill.getName().empty()) return false;
    for (const auto& s : teachableSkills) {
        if (s == skill) {
            return false; // Prevent duplicate skills
        }
    }
    teachableSkills.push_back(skill);
    return true;
}

// Function Overload 2: addTeachableSkill with string name and category
bool Student::addTeachableSkill(const std::string& skillName, const std::string& category) {
    return addTeachableSkill(Skill(skillName, category));
}

// Function Overload 3: addDesiredSkill with Skill object
bool Student::addDesiredSkill(const Skill& skill) {
    if (skill.getName().empty()) return false;
    for (const auto& s : desiredSkills) {
        if (s == skill) {
            return false; // Prevent duplicate desired skills
        }
    }
    desiredSkills.push_back(skill);
    return true;
}

// Function Overload 4: addDesiredSkill with string name and category
bool Student::addDesiredSkill(const std::string& skillName, const std::string& category) {
    return addDesiredSkill(Skill(skillName, category));
}

void Student::addInterest(const std::string& interest) {
    if (!interest.empty()) {
        interests.push_back(interest);
    }
}

// Function Overload 5: canTeach with Skill object
bool Student::canTeach(const Skill& skill) const {
    for (const auto& s : teachableSkills) {
        if (s == skill) return true;
    }
    return false;
}

// Function Overload 6: canTeach with skill name string
bool Student::canTeach(const std::string& skillName) const {
    return canTeach(Skill(skillName));
}

// Function Overload 7: wantsToLearn with Skill object
bool Student::wantsToLearn(const Skill& skill) const {
    for (const auto& s : desiredSkills) {
        if (s == skill) return true;
    }
    return false;
}

// Function Overload 8: wantsToLearn with skill name string
bool Student::wantsToLearn(const std::string& skillName) const {
    return wantsToLearn(Skill(skillName));
}

// Helper: which skills candidate teaches that this student wants to learn
std::vector<Skill> Student::getMatchedSkillsToLearn(const Student& candidate) const {
    std::vector<Skill> matched;
    for (const auto& want : desiredSkills) {
        if (candidate.canTeach(want)) {
            matched.push_back(want);
        }
    }
    return matched;
}

// Helper: which skills this student teaches that candidate wants to learn
std::vector<Skill> Student::getMatchedSkillsToTeach(const Student& candidate) const {
    std::vector<Skill> matched;
    for (const auto& offer : teachableSkills) {
        if (candidate.wantsToLearn(offer)) {
            matched.push_back(offer);
        }
    }
    return matched;
}

// Runtime Polymorphism: Overridden virtual displayProfile()
void Student::displayProfile() const {
    std::cout << "+----------------------------------------------------------------------+\n";
    std::cout << "| STUDENT PROFILE #" << std::left << std::setw(50) << getId() << "|\n";
    std::cout << "+----------------------------------------------------------------------+\n";
    std::cout << "  Name           : " << getName() << "\n";
    std::cout << "  Department     : " << department << "\n";
    std::cout << "  Academic Year  : Year " << academicYear << "\n";

    std::cout << "  Teaches        : ";
    if (teachableSkills.empty()) {
        std::cout << "(None listed)\n";
    } else {
        for (size_t i = 0; i < teachableSkills.size(); ++i) {
            std::cout << teachableSkills[i];
            if (i + 1 < teachableSkills.size()) std::cout << ", ";
        }
        std::cout << "\n";
    }

    std::cout << "  Wants to Learn : ";
    if (desiredSkills.empty()) {
        std::cout << "(None listed)\n";
    } else {
        for (size_t i = 0; i < desiredSkills.size(); ++i) {
            std::cout << desiredSkills[i];
            if (i + 1 < desiredSkills.size()) std::cout << ", ";
        }
        std::cout << "\n";
    }

    if (!interests.empty()) {
        std::cout << "  Interests      : ";
        for (size_t i = 0; i < interests.size(); ++i) {
            std::cout << interests[i];
            if (i + 1 < interests.size()) std::cout << ", ";
        }
        std::cout << "\n";
    }
    std::cout << "+----------------------------------------------------------------------+\n";
}

#ifndef STUDENT_H
#define STUDENT_H

#include "user.h"
#include "skill.h"
#include <vector>
#include <string>

/**
 * @brief Student class derived from User.
 *
 * Demonstrates:
 * - Inheritance: public inheritance from abstract class User.
 * - Polymorphism: overrides displayProfile().
 * - Composition: owns collections of Skill objects (vector<Skill>).
 * - Function Overloading: overloaded methods for adding and checking skills.
 */
class Student : public User {
private:
    std::string department;
    int academicYear;
    std::vector<Skill> teachableSkills; // Composition: Skills this student can teach
    std::vector<Skill> desiredSkills;   // Composition: Skills this student wants to learn
    std::vector<std::string> interests; // General student interests/hobbies

public:
    // Constructors
    Student();
    Student(int id, const std::string& name, const std::string& department, int academicYear);

    // Getters and Setters
    std::string getDepartment() const;
    bool setDepartment(const std::string& dept);

    int getAcademicYear() const;
    bool setAcademicYear(int year);

    const std::vector<Skill>& getTeachableSkills() const;
    const std::vector<Skill>& getDesiredSkills() const;
    const std::vector<std::string>& getInterests() const;

    // Function Overloading 1 & 2: Adding teachable skills
    bool addTeachableSkill(const Skill& skill);
    bool addTeachableSkill(const std::string& skillName, const std::string& category = "General");

    // Function Overloading 3 & 4: Adding desired skills
    bool addDesiredSkill(const Skill& skill);
    bool addDesiredSkill(const std::string& skillName, const std::string& category = "General");

    // Adding general interests
    void addInterest(const std::string& interest);

    // Function Overloading 5 & 6: Querying teachable skills
    bool canTeach(const Skill& skill) const;
    bool canTeach(const std::string& skillName) const;

    // Function Overloading 7 & 8: Querying desired skills
    bool wantsToLearn(const Skill& skill) const;
    bool wantsToLearn(const std::string& skillName) const;

    // Helper methods for compatibility analysis
    std::vector<Skill> getMatchedSkillsToTeach(const Student& candidate) const;
    std::vector<Skill> getMatchedSkillsToLearn(const Student& candidate) const;

    // Runtime Polymorphism: Overriding pure virtual displayProfile()
    void displayProfile() const override;
};

#endif // STUDENT_H

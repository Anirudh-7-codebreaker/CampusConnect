#include "file_manager.h"

#include <fstream>
#include <sstream>

namespace {

std::vector<std::string> splitValues(const std::string& text) {
    std::vector<std::string> values;
    std::stringstream input(text);
    std::string value;

    while (std::getline(input, value, ';')) {
        if (!value.empty()) {
            values.push_back(value);
        }
    }

    return values;
}

std::string joinValues(const std::vector<std::string>& values) {
    std::string result;

    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index > 0) {
            result += ';';
        }
        result += values[index];
    }

    return result;
}

}  // namespace

std::vector<Student> FileManager::loadStudents(const std::string& filePath) {
    std::vector<Student> students;
    std::ifstream input(filePath);
    std::string line;

    if (!input.is_open()) {
        return students;
    }

    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream row(line);
        std::string id;
        std::string name;
        std::string department;
        std::string yearText;
        std::string skillsText;
        std::string interestsText;

        if (!std::getline(row, id, '|') ||
            !std::getline(row, name, '|') ||
            !std::getline(row, department, '|') ||
            !std::getline(row, yearText, '|') ||
            !std::getline(row, skillsText, '|') ||
            !std::getline(row, interestsText)) {
            continue;
        }

        std::stringstream yearInput(yearText);
        int year;
        char extraCharacter;
        if (!(yearInput >> year) || (yearInput >> extraCharacter)) {
            continue;
        }

        Student student(id, name, department, year);
        for (const std::string& skill : splitValues(skillsText)) {
            student.addTeachableSkill(skill);
        }
        for (const std::string& interest : splitValues(interestsText)) {
            student.addInterest(interest);
        }
        students.push_back(student);
    }

    return students;
}

bool FileManager::saveStudents(const std::string& filePath,
                                const std::vector<Student>& students) {
    std::ofstream output(filePath);

    if (!output.is_open()) {
        return false;
    }

    for (const Student& student : students) {
        output << student.getId() << '|'
               << student.getName() << '|'
               << student.getDepartment() << '|'
               << student.getYear() << '|'
               << joinValues(student.getTeachableSkills()) << '|'
               << joinValues(student.getInterests()) << '\n';
    }

    return output.good();
}
#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>

class Student {
public:
    int id;
    std::string name;
    std::string department;
    int year;

    std::vector<std::string> interests;

    Student(int id, std::string name,
            std::string department, int year) {
        this->id = id;
        this->name = name;
        this->department = department;
        this->year = year;
    }
};

#endif
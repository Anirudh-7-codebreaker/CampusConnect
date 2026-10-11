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

    void display() const {
        std::cout << "ID: " << id << std::endl;
        std::cout << "Name: " << name << std::endl;
        std::cout << "Department: " << department << std::endl;
        std::cout << "Year: " << year << std::endl;
        std::cout << "Interests: ";
        for (const auto& interest : interests) {
            std::cout << interest << ", ";
        }
        std::cout << std::endl;
    }
};

#endif
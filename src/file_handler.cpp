#include "file_handler.h"

#include <fstream>
#include <iostream>

bool saveNewStudentName(const std::string& name) {
    std::ofstream file("Saved New Students.txt", std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Could not open Saved New Students.txt.\n";
        return false;
    }

    file << name << '\n';
    file.close();
    return true;
}

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "student.h"

#include <string>
#include <vector>

class FileManager {
public:
    static std::vector<Student> loadStudents(const std::string& filePath);
    static bool saveStudents(const std::string& filePath,
                             const std::vector<Student>& students);
};

#endif

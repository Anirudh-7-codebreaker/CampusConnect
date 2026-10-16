#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "student.h"
#include "connection_request.h"
#include <vector>
#include <string>

/**
 * @brief Handles file persistence for students and connection requests.
 *
 * Implements clean separation of concerns:
 * - Uses ifstream, ofstream, getline(), stringstream
 * - Validates records and skips malformed lines safely
 * - Persists and loads data/students.txt and data/requests.txt
 */
class FileManager {
public:
    // Ensures directory exists and loads student profiles
    static bool loadStudents(const std::string& filePath, std::vector<Student>& students);

    // Saves student profiles to disk in structured delimiter format
    static bool saveStudents(const std::string& filePath, const std::vector<Student>& students);

    // Loads connection requests from disk
    static bool loadRequests(const std::string& filePath, std::vector<ConnectionRequest>& requests);

    // Saves connection requests to disk
    static bool saveRequests(const std::string& filePath, const std::vector<ConnectionRequest>& requests);

    // Seeds realistic starter data if files are missing or empty
    static void seedSampleDataIfEmpty(const std::string& studentPath, const std::string& requestPath);

    // Directory creation helper
    static bool ensureDirectoryExists(const std::string& dirPath);
};

#endif // FILE_MANAGER_H

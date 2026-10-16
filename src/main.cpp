#include "campus_connect_app.h"
#include <iostream>
#include <exception>

/**
 * @brief Entry point for CampusConnect C++17 Terminal Application.
 *
 * CampusConnect is a student networking and peer skill exchange platform.
 * Features pure terminal-based input and output via standard std::cin and std::cout.
 *
 * Concepts demonstrated:
 * - OOP: Classes & Objects, Encapsulation, Inheritance, Runtime Polymorphism,
 *        Function Overloading, Operator Overloading, and Composition.
 * - STL Data Structures: vector, unordered_map, queue, priority_queue, list.
 * - Persistence: Flat-file storage using ifstream, ofstream, stringstream.
 */
int main() {
    try {
        CampusConnectApp app("data/students.txt", "data/requests.txt");
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "\n[Fatal Error] Exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n[Fatal Error] Unknown critical error occurred.\n";
        return 1;
    }

    return 0;
}

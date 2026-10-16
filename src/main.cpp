#include "skill_swap_app.h"
#include <iostream>
#include <exception>

/**
 * @brief Entry point for SkillSwap C++17 Terminal Application.
 *
 * SkillSwap is a peer-to-peer university skill exchange and matching platform.
 * Milestone 1 (40% Scope) demonstrates:
 * - OOP: Inheritance, Polymorphism, Encapsulation, Function & Operator Overloading.
 * - STL Data Structures: vector, unordered_map, queue, priority_queue, list.
 * - Robust file persistence (ifstream, ofstream, stringstream).
 */
int main() {
    try {
        SkillSwapApp app("data/students.txt", "data/requests.txt");
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "\n[Fatal Error] Unexpected exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n[Fatal Error] Unknown critical error occurred.\n";
        return 1;
    }

    return 0;
}

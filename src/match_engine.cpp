#include "match.h"
#include <iostream>
#include <iomanip>

void Match::display() const {
    std::cout << "+----------------------------------------------------------------------+\n";
    std::cout << "| MATCH: " << std::left << std::setw(25) << candidateName
              << " (ID #" << std::setw(4) << candidateId << ") "
              << "Score: " << std::right << std::setw(3) << compatibilityScore << "%       |\n";
    std::cout << "+----------------------------------------------------------------------+\n";
    std::cout << "  Dept & Year    : " << candidateDepartment << ", Year " << candidateYear << "\n";
    std::cout << "  Exchange Type  : " << (isMutual ? "[MUTUAL EXCHANGE] (Highest Value)" : "[ONE-WAY EXCHANGE]") << "\n";

    if (!skillsYouCanLearn.empty()) {
        std::cout << "  You Can Learn  : ";
        for (size_t i = 0; i < skillsYouCanLearn.size(); ++i) {
            std::cout << skillsYouCanLearn[i];
            if (i + 1 < skillsYouCanLearn.size()) std::cout << ", ";
        }
        std::cout << "\n";
    }

    if (!skillsYouCanTeach.empty()) {
        std::cout << "  You Can Teach  : ";
        for (size_t i = 0; i < skillsYouCanTeach.size(); ++i) {
            std::cout << skillsYouCanTeach[i];
            if (i + 1 < skillsYouCanTeach.size()) std::cout << ", ";
        }
        std::cout << "\n";
    }

    std::cout << "  Breakdown      : " << explanation << "\n";
    std::cout << "+----------------------------------------------------------------------+\n";
}

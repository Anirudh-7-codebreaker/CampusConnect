#ifndef MATCH_H
#define MATCH_H

#include <string>
#include <vector>
#include <iostream>

/**
 * @brief Represents a skill exchange match between the active student and a candidate.
 */
struct Match {
    int candidateId;
    std::string candidateName;
    std::string candidateDepartment;
    int candidateYear;
    int compatibilityScore;
    std::string explanation;
    std::vector<std::string> skillsYouCanLearn; // Candidate teaches, you learn
    std::vector<std::string> skillsYouCanTeach; // You teach, candidate learns
    bool isMutual;

    Match()
        : candidateId(0), candidateName(""), candidateDepartment(""),
          candidateYear(0), compatibilityScore(0), explanation(""), isMutual(false) {}

    Match(int id, const std::string& name, const std::string& dept, int year,
          int score, const std::string& expl,
          const std::vector<std::string>& toLearn,
          const std::vector<std::string>& toTeach,
          bool mutual)
        : candidateId(id), candidateName(name), candidateDepartment(dept),
          candidateYear(year), compatibilityScore(score), explanation(expl),
          skillsYouCanLearn(toLearn), skillsYouCanTeach(toTeach), isMutual(mutual) {}

    void display() const;
};

/**
 * @brief Comparator functor for std::priority_queue.
 *
 * In C++ STL priority_queue, providing `operator(a, b)` returning true when `a` is
 * less than `b` creates a MAX-HEAP, placing the candidate with the highest compatibility
 * score at top().
 */
struct MatchComparator {
    bool operator()(const Match& a, const Match& b) const {
        if (a.compatibilityScore != b.compatibilityScore) {
            return a.compatibilityScore < b.compatibilityScore; // Max-heap: higher score wins
        }
        // Tie-breaker: lower ID first for consistent ordering
        return a.candidateId > b.candidateId;
    }
};

#endif // MATCH_H

# Campus Connect

Campus Connect is a terminal-based student networking and skill-matching
project written entirely in standard C++17.

## Current status

The project contains student management, skill matching, and a terminal menu
for connection requests. Profiles and requests are stored only in memory and
are cleared when the program exits. No files, database, or other persistence
is used.

## Skill matching

`CampusConnect::findMatches()` ranks candidates with this score:

```text
score = 10 * learningMatches
      + 15 * exchangeMatches
      + 5 * mutualExchangeBonus
      + 2 * sharedInterests
```

Results are stored in a `std::priority_queue`, so the highest score is
returned first. Candidates with equal scores are ordered by ascending student
ID for deterministic results. Finding matches only calculates and displays
rankings; it does not send connection requests.

## Build

From the project root, run:

```text
g++ -std=c++17 -Wall -Wextra -g -I include src/main.cpp src/student.cpp src/campus_connect.cpp -o CampusConnect.exe
```

Run the program with:

```text
.\CampusConnect.exe
```

The same build is available through the **Build Campus Connect** task in VS
Code.

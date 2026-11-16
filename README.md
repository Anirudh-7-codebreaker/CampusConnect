# Campus Connect

Campus Connect is a terminal-based student networking and skill-matching
project written entirely in standard C++17.

## Current status

The project contains student management, skill matching, connection requests,
and simple file handling for newly registered student names. Each successful
registration appends one name to `Saved New Students.txt`.

## Skill matching

`CampusConnect::findMatches()` ranks candidates with this score:

```text
score = 15 * sharedTeachableSkills
      + 2 * sharedInterests
```

Results are stored in a `std::priority_queue`, so the highest score is
returned first. Candidates with equal scores are ordered by ascending student
ID for deterministic results. Finding matches only calculates and displays
rankings; it does not send connection requests.

## Build

From the project root, run:

```text
g++ -std=c++17 -Wall -Wextra -g -I include src/main.cpp src/student.cpp src/campus_connect.cpp src/file_handler.cpp -o CampusConnect.exe
```

Run the program with:

```text
.\CampusConnect.exe
```

The same build is available through the **Build Campus Connect** task in VS
Code.

## File handling

Names are appended with `std::ofstream` and `std::ios::app`, so existing names
are not overwritten. The file is created automatically when needed, and no
entry is written when registration fails.

# Campus Connect

Campus Connect is a terminal-based student networking and skill-matching
project written entirely in standard C++17.

## Current status

The project contains student management, skill matching, connection requests,
and simple file handling for student profiles. Student profiles are saved in
`data/students.txt` when the user chooses **Save Students** or exits.

Each line in the file uses this simple format:

```text
id|name|department|year|skill1;skill2|interest1;interest2
```

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
g++ -std=c++17 -Wall -Wextra -g -I include src/main.cpp src/student.cpp src/campus_connect.cpp -o CampusConnect.exe
```

Run the program with:

```text
.\CampusConnect.exe
```

The same build is available through the **Build Campus Connect** task in VS
Code.

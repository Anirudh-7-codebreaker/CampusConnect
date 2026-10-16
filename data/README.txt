SkillSwap Data Directory
=========================

This directory contains persistent flat-file records used by the FileManager module.

1. students.txt
---------------
Stores registered student profiles.
Format:
  ID|Name|Department|AcademicYear|TeachableSkills|DesiredSkills|Interests

Field Definitions:
  - ID             : Unique positive integer (e.g., 101, 102).
  - Name           : Student's full name.
  - Department     : Academic department/major.
  - AcademicYear   : Integer between 1 and 6.
  - TeachableSkills: Semicolon-separated list of "SkillName:Category".
  - DesiredSkills  : Semicolon-separated list of "SkillName:Category".
  - Interests      : Comma-separated list of extracurricular interests/tags.

2. requests.txt
---------------
Stores the history of connection requests sent between students.
Format:
  RequestID|SenderStudentID|ReceiverStudentID|Status|Timestamp

Field Definitions:
  - RequestID        : Unique sequential integer.
  - SenderStudentID  : ID of student initiating the connection.
  - ReceiverStudentID: ID of prospective peer.
  - Status           : PENDING | ACCEPTED | REJECTED.
  - Timestamp        : Formatted date-time string (YYYY-MM-DD HH:MM).

Persistence Notes:
- Lines beginning with '#' are treated as comments and ignored.
- Empty lines are skipped.
- In-memory data structures (std::vector, std::unordered_map, std::queue, std::list)
  are populated from these files on startup and serialized back on mutation or application exit.

# Campus Connect 🎓

Campus Connect is a C++-based student networking project designed to help university students discover peers with similar interests and connect with potential collaborators.

The project is currently a **console-based prototype**. It focuses on implementing the core student discovery and connection request workflow before moving toward a full-stack application.

## ✨ Features

- **Student Profiles:** Store student details, including name, department, academic year, and interests.
- **Interest-Based Discovery:** Search for students who share a particular interest.
- **Connection Requests:** Select a student and send a connection request.
- **Request Management:** Simulate accepting or rejecting a connection request.
- **Input Validation:** Check whether the selected student is a valid match.

## 🛠️ Tech Stack

- **Language:** C++
- **Standard:** C++17
- **Data Structures:** Classes, structs, vectors, and strings
- **Compiler:** GCC (MinGW on Windows)

## 📁 Project Structure

```text
CampusConnect/
├── .vscode/
│   └── tasks.json
├── include/
│   └── student.h
├── src/
│   └── main.cpp
├── .gitignore
└── README.md
```

## 🚀 Getting Started

### Prerequisites

- A C++ compiler, such as GCC through MinGW
- Visual Studio Code (recommended)
- Git (optional, for version control)

### Compile the Project

Open a terminal in the project root and run:

```bash
g++ -std=c++17 -Wall -Wextra -I ./include ./src/main.cpp -o CampusConnect.exe
```

### Run the Project

On Windows PowerShell:

```powershell
.\CampusConnect.exe
```

## 🗺️ Roadmap

- [x] Create student profiles using a C++ class.
- [x] Discover students by shared interests.
- [x] Implement a basic connection request workflow.
- [ ] Add a menu-driven interface.
- [ ] Support multiple interests per search.
- [ ] Store student profiles and connection requests persistently.
- [ ] Introduce a backend and database.
- [ ] Develop a web interface for student networking.
- [ ] Explore real-time messaging.

## 🎯 Project Goal

The long-term goal is to build a university networking platform where students can discover like-minded peers, collaborate on projects, and build meaningful academic and professional connections.

## 📌 Current Limitations

- Student profiles are currently hardcoded.
- Data is stored in memory and is lost when the program exits.
- Connection requests are simulated rather than exchanged between real users.
- The application currently runs only in the terminal.

## 👨‍💻 Development

Campus Connect is being developed incrementally, starting with core C++ concepts and gradually progressing toward a more complete networking application.


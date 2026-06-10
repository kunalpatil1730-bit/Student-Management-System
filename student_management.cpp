#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

// ─────────────────────────────────────────────
//  Data Model
// ─────────────────────────────────────────────
struct Student {
    int    id;
    string name;
    int    age;
    string branch;
    float  gpa;
};

// ─────────────────────────────────────────────
//  Constants
// ─────────────────────────────────────────────
const string FILE_NAME = "students.dat";
const string SEPARATOR = "────────────────────────────────────────────────────────────";

// ─────────────────────────────────────────────
//  Utility helpers
// ─────────────────────────────────────────────
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseScreen() {
    cout << "\n  Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void printHeader(const string& title) {
    cout << "\n  " << SEPARATOR << "\n";
    cout << "   ★  " << title << "\n";
    cout << "  " << SEPARATOR << "\n\n";
}

// ─────────────────────────────────────────────
//  File I/O
// ─────────────────────────────────────────────

// Encode one student to a single CSV line
string encode(const Student& s) {
    ostringstream oss;
    oss << s.id << "|" << s.name << "|" << s.age << "|"
        << s.branch << "|" << fixed << setprecision(2) << s.gpa;
    return oss.str();
}

// Decode one CSV line back to a Student; returns false on bad data
bool decode(const string& line, Student& s) {
    istringstream iss(line);
    string token;
    try {
        getline(iss, token, '|'); s.id     = stoi(token);
        getline(iss, token, '|'); s.name   = token;
        getline(iss, token, '|'); s.age    = stoi(token);
        getline(iss, token, '|'); s.branch = token;
        getline(iss, token, '|'); s.gpa    = stof(token);
        return true;
    } catch (...) {
        return false;
    }
}

vector<Student> loadAll() {
    vector<Student> list;
    ifstream fin(FILE_NAME);
    if (!fin.is_open()) return list;          // file doesn't exist yet
    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        Student s;
        if (decode(line, s)) list.push_back(s);
    }
    fin.close();
    return list;
}

bool saveAll(const vector<Student>& list) {
    ofstream fout(FILE_NAME, ios::trunc);
    if (!fout.is_open()) return false;
    for (const auto& s : list) fout << encode(s) << "\n";
    fout.close();
    return true;
}

// ─────────────────────────────────────────────
//  ID helpers
// ─────────────────────────────────────────────
bool idExists(const vector<Student>& list, int id) {
    for (const auto& s : list)
        if (s.id == id) return true;
    return false;
}

int nextId(const vector<Student>& list) {
    int mx = 0;
    for (const auto& s : list) mx = max(mx, s.id);
    return mx + 1;
}

// ─────────────────────────────────────────────
//  Display helpers
// ─────────────────────────────────────────────
void printTableHeader() {
    cout << "  " << left
         << setw(6)  << "ID"
         << setw(22) << "Name"
         << setw(6)  << "Age"
         << setw(14) << "Branch"
         << setw(8)  << "GPA"
         << "\n";
    cout << "  " << string(56, '-') << "\n";
}

void printRow(const Student& s) {
    cout << "  " << left
         << setw(6)  << s.id
         << setw(22) << s.name
         << setw(6)  << s.age
         << setw(14) << s.branch
         << fixed << setprecision(2) << setw(8) << s.gpa
         << "\n";
}

// ─────────────────────────────────────────────
//  Input helpers
// ─────────────────────────────────────────────
string readString(const string& prompt) {
    string val;
    cout << "  " << prompt;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, val);
    return val;
}

// Overload that doesn't ignore first (used after non-string reads)
string readStringNoIgnore(const string& prompt) {
    string val;
    cout << "  " << prompt;
    getline(cin, val);
    return val;
}

int readInt(const string& prompt) {
    int val;
    while (true) {
        cout << "  " << prompt;
        if (cin >> val) { return val; }
        cout << "  [!] Invalid input. Please enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

float readFloat(const string& prompt) {
    float val;
    while (true) {
        cout << "  " << prompt;
        if (cin >> val) { return val; }
        cout << "  [!] Invalid input. Please enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// ─────────────────────────────────────────────
//  Operations
// ─────────────────────────────────────────────

// 1. ADD
void addStudent() {
    clearScreen();
    printHeader("ADD NEW STUDENT");

    vector<Student> list = loadAll();
    Student s;
    s.id = nextId(list);
    cout << "  Auto-assigned ID : " << s.id << "\n\n";

    s.name   = readString("Full Name   : ");
    s.age    = readInt   ("Age         : ");
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    s.branch = readStringNoIgnore("Branch      : ");
    s.gpa    = readFloat ("GPA (0-4)   : ");

    if (s.gpa < 0 || s.gpa > 4) {
        cout << "\n  [!] GPA must be between 0.00 and 4.00. Record not saved.\n";
        pauseScreen(); return;
    }
    if (s.age < 1 || s.age > 120) {
        cout << "\n  [!] Age out of valid range. Record not saved.\n";
        pauseScreen(); return;
    }

    list.push_back(s);
    if (saveAll(list))
        cout << "\n  [✓] Student added successfully (ID = " << s.id << ").\n";
    else
        cout << "\n  [✗] Error writing to file.\n";

    pauseScreen();
}

// 2. DISPLAY ALL
void displayAll() {
    clearScreen();
    printHeader("ALL STUDENT RECORDS");

    vector<Student> list = loadAll();
    if (list.empty()) {
        cout << "  No records found.\n";
        pauseScreen(); return;
    }

    printTableHeader();
    for (const auto& s : list) printRow(s);
    cout << "\n  Total records: " << list.size() << "\n";
    pauseScreen();
}

// 3. SEARCH
void searchStudent() {
    clearScreen();
    printHeader("SEARCH STUDENT");

    int id = readInt("Enter Student ID to search: ");
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<Student> list = loadAll();
    for (const auto& s : list) {
        if (s.id == id) {
            cout << "\n  Record found:\n\n";
            printTableHeader();
            printRow(s);
            pauseScreen();
            return;
        }
    }
    cout << "\n  [!] No student found with ID = " << id << ".\n";
    pauseScreen();
}

// 4. UPDATE
void updateStudent() {
    clearScreen();
    printHeader("UPDATE STUDENT RECORD");

    int id = readInt("Enter Student ID to update: ");
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<Student> list = loadAll();
    bool found = false;
    for (auto& s : list) {
        if (s.id == id) {
            found = true;
            cout << "\n  Current record:\n";
            printTableHeader(); printRow(s);
            cout << "\n  Enter new values (leave blank = keep current):\n\n";

            // Name
            cout << "  New Name   [" << s.name << "] : ";
            string tmp; getline(cin, tmp);
            if (!tmp.empty()) s.name = tmp;

            // Age
            cout << "  New Age    [" << s.age << "] : ";
            getline(cin, tmp);
            if (!tmp.empty()) {
                try { int a = stoi(tmp); if (a>0&&a<121) s.age=a; else cout<<"  [!] Invalid age kept old.\n"; }
                catch (...) { cout<<"  [!] Invalid input, kept old.\n"; }
            }

            // Branch
            cout << "  New Branch [" << s.branch << "] : ";
            getline(cin, tmp);
            if (!tmp.empty()) s.branch = tmp;

            // GPA
            cout << "  New GPA    [" << fixed<<setprecision(2)<<s.gpa << "] : ";
            getline(cin, tmp);
            if (!tmp.empty()) {
                try { float g = stof(tmp); if(g>=0&&g<=4) s.gpa=g; else cout<<"  [!] GPA out of range, kept old.\n"; }
                catch (...) { cout<<"  [!] Invalid input, kept old.\n"; }
            }
            break;
        }
    }

    if (!found) {
        cout << "\n  [!] No student found with ID = " << id << ".\n";
        pauseScreen(); return;
    }

    if (saveAll(list))
        cout << "\n  [✓] Record updated successfully.\n";
    else
        cout << "\n  [✗] Error writing to file.\n";

    pauseScreen();
}

// 5. DELETE
void deleteStudent() {
    clearScreen();
    printHeader("DELETE STUDENT RECORD");

    int id = readInt("Enter Student ID to delete: ");
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<Student> list = loadAll();
    auto before = list.size();
    list.erase(remove_if(list.begin(), list.end(),
                         [id](const Student& s){ return s.id == id; }),
               list.end());

    if (list.size() == before) {
        cout << "\n  [!] No student found with ID = " << id << ".\n";
        pauseScreen(); return;
    }

    cout << "  Are you sure you want to delete this record? (y/n): ";
    char ch; cin >> ch;
    if (ch != 'y' && ch != 'Y') {
        cout << "\n  Deletion cancelled.\n";
        pauseScreen(); return;
    }

    if (saveAll(list))
        cout << "\n  [✓] Record deleted successfully.\n";
    else
        cout << "\n  [✗] Error writing to file.\n";

    pauseScreen();
}

// 6. DISPLAY SORTED
void displaySorted() {
    clearScreen();
    printHeader("DISPLAY SORTED RECORDS");

    cout << "  Sort by:\n";
    cout << "    1. Name (A-Z)\n";
    cout << "    2. GPA  (High → Low)\n";
    cout << "    3. ID   (Ascending)\n";
    int ch = readInt("Choice: ");
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<Student> list = loadAll();
    if (list.empty()) { cout << "\n  No records found.\n"; pauseScreen(); return; }

    switch (ch) {
        case 1: sort(list.begin(), list.end(),
                     [](const Student& a, const Student& b){ return a.name < b.name; }); break;
        case 2: sort(list.begin(), list.end(),
                     [](const Student& a, const Student& b){ return a.gpa > b.gpa; }); break;
        case 3: sort(list.begin(), list.end(),
                     [](const Student& a, const Student& b){ return a.id < b.id; }); break;
        default: cout << "\n  Invalid choice.\n"; pauseScreen(); return;
    }

    cout << "\n";
    printTableHeader();
    for (const auto& s : list) printRow(s);
    cout << "\n  Total records: " << list.size() << "\n";
    pauseScreen();
}

// 7. STATISTICS
void showStats() {
    clearScreen();
    printHeader("STUDENT STATISTICS");

    vector<Student> list = loadAll();
    if (list.empty()) { cout << "  No records found.\n"; pauseScreen(); return; }

    float total = 0, hi = list[0].gpa, lo = list[0].gpa;
    int   topIdx = 0, loIdx = 0;
    for (int i = 0; i < (int)list.size(); i++) {
        total += list[i].gpa;
        if (list[i].gpa > hi) { hi = list[i].gpa; topIdx = i; }
        if (list[i].gpa < lo) { lo = list[i].gpa; loIdx  = i; }
    }
    float avg = total / list.size();

    // branch count
    vector<pair<string,int>> branches;
    for (const auto& s : list) {
        bool found = false;
        for (auto& b : branches) if (b.first == s.branch) { b.second++; found=true; break; }
        if (!found) branches.push_back({s.branch, 1});
    }

    cout << "  Total Students  : " << list.size() << "\n";
    cout << fixed << setprecision(2);
    cout << "  Average GPA     : " << avg << "\n";
    cout << "  Highest GPA     : " << hi << "  (" << list[topIdx].name << ", ID " << list[topIdx].id << ")\n";
    cout << "  Lowest  GPA     : " << lo << "  (" << list[loIdx].name  << ", ID " << list[loIdx].id  << ")\n";
    cout << "\n  Students per Branch:\n";
    for (const auto& b : branches)
        cout << "    " << left << setw(16) << b.first << " : " << b.second << "\n";

    pauseScreen();
}

// ─────────────────────────────────────────────
//  Main Menu
// ─────────────────────────────────────────────
void showMenu() {
    clearScreen();
    cout << "\n";
    cout << "  ╔══════════════════════════════════════╗\n";
    cout << "  ║    STUDENT MANAGEMENT SYSTEM  v1.0   ║\n";
    cout << "  ╠══════════════════════════════════════╣\n";
    cout << "  ║  1.  Add Student                     ║\n";
    cout << "  ║  2.  Display All Students            ║\n";
    cout << "  ║  3.  Search Student by ID            ║\n";
    cout << "  ║  4.  Update Student Record           ║\n";
    cout << "  ║  5.  Delete Student Record           ║\n";
    cout << "  ║  6.  Display Sorted Records          ║\n";
    cout << "  ║  7.  Statistics                      ║\n";
    cout << "  ║  0.  Exit                            ║\n";
    cout << "  ╚══════════════════════════════════════╝\n";
    cout << "\n  Enter your choice: ";
}

int main() {
    int choice;
    do {
        showMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }
        switch (choice) {
            case 1: addStudent();     break;
            case 2: displayAll();     break;
            case 3: searchStudent();  break;
            case 4: updateStudent();  break;
            case 5: deleteStudent();  break;
            case 6: displaySorted();  break;
            case 7: showStats();      break;
            case 0:
                clearScreen();
                cout << "\n  Goodbye! Data saved to '" << FILE_NAME << "'.\n\n";
                break;
            default:
                cout << "\n  [!] Invalid option. Try again.\n";
                pauseScreen();
        }
    } while (choice != 0);

    return 0;
}

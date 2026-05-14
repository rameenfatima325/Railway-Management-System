#pragma once
#include "types.h"
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;

class OperationLog {
private:
    LogEntry* top;
    int       logCount;

    void destroyLog() {
        while (top != nullptr) {
            LogEntry* temp = top;
            top = top->next;
            delete temp;
        }
        logCount = 0;
    }

    string currentTimestamp() {
        time_t now = time(nullptr);
        tm t;
#ifdef _WIN32
        localtime_s(&t, &now);
#else
        localtime_r(&now, &t);
#endif
        string h = to_string(t.tm_hour);
        string m = to_string(t.tm_min);
        string s = to_string(t.tm_sec);

        if (h.size() < 2) h = "0" + h;
        if (m.size() < 2) m = "0" + m;
        if (s.size() < 2) s = "0" + s;

        return h + ":" + m + ":" + s;
    }

public:
    OperationLog() : top(nullptr), logCount(0) {}
    ~OperationLog() { destroyLog(); }

    void push(string action, string moduleSource) {
        LogEntry* entry = new LogEntry;
        entry->action = action;
        entry->moduleSource = moduleSource;
        entry->timestamp = currentTimestamp();
        entry->next = top;
        top = entry;
        ++logCount;
    }

    void pushRaw(LogEntry* entry) {
        entry->next = top;
        top = entry;
        ++logCount;
    }

    void pop() {
        if (top == nullptr) {
            cout << "[LOG] Stack is already empty." << endl;
            return;
        }
        LogEntry* temp = top;
        top = top->next;
        delete temp;
        --logCount;
    }

    LogEntry* peek() { return top; }

    void displayRecentLogs(int n) {
        if (top == nullptr) {
            cout << "[LOG] No log entries found." << endl;
            return;
        }
        cout << "===== Recent " << n << " Log Entries =====" << endl;
        LogEntry* cur = top;
        int count = 0;
        while (cur != nullptr && count < n) {
            cout << "[" << cur->timestamp << "] "
                << "[" << cur->moduleSource << "] "
                << cur->action << endl;
            cur = cur->next;
            ++count;
        }
        cout << "=================================" << endl;
    }

    void displayAllLogs() {
        if (top == nullptr) {
            cout << "[LOG] No log entries found." << endl;
            return;
        }
        cout << "===== Complete Operation Log (" << logCount << " entries) =====" << endl;
        LogEntry* cur = top;
        int index = 1;
        while (cur != nullptr) {
            cout << index++ << ". "
                << "[" << cur->timestamp << "] "
                << "[" << cur->moduleSource << "] "
                << cur->action << endl;
            cur = cur->next;
        }
        cout << "===================================================" << endl;
    }

    bool isEmpty() { return top == nullptr; }
    int  getLogCount() { return logCount; }

    void saveToFile(string filename) {
        ofstream file(filename.c_str());
        if (!file.is_open()) {
            cout << "[LOG] Error: could not open '" << filename << "' for writing." << endl;
            return;
        }
        LogEntry* cur = top;
        while (cur != nullptr) {
            file << cur->timestamp << "|" << cur->moduleSource << "|" << cur->action << endl;
            cur = cur->next;
        }
        file.close();
        cout << "[LOG] Log saved to '" << filename << "' (" << logCount << " entries)." << endl;
    }

    void loadFromFile(string filename) {
        ifstream file(filename.c_str());
        if (!file.is_open()) {
            cout << "[LOG] Error: could not open '" << filename << "' for reading." << endl;
            return;
        }

        destroyLog();

        string lines[1000];
        int lineCount = 0;
        string line;
        while (getline(file, line) && lineCount < 1000) {
            lines[lineCount++] = line;
        }
        file.close();

        for (int i = lineCount - 1; i >= 0; --i) {
            string& ln = lines[i];
            size_t p1 = ln.find('|');
            size_t p2 = ln.find('|', p1 + 1);
            if (p1 == string::npos || p2 == string::npos) continue;

            string ts = ln.substr(0, p1);
            string mod = ln.substr(p1 + 1, p2 - p1 - 1);
            string action = ln.substr(p2 + 1);

            LogEntry* entry = new LogEntry;
            entry->timestamp = ts;
            entry->moduleSource = mod;
            entry->action = action;
            entry->next = top;
            top = entry;
            ++logCount;
        }
        cout << "[LOG] Log loaded from '" << filename << "' (" << logCount << " entries)." << endl;
    }
};
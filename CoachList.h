#pragma once
#include "types.h"
#include "Seatingchart.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class CoachList {
private:
    CoachNode* head;
    CoachNode* tail;
    int coachCount;

public:
    CoachList() {
        head = nullptr;
        tail = nullptr;
        coachCount = 0;
    }

    ~CoachList() {
        CoachNode* current = head;
        while (current != nullptr) {
            CoachNode* nextCoach = current->next;
            if (current->seats != nullptr) {
                delete current->seats;
            }
            delete current;
            current = nextCoach;
        }
    }

    void addCoachFront(string coachID, string type, int seats) {
        if (findCoach(coachID) != nullptr) {
            cout << "Error: Coach " << coachID << " already exists!" << endl;
            return;
        }

        CoachNode* newNode = new CoachNode();
        newNode->coachID = coachID;
        newNode->coachType = type;
        newNode->totalSeats = seats;
        newNode->seats = nullptr;
        newNode->prev = nullptr;
        newNode->next = nullptr;

        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        }
        else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }
        coachCount++;
    }

    void addCoachEnd(string coachID, string type, int seats) {
        if (findCoach(coachID) != nullptr) {
            cout << "Error: Coach " << coachID << " already exists!" << endl;
            return;
        }

        CoachNode* newNode = new CoachNode();
        newNode->coachID = coachID;
        newNode->coachType = type;
        newNode->totalSeats = seats;
        newNode->seats = nullptr;
        newNode->prev = nullptr;
        newNode->next = nullptr;

        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        }
        else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        coachCount++;
    }


    void addCoachAt(int position, string coachID, string type, int seats) {
        if (findCoach(coachID) != nullptr) {
            cout << "Error: Coach " << coachID << " already exists!" << endl;
            return;
        }

        if (position <= 1) {
            addCoachFront(coachID, type, seats);
            return;
        }
        if (position > coachCount) {
            addCoachEnd(coachID, type, seats);
            return;
        }

        CoachNode* current = head;
        for (int i = 1; i < position; i++) {
            current = current->next;
        }

        CoachNode* newNode = new CoachNode();
        newNode->coachID = coachID;
        newNode->coachType = type;
        newNode->totalSeats = seats;
        newNode->seats = nullptr;

        newNode->next = current;
        newNode->prev = current->prev;
        if (current->prev != nullptr)
            current->prev->next = newNode;
        else
            head = newNode;
        current->prev = newNode;
        coachCount++;
    }

    void removeCoach(string coachID) {
        CoachNode* target = findCoach(coachID);
        if (target == nullptr) {
            cout << "Error: Coach " << coachID << " not found!" << endl;
            return;
        }
        if (target == head) {
            head = target->next;
            if (head != nullptr)
                head->prev = nullptr;
        }
        else if (target == tail) {
            tail = target->prev;
            if (tail != nullptr)
                tail->next = nullptr;
        }
        else {
            target->prev->next = target->next;
            target->next->prev = target->prev;
        }
        if (target->seats != nullptr) {
            delete target->seats;
        }
        delete target;
        coachCount--;
    }

    CoachNode* findCoach(string coachID) {
        CoachNode* current = head;
        while (current != nullptr) {
            if (current->coachID == coachID)
                return current;
            current = current->next;
        }
        return nullptr;
    }

    void reverseCoachOrder() {
        if (head == nullptr || head->next == nullptr)
            return;
        CoachNode* current = head;
        CoachNode* temp = nullptr;
        while (current != nullptr) {
            temp = current->prev;
            current->prev = current->next;
            current->next = temp;
            current = current->prev;
        }
        temp = head;
        head = tail;
        tail = temp;
    }

    void displayCoaches() {
        if (head == nullptr) {
            cout << "This train has no coaches attached." << endl;
            return;
        }
        cout << "TRAIN COACHES (Front to Back)" << endl;
        CoachNode* current = head;
        int position = 1;
        while (current != nullptr) {
            cout << "[" << position << "] Coach ID: " << current->coachID
                << " | Type: " << current->coachType
                << " | Seats: " << current->totalSeats << endl;
            current = current->next;
            position++;
        }
        cout << "-------------------------------------" << endl;
    }

    int getCoachCount() { return coachCount; }

    string getTailID() {
        if (tail == nullptr) return "";
        return tail->coachID;
    }

    string getCoachIDAt(int position) {
        if (position < 1) return "";
        CoachNode* current = head;
        for (int i = 1; i < position && current != nullptr; i++)
            current = current->next;
        if (current == nullptr) return "";
        return current->coachID;
    }

    int getCoachPosition(string coachID) {
        CoachNode* current = head;
        int pos = 1;
        while (current != nullptr) {
            if (current->coachID == coachID) return pos;
            current = current->next;
            pos++;
        }
        return -1;
    }

    void saveToFile(string filename) {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cout << "Error opening file for coaches!" << endl;
            return;
        }

        CoachNode* current = head;
        int coachIndex = 0;
        while (current != nullptr) {
            outFile << current->coachID << "," << current->coachType << "," << current->totalSeats << endl;

            if (current->seats != nullptr) {
                string base = filename.substr(0, filename.find_last_of("."));
                string seatFilename = base + "_coach_" + to_string(coachIndex) + ".txt";
                string seatStructFilename = base + "_coach_" + to_string(coachIndex) + "_structure.txt";
                current->seats->saveToFile(seatFilename);
                current->seats->saveStructureToFile(seatStructFilename);
            }

            current = current->next;
            coachIndex++;
        }
        outFile.close();
        cout << "Coaches saved successfully." << endl;
    }

    void loadFromFile(string filename) {
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            cout << "Error: Coach file not found!" << endl;
            return;
        }
        string line;
        int coachIndex = 0;
        while (getline(inFile, line)) {
            size_t firstComma = line.find(',');
            size_t secondComma = (firstComma != string::npos)
                ? line.find(',', firstComma + 1)
                : string::npos;
            if (firstComma == string::npos || secondComma == string::npos)
                continue;

            string id = line.substr(0, firstComma);
            string type = line.substr(firstComma + 1, secondComma - firstComma - 1);
            string tempSeats = line.substr(secondComma + 1);
            int seats = stoi(tempSeats);
            addCoachEnd(id, type, seats);

            CoachNode* loadedCoach = findCoach(id);
            if (loadedCoach != nullptr) {
                string base = filename.substr(0, filename.find_last_of("."));
                string seatFilename = base + "_coach_" + to_string(coachIndex) + ".txt";
                loadedCoach->seats = new SeatingChart(seats);
                loadedCoach->seats->loadFromFile(seatFilename);
            }
            coachIndex++;
        }
        inFile.close();
        cout << "Coaches loaded successfully." << endl;
    }
};
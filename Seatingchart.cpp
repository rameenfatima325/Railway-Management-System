#include "Seatingchart.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

static void writeBSTInorder(SeatNode* node, ofstream& file) {
    if (node == nullptr)
        return;
    writeBSTInorder(node->left, file);
    file << "SEAT " << node->seatNumber << " " << node->isBooked << " ";
    if (node->isBooked && !node->passengerName.empty())
        file << node->passengerName;
    else
        file << "NONE";
    file << endl;
    writeBSTInorder(node->right, file);
}

SeatingChart::SeatingChart(int numSeats) : bstRoot(nullptr), totalSeats(0) {
    for (int i = 0; i < 50; i++)
        hashTable[i] = nullptr;
    if (numSeats > 0) {
        initializeSeats(numSeats);
    }
}

SeatingChart::~SeatingChart() {
    destroyBST(bstRoot);
    for (int i = 0; i < 50; i++)
        hashTable[i] = nullptr;
}

int SeatingChart::hashFunction(int seatNumber) {
    return seatNumber % 50;
}

SeatNode* SeatingChart::hashSearch(int seatNum) {
    int index = hashFunction(seatNum);
    SeatNode* cur = hashTable[index];
    while (cur != nullptr) {
        if (cur->seatNumber == seatNum)
            return cur;
        cur = cur->next;
    }
    return nullptr;
}

SeatNode* SeatingChart::bstInsert(SeatNode* root, SeatNode* newNode) {
    if (root == nullptr)
        return newNode;

    if (newNode->seatNumber < root->seatNumber)
        root->left = bstInsert(root->left, newNode);
    else if (newNode->seatNumber > root->seatNumber)
        root->right = bstInsert(root->right, newNode);

    return root;
}

SeatNode* SeatingChart::bstMinNode(SeatNode* node) {
    SeatNode* cur = node;
    while (cur && cur->left != nullptr)
        cur = cur->left;
    return cur;
}

SeatNode* SeatingChart::bstDelete(SeatNode* root, int seatNum) {
    if (root == nullptr)
        return nullptr;

    if (seatNum < root->seatNumber) {
        root->left = bstDelete(root->left, seatNum);
    }
    else if (seatNum > root->seatNumber) {
        root->right = bstDelete(root->right, seatNum);
    }
    else {
        int idx = hashFunction(root->seatNumber);
        if (hashTable[idx] == root) {
            hashTable[idx] = root->next;
        }
        else {
            SeatNode* prev = hashTable[idx];
            while (prev != nullptr && prev->next != root)
                prev = prev->next;
            if (prev != nullptr)
                prev->next = root->next;
        }
        root->next = nullptr; 

        if (root->left == nullptr) {
            SeatNode* temp = root->right;
            delete root;
            return temp;
        }
        else if (root->right == nullptr) {
            SeatNode* temp = root->left;
            delete root;
            return temp;
        }
        else {
            SeatNode* successor = bstMinNode(root->right);
            root->seatNumber = successor->seatNumber;
            root->isBooked = successor->isBooked;
            root->passengerName = successor->passengerName;
            int succIdx = hashFunction(successor->seatNumber);
            if (hashTable[succIdx] == successor) {
                hashTable[succIdx] = root;
            }
            else {
                SeatNode* prev = hashTable[succIdx];
                while (prev != nullptr && prev->next != successor)
                    prev = prev->next;
                if (prev != nullptr)
                    prev->next = root;
            }
            root->next = successor->next;
            successor->next = nullptr;
            root->right = bstDelete(root->right, successor->seatNumber);
        }
    }
    return root;
}

SeatNode* SeatingChart::bstSearch(SeatNode* root, int seatNum) {
    if (root == nullptr)
        return nullptr;
    if (seatNum == root->seatNumber)
        return root;
    if (seatNum < root->seatNumber)
        return bstSearch(root->left, seatNum);
    return bstSearch(root->right, seatNum);
}

void SeatingChart::bstInorder(SeatNode* root) {
    if (root == nullptr)
        return;
    bstInorder(root->left);
    cout << "  Seat " << root->seatNumber
        << " | " << (root->isBooked ? "BOOKED" : "AVAILABLE");
    if (root->isBooked)
        cout << " | Passenger: " << root->passengerName;
    cout << endl;
    bstInorder(root->right);
}

void SeatingChart::bstInorderBooked(SeatNode* root) {
    if (root == nullptr)
        return;
    bstInorderBooked(root->left);
    if (root->isBooked)
        cout << "  Seat " << root->seatNumber
        << " | Passenger: " << root->passengerName << endl;
    bstInorderBooked(root->right);
}

void SeatingChart::bstInorderAvailable(SeatNode* root) {
    if (root == nullptr)
        return;
    bstInorderAvailable(root->left);
    if (!root->isBooked)
        cout << "  Seat " << root->seatNumber << endl;
    bstInorderAvailable(root->right);
}

void SeatingChart::destroyBST(SeatNode* node) {
    if (node == nullptr)
        return;
    destroyBST(node->left);
    destroyBST(node->right);
    delete node;
}

void SeatingChart::saveBSTPreorder(SeatNode* root, ofstream& file) {
    if (root == nullptr) {
        file << "# ";
        return;
    }

    file << root->seatNumber << " " << (root->isBooked ? 1 : 0) << " ";
    if (root->isBooked && !root->passengerName.empty()) {
        string safeName = root->passengerName;
        for (size_t k = 0; k < safeName.size(); k++)
            if (safeName[k] == ' ') safeName[k] = '_';
        file << safeName;
    }
    else {
        file << "NONE";
    }
    file << " ";

    saveBSTPreorder(root->left, file);
    saveBSTPreorder(root->right, file);
}

SeatNode* SeatingChart::loadBSTFromPreorder(ifstream& file, int& lineCount, int maxSeats) {
    string token;
    if (!(file >> token)) {
        return nullptr;
    }

    if (token == "#") {
        return nullptr;
    }

    int seatNum = stoi(token);
    if (seatNum < 1 || seatNum > maxSeats) {
        return nullptr;
    }

    int booked;
    string passengerName;
    file >> booked >> passengerName;

    if (passengerName != "NONE") {
        for (size_t k = 0; k < passengerName.size(); k++)
            if (passengerName[k] == '_') passengerName[k] = ' ';
    }
    else {
        passengerName = "";
    }

    SeatNode* node = new SeatNode();
    node->seatNumber = seatNum;
    node->isBooked = (booked == 1);
    node->passengerName = passengerName;
    node->left = nullptr;
    node->right = nullptr;

    int hashIdx = hashFunction(seatNum);
    node->next = hashTable[hashIdx];
    hashTable[hashIdx] = node;

    node->left = loadBSTFromPreorder(file, lineCount, maxSeats);
    node->right = loadBSTFromPreorder(file, lineCount, maxSeats);

    return node;
}

void SeatingChart::initializeSeats(int numSeats) {
    totalSeats = numSeats;

    for (int seatNum = 1; seatNum <= numSeats; seatNum++) {
        SeatNode* node = new SeatNode();
        node->seatNumber = seatNum;
        node->isBooked = false;
        node->passengerName = "";
        node->left = nullptr;
        node->right = nullptr;
        node->next = nullptr;

        int index = hashFunction(seatNum);
        node->next = hashTable[index];
        hashTable[index] = node;

        bstRoot = bstInsert(bstRoot, node);
    }
}

bool SeatingChart::bookSeat(int seatNum, string passengerName) {
    if (seatNum < 1 || seatNum > totalSeats) {
        cout << "  [ERROR] Seat " << seatNum << " does not exist." << endl;
        return false;
    }

    SeatNode* seat = hashSearch(seatNum);
    if (seat == nullptr) {
        cout << "  [ERROR] Seat " << seatNum << " not found in system." << endl;
        return false;
    }
    if (seat->isBooked) {
        cout << "  [ERROR] Seat " << seatNum
            << " is already booked by " << seat->passengerName << "." << endl;
        return false;
    }

    seat->isBooked = true;
    seat->passengerName = passengerName;
    return true;
}

bool SeatingChart::cancelSeat(int seatNum) {
    if (seatNum < 1 || seatNum > totalSeats) {
        cout << "  [ERROR] Seat " << seatNum << " does not exist." << endl;
        return false;
    }

    SeatNode* seat = hashSearch(seatNum);
    if (seat == nullptr) {
        cout << "  [ERROR] Seat " << seatNum << " not found in system." << endl;
        return false;
    }
    if (!seat->isBooked) {
        cout << "  [ERROR] Seat " << seatNum << " is already available." << endl;
        return false;
    }

    seat->isBooked = false;
    seat->passengerName = "";
    return true;
}

bool SeatingChart::isSeatAvailable(int seatNum) {
    if (seatNum < 1 || seatNum > totalSeats)
        return false;

    SeatNode* seat = hashSearch(seatNum);
    if (seat == nullptr) return false;
    return !seat->isBooked;
}

string SeatingChart::getPassengerName(int seatNum) {
    SeatNode* seat = hashSearch(seatNum);
    if (seat == nullptr) return "";
    return seat->passengerName;
}

void SeatingChart::displayAllSeats() {
    cout << endl << "-------------------------------" << endl;
    cout << "|       ALL SEATS (sorted)    |" << endl;
    cout << "-------------------------------" << endl;
    if (bstRoot == nullptr)
        cout << "  (no seats initialised)\n";
    else
        bstInorder(bstRoot);
}

void SeatingChart::displayBookedSeats() {
    cout << endl << "-------------------------------" << endl;
    cout << "|       BOOKED SEATS          |" << endl;
    cout << "-------------------------------" << endl;
    if (bstRoot == nullptr)
        cout << "  (no seats initialised)\n";
    else
        bstInorderBooked(bstRoot);
}

void SeatingChart::displayAvailableSeats() {
    cout << endl << "-------------------------------" << endl;
    cout << "|       AVAILABLE SEATS       |" << endl;
    cout << "-------------------------------" << endl;
    if (bstRoot == nullptr)
        cout << "  (no seats initialised)\n";
    else
        bstInorderAvailable(bstRoot);
}

int SeatingChart::getTotalSeats() {
    return totalSeats;
}

int SeatingChart::getBookedCount() {
    int count = 0;
    for (int i = 0; i < 50; i++) {
        SeatNode* cur = hashTable[i];
        while (cur != nullptr) {
            if (cur->isBooked) count++;
            cur = cur->next;
        }
    }
    return count;
}

void SeatingChart::saveToFile(string filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "  [ERROR] Cannot open file: " << filename << endl;
        return;
    }

    file << "SEATING_CHART" << endl;
    file << "TOTAL_SEATS " << totalSeats << endl;

    for (int i = 0; i < 50; i++) {
        SeatNode* cur = hashTable[i];
        while (cur != nullptr) {
            if (cur->isBooked) {
                string safeName = cur->passengerName;
                for (size_t k = 0; k < safeName.size(); k++)
                    if (safeName[k] == ' ') safeName[k] = '_';
                file << "SEAT " << cur->seatNumber << " 1 " << safeName << endl;
            }
            cur = cur->next;
        }
    }

    file.close();
    cout << "  [OK] Seating chart saved to '" << filename << "' (flat)." << endl;
}

void SeatingChart::saveStructureToFile(string filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "  [ERROR] Cannot open structure file: " << filename << endl;
        return;
    }

    file << "SEATING_CHART_STRUCTURE" << endl;
    file << "TOTAL_SEATS " << totalSeats << endl;
    file << "BST_PREORDER ";
    saveBSTPreorder(bstRoot, file);
    file << endl;

    file.close();
    cout << "  [OK] BST structure saved to '" << filename << "' (preorder)." << endl;
}

void SeatingChart::loadFromFile(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "  [ERROR] Cannot open file: " << filename << endl;
        return;
    }

    string token;
    file >> token;

    if (token != "SEATING_CHART_STRUCTURE") {
        if (token == "SEATING_CHART") {
            int numSeats = 0;
            file >> token >> numSeats;

            destroyBST(bstRoot);
            bstRoot = nullptr;
            for (int i = 0; i < 50; i++) hashTable[i] = nullptr;
            totalSeats = 0;
            initializeSeats(numSeats);

            while (file >> token) {
                if (token != "SEAT")
                    break;
                int seatNum, booked;
                string passenger;
                file >> seatNum >> booked >> passenger;

                SeatNode* seat = hashSearch(seatNum);
                if (seat == nullptr)
                    continue;
                seat->isBooked = (booked == 1);
                if (passenger != "NONE") {
                    for (size_t k = 0; k < passenger.size(); k++)
                        if (passenger[k] == '_') passenger[k] = ' ';
                    seat->passengerName = passenger;
                }
                else {
                    seat->passengerName = "";
                }
            }

            file.close();
            cout << "  [OK] Seating chart loaded from '" << filename << endl;
            return;
        }

        cout << "  [ERROR] File format invalid." << endl;
        file.close();
        return;
    }

    int numSeats = 0;
    file >> token >> numSeats;

    destroyBST(bstRoot);
    bstRoot = nullptr;
    for (int i = 0; i < 50; i++) hashTable[i] = nullptr;
    totalSeats = 0;

    file >> token;

    int lineCount = 0;
    bstRoot = loadBSTFromPreorder(file, lineCount, numSeats);
    totalSeats = numSeats;

    file.close();
    cout << "  [OK] Seating chart loaded from '" << filename << endl;
}
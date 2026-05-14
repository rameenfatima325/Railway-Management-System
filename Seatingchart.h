#pragma once
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

struct SeatNode {
    int seatNumber;
    bool isBooked;
    string passengerName;
    SeatNode* left;
    SeatNode* right;
    SeatNode* next;

    SeatNode() : seatNumber(0), isBooked(false), passengerName(""),
        left(nullptr), right(nullptr), next(nullptr) {
    }
};

class SeatingChart {
private:
    SeatNode* hashTable[50];
    SeatNode* bstRoot;
    int totalSeats;

    int hashFunction(int seatNumber);

    SeatNode* bstInsert(SeatNode* root, SeatNode* newNode);
    SeatNode* bstDelete(SeatNode* root, int seatNum);
    SeatNode* bstSearch(SeatNode* root, int seatNum);
    SeatNode* bstMinNode(SeatNode* node);
    void bstInorder(SeatNode* root);
    void bstInorderBooked(SeatNode* root);
    void bstInorderAvailable(SeatNode* root);
    void destroyBST(SeatNode* node);

    void saveBSTPreorder(SeatNode* root, ofstream& file);
    SeatNode* loadBSTFromPreorder(ifstream& file, int& lineCount, int maxSeats);

    SeatNode* hashSearch(int seatNum);

public:
    SeatingChart(int numSeats = 0);
    ~SeatingChart();

    void initializeSeats(int numSeats);

    bool bookSeat(int seatNum, string passengerName);
    bool cancelSeat(int seatNum);
    bool isSeatAvailable(int seatNum);
    string getPassengerName(int seatNum);

    void displayAllSeats();
    void displayBookedSeats();
    void displayAvailableSeats();

    int getTotalSeats();
    int getBookedCount();

    void saveToFile(string filename);
    void saveStructureToFile(string filename);
    void loadFromFile(string filename);
};
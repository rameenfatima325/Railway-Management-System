#pragma once
#include <iostream>
#include <string>
using namespace std;

class SeatingChart;
class CoachList;


struct CoachNode {
    string coachID;     
    string coachType;  
    int totalSeats;
    SeatingChart* seats;    
    CoachNode* prev;
    CoachNode* next;
};


struct TrainNode {
    int trainID;
    string trainName;
    string route;       
    CoachList* coaches;     
    int height;      
    TrainNode* left;
    TrainNode* right;
};


struct LogEntry {
    string action;     
    string timestamp;   
    string moduleSource;
    LogEntry* next;      
};


struct StationNode {
    int stationID;
    string cityName;
    bool active;
};


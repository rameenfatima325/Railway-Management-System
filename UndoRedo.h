#pragma once
#include <iostream>
#include <string>
using namespace std;

const int ACTION_ADD_TRAIN = 101;
const int ACTION_DEL_TRAIN = 102;
const int ACTION_ADD_COACH = 201;
const int ACTION_REM_COACH = 202;
const int ACTION_REV_COACHES = 203;
const int ACTION_ADD_STATION = 301;
const int ACTION_REM_STATION = 302;
const int ACTION_ADD_TRACK = 303;
const int ACTION_REM_TRACK = 304;
const int ACTION_BOOK_SEAT = 401;
const int ACTION_CANCEL_SEAT = 402;
const int ACTION_CLEAR_LOG = 501;

struct LogSnapshot {
    string timestamp;
    string moduleSource;
    string action;
    LogSnapshot* next;
    LogSnapshot() : next(nullptr) {}
};

struct SavedTrack {
    int neighbourID; 
    int distance;      
    SavedTrack* next;
    SavedTrack() : neighbourID(-1), distance(0), next(nullptr) {}
};

class CoachList;
class SeatingChart;

struct ActionRecord {
    int actionType;

    int trainID;
    string trainName;
    string trainRoute;

    string coachID;
    string coachType;
    int coachSeats;
    int coachPosition;
    string coachAfterID;

    int stationID;
    string stationName;
    int fromStationID;
    int toStationID;
    int trackDistance;

    int seatNum;
    string passengerName;
    string seatCoachID;

    LogSnapshot* savedLog;

    CoachList* savedCoaches;

    SeatingChart* savedSeats;

    SavedTrack* savedTracks;

    ActionRecord* next;

    ActionRecord() :
        actionType(0), trainID(0), trainName(""), trainRoute(""),
        coachID(""), coachType(""), coachSeats(0), coachPosition(0),
        coachAfterID(""),
        stationID(-1), stationName(""), fromStationID(-1),
        toStationID(-1), trackDistance(0),
        seatNum(0), passengerName(""), seatCoachID(""),
        savedLog(nullptr), savedCoaches(nullptr), savedSeats(nullptr),
        savedTracks(nullptr), next(nullptr) {
    }
};

struct ActionStack {
    ActionRecord* top;
    int count;

    ActionStack() : top(nullptr), count(0) {}
    ~ActionStack() { clear(); }

    void push(ActionRecord* rec) {
        rec->next = top;
        top = rec;
        ++count;
    }

    ActionRecord* pop() {
        if (top == nullptr) return nullptr;
        ActionRecord* rec = top;
        top = top->next;
        rec->next = nullptr;
        --count;
        return rec;
    }

    ActionRecord* peek() { return top; }
    bool isEmpty() { return top == nullptr; }

    void clear() {
        while (top != nullptr) {
            ActionRecord* tmp = top;
            top = top->next;
            LogSnapshot* snap = tmp->savedLog;
            while (snap != nullptr) {
                LogSnapshot* ns = snap->next;
                delete snap;
                snap = ns;
            }
            if (tmp->savedCoaches != nullptr) {
                delete tmp->savedCoaches;
                tmp->savedCoaches = nullptr;
            }
            if (tmp->savedSeats != nullptr) {
                delete tmp->savedSeats;
                tmp->savedSeats = nullptr;
            }
            SavedTrack* st = tmp->savedTracks;
            while (st != nullptr) {
                SavedTrack* ns = st->next;
                delete st;
                st = ns;
            }
            tmp->savedTracks = nullptr;
            delete tmp;
        }
        count = 0;
    }
};

class TrainRegistry;
class RailwayNetwork;
class OperationLog;

class UndoRedoManager {
private:
    ActionStack undoStack;
    ActionStack redoStack;
    OperationLog* log;     

public:
    UndoRedoManager(OperationLog& opLog) : log(&opLog) {}
    ~UndoRedoManager() {}

    void recordAction(ActionRecord* rec) {
        undoStack.push(rec);
        redoStack.clear();   
    }

    bool canUndo() { return !undoStack.isEmpty(); }
    bool canRedo() { return !redoStack.isEmpty(); }

    void undo(TrainRegistry& registry, RailwayNetwork& network);
    void redo(TrainRegistry& registry, RailwayNetwork& network);

    void displayUndoStack() {
        if (undoStack.isEmpty()) {
            cout << "[UNDO] Nothing to undo." << endl;
            return;
        }
        cout << "  Undo stack top: ";
        printActionSummary(undoStack.peek());
    }

    void displayRedoStack() {
        if (redoStack.isEmpty()) {
            cout << "[REDO] Nothing to redo." << endl;
            return;
        }
        cout << "  Redo stack top: ";
        printActionSummary(redoStack.peek());
    }

private:
    void printActionSummary(ActionRecord* r) {
        if (r == nullptr) { cout << "(none)" << endl; return; }
        switch (r->actionType) {
        case ACTION_ADD_TRAIN:
            cout << "Add Train #" << r->trainID << " '" << r->trainName << "'" << endl; break;
        case ACTION_DEL_TRAIN:
            cout << "Delete Train #" << r->trainID << " '" << r->trainName << "'" << endl; break;
        case ACTION_ADD_COACH:
            cout << "Add Coach '" << r->coachID << "' to Train #" << r->trainID << endl; break;
        case ACTION_REM_COACH:
            cout << "Remove Coach '" << r->coachID << "' from Train #" << r->trainID << endl; break;
        case ACTION_REV_COACHES:
            cout << "Reverse coaches on Train #" << r->trainID << endl; break;
        case ACTION_ADD_STATION:
            cout << "Add Station '" << r->stationName << "' (ID " << r->stationID << ")" << endl; break;
        case ACTION_REM_STATION:
            cout << "Remove Station '" << r->stationName << "' (ID " << r->stationID << ")" << endl; break;
        case ACTION_ADD_TRACK:
            cout << "Add Track " << r->fromStationID << " <-> " << r->toStationID
                << " (" << r->trackDistance << " km)" << endl; break;
        case ACTION_REM_TRACK:
            cout << "Remove Track " << r->fromStationID << " <-> " << r->toStationID << endl; break;
        case ACTION_BOOK_SEAT:
            cout << "Book Seat " << r->seatNum << " (Coach " << r->seatCoachID
                << ", Train #" << r->trainID << ") for " << r->passengerName << endl; break;
        case ACTION_CANCEL_SEAT:
            cout << "Cancel Seat " << r->seatNum << " (Coach " << r->seatCoachID
                << ", Train #" << r->trainID << ")" << endl; break;
        case ACTION_CLEAR_LOG:
            cout << "Clear operation log" << endl; break;
        default:
            cout << "Unknown action " << r->actionType << endl;
        }
    }
};
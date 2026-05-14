#pragma once
#include "UndoRedo.h"
#include "TrainRegistry.h"
#include "RailwayNetwork.h"
#include "OperationLog.h"


static string actionDescription(ActionRecord* r) {
    switch (r->actionType) {
    case ACTION_ADD_TRAIN:   return "Train #" + to_string(r->trainID) + " '" + r->trainName + "' registered";
    case ACTION_DEL_TRAIN:   return "Train #" + to_string(r->trainID) + " '" + r->trainName + "' deleted";
    case ACTION_ADD_COACH:   return "Coach '" + r->coachID + "' added to Train #" + to_string(r->trainID);
    case ACTION_REM_COACH:   return "Coach '" + r->coachID + "' removed from Train #" + to_string(r->trainID);
    case ACTION_REV_COACHES: return "Train #" + to_string(r->trainID) + " coaches reversed";
    case ACTION_ADD_STATION: return "Station '" + r->stationName + "' added (ID " + to_string(r->stationID) + ")";
    case ACTION_REM_STATION: return "Station '" + r->stationName + "' (ID " + to_string(r->stationID) + ") removed";
    case ACTION_ADD_TRACK:   return "Track added: Station " + to_string(r->fromStationID) + " <-> Station " + to_string(r->toStationID) + " (" + to_string(r->trackDistance) + " km)";
    case ACTION_REM_TRACK:   return "Track removed: Station " + to_string(r->fromStationID) + " <-> Station " + to_string(r->toStationID);
    case ACTION_BOOK_SEAT:   return "Seat " + to_string(r->seatNum) + " (Coach " + r->seatCoachID + ", Train #" + to_string(r->trainID) + ") booked by " + r->passengerName;
    case ACTION_CANCEL_SEAT: return "Seat " + to_string(r->seatNum) + " (Coach " + r->seatCoachID + ", Train #" + to_string(r->trainID) + ") cancelled";
    case ACTION_CLEAR_LOG:   return "Operation log cleared";
    default:                 return "Unknown action";
    }
}


static void clearOperationLog(OperationLog* opLog) {
    if (opLog == nullptr) return;
    while (!opLog->isEmpty())
        opLog->pop();
}

static void restoreLogFromSnapshot(ActionRecord* r, OperationLog* opLog) {
    if (opLog == nullptr || r->savedLog == nullptr) return;

    LogSnapshot* snaps[1000];
    int snapCount = 0;
    LogSnapshot* sn = r->savedLog;
    while (sn != nullptr && snapCount < 1000) {
        snaps[snapCount++] = sn;
        sn = sn->next;
    }

    for (int i = snapCount - 1; i >= 0; --i) {
        LogEntry* entry = new LogEntry;
        entry->action = snaps[i]->action;
        entry->moduleSource = snaps[i]->moduleSource;
        entry->timestamp = snaps[i]->timestamp;
        entry->next = nullptr;
        opLog->pushRaw(entry);
    }
}


static void doAction(ActionRecord* r,
    TrainRegistry& registry,
    RailwayNetwork& network,
    OperationLog* opLog) {
    switch (r->actionType) {

    case ACTION_ADD_TRAIN:
        registry.insertTrain(r->trainID, r->trainName, r->trainRoute);
        break;

    case ACTION_DEL_TRAIN: {
        if (registry.trainExists(r->trainID)) {
            TrainNode* t = registry.searchTrain(r->trainID);
            if (t != nullptr) {
                if (r->savedCoaches != nullptr) {
                    delete r->savedCoaches;
                    r->savedCoaches = nullptr;
                }
                r->savedCoaches = t->coaches;
                t->coaches = nullptr; 
            }
            registry.deleteTrain(r->trainID);
        }
        break;
    }

    case ACTION_ADD_COACH: {
        TrainNode* train = registry.searchTrain(r->trainID);
        if (train == nullptr) break;
        if (train->coaches == nullptr)
            train->coaches = new CoachList();

        if (r->coachPosition == 0) {
            train->coaches->addCoachFront(r->coachID, r->coachType, r->coachSeats);
        }
        else if (r->coachPosition == -1 || r->coachAfterID.empty()) {
            train->coaches->addCoachEnd(r->coachID, r->coachType, r->coachSeats);
        }
        else {
            int predPos = train->coaches->getCoachPosition(r->coachAfterID);
            if (predPos == -1)
                train->coaches->addCoachEnd(r->coachID, r->coachType, r->coachSeats);
            else
                train->coaches->addCoachAt(predPos + 1, r->coachID, r->coachType, r->coachSeats);
        }

        CoachNode* nc = train->coaches->findCoach(r->coachID);
        if (nc != nullptr && nc->seats == nullptr)
            nc->seats = new SeatingChart(r->coachSeats);
        break;
    }

    case ACTION_REM_COACH: {
        TrainNode* train = registry.searchTrain(r->trainID);
        if (train == nullptr || train->coaches == nullptr) break;

        CoachNode* target = train->coaches->findCoach(r->coachID);
        if (target != nullptr) {
            if (r->savedSeats != nullptr) {
                delete r->savedSeats;
                r->savedSeats = nullptr;
            }
            r->savedSeats = target->seats;
            target->seats = nullptr; 
        }
        train->coaches->removeCoach(r->coachID);
        break;
    }

    case ACTION_REV_COACHES: {
        TrainNode* train = registry.searchTrain(r->trainID);
        if (train != nullptr && train->coaches != nullptr)
            train->coaches->reverseCoachOrder();
        break;
    }

    case ACTION_ADD_STATION:
        network.addStation(r->stationName);
        break;

    case ACTION_REM_STATION: {
        SavedTrack* st = r->savedTracks;
        while (st != nullptr) {
            SavedTrack* ns = st->next;
            delete st;
            st = ns;
        }
        r->savedTracks = nullptr;

        int nbIDs[MAX_STATIONS], nbDists[MAX_STATIONS];
        int nbCount = network.getNeighbours(r->stationID, nbIDs, nbDists);
        for (int i = 0; i < nbCount; ++i) {
            SavedTrack* entry = new SavedTrack();
            entry->neighbourID = nbIDs[i];
            entry->distance = nbDists[i];
            entry->next = r->savedTracks;
            r->savedTracks = entry;
        }

        network.removeStation(r->stationID);
        break;
    }

    case ACTION_ADD_TRACK:
        network.addTrack(r->fromStationID, r->toStationID, r->trackDistance);
        break;

    case ACTION_REM_TRACK:
        network.removeTrack(r->fromStationID, r->toStationID);
        break;

    case ACTION_BOOK_SEAT: {
        TrainNode* train = registry.searchTrain(r->trainID);
        if (train == nullptr || train->coaches == nullptr) break;
        CoachNode* coach = train->coaches->findCoach(r->seatCoachID);
        if (coach == nullptr || coach->seats == nullptr) break;
        coach->seats->bookSeat(r->seatNum, r->passengerName);
        break;
    }

    case ACTION_CANCEL_SEAT: {
        TrainNode* train = registry.searchTrain(r->trainID);
        if (train == nullptr || train->coaches == nullptr) break;
        CoachNode* coach = train->coaches->findCoach(r->seatCoachID);
        if (coach == nullptr || coach->seats == nullptr) break;
        coach->seats->cancelSeat(r->seatNum);
        break;
    }

    case ACTION_CLEAR_LOG:
        clearOperationLog(opLog);
        break;

    default:
        break;
    }
}

static void undoAction(ActionRecord* r,
    TrainRegistry& registry,
    RailwayNetwork& network,
    OperationLog* opLog) {
    switch (r->actionType) {

    case ACTION_ADD_TRAIN:
        registry.deleteTrain(r->trainID);
        break;

    case ACTION_DEL_TRAIN:
        registry.insertTrain(r->trainID, r->trainName, r->trainRoute);
        {
            TrainNode* restored = registry.searchTrain(r->trainID);
            if (restored != nullptr && r->savedCoaches != nullptr) {
                restored->coaches = r->savedCoaches;
                r->savedCoaches = nullptr;  
            }
        }
        break;

    case ACTION_ADD_COACH: {
        TrainNode* train = registry.searchTrain(r->trainID);
        if (train == nullptr || train->coaches == nullptr) break;
        train->coaches->removeCoach(r->coachID);
        break;
    }

    case ACTION_REM_COACH: {
        TrainNode* train = registry.searchTrain(r->trainID);
        if (train == nullptr) break;
        if (train->coaches == nullptr)
            train->coaches = new CoachList();

        if (r->coachAfterID.empty()) {
            train->coaches->addCoachFront(r->coachID, r->coachType, r->coachSeats);
        }
        else {
            int predPos = train->coaches->getCoachPosition(r->coachAfterID);
            if (predPos == -1)
                train->coaches->addCoachEnd(r->coachID, r->coachType, r->coachSeats);
            else
                train->coaches->addCoachAt(predPos + 1, r->coachID, r->coachType, r->coachSeats);
        }

        CoachNode* nc = train->coaches->findCoach(r->coachID);
        if (nc != nullptr) {
            if (nc->seats != nullptr) { delete nc->seats; nc->seats = nullptr; }
            if (r->savedSeats != nullptr) {
                nc->seats = r->savedSeats;
                r->savedSeats = nullptr; 
            }
            else {
                nc->seats = new SeatingChart(r->coachSeats); 
            }
        }
        break;
    }

    case ACTION_REV_COACHES: {
        TrainNode* train = registry.searchTrain(r->trainID);
        if (train != nullptr && train->coaches != nullptr)
            train->coaches->reverseCoachOrder();
        break;
    }

    case ACTION_ADD_STATION:
        network.removeStation(r->stationID);
        break;

    case ACTION_REM_STATION:
        network.addStationAt(r->stationID, r->stationName);
        {
            SavedTrack* st = r->savedTracks;
            while (st != nullptr) {
                network.addTrack(r->stationID, st->neighbourID, st->distance);
                st = st->next;
            }
        }
        break;

    case ACTION_ADD_TRACK:
        network.removeTrack(r->fromStationID, r->toStationID);
        break;

    case ACTION_REM_TRACK:
        network.addTrack(r->fromStationID, r->toStationID, r->trackDistance);
        break;

    case ACTION_BOOK_SEAT: {
        TrainNode* train = registry.searchTrain(r->trainID);
        if (train == nullptr || train->coaches == nullptr) break;
        CoachNode* coach = train->coaches->findCoach(r->seatCoachID);
        if (coach == nullptr || coach->seats == nullptr) break;
        coach->seats->cancelSeat(r->seatNum);
        break;
    }

    case ACTION_CANCEL_SEAT: {
        TrainNode* train = registry.searchTrain(r->trainID);
        if (train == nullptr || train->coaches == nullptr) break;
        CoachNode* coach = train->coaches->findCoach(r->seatCoachID);
        if (coach == nullptr || coach->seats == nullptr) break;
        coach->seats->bookSeat(r->seatNum, r->passengerName);
        break;
    }

    case ACTION_CLEAR_LOG:
        restoreLogFromSnapshot(r, opLog);
        break;

    default:
        break;
    }
}

static string undoDescription(ActionRecord* r) {
    switch (r->actionType) {
    case ACTION_ADD_TRAIN:   return "Train #" + to_string(r->trainID) + " '" + r->trainName + "' registration undone (deleted)";
    case ACTION_DEL_TRAIN:   return "Train #" + to_string(r->trainID) + " '" + r->trainName + "' restored";
    case ACTION_ADD_COACH:   return "Coach '" + r->coachID + "' addition undone (removed) from Train #" + to_string(r->trainID);
    case ACTION_REM_COACH:   return "Coach '" + r->coachID + "' removal undone (restored) on Train #" + to_string(r->trainID);
    case ACTION_REV_COACHES: return "Train #" + to_string(r->trainID) + " coach reversal undone";
    case ACTION_ADD_STATION: return "Station '" + r->stationName + "' (ID " + to_string(r->stationID) + ") addition undone (removed)";
    case ACTION_REM_STATION: return "Station '" + r->stationName + "' (ID " + to_string(r->stationID) + ") removal undone (restored)";
    case ACTION_ADD_TRACK:   return "Track Station " + to_string(r->fromStationID) + " <-> " + to_string(r->toStationID) + " addition undone (removed)";
    case ACTION_REM_TRACK:   return "Track Station " + to_string(r->fromStationID) + " <-> " + to_string(r->toStationID) + " removal undone (restored)";
    case ACTION_BOOK_SEAT:   return "Seat " + to_string(r->seatNum) + " booking undone (cancelled)";
    case ACTION_CANCEL_SEAT: return "Seat " + to_string(r->seatNum) + " cancellation undone (re-booked for " + r->passengerName + ")";
    case ACTION_CLEAR_LOG:   return "Log-clear undone (log restored)";
    default:                 return "Undo of unknown action";
    }
}

inline void UndoRedoManager::undo(TrainRegistry& registry,
    RailwayNetwork& network) {
    if (undoStack.isEmpty()) {
        cout << "[UNDO] Nothing to undo." << endl;
        return;
    }

    ActionRecord* rec = undoStack.pop();

    cout << "[UNDO] Reversing: ";
    printActionSummary(rec);

    undoAction(rec, registry, network, log);

    if (log)
        log->push("[UNDO] " + undoDescription(rec), "UNDO");

    redoStack.push(rec);
}


inline void UndoRedoManager::redo(TrainRegistry& registry,
    RailwayNetwork& network) {
    if (redoStack.isEmpty()) {
        cout << "[REDO] Nothing to redo." << endl;
        return;
    }

    ActionRecord* rec = redoStack.pop();

    cout << "[REDO] Re-applying: ";
    printActionSummary(rec);

    doAction(rec, registry, network, log);

    if (log)
        log->push("[REDO] " + actionDescription(rec), "REDO");

    undoStack.push(rec);
}
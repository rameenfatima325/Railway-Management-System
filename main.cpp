#include <iostream>
#include <string>
#include "types.h"
#include "TrainRegistry.h"
#include "CoachList.h"
#include "RailwayNetwork.h"
#include "Seatingchart.h"
#include "OperationLog.h"
#include "UndoRedo.h"
#include "UndoRedoImp.h"
using namespace std;

const string RESET = "\033[0m";
const string BOLD = "\033[1m";
const string DIM = "\033[2m";
const string ITALIC = "\033[3m";
const string ULINE = "\033[4m";
const string RED = "\033[91m";
const string GREEN = "\033[92m";
const string YELLOW = "\033[93m";
const string BLUE = "\033[94m";
const string MAGENTA = "\033[95m";
const string CYAN = "\033[96m";
const string WHITE = "\033[97m";
const string DRED = "\033[31m";
const string DGREEN = "\033[32m";
const string DYELLOW = "\033[33m";
const string DBLUE = "\033[34m";
const string DMAGENTA = "\033[35m";
const string DCYAN = "\033[36m";
const string BG_RED = "\033[41m";
const string BG_GREEN = "\033[42m";
const string BG_YELLOW = "\033[43m";
const string BG_BLUE = "\033[44m";
const string BG_CYAN = "\033[46m";
const string BG_WHITE = "\033[47m";
const string ORANGE = "\033[38;5;208m";
const string PINK = "\033[38;5;213m";
const string TEAL = "\033[38;5;43m";
const string PURPLE = "\033[38;5;129m";
const string LIME = "\033[38;5;118m";
const string GOLD = "\033[38;5;220m";
const string GREY = "\033[38;5;244m";
const string SKYBLUE = "\033[38;5;75m";
const string MINT = "\033[38;2;170;255;195m";
const string CORAL = "\033[38;2;255;127;80m";
const string SLATE = "\033[38;2;112;128;144m";


void printBanner(const string& title, const string& col) {
    cout << BOLD << col << "[==========================================]\n" << "[  " << title;
    int pad = 40 - (int)title.size();
    for (int i = 0; i < pad; i++) cout << ' ';
    cout << "]\n" << "[==========================================]\n" << RESET;
}

void printSubBanner(const string& title, const string& col) {
    cout << BOLD << col << "--------------------------------------------\n" << "[ " << title;
    int pad = 40 - (int)title.size();
    for (int i = 0; i < pad; i++) cout << ' ';
    cout << "]\n" << "--------------------------------------------\n" << RESET;
}

void printDivider(const string& col = DIM) {
    cout << col << "--------------------------------------------\n" << RESET;
}

void menuItem(int n, const string& label, const string& accent = CYAN) {
    cout << "  " << BOLD << accent << n << ". " << label << RESET << "\n";
}
void msgOK(const string& s) { cout << BOLD << GREEN << s << RESET << "\n"; }
void msgErr(const string& s) { cout << BOLD << RED << s << RESET << "\n"; }
void msgWarn(const string& s) { cout << BOLD << YELLOW << s << RESET << "\n"; }
void msgInfo(const string& s) { cout << BOLD << CYAN << s << RESET << "\n"; }

TrainRegistry   registry;
RailwayNetwork  network;
OperationLog    systemLog;
UndoRedoManager undoRedo(systemLog);
void menuTrainRegistry();
void menuCoachManagement();
void menuRailwayNetwork();
void menuSeatingChart();
void menuOperationLog();
void saveAll();
void loadAll();

void flushLine() {
    char c;
    while (cin.get(c) && c != '\n') {}
}

int readInt(const string& prompt) {
    while (true) {
        cout << BOLD << BLUE << prompt << RESET;
        string line;
        getline(cin, line);
        if (line.empty()) {
            msgErr("Input cannot be blank. Try again.");
            continue;
        }
        bool valid = true;
        int  start = 0;
        if (line[0] == '-' || line[0] == '+') start = 1;
        if (start == (int)line.size()) valid = false;
        for (int i = start; i < (int)line.size() && valid; i++) {
            if (line[i] < '0' || line[i] > '9') valid = false;
        }
        if (!valid) {
            msgErr("That's not a valid number. Try again.");
            continue;
        }
        int val = 0;
        bool neg = (line[0] == '-');
        for (int i = start; i < (int)line.size(); i++)
            val = val * 10 + (line[i] - '0');
        if (neg) val = -val;
        return val;
    }
}

int readPositiveInt(const string& prompt) {
    while (true) {
        int val = readInt(prompt);
        if (val > 0) return val;
        msgErr("Value must be greater than zero. Try again.");
    }
}

int readNonNegInt(const string& prompt) {
    while (true) {
        int val = readInt(prompt);
        if (val >= 0) return val;
        msgErr("Value must be 0 or greater. Try again.");
    }
}

int readChoice(int lo, int hi) {
    while (true) {
        cout << "\n" << BOLD << MAGENTA << " Choice [" << lo << "-" << hi << "]: " << RESET;
        string line;
        getline(cin, line);
        string trimmed;
        for (char c : line) if (c != ' ' && c != '\t') trimmed += c;
        if (trimmed.empty()) { msgErr("Please enter a number."); continue; }
        bool valid = true;
        for (char c : trimmed) if (c < '0' || c > '9') { valid = false; break; }
        if (!valid) { msgErr("Please enter a valid number."); continue; }
        int val = 0;
        for (char c : trimmed) val = val * 10 + (c - '0');
        if (val >= lo && val <= hi) return val;
        msgErr("Please enter a number between " + to_string(lo) + " and " + to_string(hi) + ".");
    }
}

string readLine(const string& prompt) {
    while (true) {
        cout << BOLD << BLUE << prompt << RESET;
        string s;
        getline(cin, s);
        size_t start = s.find_first_not_of(" \t");
        if (start == string::npos) {
            msgErr("Input cannot be blank. Try again.");
            continue;
        }
        size_t end = s.find_last_not_of(" \t");
        return s.substr(start, end - start + 1);
    }
}

string readWord(const string& prompt) {
    while (true) {
        string s = readLine(prompt);
        bool hasSpace = false;
        for (int i = 0; i < (int)s.size(); i++) {
            if (s[i] == ' ' || s[i] == '\t') { hasSpace = true; break; }
        }
        if (!hasSpace) return s;
        return s.substr(0, s.find(' '));
    }
}

int main() {
    loadAll();
    while (true) {
        cout << "\n";
        printBanner("  THE IRON NEXUS  |  MAIN MENU      ", CYAN);
        cout << "\n";
        menuItem(1, "Train Registry      (M1)", CYAN);
        menuItem(2, "Coach Management    (M2)", GREEN);
        menuItem(3, "Railway Network     (M3)", YELLOW);
        menuItem(4, "Seating Chart       (M4)", MAGENTA);
        menuItem(5, "Operation Log       (M5)", BLUE);
        printDivider();
        menuItem(6, "Save All Data", DGREEN);
        menuItem(7, "Undo last action", DYELLOW);
        menuItem(8, "Redo last undone action", DYELLOW);
        printDivider();
        menuItem(9, "Exit", RED);

        int choice = readChoice(1, 9);
        switch (choice) {
        case 1: menuTrainRegistry();   break;
        case 2: menuCoachManagement(); break;
        case 3: menuRailwayNetwork();  break;
        case 4: menuSeatingChart();    break;
        case 5: menuOperationLog();    break;
        case 6: saveAll();             break;
        case 7: undoRedo.undo(registry, network); break;
        case 8: undoRedo.redo(registry, network); break;
        case 9:
            saveAll();
            cout << "\n" << BOLD << GREEN
                << "  Exiting The Iron Nexus. Goodbye!\n" << RESET;
            return 0;
        }
    }
}

void menuTrainRegistry() {
    while (true) {
        cout << "\n";
        printSubBanner("M1: TRAIN REGISTRY", CYAN);
        cout << "\n";
        menuItem(1, "Add a Train", CYAN);
        menuItem(2, "Delete a Train", RED);
        menuItem(3, "Search for a Train", YELLOW);
        printDivider(DCYAN);
        menuItem(4, "Display All Trains (In-Order)", DBLUE);
        menuItem(5, "Display All Trains (Pre-Order)", DBLUE);
        menuItem(6, "Display All Trains (Post-Order)", DBLUE);
        printDivider(DCYAN);
        menuItem(7, "Back to Main Menu", DIM);

        int choice = readChoice(1, 7);

        if (choice == 1) {
            cout << "\n" << BOLD << CYAN << "  [ ADD TRAIN ]\n" << RESET;
            int    id = readPositiveInt("  Enter Train ID: ");
            string name = readLine("  Enter Train Name: ");
            string route = readLine("  Enter Route: ");
            if (registry.trainExists(id)) {
                msgErr("Train ID " + to_string(id) + " already exists.");
                continue;
            }
            registry.insertTrain(id, name, route);
            msgOK("Train " + to_string(id) + " '" + name + "' added successfully!");
            systemLog.push("Train #" + to_string(id) + " '" + name + "' registered", "M1");
            ActionRecord* rec = new ActionRecord();
            rec->actionType = ACTION_ADD_TRAIN;
            rec->trainID = id;
            rec->trainName = name;
            rec->trainRoute = route;
            undoRedo.recordAction(rec);
        }
        else if (choice == 2) {
            cout << "\n" << BOLD << RED << "  [ DELETE TRAIN ]\n" << RESET;
            int id = readPositiveInt("  Enter Train ID to delete: ");
            if (registry.trainExists(id)) {
                TrainNode* t = registry.searchTrain(id);
                string tName = t->trainName;
                string tRoute = t->route;
                CoachList* savedCoaches = t->coaches;
                t->coaches = nullptr;  
                registry.deleteTrain(id);
                msgOK("Train #" + to_string(id) + " deleted.");
                systemLog.push("Train #" + to_string(id) + " deleted", "M1");
                ActionRecord* rec = new ActionRecord();
                rec->actionType = ACTION_DEL_TRAIN;
                rec->trainID = id;
                rec->trainName = tName;
                rec->trainRoute = tRoute;
                rec->savedCoaches = savedCoaches;
                undoRedo.recordAction(rec);
            }
            else {
                msgErr("Train " + to_string(id) + " not found.");
            }
        }
        else if (choice == 3) {
            cout << "\n" << BOLD << YELLOW << "  [ SEARCH TRAIN ]\n" << RESET;
            int id = readPositiveInt("  Enter Train ID to search: ");
            TrainNode* found = registry.searchTrain(id);
            if (found)
                cout << BOLD << GREEN << "  Found: " << RESET
                << CYAN << found->trainName << RESET
                << WHITE << " on " << RESET
                << YELLOW << found->route << RESET << "\n";
            else
                msgErr("Train not found.");
        }
        else if (choice == 4) {
            cout << "\n" << BOLD << DBLUE << "  [ IN-ORDER TRAVERSAL ]\n" << RESET;
            registry.displayAll_Inorder();
        }
        else if (choice == 5) {
            cout << "\n" << BOLD << DBLUE << "  [ PRE-ORDER TRAVERSAL ]\n" << RESET;
            registry.displayAll_Preorder();
        }
        else if (choice == 6) {
            cout << "\n" << BOLD << DBLUE << "  [ POST-ORDER TRAVERSAL ]\n" << RESET;
            registry.displayAll_Postorder();
        }
        else {
            break;
        }
    }
}

void menuCoachManagement() {
    cout << "\n";
    printSubBanner("M2: COACH MANAGEMENT", GREEN);

    int tID = readPositiveInt("\n  Enter Train ID to manage: ");
    TrainNode* targetTrain = registry.searchTrain(tID);
    if (targetTrain == nullptr) {
        msgErr("Train " + to_string(tID) + " does not exist. Register it in M1 first!");
        return;
    }
    if (targetTrain->coaches == nullptr)
        targetTrain->coaches = new CoachList();

    while (true) {
        cout << "\n" << BOLD << GREEN
            << "  Managing coaches for Train " << tID
            << " (" << targetTrain->trainName << ")\n" << RESET;
        printDivider(DGREEN);

        menuItem(1, "Add Coach to Front", GREEN);
        menuItem(2, "Add Coach to End", GREEN);
        menuItem(3, "Add Coach at Position", GREEN);
        menuItem(4, "Remove a Coach", RED);
        menuItem(5, "Display All Coaches", CYAN);
        menuItem(6, "Reverse Coach Order", YELLOW);
        printDivider(DGREEN);
        menuItem(7, "Back to Main Menu", DIM);

        int choice = readChoice(1, 7);

        if (choice >= 1 && choice <= 3) {
            cout << "\n" << BOLD << GREEN << "  [ ADD COACH ]\n" << RESET;
            string cID = readWord("  Enter Coach ID (e.g. C1): ");

            if (targetTrain->coaches->findCoach(cID) != nullptr) {
                msgErr("Error: Coach " + cID + " already exists on this train!");
            }
            else {
                string type = readWord("  Enter Coach Type (VIP/Economy): ");
                int    seats = readPositiveInt("  Enter Total Seats: ");
                int    insertPos = 0;
                string prevID = "";

                if (choice == 1) {
                    targetTrain->coaches->addCoachFront(cID, type, seats);
                }
                else if (choice == 2) {
                    prevID = targetTrain->coaches->getTailID();
                    targetTrain->coaches->addCoachEnd(cID, type, seats);
                    insertPos = -1;
                }
                else {
                    insertPos = readPositiveInt("  Enter Position to Insert Coach: ");
                    prevID = targetTrain->coaches->getCoachIDAt(insertPos - 1);
                    targetTrain->coaches->addCoachAt(insertPos, cID, type, seats);
                }

                CoachNode* newCoach = targetTrain->coaches->findCoach(cID);
                if (newCoach != nullptr && newCoach->seats == nullptr) {
                    newCoach->seats = new SeatingChart(seats);
                    msgInfo("Seating chart created with " + to_string(seats) + " seats.");
                }
                msgOK("Coach " + cID + " attached successfully!");
                systemLog.push("Coach '" + cID + "' added to Train #" + to_string(tID), "M2");
                ActionRecord* rec = new ActionRecord();
                rec->actionType = ACTION_ADD_COACH;
                rec->trainID = tID;
                rec->coachID = cID;
                rec->coachType = type;
                rec->coachSeats = seats;
                rec->coachPosition = insertPos;
                rec->coachAfterID = prevID;
                undoRedo.recordAction(rec);
            }
        }
        else if (choice == 4) {
            cout << "\n" << BOLD << RED << "  [ REMOVE COACH ]\n" << RESET;
            string cID = readWord("  Enter Coach ID to remove: ");
            CoachNode* target = targetTrain->coaches->findCoach(cID);
            if (target != nullptr) {
                string savedType = target->coachType;
                int    savedSeats = target->totalSeats;
                int    coachPos = targetTrain->coaches->getCoachPosition(cID);
                string predID = (coachPos > 1)
                    ? targetTrain->coaches->getCoachIDAt(coachPos - 1): "";
                SeatingChart* savedChart = target->seats;
                target->seats = nullptr;
                targetTrain->coaches->removeCoach(cID);
                msgOK("Coach " + cID + " removed.");
                systemLog.push("Coach '" + cID + "' removed from Train #" + to_string(tID), "M2");
                ActionRecord* rec = new ActionRecord();
                rec->actionType = ACTION_REM_COACH;
                rec->trainID = tID;
                rec->coachID = cID;
                rec->coachType = savedType;
                rec->coachSeats = savedSeats;
                rec->coachPosition = coachPos;
                rec->coachAfterID = predID;  
                rec->savedSeats = savedChart;
                undoRedo.recordAction(rec);
            }
            else {
                msgErr("Coach " + cID + " not found.");
            }
        }
        else if (choice == 5) {
            cout << "\n" << BOLD << CYAN << "  [ COACH LIST ]\n" << RESET;
            targetTrain->coaches->displayCoaches();
        }
        else if (choice == 6) {
            targetTrain->coaches->reverseCoachOrder();
            msgOK("Coach order reversed for Train #" + to_string(tID) + ".");
            systemLog.push("Train #" + to_string(tID) + " coaches reversed", "M2");
            ActionRecord* rec = new ActionRecord();
            rec->actionType = ACTION_REV_COACHES;
            rec->trainID = tID;
            undoRedo.recordAction(rec);
        }
        else {
            break;
        }
    }
}

void menuRailwayNetwork() {
    cout << "\n";
    printSubBanner("M3: RAILWAY NETWORK", YELLOW);

    while (true) {
        cout << "\n";
        menuItem(1, "Add a Station", GREEN);
        menuItem(2, "Remove a Station", RED);
        menuItem(3, "Add a Track", CYAN);
        menuItem(4, "Remove a Track", DRED);
        menuItem(5, "Find Shortest Path", YELLOW);
        menuItem(6, "Display Network", MAGENTA);
        printDivider(DYELLOW);
        menuItem(7, "Back to Main Menu", DIM);

        int choice = readChoice(1, 7);

        if (choice == 1) {
            cout << "\n" << BOLD << GREEN << "  [ ADD STATION ]\n" << RESET;
            string cityName = readLine("  Enter city name: ");
            int newID = network.addStation(cityName);
            if (newID == -1) {
                msgErr("Failed to add station (limit reached).");
            }
            else {
                msgOK("Station '" + cityName + "' added with ID " + to_string(newID) + ".");
                systemLog.push("Station '" + cityName + "' added (ID " + to_string(newID) + ")", "M3");
                ActionRecord* rec = new ActionRecord();
                rec->actionType = ACTION_ADD_STATION;
                rec->stationID = newID;
                rec->stationName = cityName;
                undoRedo.recordAction(rec);
            }
        }
        else if (choice == 2) {
            cout << "\n" << BOLD << RED << "  [ REMOVE STATION ]\n" << RESET;
            int stationID = readNonNegInt("  Enter Station ID to remove: ");
            string savedName = network.getStationName(stationID);
            if (savedName != "INVALID") {
                ActionRecord* rec = new ActionRecord();
                rec->actionType = ACTION_REM_STATION;
                rec->stationID = stationID;
                rec->stationName = savedName;

                int nbIDs[MAX_STATIONS], nbDists[MAX_STATIONS];
                int nbCount = network.getNeighbours(stationID, nbIDs, nbDists);
                for (int i = 0; i < nbCount; ++i) {
                    SavedTrack* entry = new SavedTrack();
                    entry->neighbourID = nbIDs[i];
                    entry->distance = nbDists[i];
                    entry->next = rec->savedTracks;
                    rec->savedTracks = entry;
                }

                network.removeStation(stationID);
                msgOK("Station '" + savedName + "' (ID " + to_string(stationID) + ") removed.");
                systemLog.push("Station '" + savedName + "' (ID " + to_string(stationID) + ") removed", "M3");
                undoRedo.recordAction(rec);
            }
            else {
                msgErr("No active station with ID " + to_string(stationID) + ".");
            }
        }
        else if (choice == 3) {
            cout << "\n" << BOLD << CYAN << "  [ ADD TRACK ]\n" << RESET;
            int fromID = readNonNegInt("  Enter From Station ID: ");
            int toID = readNonNegInt("  Enter To Station ID: ");
            int distance = readPositiveInt("  Enter Distance (km): ");
            if (network.addTrack(fromID, toID, distance)) {
                msgOK("Track added: Station " + to_string(fromID) +
                    " TO Station " + to_string(toID) +
                    " (" + to_string(distance) + " km).");
                systemLog.push("Track added: Station " + to_string(fromID) +
                    " <-> Station " + to_string(toID) +
                    " (" + to_string(distance) + " km)", "M3");
                ActionRecord* rec = new ActionRecord();
                rec->actionType = ACTION_ADD_TRACK;
                rec->fromStationID = fromID;
                rec->toStationID = toID;
                rec->trackDistance = distance;
                undoRedo.recordAction(rec);
            }
        }
        else if (choice == 4) {
            cout << "\n" << BOLD << RED << "  [ REMOVE TRACK ]\n" << RESET;
            int fromID = readNonNegInt("  Enter From Station ID: ");
            int toID = readNonNegInt("  Enter To Station ID: ");
            int savedDist = network.getDistance(fromID, toID);
            if (savedDist > 0) {
                network.removeTrack(fromID, toID);
                msgOK("Track removed: Station " + to_string(fromID) +
                    " TO Station " + to_string(toID) + ".");
                systemLog.push("Track removed: Station " + to_string(fromID) +
                    " <-> Station " + to_string(toID), "M3");
                ActionRecord* rec = new ActionRecord();
                rec->actionType = ACTION_REM_TRACK;
                rec->fromStationID = fromID;
                rec->toStationID = toID;
                rec->trackDistance = savedDist;
                undoRedo.recordAction(rec);
            }
            else {
                msgErr("No track exists between Station " + to_string(fromID) +
                    " and Station " + to_string(toID) + ".");
            }
        }
        else if (choice == 5) {
            cout << "\n" << BOLD << YELLOW << "  [ SHORTEST PATH ]\n" << RESET;
            int fromID = readNonNegInt("  Enter From Station ID: ");
            int toID = readNonNegInt("  Enter To Station ID: ");
            network.findShortestPath(fromID, toID);
            systemLog.push("Shortest path: Station " + to_string(fromID) +
                " to Station " + to_string(toID), "M3");
        }
        else if (choice == 6) {
            cout << "\n" << BOLD << MAGENTA << "  [ NETWORK MAP ]\n" << RESET;
            network.displayNetwork();
        }
        else {
            break;
        }
    }
}

void menuSeatingChart() {
    cout << "\n";
    printSubBanner("M4: SEATING CHART", MAGENTA);

    int trainID = readPositiveInt("\n  Enter Train ID: ");
    TrainNode* targetTrain = registry.searchTrain(trainID);
    if (targetTrain == nullptr) {
        msgErr("Train " + to_string(trainID) + " not found!");
        return;
    }
    if (targetTrain->coaches == nullptr || targetTrain->coaches->getCoachCount() == 0) {
        msgErr("This train has no coaches. Add coaches in M2 first!");
        return;
    }

    while (true) {
        cout << "\n" << BOLD << MAGENTA
            << "  Coaches on Train " << trainID
            << " (" << targetTrain->trainName << ")\n" << RESET;
        printDivider(DMAGENTA);
        targetTrain->coaches->displayCoaches();

        cout << BOLD << DIM << "  (enter 0 to go back to Main Menu)\n" << RESET;
        string coachID = readWord("\n  Enter Coach ID to manage seats: ");
        if (coachID == "0") return;

        CoachNode* targetCoach = targetTrain->coaches->findCoach(coachID);
        if (targetCoach == nullptr) {
            msgErr("Coach " + coachID + " not found on this train!");
            continue;
        }
        if (targetCoach->seats == nullptr) {
            msgErr("This coach has no seating chart initialized!");
            msgWarn("Delete and re-add this coach to fix this.");
            continue;
        }

        SeatingChart* seatMgr = targetCoach->seats;
        while (true) {
            cout << "\n";
            // Status bar
            int booked = seatMgr->getBookedCount();
            int total = seatMgr->getTotalSeats();
            int avail = total - booked;
            cout << BOLD << BG_BLUE << WHITE
                << "  Coach " << coachID
                << "  |  Total: " << total
                << "  |  " << RESET
                << BOLD << BG_GREEN << WHITE
                << " Avail: " << avail << " "
                << RESET
                << BOLD << BG_RED << WHITE
                << " Booked: " << booked << " "
                << RESET << "\n\n";

            menuItem(1, "Book a Seat", GREEN);
            menuItem(2, "Cancel a Seat", RED);
            printDivider(DMAGENTA);
            menuItem(3, "Display All Seats", CYAN);
            menuItem(4, "Display Booked Seats", DRED);
            menuItem(5, "Display Available Seats", DGREEN);
            menuItem(6, "Check Seat Status", YELLOW);
            printDivider(DMAGENTA);
            menuItem(7, "Back to Main Menu", DIM);

            int choice = readChoice(1, 7);

            if (choice == 1) {
                cout << "\n" << BOLD << GREEN << "  [ BOOK SEAT ]\n" << RESET;
                int    seatNum = readPositiveInt("  Enter Seat Number: ");
                string passengerName = readLine("  Enter Passenger Name: ");
                if (seatMgr->bookSeat(seatNum, passengerName)) {
                    msgOK("Seat " + to_string(seatNum) + " booked for " + passengerName + ".");
                    systemLog.push("Seat " + to_string(seatNum) + " (Coach " + coachID +
                        ") booked by " + passengerName +
                        " (Train #" + to_string(trainID) + ")", "M4");
                    ActionRecord* rec = new ActionRecord();
                    rec->actionType = ACTION_BOOK_SEAT;
                    rec->trainID = trainID;
                    rec->seatCoachID = coachID;
                    rec->seatNum = seatNum;
                    rec->passengerName = passengerName;
                    undoRedo.recordAction(rec);
                }
            }
            else if (choice == 2) {
                cout << "\n" << BOLD << RED << "  [ CANCEL SEAT ]\n" << RESET;
                int seatNum = readPositiveInt("  Enter Seat Number to cancel: ");
                string savedPassenger = seatMgr->getPassengerName(seatNum);
                if (seatMgr->cancelSeat(seatNum)) {
                    msgOK("Seat " + to_string(seatNum) + " cancelled.");
                    systemLog.push("Seat " + to_string(seatNum) + " (Coach " + coachID +
                        ") cancelled (Train #" + to_string(trainID) + ")", "M4");
                    ActionRecord* rec = new ActionRecord();
                    rec->actionType = ACTION_CANCEL_SEAT;
                    rec->trainID = trainID;
                    rec->seatCoachID = coachID;
                    rec->seatNum = seatNum;
                    rec->passengerName = savedPassenger;
                    undoRedo.recordAction(rec);
                }
            }
            else if (choice == 3) {
                cout << "\n" << BOLD << CYAN << "  [ ALL SEATS ]\n" << RESET;
                seatMgr->displayAllSeats();
            }
            else if (choice == 4) {
                cout << "\n" << BOLD << RED << "  [ BOOKED SEATS ]\n" << RESET;
                seatMgr->displayBookedSeats();
            }
            else if (choice == 5) {
                cout << "\n" << BOLD << GREEN << "  [ AVAILABLE SEATS ]\n" << RESET;
                seatMgr->displayAvailableSeats();
            }
            else if (choice == 6) {
                cout << "\n" << BOLD << YELLOW << "  [ CHECK SEAT ]\n" << RESET;
                int seatNum = readPositiveInt("  Enter Seat Number: ");
                if (seatMgr->isSeatAvailable(seatNum)) {
                    cout << "  " << BOLD << BG_GREEN << WHITE
                        << " SEAT " << seatNum << " IS AVAILABLE " << RESET << "\n";
                }
                else {
                    string passenger = seatMgr->getPassengerName(seatNum);
                    if (passenger.empty()) {
                        msgErr("Seat " + to_string(seatNum) + " does not exist or is invalid.");
                    }
                    else {
                        cout << "  " << BOLD << BG_RED << WHITE
                            << " SEAT " << seatNum << " IS BOOKED " << RESET
                            << "  by " << BOLD << YELLOW << passenger << RESET << "\n";
                    }
                }
            }
            else {
                break;
            }
        }
    }
}

void menuOperationLog() {
    cout << "\n";
    printSubBanner("M5: OPERATION LOG", BLUE);

    while (true) {
        cout << "\n";
        menuItem(1, "Display Last 10 Log Entries", CYAN);
        menuItem(2, "Display All Log Entries", CYAN);
        menuItem(3, "Check if Log is Empty", YELLOW);
        menuItem(4, "Get Total Log Count", YELLOW);
        menuItem(5, "Show Undo/Redo Status", MAGENTA);
        menuItem(6, "Clear Log", RED);
        printDivider(DBLUE);
        menuItem(7, "Back to Main Menu", DIM);

        int choice = readChoice(1, 7);

        if (choice == 1) {
            cout << "\n" << BOLD << CYAN << "  [ LAST 10 LOG ENTRIES ]\n" << RESET;
            systemLog.displayRecentLogs(10);
        }
        else if (choice == 2) {
            cout << "\n" << BOLD << CYAN << "  [ ALL LOG ENTRIES ]\n" << RESET;
            systemLog.displayAllLogs();
        }
        else if (choice == 3) {
            if (systemLog.isEmpty())
                msgWarn("Log is currently empty.");
            else
                msgInfo("Log contains " + to_string(systemLog.getLogCount()) + " entries.");
        }
        else if (choice == 4) {
            cout << "  " << BOLD << CYAN << "Total log entries: " << RESET
                << BOLD << WHITE << systemLog.getLogCount() << RESET << "\n";
        }
        else if (choice == 5) {
            cout << "\n";
            printDivider(DMAGENTA);
            cout << BOLD << MAGENTA << "  UNDO / REDO STATUS\n" << RESET;
            printDivider(DMAGENTA);
            if (undoRedo.canUndo()) {
                cout << BOLD << GREEN << "  [UNDO] Available\n" << RESET;
                undoRedo.displayUndoStack();
            }
            else {
                msgWarn("[UNDO] Nothing to undo.");
            }
            if (undoRedo.canRedo()) {
                cout << BOLD << GREEN << "  [REDO] Available\n" << RESET;
                undoRedo.displayRedoStack();
            }
            else {
                msgWarn("[REDO] Nothing to redo.");
            }
            printDivider(DMAGENTA);
        }
        else if (choice == 6) {
            cout << "\n" << BOLD << BG_RED << WHITE
                << "  Are you sure you want to clear the log? (y/n): "
                << RESET << " ";
            string confirm = readLine("");
            if (confirm == "y" || confirm == "Y") {
                ActionRecord* rec = new ActionRecord();
                rec->actionType = ACTION_CLEAR_LOG;
                LogSnapshot* snapHead = nullptr;
                LogEntry* cur = systemLog.peek();
                while (cur != nullptr) {
                    LogSnapshot* s = new LogSnapshot();
                    s->timestamp = cur->timestamp;
                    s->moduleSource = cur->moduleSource;
                    s->action = cur->action;
                    s->next = snapHead;
                    snapHead = s;
                    cur = cur->next;
                }
                rec->savedLog = snapHead;

                while (!systemLog.isEmpty()) {
                    systemLog.pop();
                }
                undoRedo.recordAction(rec);
                msgOK("Operation log cleared. (Tip: Use Undo from Main Menu to restore it.)");
            }
            else {
                msgWarn("Log clear cancelled.");
            }
        }
        else {
            break;
        }
    }
}

void saveAll() {
    cout << "\n" << BOLD << YELLOW << "  Saving all system data...\n" << RESET;
    printDivider(DYELLOW);
    registry.saveToFile("trains.txt");
    registry.saveStructureToFile("trains_structure.txt");
    registry.saveAllCoaches();
    msgOK("Coaches and seating charts saved.");
    network.saveToFile("network.txt");
    network.saveStructureToFile("network_structure.txt");
    systemLog.saveToFile("operations.txt");
    printDivider(DYELLOW);
    msgOK("All system data saved successfully!");
}

void loadAll() {
    cout << "\n" << BOLD << YELLOW << "  Loading system data...\n" << RESET;
    printDivider(DYELLOW);
    registry.loadFromFile("trains.txt");
    registry.loadAllCoaches();
    msgOK("Coaches and seating charts loaded.");
    network.loadFromFile("network.txt");
    systemLog.loadFromFile("operations.txt");
    printDivider(DYELLOW);
    msgOK("System loaded successfully!");
}
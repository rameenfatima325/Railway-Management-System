#pragma once
#include "types.h"
#include <iostream>
#include <fstream>
using namespace std;

const int MAX_STATIONS = 15;
const int NO_TRACK = -1;
const int INF = 999999999;

class RailwayNetwork {
private:
    StationNode stations[MAX_STATIONS];
    int  adjMatrix[MAX_STATIONS][MAX_STATIONS];
    int  stationCount;

    int minDistance(int dist[], bool visited[]) {
        int minVal = INF;
        int minIdx = -1;
        for (int i = 0; i < MAX_STATIONS; ++i) {
            if (!visited[i] && stations[i].active && dist[i] < minVal) {
                minVal = dist[i];
                minIdx = i;
            }
        }
        return minIdx;
    }

    void printPath(int parent[], int dest) {
        if (parent[dest] == -1) {
            cout << stations[dest].cityName;
            return;
        }
        printPath(parent, parent[dest]);
        cout << " -> " << stations[dest].cityName;
    }

    bool isValidID(int id) {
        return (id >= 0 && id < MAX_STATIONS && stations[id].active);
    }

public:
    RailwayNetwork() : stationCount(0) {
        for (int i = 0; i < MAX_STATIONS; ++i) {
            stations[i].stationID = i;
            stations[i].cityName = "";
            stations[i].active = false;
            for (int j = 0; j < MAX_STATIONS; ++j)
                adjMatrix[i][j] = (i == j) ? 0 : NO_TRACK;
        }
    }

    int addStation(string cityName) {
        if (stationCount >= MAX_STATIONS) {
            cout << "[M3] Error: Maximum station limit (" << MAX_STATIONS << ") reached." << endl;
            return -1;
        }
        for (int i = 0; i < MAX_STATIONS; ++i) {
            if (!stations[i].active) {
                stations[i].cityName = cityName;
                stations[i].active = true;
                ++stationCount;
                return i;
            }
        }
        return -1;
    }


    bool addStationAt(int slotID, string cityName) {
        if (slotID < 0 || slotID >= MAX_STATIONS) return false;
        if (stations[slotID].active) return false;
        if (stationCount >= MAX_STATIONS) return false;
        stations[slotID].cityName = cityName;
        stations[slotID].active = true;
        ++stationCount;
        return true;
    }

    void removeStation(int stationID) {
        if (!isValidID(stationID)) {
            cout << "[M3] Error: Station ID " << stationID << " not found or already inactive." << endl;
            return;
        }
        string name = stations[stationID].cityName;
        for (int j = 0; j < MAX_STATIONS; ++j) {
            adjMatrix[stationID][j] = (stationID == j) ? 0 : NO_TRACK;
            adjMatrix[j][stationID] = (stationID == j) ? 0 : NO_TRACK;
        }
        stations[stationID].active = false;
        stations[stationID].cityName = "";
        --stationCount;
    }

    bool addTrack(int fromID, int toID, int distance) {
        if (!isValidID(fromID)) { cout << "[M3] Error: Station ID " << fromID << " is not active." << endl; return false; }
        if (!isValidID(toID)) { cout << "[M3] Error: Station ID " << toID << " is not active." << endl; return false; }
        if (fromID == toID) { cout << "[M3] Error: Cannot add a track from a station to itself." << endl; return false; }
        if (distance <= 0) { cout << "[M3] Error: Track distance must be a positive integer." << endl; return false; }
        adjMatrix[fromID][toID] = distance;
        adjMatrix[toID][fromID] = distance;
        return true;
    }

    void removeTrack(int fromID, int toID) {
        if (!isValidID(fromID) || !isValidID(toID)) {
            cout << "[M3] Error: One or both station IDs are invalid." << endl;
            return;
        }
        if (adjMatrix[fromID][toID] == NO_TRACK) {
            cout << "[M3] Error: No track exists between "
                << stations[fromID].cityName << " and "
                << stations[toID].cityName << "." << endl;
            return;
        }
        adjMatrix[fromID][toID] = NO_TRACK;
        adjMatrix[toID][fromID] = NO_TRACK;
    }

    void findShortestPath(int fromID, int toID) {
        if (!isValidID(fromID)) { cout << "[M3] Error: Source station ID " << fromID << " is invalid." << endl; return; }
        if (!isValidID(toID)) { cout << "[M3] Error: Destination station ID " << toID << " is invalid." << endl; return; }
        if (fromID == toID) { cout << "[M3] Source and destination are the same station." << endl; return; }

        int  dist[MAX_STATIONS];
        bool visited[MAX_STATIONS];
        int  parent[MAX_STATIONS];

        for (int i = 0; i < MAX_STATIONS; ++i) {
            dist[i] = INF;
            visited[i] = false;
            parent[i] = -1;
        }
        dist[fromID] = 0;

        for (int iter = 0; iter < MAX_STATIONS - 1; ++iter) {
            int u = minDistance(dist, visited);
            if (u == -1) break;
            visited[u] = true;

            for (int v = 0; v < MAX_STATIONS; ++v) {
                if (!visited[v]
                    && stations[v].active
                    && adjMatrix[u][v] != NO_TRACK
                    && dist[u] != INF
                    && dist[u] + adjMatrix[u][v] < dist[v])
                {
                    dist[v] = dist[u] + adjMatrix[u][v];
                    parent[v] = u;
                }
            }
        }

        if (dist[toID] == INF) {
            cout << "[M3] No path found between '"
                << stations[fromID].cityName << "' and '"
                << stations[toID].cityName << "'." << endl;
        }
        else {
            cout << "[M3] Shortest path: ";
            printPath(parent, toID);
            cout << endl << "[M3] Total distance: " << dist[toID] << " km" << endl;
        }
    }

    int getDistance(int fromID, int toID) {
        if (!isValidID(fromID) || !isValidID(toID)) return NO_TRACK;
        return adjMatrix[fromID][toID];
    }

    int getNeighbours(int stationID, int neighbourIDs[], int distances[]) {
        if (!isValidID(stationID)) return 0;
        int count = 0;
        for (int j = 0; j < MAX_STATIONS; ++j) {
            if (j != stationID
                && stations[j].active
                && adjMatrix[stationID][j] != NO_TRACK
                && adjMatrix[stationID][j] != 0) {
                neighbourIDs[count] = j;
                distances[count] = adjMatrix[stationID][j];
                ++count;
            }
        }
        return count;
    }

    int getStationID(string cityName) {
        for (int i = 0; i < MAX_STATIONS; ++i)
            if (stations[i].active && stations[i].cityName == cityName)
                return i;
        return -1;
    }

    string getStationName(int id) {
        if (id >= 0 && id < MAX_STATIONS && stations[id].active)
            return stations[id].cityName;
        return "INVALID";
    }

    int getStationCount() { return stationCount; }

    void displayNetwork() {
        if (stationCount == 0) {
            cout << "[M3] Network is empty - no stations registered." << endl;
            return;
        }

        cout << endl << "Railway Network (" << stationCount << " stations)" << endl;

        int active[MAX_STATIONS];
        int cnt = 0;
        for (int i = 0; i < MAX_STATIONS; ++i)
            if (stations[i].active) active[cnt++] = i;

        cout << "Station IDs:" << endl;
        for (int i = 0; i < cnt; ++i)
            cout << "  [" << active[i] << "] " << stations[active[i]].cityName << endl;
        cout << endl;

        int colW = 5; 
        for (int i = 0; i < cnt; ++i) {
            int nlen = (int)stations[active[i]].cityName.size();
            if (nlen > 10) nlen = 10;
            if (nlen + 1 > colW) colW = nlen + 1;
        }
        for (int i = 0; i < MAX_STATIONS; ++i)
            for (int j = 0; j < MAX_STATIONS; ++j)
                if (adjMatrix[i][j] != NO_TRACK && adjMatrix[i][j] > 0) {
                    int d = adjMatrix[i][j], digits = 1;
                    while (d >= 10) { d /= 10; ++digits; }
                    if (digits + 1 > colW) colW = digits + 1;
                }

        auto pad = [&](const string& s, int w) {
            cout << s;
            for (int k = (int)s.size(); k < w; ++k) cout << ' ';
            };
        auto padR = [&](const string& s, int w) {
            for (int k = (int)s.size(); k < w; ++k) cout << ' ';
            cout << s;
            };

        int labelW = colW;

        for (int k = 0; k < labelW; ++k) cout << ' ';
        cout << ' '; 

        for (int i = 0; i < cnt; ++i) {
            string hdr = "[" + to_string(active[i]) + "]";
            padR(hdr, colW);
            if (i < cnt - 1) cout << ' ';
        }
        cout << endl;

        for (int k = 0; k < labelW; ++k) cout << '-';
        cout << '+';
        for (int i = 0; i < cnt; ++i) {
            for (int k = 0; k < colW; ++k) cout << '-';
            if (i < cnt - 1) cout << '-';
        }
        cout << endl;

        for (int i = 0; i < cnt; ++i) {
            string rname = stations[active[i]].cityName;
            if ((int)rname.size() > labelW - 1) rname = rname.substr(0, labelW - 1);
            pad(rname, labelW);
            cout << '|';

            for (int j = 0; j < cnt; ++j) {
                int d = adjMatrix[active[i]][active[j]];
                string cell;
                if (active[i] == active[j]) cell = "0";
                else if (d == NO_TRACK)     cell = "-";
                else                        cell = to_string(d);
                padR(cell, colW);
                if (j < cnt - 1) cout << ' ';
            }
            cout << endl;
        }

        cout << endl << "Direct connections:" << endl;
        bool found = false;
        for (int i = 0; i < cnt; ++i) {
            for (int j = i + 1; j < cnt; ++j) {
                int d = adjMatrix[active[i]][active[j]];
                if (d != NO_TRACK && d != 0) {
                    cout << "  " << stations[active[i]].cityName
                        << " [" << active[i] << "] <-> "
                        << stations[active[j]].cityName
                        << " [" << active[j] << "] : " << d << " km" << endl;
                    found = true;
                }
            }
        }
        if (!found) cout << "  (no tracks yet)" << endl;
        cout << endl;

        cout << "Network Graph:" << endl;
        cout << "==============" << endl;
        for (int i = 0; i < cnt; ++i) {
            int id = active[i];
            string name = stations[id].cityName;
            if (name.size() > 10) name = name.substr(0, 10);
            cout << "  [" << id << "] " << name << endl;
            int nbIDs[MAX_STATIONS], nbDists[MAX_STATIONS];
            int nbCount = getNeighbours(id, nbIDs, nbDists);
            for (int k = 0; k < nbCount; ++k) {
                if (nbIDs[k] > id) {
                    string nbName = stations[nbIDs[k]].cityName;
                    if (nbName.size() > 10) nbName = nbName.substr(0, 10);
                    cout << "   |---- " << nbDists[k] << " km ----> [" << nbIDs[k] << "] " << nbName << endl;
                }
            }
        }
        cout << endl;
    }

    void saveToFile(string filename) {
        ofstream file(filename.c_str());
        if (!file.is_open()) {
            cout << "[M3] Error: could not open '" << filename << "' for writing." << endl;
            return;
        }

        file << "STATIONS" << endl;
        for (int i = 0; i < MAX_STATIONS; ++i) {
            file << i << "|"
                << (stations[i].active ? "1" : "0") << "|"
                << stations[i].cityName << endl;
        }

        file << "MATRIX" << endl;
        for (int i = 0; i < MAX_STATIONS; ++i) {
            for (int j = 0; j < MAX_STATIONS; ++j) {
                file << adjMatrix[i][j];
                if (j < MAX_STATIONS - 1) file << " ";
            }
            file << endl;
        }

        file.close();
        cout << "[M3] Network saved to '" << filename << endl;
    }

    void saveStructureToFile(string filename) {
        ofstream file(filename.c_str());
        if (!file.is_open()) {
            cout << "[M3] Error: could not open '" << filename << "' for writing." << endl;
            return;
        }

        file << "RAILWAY_NETWORK_STRUCTURE" << endl;
        file << "STATION_COUNT " << stationCount << endl;
        file << endl;

        for (int i = 0; i < MAX_STATIONS; ++i) {
            if (!stations[i].active) continue;
            file << "STATION " << i << " \"" << stations[i].cityName << "\"" << endl;
            bool hasNeighbour = false;
            for (int j = 0; j < MAX_STATIONS; ++j) {
                if (i != j && adjMatrix[i][j] != NO_TRACK && adjMatrix[i][j] != 0) {
                    file << "  --> " << j
                        << " \"" << stations[j].cityName << "\""
                        << " : " << adjMatrix[i][j] << " km" << endl;
                    hasNeighbour = true;
                }
            }
            if (!hasNeighbour)
                file << "  (no connections)" << endl;
            file << endl;
        }

        file.close();
        cout << "[M3] Network structure saved to '" << filename << endl;
    }

    void loadFromFile(string filename) {
        ifstream file(filename.c_str());
        if (!file.is_open()) {
            cout << "[M3] Error: could not open '" << filename << "' for reading." << endl;
            return;
        }

        stationCount = 0;
        for (int i = 0; i < MAX_STATIONS; ++i) {
            stations[i].active = false;
            stations[i].cityName = "";
            for (int j = 0; j < MAX_STATIONS; ++j)
                adjMatrix[i][j] = (i == j) ? 0 : NO_TRACK;
        }

        string section = "";
        string line;
        int row = 0;

        while (getline(file, line)) {
            if (line == "STATIONS") { section = "STATIONS"; continue; }
            if (line == "MATRIX") { section = "MATRIX";   row = 0; continue; }

            if (section == "STATIONS") {
                size_t p1 = line.find('|');
                size_t p2 = line.find('|', p1 + 1);
                if (p1 == string::npos || p2 == string::npos) continue;
                int id = stoi(line.substr(0, p1));
                int active = stoi(line.substr(p1 + 1, p2 - p1 - 1));
                string name = line.substr(p2 + 1);
                if (id >= 0 && id < MAX_STATIONS) {
                    stations[id].stationID = id;
                    stations[id].active = (active == 1);
                    stations[id].cityName = name;
                    if (active == 1) ++stationCount;
                }
            }
            else if (section == "MATRIX") {
                if (row < MAX_STATIONS) {
                    int col = 0;
                    string token = "";
                    for (size_t k = 0; k <= line.size(); ++k) {
                        if (k == line.size() || line[k] == ' ') {
                            if (!token.empty() && col < MAX_STATIONS) {
                                adjMatrix[row][col++] = stoi(token);
                                token = "";
                            }
                        }
                        else {
                            token += line[k];
                        }
                    }
                    ++row;
                }
            }
        }
        file.close();
        cout << "[M3] Network loaded from '" << filename << "' ("
            << stationCount << " stations)." << endl;
    }
};
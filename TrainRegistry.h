#pragma once
#include "types.h"
#include "CoachList.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

class TrainRegistry {
private:
    TrainNode* root;

    int getHeight(TrainNode* node) {
        return (node == nullptr) ? 0 : node->height;
    }

    int getbalanceFactor(TrainNode* node) {
        return (node == nullptr) ? 0 : getHeight(node->left) - getHeight(node->right);
    }

    int max(int a, int b) {
        return (a > b) ? a : b;
    }

    TrainNode* rotateRight(TrainNode* node) {
        TrainNode* x = node->left;
        TrainNode* T2 = x->right;
        x->right = node;
        node->left = T2;
        node->height = max(getHeight(node->left), getHeight(node->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        return x;
    }

    TrainNode* rotateLeft(TrainNode* node) {
        TrainNode* y = node->right;
        TrainNode* T2 = y->left;
        y->left = node;
        node->right = T2;
        node->height = max(getHeight(node->left), getHeight(node->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        return y;
    }

    TrainNode* leftRightRotate(TrainNode* node) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    TrainNode* rightLeftRotate(TrainNode* node) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    TrainNode* balance(TrainNode* node) {
        int bf = getbalanceFactor(node);
        if (bf > 1 && getbalanceFactor(node->left) >= 0) return rotateRight(node);
        if (bf > 1 && getbalanceFactor(node->left) < 0) return leftRightRotate(node);
        if (bf < -1 && getbalanceFactor(node->right) <= 0) return rotateLeft(node);
        if (bf < -1 && getbalanceFactor(node->right) >  0) return rightLeftRotate(node);
        return node;
    }

    TrainNode* insertNode(TrainNode* node, int id, string name, string route) {
        if (node == nullptr) {
            TrainNode* newNode = new TrainNode();
            newNode->trainID = id;
            newNode->trainName = name;
            newNode->route = route;
            newNode->coaches = nullptr;
            newNode->height = 1;
            newNode->left = nullptr;
            newNode->right = nullptr;
            return newNode;
        }

        if (id < node->trainID)
            node->left = insertNode(node->left, id, name, route);
        else if (id > node->trainID)
            node->right = insertNode(node->right, id, name, route);
        else
            return node;

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        return balance(node);
    }

    TrainNode* deleteNode(TrainNode* node, int id) {
        if (node == nullptr)
            return node;

        if (id < node->trainID)
            node->left = deleteNode(node->left, id);
        else if (id > node->trainID)
            node->right = deleteNode(node->right, id);
        else {
            if (node->left == nullptr || node->right == nullptr) {
                TrainNode* temp = node;
                node = (node->left == nullptr) ? node->right : node->left;
                if (temp->coaches != nullptr)
                    delete temp->coaches;
                delete temp;
            }
            else {
                TrainNode* temp = node->right;
                while (temp->left != nullptr) {
                    temp = temp->left;
                }
                int        succID = temp->trainID;
                string     succName = temp->trainName;
                string     succRoute = temp->route;
                CoachList* succCoaches = temp->coaches;
                temp->coaches = nullptr;

                if (node->coaches != nullptr) delete node->coaches;
                node->trainID = succID;
                node->trainName = succName;
                node->route = succRoute;
                node->coaches = succCoaches;

                node->right = deleteNode(node->right, succID);
            }
        }
        if (node == nullptr)
            return node;
        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        return balance(node);
    }

    TrainNode* searchNode(TrainNode* node, int id) {
        if (node == nullptr || node->trainID == id)
            return node;
        if (id < node->trainID)
            return searchNode(node->left, id);
        return searchNode(node->right, id);
    }

    void inorder(TrainNode* node) {
        if (node != nullptr) {
            inorder(node->left);
            cout << "ID: " << node->trainID
                << " | Name: " << node->trainName
                << " | Route: " << node->route << endl;
            inorder(node->right);
        }
    }

    void preorder(TrainNode* node) {
        if (node != nullptr) {
            cout << "ID: " << node->trainID
                << " | Name: " << node->trainName
                << " | Route: " << node->route << endl;
            preorder(node->left);
            preorder(node->right);
        }
    }

    void postorder(TrainNode* node) {
        if (node != nullptr) {
            postorder(node->left);
            postorder(node->right);
            cout << "ID: " << node->trainID
                << " | Name: " << node->trainName
                << " | Route: " << node->route << endl;
        }
    }

    void saveTreeStructurePreorder(TrainNode* node, ofstream& outFile) {
        if (node == nullptr) {
            outFile << "# ";
            return;
        }
        string safeName = node->trainName;
        string safeRoute = node->route;
        for (size_t i = 0; i < safeName.size(); i++) if (safeName[i] == ' ') safeName[i] = '_';
        for (size_t i = 0; i < safeRoute.size(); i++) if (safeRoute[i] == ' ') safeRoute[i] = '_';
        outFile << node->trainID << "|" << safeName << "|" << safeRoute << "|" << node->height << " ";
        saveTreeStructurePreorder(node->left, outFile);
        saveTreeStructurePreorder(node->right, outFile);
    }

    TrainNode* loadTreeStructurePreorder(ifstream& inFile) {
        string token;
        if (!(inFile >> token))
            return nullptr;
        if (token == "#")
            return nullptr;

        size_t p1 = token.find('|');
        size_t p2 = token.find('|', p1 + 1);
        size_t p3 = token.find('|', p2 + 1);
        if (p1 == string::npos || p2 == string::npos || p3 == string::npos)
            return nullptr;

        int    id = stoi(token.substr(0, p1));
        string name = token.substr(p1 + 1, p2 - p1 - 1);
        string route = token.substr(p2 + 1, p3 - p2 - 1);
        int    height = stoi(token.substr(p3 + 1));

        for (size_t i = 0; i < name.size(); i++) if (name[i] == '_') name[i] = ' ';
        for (size_t i = 0; i < route.size(); i++) if (route[i] == '_') route[i] = ' ';

        TrainNode* node = new TrainNode();
        node->trainID = id;
        node->trainName = name;
        node->route = route;
        node->coaches = nullptr;
        node->height = height;
        node->left = loadTreeStructurePreorder(inFile);
        node->right = loadTreeStructurePreorder(inFile);
        return node;
    }

    void saveHelper(TrainNode* node, ofstream& outFile) {
        if (node == nullptr) return;
        outFile << node->trainID << "," << node->trainName << "," << node->route << endl;
        saveHelper(node->left, outFile);
        saveHelper(node->right, outFile);
    }

    void saveCoachesHelper(TrainNode* node) {
        if (node == nullptr) return;
        saveCoachesHelper(node->left);
        if (node->coaches != nullptr) {
            string fname = "coaches_" + to_string(node->trainID) + ".txt";
            node->coaches->saveToFile(fname);
        }
        saveCoachesHelper(node->right);
    }

    void loadCoachesHelper(TrainNode* node) {
        if (node == nullptr) return;
        loadCoachesHelper(node->left);
        {
            string fname = "coaches_" + to_string(node->trainID) + ".txt";
            ifstream test(fname.c_str());
            if (test.is_open()) {
                test.close();
                if (node->coaches == nullptr)
                    node->coaches = new CoachList();
                node->coaches->loadFromFile(fname);
            }
        }
        loadCoachesHelper(node->right);
    }

    void destroyTree(TrainNode* node) {
        if (node != nullptr) {
            destroyTree(node->left);
            destroyTree(node->right);
            if (node->coaches != nullptr)
                delete node->coaches;
            delete node;
        }
    }

public:
    TrainRegistry() { root = nullptr; }
    ~TrainRegistry() { destroyTree(root); }

    void insertTrain(int id, string name, string route) {
        root = insertNode(root, id, name, route);
    }

    void deleteTrain(int id) {
        if (!trainExists(id)) {
            cout << "Error: Train " << id << " not found!" << endl;
            return;
        }
        root = deleteNode(root, id);
    }

    TrainNode* searchTrain(int id) {
        return searchNode(root, id);
    }

    void displayAll_Inorder() { if (!root) { cout << "No trains in registry." << endl; return; } inorder(root); }
    void displayAll_Preorder() { if (!root) { cout << "No trains in registry." << endl; return; } preorder(root); }
    void displayAll_Postorder() { if (!root) { cout << "No trains in registry." << endl; return; } postorder(root); }

    bool trainExists(int id) { return searchNode(root, id) != nullptr; }

    void saveToFile(string filename) {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cout << "Error opening file!" << endl;
            return;
        }
        saveHelper(root, outFile);
        outFile.close();
        cout << "[M1] Trains saved to '" << filename << endl;
    }

    void saveStructureToFile(string filename) {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cout << "Error opening structure file!" << endl;
            return;
        }
        outFile << "TRAIN_REGISTRY_STRUCTURE" << endl;
        outFile << "AVL_PREORDER ";
        saveTreeStructurePreorder(root, outFile);
        outFile << endl;
        outFile.close();
        cout << "[M1] AVL tree structure saved to '" << filename << endl;
    }

    void loadFromFile(string filename) {
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            cout << "Error: File not found!" << endl;
            return;
        }

        string header;
        inFile >> header;

        if (header == "TRAIN_REGISTRY_STRUCTURE") {
            string token;
            inFile >> token;
            destroyTree(root);
            root = loadTreeStructurePreorder(inFile);
            inFile.close();
            cout << "[M1] Trains loaded from '" << filename << endl;
            return;
        }

        inFile.seekg(0);
        destroyTree(root);
        root = nullptr;

        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            size_t c1 = line.find(',');
            size_t c2 = line.find(',', c1 + 1);
            if (c1 == string::npos || c2 == string::npos) continue;
            int    id = stoi(line.substr(0, c1));
            string name = line.substr(c1 + 1, c2 - c1 - 1);
            string route = line.substr(c2 + 1);
            insertTrain(id, name, route);
        }
        inFile.close();
        cout << "[M1] Trains loaded from '" << filename << endl;
    }

    void saveAllCoaches() { saveCoachesHelper(root); }
    void loadAllCoaches() { loadCoachesHelper(root); }
};
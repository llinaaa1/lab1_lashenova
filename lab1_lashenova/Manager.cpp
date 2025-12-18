#include "Manager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Utils.h"

Manager::Manager() : next_pipe_id(1), next_station_id(1) {}

int Manager::makePipeId() { return next_pipe_id++; }
int Manager::makeStationId() { return next_station_id++; }

int Manager::addPipe(const string& name, double diameter, bool in_repair) {
    int id = makePipeId();
    Pipe pipe(id, name, diameter, in_repair);
    pipes[id] = pipe;
    return id;
}

int Manager::addPipe(const Pipe& pipe) {
    int id = makePipeId();
    pipes[id] = pipe;
    return id;
}

bool Manager::removePipeById(int id) {
    return pipes.erase(id) > 0;
}
//
//Pipe& Manager::getPipeById(int id) {
//    Pipe not_found_pipe;
//    auto it = pipes.find(id);
//    if (it != pipes.end()) {
//        return it->second;
//    }
//    return not_found_pipe;
//}

vector<Pipe> Manager::findPipesByName(const string& substring) {
    vector<Pipe> result;
    for (const auto& pair : pipes) {
        if (pair.second.getName().find(substring) != string::npos) {
            result.push_back(pair.second);
        }
    }
    return result;
}

vector<Pipe> Manager::findPipesByRepairFlag(bool in_repair) {
    vector<Pipe> result;
    for (const auto& pair : pipes) {
        if (pair.second.isInRepair() == in_repair) {
            result.push_back(pair.second);
        }
    }
    return result;
}

const unordered_map<int, Pipe>& Manager::getPipes() const { return pipes; }
size_t Manager::getPipeCount() const { return pipes.size(); }

int Manager::addStation(const string& name, int total, int working, const string& classification) {
    int id = makeStationId();
    stations.emplace(id, CompressorStation(id, name, total, working, classification));
    return id;
}

int Manager::addStation(const CompressorStation& station) {
    int id = makeStationId();
    stations.emplace(id, CompressorStation(id, station.getName(), station.getTotalWorkshops(),
        station.getWorkingWorkshops(), station.getClassification()));
    return id;
}

bool Manager::removeStationById(int id) {
    auto it = stations.find(id);
    if (it == stations.end()) return false;
    stations.erase(it);
    return true;
}

CompressorStation& Manager::getStationById(int id) {
    static CompressorStation emptyStation;
    auto it = stations.find(id);
    if (it != stations.end()) {
        return it->second;
    }
    return emptyStation;
}

vector<CompressorStation> Manager::findStationsByName(const string& substring) {
    vector<CompressorStation> result;
    for (const auto& pair : stations) {
        if (pair.second.getName().find(substring) != string::npos) {
            result.push_back(pair.second);
        }
    }
    return result;
}

vector<CompressorStation> Manager::findStationsByIdlePercent(double minIdlePercent) {
    vector<CompressorStation> result;
    for (const auto& pair : stations) {
        if (pair.second.percentIdle() >= minIdlePercent) {
            result.push_back(pair.second);
        }
    }
    return result;
}

const unordered_map<int, CompressorStation>& Manager::getStations() const { return stations; }
size_t Manager::getStationCount() const { return stations.size(); }

bool Manager::saveToFile(const string& filename) {
    ofstream os(filename);
    if (!os) return false;

    for (const auto& p : pipes) {
        os << "PIPE|" << p.second.getId() << "|"
            << p.second.getName() << "|"
            << p.second.getDiameter() << "|"
            << p.second.isInRepair() << "\n";
    }
    for (const auto& s : stations) {
        os << "STATION|" << s.second.getId() << "|"
            << s.second.getName() << "|"
            << s.second.getTotalWorkshops() << "|"
            << s.second.getWorkingWorkshops() << "|"
            << s.second.getClassification() << "\n";
    }
    os.close();
    return true;
}

bool Manager::loadFromFile(const string& filename) {
    ifstream is(filename);
    if (!is) return false;

    pipes.clear();
    stations.clear();
    string line;

    while (getline(is, line)) {
        if (line.empty()) continue;

        istringstream iss(line);
        string type;
        getline(iss, type, '|');

        if (type == "PIPE") {
            string id_str, name, diam_str, repair_str;
            getline(iss, id_str, '|');
            getline(iss, name, '|');
            getline(iss, diam_str, '|');
            getline(iss, repair_str, '|');

            int id = stoi(id_str);
            double diameter = stod(diam_str);
            bool in_repair = (repair_str == "1");

            pipes.emplace(id, Pipe(id, name, diameter, in_repair));
            if (id >= next_pipe_id) next_pipe_id = id + 1;
        }
        else if (type == "STATION") {
            string id_str, name, total_str, working_str, classification;
            getline(iss, id_str, '|');
            getline(iss, name, '|');
            getline(iss, total_str, '|');
            getline(iss, working_str, '|');
            getline(iss, classification, '|');

            int id = stoi(id_str);
            int total = stoi(total_str);
            int working = stoi(working_str);

            stations.emplace(id, CompressorStation(id, name, total, working, classification));
            if (id >= next_station_id) next_station_id = id + 1;
        }
    }

    is.close();
    return true;
}

void Manager::batchEditPipes(const vector<int>& ids, int changeRepairFlag) {
    for (int id : ids) {
        auto it = pipes.find(id);
        if (it != pipes.end()) {
            Pipe& p = it->second;
            if (changeRepairFlag == 0) p.setInRepair(false);
            else if (changeRepairFlag == 1) p.setInRepair(true);
        }
    }
}

void Manager::batchEditStations(const vector<int>& ids, int workingStationsFlag) {
    for (int id : ids) {
        auto it = stations.find(id);
        if (it != stations.end()) {
            CompressorStation& cs = it->second;

            if (workingStationsFlag != 0) {
                int currentWorking = cs.getWorkingWorkshops();
                int total = cs.getTotalWorkshops();

                if (workingStationsFlag == 1) {
                    if (currentWorking < total) {
                        cs.setWorkingWorkshops(currentWorking + 1);
                    }
                }
                else if (workingStationsFlag == -1) {
                    if (currentWorking > 0) {
                        cs.setWorkingWorkshops(currentWorking - 1);
                    }
                }
            }
        }
    }
}

void Manager::addPipe() {
    cout << "Pipe name: ";
    string name;
    INPUT_LINE(cin, name);

    cout << "Diameter (number): ";
    int diam = GetCorrectNumber(1, 10000);

    cout << "In repair? (1 - yes/0 - no): ";
    bool inrep = GetCorrectNumber(0, 1) == 1;

    int id = addPipe(name, diam, inrep);
    cout << "Pipe added with ID=" << id << "\n";
}

void Manager::editPipe() {
    cout << "Pipe ID to edit: ";
    int id = GetCorrectNumber(1, 10000);

    if (!pipes.contains(id)) {
        cout << "Pipe with this ID not found\n";
        return;
    }
    cout << "Current data:\n" << pipes[id] << "\n";

    cout << "In repair? (1-yes/0-no/2-no change): ";
    int repairChoice = GetCorrectNumber(0, 2);
    pipes[id].setInRepair(repairChoice);
    //if (repairChoice == 0) {
    //    p.setInRepair(false);
    //    cout << "Repair status set to: NO\n";
    //}
    //else if (repairChoice == 1) {
    //    p.setInRepair(true);
    //    cout << "Repair status set to: YES\n";
    //}
    cout << "Edited.\n";
}

void Manager::deletePipe() {
    cout << "Pipe ID to delete: ";
    int id = GetCorrectNumber(1, 10000);
    if (removePipeById(id)) cout << "Deleted.\n"; else cout << "Not found.\n";
}

void Manager::batchEditPipesUI() {
    cout << "Search pipes for batch editing:\n";
    cout << "1. By name\n";
    cout << "2. By repair status\n";
    cout << "3. By IDs\n";
    cout << "Choice: ";

    int choice = GetCorrectNumber(1, 3);

    set<int> ids;

    if (choice == 1) {
        cout << "Enter substring of name: ";
        string q;
        INPUT_LINE(cin, q);

        vector<Pipe> found = findPipesByName(q);
        for (const auto& pipe : found) {
            ids.insert(pipe.getId());
        }
    }
    else if (choice == 2) {
        cout << "Search pipes in repair? (1-yes, 0-no): ";
        bool inRepair = GetCorrectNumber(0, 1) == 1;

        vector<Pipe> found = findPipesByRepairFlag(inRepair);
        for (const auto& pipe : found) {
            ids.insert(pipe.getId());
        }
    }
    else if (choice == 3) {
        cout << "Enter IDs (0 to finish):\n";
        while (true) {
            cout << "ID: ";
            int id = GetCorrectNumber(0, 10000);
            if (id == 0) break;

            if (!pipes.contains(id)) {
                cout << "Pipe with ID " << id << " not found!\n";
            }
            else {
                ids.insert(id);
                cout << "Added pipe ID: " << id << "\n";
            }
        }
    }
    else {
        cout << "Invalid choice.\n";
        return;
    }

    if (ids.empty()) {
        cout << "No pipes selected for editing.\n";
        return;
    }

    cout << "\nSelected " << ids.size() << " pipes for editing:\n";


    cout << "\nSet repair status: (1) In repair, (0) Not in repair: ";
    bool newStatus = GetCorrectNumber(0, 1) == 1;

    cout << "Confirm? (1-yes, 0-no): ";
    if (GetCorrectNumber(0, 1) == 1) {
        vector<int> idVector(ids.begin(), ids.end());
        batchEditPipes(idVector, newStatus ? 1 : 0);
        cout << "Updated " << ids.size() << " pipes.\n";
    }
}

void Manager::listAllPipes() {
    cout << "Total pipes: " << getPipeCount() << "\n";
    for (const auto& pair : getPipes()) cout << pair.second << "\n";
}

void Manager::addStation() {
    cout << "Compressor Station name: ";
    string name;
    INPUT_LINE(cin, name);
    cout << "Total workshops: ";
    int total = GetCorrectNumber(1, 10000);

    int working;
    while (true) {
        cout << "Working workshops: ";
        working = GetCorrectNumber(0, 10000);

        if (working <= total) {
            break;
        }
        else {
            cout << "Error: Working workshops cannot be more than total workshops (" << total << ")\n";
            cout << "Please enter a valid number (0-" << total << "): ";
        }
    }

    cout << "Classification: ";
    string cls;
    INPUT_LINE(cin, cls);

    int id = addStation(name, total, working, cls);
    cout << "Station added with ID=" << id << "\n";
}

void Manager::editStation() {
    cout << "Compressor Station ID to edit: ";
    int id = GetCorrectNumber(1, 10000);
    CompressorStation& s = getStationById(id);
    if (s.getId() == 0) {
        cout << "Not found.\n";
        return;
    }
    cout << s << "\n";

    cout << "New working: ";
    string work;
    INPUT_LINE(cin, work);

    if (!work.empty()) {
        try {
            int new_working = stoi(work);
            if (new_working > s.getTotalWorkshops()) {
                cout << "Error: Working workshops (" << new_working
                    << ") cannot be more than total workshops (" << s.getTotalWorkshops() << ")\n";
            }
            else {
                s.setWorkingWorkshops(new_working);
                cout << "Working workshops updated to: " << new_working << endl;
            }
        }
        catch (...) {
            cout << "Invalid number format\n";
        }
    }
    cout << "Edited.\n";
}

void Manager::deleteStation() {
    cout << "Compressor Station ID to delete: ";
    int id = GetCorrectNumber(1, 10000);
    if (removeStationById(id)) cout << "Deleted.\n"; else cout << "Not found.\n";
}

void Manager::batchEditStationsUI() {
    cout << "\n=== Batch Edit Stations ===\n";
    cout << "Search stations by:\n";
    cout << "1. By name\n";
    cout << "2. By idle percent\n";
    cout << "3. By IDs\n";
    cout << "Choice: ";

    int choice = GetCorrectNumber(1, 3);

    set<int> ids;

    if (choice == 1) {
        cout << "Enter substring of name: ";
        string q;
        INPUT_LINE(cin, q);

        vector<CompressorStation> found = findStationsByName(q);
        for (const auto& station : found) {
            ids.insert(station.getId());
        }
    }
    else if (choice == 2) {
        cout << "Minimum idle percent (0-100): ";
        double perc = GetCorrectNumber(0.0, 100.0);

        vector<CompressorStation> found = findStationsByIdlePercent(perc);
        for (const auto& station : found) {
            ids.insert(station.getId());
        }
    }
    else if (choice == 3) {
        cout << "Enter IDs (0 to finish):\n";
        while (true) {
            cout << "ID: ";
            int inputId = GetCorrectNumber(0, 10000);
            if (inputId == 0) break;

            CompressorStation& station = getStationById(inputId);
            if (station.getId() == 0) {
                cout << "Station with ID " << inputId << " not found!\n";
            }
            else {
                ids.insert(inputId);
                cout << "Added station ID: " << inputId << "\n";
            }
        }
    }

    if (ids.empty()) {
        cout << "No stations selected for editing.\n";
        return;
    }

    cout << "\nSelected " << ids.size() << " stations:\n";
    for (int id : ids) {
        cout << getStationById(id) << "\n";
    }

    cout << "\nChange working workshops:\n";
    cout << "0. No change\n";
    cout << "1. Increase by 1\n";
    cout << "2. Decrease by 1\n";
    cout << "Choice: ";

    int wf = GetCorrectNumber(0, 2);

    if (wf == 0) {
        cout << "No changes made.\n";
        return;
    }

    int workingStationsFlag = (wf == 1) ? 1 : -1;

    cout << "Confirm? (1-yes, 0-no): ";
    if (GetCorrectNumber(0, 1) == 1) {
        vector<int> idVector(ids.begin(), ids.end());
        batchEditStations(idVector, workingStationsFlag);
        cout << "Updated " << ids.size() << " stations.\n";
    }
    else {
        cout << "Operation cancelled.\n";
    }
}

void Manager::listAllStations() {
    cout << "Total stations: " << getStationCount() << "\n";
    for (const auto& pair : getStations()) cout << pair.second << "\n";
}

void Manager::saveToFileUI() {
    cout << "Enter filename to save: ";
    string fname;
    INPUT_LINE(cin, fname);
    if (saveToFile(fname)) cout << "Saved.\n"; else cout << "Error saving.\n";
}

void Manager::loadFromFileUI() {
    cout << "Enter filename to load: ";
    string fname;
    INPUT_LINE(cin, fname);
    if (loadFromFile(fname)) cout << "Loaded.\n"; else cout << "Error loading.\n";
}
#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <set>
#include <ctime>
#include "Manager.h"
#include "Utils.h"

using namespace std;

void addPipe(Manager& manager) {
    cout << "Pipe name: ";
    string name;
    INPUT_LINE(cin, name);

    cout << "Diameter (number): ";
    int diam = GetCorrectNumber(1, 10000);

    cout << "In repair? (1 - yes/0 - no): ";
    bool inrep = GetCorrectNumber(0, 1) == 1;

    int id = manager.addPipe(name, diam, inrep);
    cout << "Pipe added with ID=" << id << "\n";
}

void editPipe(Manager& manager){
    cout << "Pipe ID to edit: ";
    int id = GetCorrectNumber(1, 10000);

    Pipe& p = manager.getPipeById(id);
    if (p.getId() == 0) {
        cout << "Pipe with this ID not found\n";
        return;
    }
    cout << "Current data:\n" << p << "\n";

    cout << "In repair? (1-yes/0-no/2-no change): ";
    int repairChoice = GetCorrectNumber(0, 2);
    if (repairChoice == 0) {
        p.setInRepair(false);
        cout << "Repair status set to: NO\n";
    }
    else if (repairChoice == 1) {
        p.setInRepair(true);
        cout << "Repair status set to: YES\n";
    }
    cout << "Edited.\n";
}

void deletePipe(Manager& manager){
    cout << "Pipe ID to delete: ";
    int id = GetCorrectNumber(1,10000);
    if (manager.removePipeById(id)) cout << "Deleted.\n"; else cout << "Not found.\n";
}

void batchEditPipes(Manager& manager){
    cout << "Search pipes for batch editing:\n";
    cout << "1. By name\n";
    cout << "2. By repair status\n";
    cout << "3. By IDs\n";
    cout << "Choice: ";

    int choice = GetCorrectNumber(1,3);

    set<int> ids;

    if (choice == 1) {
        cout << "Enter substring of name: ";
        string q;
        INPUT_LINE(cin, q);

        ids = find_by_filter(manager.getPipes(), filter_pipe_by_name, q);
        cout << "Found " << ids.size() << " pipes:\n";

        for (int id : ids) {
            cout << manager.getPipeById(id) << "\n";
        }
    }
    else if (choice == 2) { 
        cout << "Search pipes in repair? (1-yes, 0-no): ";
        bool inRepair = GetCorrectNumber(0, 1) == 1;

        ids = find_by_filter(manager.getPipes(), filter_pipe_by_repair, inRepair);
        cout << "Found " << ids.size() << " pipes:\n";

        for (int id : ids) {
            cout << manager.getPipeById(id) << "\n";
        }
    }
    else if (choice == 3) {
        cout << "Enter IDs (0 to finish):\n";
        while (true) {
            cout << "ID: ";
            int id = GetCorrectNumber(0, 10000);
            if (id == 0) break;

            Pipe& pipe = manager.getPipeById(id);
            if (pipe.getId() == 0) {
                cout << "Pipe with ID " << id << " not found!\n";
            } else {
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
    for (int id : ids) {
        cout << manager.getPipeById(id) << "\n";
    }

    cout << "\nSet repair status: (1) In repair, (0) Not in repair: ";
    bool newStatus = GetCorrectNumber(0, 1) == 1;

    cout << "Confirm? (1-yes, 0-no): ";
    if (GetCorrectNumber(0, 1) == 1) {
        vector<int> idVector(ids.begin(), ids.end());
        manager.batchEditPipes(idVector, newStatus ? 1 : 0);
        cout << "Updated " << ids.size() << " pipes.\n";
    }
}

void listAllPipes(Manager& manager){
    cout << "Total pipes: " << manager.getPipeCount() << "\n";
    for (const auto& pair : manager.getPipes()) cout << pair.second << "\n";
}

void addStation(Manager& manager){
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

    int id = manager.addStation(name, total, working, cls);
    cout << "Station added with ID=" << id << "\n";
}

void editStation(Manager& manager){
    cout << "Compressor Station ID to edit: ";
    int id = GetCorrectNumber(1, 10000);
    CompressorStation& s = manager.getStationById(id);
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

void deleteStation(Manager& manager) {
    cout << "Compressor Station ID to delete: ";
    int id = GetCorrectNumber(1, 10000);
    if (manager.removeStationById(id)) cout << "Deleted.\n"; else cout << "Not found.\n";
}

void batchEditStations(Manager& manager) {
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

        ids = find_by_filter(manager.getStations(), filter_station_by_name, q);
        cout << "Found " << ids.size() << " stations:\n";

        for (int id : ids) {
            cout << manager.getStationById(id) << "\n";
        }
    }
    else if (choice == 2) {
        cout << "Minimum idle percent (0-100): ";
        double perc = GetCorrectNumber(0.0, 100.0);

        ids = find_by_filter(manager.getStations(), filter_station_by_idle_percent, perc);
        cout << "Found " << ids.size() << " stations:\n";

        for (int id : ids) {
            cout << manager.getStationById(id) << "\n";
        }
    }
    else if (choice == 3) {
        cout << "Enter IDs (0 to finish):\n";
        while (true) {
            cout << "ID: ";
            int inputId = GetCorrectNumber(0, 10000);
            if (inputId == 0) break;

            CompressorStation& station = manager.getStationById(inputId);
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
        cout << manager.getStationById(id) << "\n";
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
        manager.batchEditStations(idVector, workingStationsFlag);
        cout << "Updated " << ids.size() << " stations.\n";
    }
    else {
        cout << "Operation cancelled.\n";
    }
}

void listAllStations(Manager& manager) {
    cout << "Total stations: " << manager.getStationCount() << "\n";
    for (const auto& pair : manager.getStations()) cout << pair.second << "\n";
}

void saveToFile(Manager& manager) {
    cout << "Enter filename to save: ";
    string fname;
    INPUT_LINE(cin, fname);
    if (manager.saveToFile(fname)) cout << "Saved.\n"; else cout << "Error saving.\n";
}

void loadFromFile(Manager& manager) {
    cout << "Enter filename to load: ";
    string fname;
    INPUT_LINE(cin, fname);
    if (manager.loadFromFile(fname)) cout << "Loaded.\n"; else cout << "Error loading.\n";
}

void printMenu() {
    cout << "\nMain Menu:\n";
    cout << "1) Add Pipe\n";
    cout << "2) Edit Pipe\n";
    cout << "3) Delete Pipe\n";
    cout << "4) Search and Batch Edit Pipes\n";
    cout << "5) List All Pipes\n";
    cout << "6) Add Compressor Station\n";
    cout << "7) Edit Compressor Station\n";
    cout << "8) Delete Compressor Station\n";
    cout << "9) Search and Batch Compressor Stations\n";
    cout << "10) List All Compressor Stations\n";
    cout << "11) Save to File\n";
    cout << "12) Load from File\n";
    cout << "0) Exit\n";
    cout << "Choose an option: ";
}

int main() {
    redirect_output_wrapper cerr_out(cerr);

    string filename = "log_" + to_string(time(nullptr));

    ofstream logfile(filename);
    if (logfile)
        cerr_out.redirect(logfile);

    Manager manager;
    cout << "=== Pipe and Compressor Station Manager ===\n";


    bool running = true;
    while (running) {
        printMenu();
        switch (GetCorrectNumber(0, 12)) {
        case 1: addPipe(manager); break;
        case 2: editPipe(manager); break;
        case 3: deletePipe(manager); break;
        case 4: batchEditPipes(manager); break;
        case 5: listAllPipes(manager); break;
        case 6: addStation(manager); break;
        case 7: editStation(manager); break;
        case 8: deleteStation(manager); break;
        case 9: batchEditStations(manager); break;
        case 10: listAllStations(manager); break;
        case 11: saveToFile(manager); break;
        case 12: loadFromFile(manager); break;

        case 0: {
            running = false;
            cout << "Goodbye!\n";
            break;
        }
        default: 
            cout << "Invalid option.\n";
            break;
        }
    }
    return 0;
}
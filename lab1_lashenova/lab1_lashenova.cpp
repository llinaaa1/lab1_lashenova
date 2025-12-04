#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <set>
#include <sstream>
#include "Manager.h"
#include "Utils.h"

using namespace std;

void addPipe(Manager& manager) {
    cout << "Pipe name: ";
    string name;
    INPUT_LINE(cin, name);

    cout << "Diameter (number): ";
    double diam = GetCorrectNumber(1.0, 10000.0);

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
    }
    cout << "Current data:\n" << p << "\n";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

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
    cout << "1) by name\n";
    cout << "2) by 'in repair' flag\n";
    cout << "3) enter IDs manually\n";
    cout << "Choose search method: ";
    int m = GetCorrectNumber(1,3);

    set<int> ids;

    if (m == 1) {
    cout << "Enter substring of name: ";
    string q;
    INPUT_LINE(cin, q);
    auto res = manager.findPipesByName(q);
    cout << "Found " << res.size() << " pipes:\n";
    for (auto p : res) {
        cout << p << "\n";
        ids.insert(p.getId());
    }
}
    else if (m == 2) { 
        cout << "Search pipes in repair? (y = yes, n = no): ";
        string rep;
        INPUT_LINE(cin, rep);
        bool flag = (rep.size() > 0 && (rep[0] == 'y' || rep[0] == 'Y'));
        auto res = manager.findPipesByRepairFlag(flag);
        cout << "Found " << res.size() << " pipes:\n";
        for (auto p : res) {
            cout << p << "\n";
            ids.insert(p.getId());
        }
}
    else if (m == 3) {
        cout << "Enter IDs separated by space: ";
        string line;
        INPUT_LINE(cin, line);
        istringstream iss(line);
        int v;
        while (iss >> v) ids.insert(v);
        cout << "Selected " << ids.size() << " pipes for editing.\n";
    }
    else {
        cout << "Invalid choice.\n";
        return;
    }

    if (ids.empty()) {
        cout << "No pipes selected for editing.\n";
        return;
    }

    cout << "You selected " << ids.size() << " pipes for batch editing.\n";
    cout << "Do you want to edit all of them? (1-yes/0-no): ";
    int confirm = GetCorrectNumber(0, 1);

    if (confirm == 0) {
        set<int> selectedIds;
        cout << "Enter the IDs you want to edit (separated by space): ";
        string idLine;
        INPUT_LINE(cin, idLine);
        istringstream idStream(idLine);
        int selectedId;
        while (idStream >> selectedId) {
            if (ids.count(selectedId) > 0) {
                selectedIds.insert(selectedId);
            }
            else {
                cout << "ID " << selectedId << " is not in the found list.\n";
            }
        }
        ids = selectedIds;
    }

    if (ids.empty()) {
        cout << "No pipes selected for editing.\n";
        return;
    }

    cout << "Batch editing " << ids.size() << " pipes.\n";
    cout << "Change 'in repair' flag? (0 = no change, 1 = set true, 2 = set false)\n";
    cout << "Choice: ";
    int rf = GetCorrectNumber(0, 2);
    int changeFlag = -1;
    if (rf == 1) changeFlag = 1; // Установить 'в ремонте'
    else if (rf == 2) changeFlag = 0; // Установить 'не в ремонте'

    for (int id : ids) {
        Pipe& p = manager.getPipeById(id);  
        if (p.getId() != 0) {
            if (changeFlag == 0) {
                p.setInRepair(false);
            }
            else if (changeFlag == 1) {
                p.setInRepair(true);
            }
        }
    }
    cout << "Batch editing completed for " << ids.size() << " pipes.\n";
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

    cout << "New name: ";
    string n;
    INPUT_LINE(cin, n);

    cout << "New working: ";
    string work;
    INPUT_LINE(cin, work);

    if (!n.empty()) s.setName(n);

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

void batchEditStations(Manager& manager){
    cout << "Search stations for batch editing:\n";
    cout << "1) by name\n";
    cout << "2) by idle percent\n";
    cout << "3) enter IDs manually\n";
    cout << "Choose search method: ";

    int n = GetCorrectNumber(1, 3);

    set<int> ids;

    if (n == 1) { 
        cout << "Enter substring of name: ";
        string q;
        INPUT_LINE(cin, q);
        auto res = manager.findStationsByName(q);
        cout << "Found " << res.size() << " stations:\n";
        for (auto s : res) {
            cout << s << "\n";
            ids.insert(s.getId());
        }
    }
    else if (n == 2) {
        cout << "Min idle percent (e.g., 50.0): ";
        double perc = GetCorrectNumber(0.0, 100.0);
        auto res = manager.findStationsByIdlePercent(perc);
        cout << "Found " << res.size() << " stations:\n";
        for (auto s : res) {
            cout << s << "\n";
            ids.insert(s.getId());
        }
    }
    else if (n == 3) {
        cout << "Enter IDs: ";
        string line;
        INPUT_LINE(cin, line);
        istringstream iss(line);
        int v;
        while (iss >> v) ids.insert(v);
        cout << "Selected " << ids.size() << " stations for editing.\n";
    }

    if (ids.empty()) {
        cout << "No stations selected for editing.\n";
        return;
    }

    cout << "You selected " << ids.size() << " stations for batch editing.\n";
    cout << "Do you want to edit all of them? (1-yes/0-no): ";
    int confirm = GetCorrectNumber(0, 1);

    if (confirm == 0) {
        set<int> selectedIds;
        cout << "Enter the IDs you want to edit (separated by space): ";
        string idLine;
        INPUT_LINE(cin, idLine);
        istringstream idStream(idLine);
        int selectedId;
        while (idStream >> selectedId) {
            if (ids.count(selectedId) > 0) {
                selectedIds.insert(selectedId);
            }
            else {
                cout << "ID " << selectedId << " is not in the found list.\n";
            }
        }
        ids = selectedIds;
    }

    if (ids.empty()) {
        cout << "No stations selected for editing.\n";
        return;
    }

    cout << "Batch editing " << ids.size() << " stations.\n";
    cout << "Change working workshops? (0 = no change, 1 = +1, 2 = -1)\n";
    cout << "Choice: ";
    int wf = GetCorrectNumber(0, 2);
    int workingStationsFlag = 0;
    if (wf == 1) workingStationsFlag = 1;   
    else if (wf == 2) workingStationsFlag = -1; 
    vector<int> idVector(ids.begin(), ids.end());
    manager.batchEditStations(idVector, "", workingStationsFlag);
    cout << "Batch editing completed for " << ids.size() << " stations.\n";
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
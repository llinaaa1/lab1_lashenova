#include <iostream>
#include <string>
#include <limits>
#include <fstream>

using namespace std;

struct Pipe {
    string name;
    double length;
    double diameter;
    bool UnderRepair;
};

struct OilPumpingStation {
    string name;
    int totalPumps;
    int workingPumps;
    double maxCapacity;
    int stationClass;
};

template <typename T>
T getValidInput(const string& prompt) {
    T value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error: Enter a valid number.\n";
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

void createPipe(Pipe& pipe) {
    cout << "\n=== Add New Pipe ===" << endl;
    cout << "Enter pipe name: ";
    getline(cin, pipe.name);

    pipe.length = getValidInput<double>("Enter pipe length (km): ");
    while (pipe.length <= 0) {
        cout << "Length must be positive. Try again.\n";
        pipe.length = getValidInput<double>("Enter pipe length (km): ");
    }

    pipe.diameter = getValidInput<double>("Enter pipe diameter (mm): ");
    while (pipe.diameter <= 0) {
        cout << "Diameter must be positive. Try again.\n";
        pipe.diameter = getValidInput<double>("Enter pipe diameter (mm): ");
    }

    pipe.UnderRepair = false;
    cout << "Pipe '" << pipe.name << "' added successfully!\n" << endl;
}

void createStation(OilPumpingStation& station) {
    cout << "\n=== Add New Oil Pumping Station ===" << endl;
    cout << "Enter station name: ";
    getline(cin, station.name);

    station.totalPumps = getValidInput<int>("Enter total number of pumps: ");
    while (station.totalPumps <= 0) {
        cout << "Number must be positive. Try again.\n";
        station.totalPumps = getValidInput<int>("Enter total number of pumps: ");
    }

    station.workingPumps = getValidInput<int>("Enter working pumps: ");
    while (station.workingPumps < 0 || station.workingPumps > station.totalPumps) {
        cout << "Working pumps cannot be negative or exceed total. Try again.\n";
        station.workingPumps = getValidInput<int>("Enter working pumps: ");
    }

    station.maxCapacity = getValidInput<double>("Enter maximum capacity (m^3/hour): ");
    while (station.maxCapacity <= 0) {
        cout << "Capacity must be positive. Try again.\n";
        station.maxCapacity = getValidInput<double>("Enter maximum capacity (m³/hour): ");
    }

    station.stationClass = getValidInput<int>("Enter station class (1-5): ");
    while (station.stationClass < 1 || station.stationClass > 5) {
        cout << "Class must be between 1 and 5. Try again.\n";
        station.stationClass = getValidInput<int>("Enter station class (1-5): ");
    }

    cout << "Station '" << station.name << "' added successfully!\n" << endl;
}

void displayAllObjects(const Pipe& pipe, const OilPumpingStation& station) {
    cout << "\n=== ALL OBJECTS ===" << endl;

    cout << "--- Pipe ---" << endl;
    if (pipe.name.empty()) {
        cout << "No pipe created." << endl;
    }
    else {
        cout << "Name: " << pipe.name << endl;
        cout << "Length: " << pipe.length << " km" << endl;
        cout << "Diameter: " << pipe.diameter << " mm" << endl;
        cout << "Under repair: " << (pipe.UnderRepair ? "Yes" : "No") << endl;
    }

    cout << "\n--- Oil Pumping Station ---" << endl;
    if (station.name.empty()) {
        cout << "No station created." << endl;
    }
    else {
        cout << "Name: " << station.name << endl;
        cout << "Total pumps: " << station.totalPumps << endl;
        cout << "Working pumps: " << station.workingPumps << endl;
        cout << "Max capacity: " << station.maxCapacity << " m³/hour" << endl;
        cout << "Station class: " << station.stationClass << endl;
        double efficiency = (station.totalPumps > 0) ?
            (static_cast<double>(station.workingPumps) / station.totalPumps) * 100 : 0;
        cout << "Current efficiency: " << efficiency << "%" << endl;
    }
    cout << "--------------------------------" << endl;
}

void editPipe(Pipe& pipe) {
    if (pipe.name.empty()) {
        cout << "Error: Create a pipe first." << endl;
        return;
    }
    cout << "\n=== Edit Pipe '" << pipe.name << "' ===" << endl;
    cout << "Current repair status: " << (pipe.UnderRepair ? "Under repair" : "Operational") << endl;
    cout << "1. Start repair" << endl;
    cout << "2. Finish repair" << endl;
    cout << "0. Cancel" << endl;

    int choice = getValidInput<int>("Select action: ");

    switch (choice) {
    case 1:
        if (!pipe.UnderRepair) {
            pipe.UnderRepair = true;
            cout << "Repair started." << endl;
        }
        else {
            cout << "Pipe is already under repair." << endl;
        }
        break;
    case 2:
        if (pipe.UnderRepair) {
            pipe.UnderRepair = false;
            cout << "Repair finished." << endl;
        }
        else {
            cout << "Pipe is not under repair." << endl;
        }
        break;
    case 0:
        cout << "Canceled." << endl;
        break;
    default:
        cout << "Invalid choice." << endl;
    }
}

void editStation(OilPumpingStation& station) {
    if (station.name.empty()) {
        cout << "Error: Create a station first." << endl;
        return;
    }
    cout << "\n=== Edit Station '" << station.name << "' ===" << endl;
    cout << "Working pumps: " << station.workingPumps << " of " << station.totalPumps << endl;
    cout << "1. Start pump" << endl;
    cout << "2. Stop pump" << endl;
    cout << "0. Cancel" << endl;

    int choice = getValidInput<int>("Select action: ");

    switch (choice) {
    case 1:
        if (station.workingPumps < station.totalPumps) {
            station.workingPumps++;
            cout << "Pump started. Now working: " << station.workingPumps << " pumps." << endl;
        }
        else {
            cout << "Error: All pumps are already working." << endl;
        }
        break;
    case 2:
        if (station.workingPumps > 0) {
            station.workingPumps--;
            cout << "Pump stopped. Now working: " << station.workingPumps << " pumps." << endl;
        }
        else {
            cout << "Error: No working pumps to stop." << endl;
        }
        break;
    case 0:
        cout << "Canceled." << endl;
        break;
    default:
        cout << "Invalid choice." << endl;
    }
}

void saveToFile(const Pipe& pipe, const OilPumpingStation& station, const string& filename = "pipeline_data.txt") {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open file for writing." << endl;
        return;
    }

    if (!pipe.name.empty()) {
        file << "PIPE" << endl;
        file << pipe.name << endl;
        file << pipe.length << endl;
        file << pipe.diameter << endl;
        file << pipe.UnderRepair << endl;
    }

    if (!station.name.empty()) {
        file << "STATION" << endl;
        file << station.name << endl;
        file << station.totalPumps << endl;
        file << station.workingPumps << endl;
        file << station.maxCapacity << endl;
        file << station.stationClass << endl;
    }

    file.close();
    cout << "Data saved to file '" << filename << "'." << endl;
}

void loadFromFile(Pipe& pipe, OilPumpingStation& station, const string& filename = "pipeline_data.txt") {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open file for reading." << endl;
        return;
    }

    string line;
    pipe.name = "";
    station.name = "";

    while (getline(file, line)) {
        if (line == "PIPE") {
            getline(file, pipe.name);
            file >> pipe.length >> pipe.diameter >> pipe.UnderRepair;
            file.ignore();
        }
        else if (line == "STATION") {
            getline(file, station.name);
            file >> station.totalPumps >> station.workingPumps >> station.maxCapacity >> station.stationClass;
            file.ignore();
        }
    }

    file.close();
    cout << "Data loaded from file '" << filename << "'." << endl;
}

void displayMenu() {
    cout << "\n=================================" << endl;
    cout << "    MENU" << endl;
    cout << "=================================" << endl;
    cout << "1. Add pipe" << endl;
    cout << "2. Add pumping station" << endl;
    cout << "3. View all objects" << endl;
    cout << "4. Edit pipe repair status" << endl;
    cout << "5. Manage station pumps" << endl;
    cout << "6. Save data to file" << endl;
    cout << "7. Load data from file" << endl;
    cout << "0. Exit" << endl;
    cout << "=================================" << endl;
    cout << "Enter command number: ";
}

void processCommand(int command, Pipe& pipe, OilPumpingStation& station) {
    switch (command) {
    case 1:
        createPipe(pipe);
        break;
    case 2:
        createStation(station);
        break;
    case 3:
        displayAllObjects(pipe, station);
        break;
    case 4:
        editPipe(pipe);
        break;
    case 5:
        editStation(station);
        break;
    case 6:
        saveToFile(pipe, station);
        break;
    case 7:
        loadFromFile(pipe, station);
        break;
    case 0:
        cout << "Exiting program. Goodbye!" << endl;
        exit(0);
    default:
        cout << "Command not recognized. Please select from menu." << endl;
    }
}

int main() {
    Pipe myPipe;
    OilPumpingStation myStation;

    while (true) {
        displayMenu();
        int command = getValidInput<int>("");
        processCommand(command, myPipe, myStation);
    }

    return 0;
}
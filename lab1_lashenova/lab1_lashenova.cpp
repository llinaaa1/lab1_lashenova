#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <set>
#include <ctime>
#include "Manager.h"
#include "Utils.h"

using namespace std;


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
        case 1: manager.addPipe(); break;
        case 2: manager.editPipe(); break;
        case 3: manager.deletePipe(); break;
        case 4: manager.batchEditPipesUI(); break;
        case 5: manager.listAllPipes(); break;
        case 6: manager.addStation(); break;
        case 7: manager.editStation(); break;
        case 8: manager.deleteStation(); break;
        case 9: manager.batchEditStationsUI(); break;
        case 10: manager.listAllStations(); break;
        case 11: manager.saveToFileUI(); break;
        case 12: manager.loadFromFileUI(); break;

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
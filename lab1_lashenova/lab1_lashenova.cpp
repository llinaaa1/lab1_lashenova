#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <algorithm>
#include <sstream>
#include "Manager.h"

// Вспомогательные функции
static void ignoreLine() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static std::string inputLine(const std::string& prompt) {
    std::string s;
    std::cout << prompt;
    std::getline(std::cin, s);
    return s;
}

static int inputInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        int v;
        if (std::cin >> v) {
            ignoreLine();
            return v;
        }
        else {
            std::cout << "Invalid input. Please try again.\n";
            std::cin.clear();
            ignoreLine();
        }
    }
}

static double inputDouble(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        double v;
        if (std::cin >> v) { ignoreLine(); return v; }
        else {
            std::cout << "Invalid input. Please try again.\n";
            std::cin.clear();
            ignoreLine();
        }
    }
}

void addSampleData(Manager& m) {
    m.addPipe("MainLine-1", 500.0, false);
    m.addPipe("Feeder-A", 250.0, true);
    m.addPipe("Bypass-02", 300.0, false);
    m.addStation("CS-North", 10, 8, "A");
    m.addStation("CS-South", 6, 2, "B");
    m.addStation("CS-East", 12, 12, "A+");
}

int main() {
    Manager manager;
    std::cout << "=== Pipe and Compressor Station Manager ===\n";
    std::string logf = inputLine("Enter log filename (or press Enter for actions.log): ");
    if (!logf.empty()) manager.setLogFilename(logf);

    bool running = true;
    while (running) {
        std::cout << "\nMain Menu:\n";
        std::cout << "1) Add Pipe\n";
        std::cout << "2) Edit Pipe\n";
        std::cout << "3) Delete Pipe\n";
        std::cout << "4) Search and Batch Edit Pipes\n";
        std::cout << "5) List All Pipes\n";
        std::cout << "6) Add Compressor Station\n";
        std::cout << "7) Edit Compressor Station\n";
        std::cout << "8) Delete Compressor Station\n";
        std::cout << "9) Search Compressor Stations\n";
        std::cout << "10) List All Compressor Stations\n";
        std::cout << "11) Save to File\n";
        std::cout << "12) Load from File\n";
        std::cout << "13) Add Sample Data\n";
        std::cout << "0) Exit\n";
        int choice = inputInt("Choose an option: ");
        switch (choice) {
        case 1: {
            std::string name = inputLine("Pipe name: ");
            double diam = inputDouble("Diameter (number): ");
            std::string rep = inputLine("In repair? (y/n): ");
            bool inrep = (rep.size() > 0 && (rep[0] == 'y' || rep[0] == 'Y'));
            uint64_t id = manager.addPipe(name, diam, inrep);
            std::cout << "Pipe added with ID=" << id << "\n";
            break;
        }
        case 2: {
            uint64_t id = (uint64_t)inputInt("Pipe ID to edit: ");
            Pipe* p = manager.findPipeById(id);
            if (!p) { std::cout << "Pipe with this ID not found\n"; break; }
            std::cout << "Current data:\n" << *p << "\n";
            std::string newName = inputLine("New name (Enter = no change): ");
            std::string dstr = inputLine("New diameter (Enter = no change): ");
            if (!newName.empty()) p->setName(newName);
            if (!dstr.empty()) {
                try { double d = std::stod(dstr); p->setDiameter(d); }
                catch (...) { std::cout << "Diameter not changed: invalid input\n"; }
            }
            std::string rep = inputLine("In repair? (y/n/Enter = no change): ");
            if (!rep.empty()) {
                bool inrep = (rep[0] == 'y' || rep[0] == 'Y');
                p->setInRepair(inrep);
            }
            manager.writeLog("Edited pipe id=" + std::to_string(p->getId()));
            std::cout << "Edited.\n";
            break;
        }
        case 3: {
            uint64_t id = (uint64_t)inputInt("Pipe ID to delete: ");
            if (manager.removePipeById(id)) std::cout << "Deleted.\n"; else std::cout << "Not found.\n";
            break;
        }
        case 4: {
            std::cout << "Search pipes for batch editing:\n";
            std::cout << "1) by name\n";
            std::cout << "2) by 'in repair' flag\n";
            std::cout << "3) enter IDs manually\n";
            int m = inputInt("Choose search method: ");

            std::vector<uint64_t> ids;

            if (m == 1) {
                std::string q = inputLine("Enter substring of name: ");
                auto res = manager.findPipesByName(q);
                std::cout << "Found " << res.size() << " pipes:\n";
                for (auto p : res) {
                    std::cout << *p << "\n";
                    ids.push_back(p->getId());
                }
            }
            else if (m == 2) {
                std::string rep = inputLine("Search pipes in repair? (y = yes, n = no): ");
                bool flag = (rep.size() > 0 && (rep[0] == 'y' || rep[0] == 'Y'));
                auto res = manager.findPipesByRepairFlag(flag);
                std::cout << "Found " << res.size() << " pipes:\n";
                for (auto p : res) {
                    std::cout << *p << "\n";
                    ids.push_back(p->getId());
                }
            }
            else if (m == 3) {
                std::string line = inputLine("Enter IDs separated by space: ");
                std::istringstream iss(line);
                uint64_t v;
                while (iss >> v) ids.push_back(v);
                std::cout << "Selected " << ids.size() << " pipes for editing.\n";
            }
            else {
                std::cout << "Invalid choice.\n";
                break;
            }

            if (ids.empty()) {
                std::cout << "No pipes selected for editing.\n";
                break;
            }

            // Подтверждение выбора для пакетного редактирования
            std::cout << "You selected " << ids.size() << " pipes for batch editing.\n";
            std::cout << "Do you want to edit all of them? (y/n): ";
            std::string confirm;
            std::getline(std::cin, confirm);

            if (!confirm.empty() && (confirm[0] == 'n' || confirm[0] == 'N')) {
                // Пользователь хочет выбрать подмножество
                std::vector<uint64_t> selectedIds;
                std::cout << "Enter the IDs you want to edit (separated by space): ";
                std::string idLine;
                std::getline(std::cin, idLine);
                std::istringstream idStream(idLine);
                uint64_t selectedId;
                while (idStream >> selectedId) {
                    if (std::find(ids.begin(), ids.end(), selectedId) != ids.end()) {
                        selectedIds.push_back(selectedId);
                    }
                    else {
                        std::cout << "ID " << selectedId << " is not in the found list.\n";
                    }
                }
                ids = selectedIds;
            }

            if (ids.empty()) {
                std::cout << "No pipes selected for editing.\n";
                break;
            }

            // массовое редактирование
            std::cout << "Batch editing " << ids.size() << " pipes.\n";
            std::string newName = inputLine("New name for selected (Enter = no change): ");
            std::string dstr = inputLine("New diameter for selected (Enter = no change): ");
            double newDiameter = -1.0;
            if (!dstr.empty()) {
                try { newDiameter = std::stod(dstr); }
                catch (...) { newDiameter = -1.0; }
            }
            std::cout << "Change 'in repair' flag? (0 = no change, 1 = set true, 2 = set false)\n";
            int rf = inputInt("Choice: ");
            int changeFlag = -1;
            if (rf == 1) changeFlag = 1;
            else if (rf == 2) changeFlag = 0;
            else changeFlag = -1;

            manager.batchEditPipes(ids, newName, newDiameter, changeFlag);
            std::cout << "Batch editing completed for " << ids.size() << " pipes.\n";
            break;
        }
        case 5: {
            std::cout << "Total pipes: " << manager.getPipeCount() << "\n";
            for (const auto& pair : manager.getPipes()) std::cout << pair.second << "\n";
            break;
        }
        case 6: {
            std::string name = inputLine("Compressor Station name: ");
            int total = inputInt("Total workshops: ");
            int working = inputInt("Working workshops: ");
            std::string cls = inputLine("Classification: ");
            uint64_t id = manager.addStation(name, total, working, cls);
            std::cout << "Compressor Station added with ID=" << id << "\n";
            break;
        }
        case 7: {
            uint64_t id = (uint64_t)inputInt("Compressor Station ID to edit: ");
            CompressorStation* s = manager.findStationById(id);
            if (!s) { std::cout << "Not found.\n"; break; }
            std::cout << *s << "\n";
            std::string n = inputLine("New name (Enter = no change): ");
            std::string tot = inputLine("New total (Enter = no change): ");
            std::string work = inputLine("New working (Enter = no change): ");
            std::string cls = inputLine("New classification (Enter = no change): ");
            if (!n.empty()) s->setName(n);
            if (!tot.empty()) { try { s->setTotalWorkshops(std::stoi(tot)); } catch (...) {} }
            if (!work.empty()) { try { s->setWorkingWorkshops(std::stoi(work)); } catch (...) {} }
            if (!cls.empty()) s->setClassification(cls);
            manager.writeLog("Edited station id=" + std::to_string(s->getId()));
            std::cout << "Edited.\n";
            break;
        }
        case 8: {
            uint64_t id = (uint64_t)inputInt("Compressor Station ID to delete: ");
            if (manager.removeStationById(id)) std::cout << "Deleted.\n"; else std::cout << "Not found.\n";
            break;
        }
        case 9: {
            std::cout << "Search Compressor Stations: 1) by name  2) by idle workshops percentage (>=)\n";
            int m = inputInt("Choice: ");
            if (m == 1) {
                std::string q = inputLine("Name substring: ");
                auto res = manager.findStationsByName(q);
                std::cout << "Found " << res.size() << " stations:\n";
                for (auto s : res) std::cout << *s << "\n";
            }
            else if (m == 2) {
                double perc = inputDouble("Min idle percent (e.g., 50.0): ");
                auto res = manager.findStationsByIdlePercent(perc);
                std::cout << "Found " << res.size() << " stations:\n";
                for (auto s : res) std::cout << *s << "\n";
            }
            else std::cout << "Invalid choice.\n";
            break;
        }
        case 10: {
            std::cout << "Total stations: " << manager.getStationCount() << "\n";
            for (const auto& pair : manager.getStations()) std::cout << pair.second << "\n";
            break;
        }
        case 11: {
            std::string fname = inputLine("Enter filename to save: ");
            if (manager.saveToFile(fname)) std::cout << "Saved.\n"; else std::cout << "Error saving.\n";
            break;
        }
        case 12: {
            std::string fname = inputLine("Enter filename to load: ");
            if (manager.loadFromFile(fname)) std::cout << "Loaded.\n"; else std::cout << "Error loading.\n";
            break;
        }
        case 13: {
            addSampleData(manager);
            std::cout << "Sample data added.\n";
            break;
        }
        case 0: {
            running = false;
            std::cout << "Goodbye!\n";
            break;
        }
        default:
            std::cout << "Invalid option.\n";
            break;
        }
    }
    return 0;
}
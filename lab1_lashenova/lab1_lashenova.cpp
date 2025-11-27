#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <set>
#include <sstream>
#include "Manager.h"
#include <chrono>
#include <fstream>

using namespace std::chrono;
using namespace std;

#define INPUT_LINE(in, str) getline(in>>ws, str); \
						cerr << str << endl

class redirect_output_wrapper
{
    ostream& stream;
    streambuf* const old_buf;
public:
    redirect_output_wrapper(ostream& src)
        :old_buf(src.rdbuf()), stream(src)
    {
    }

    ~redirect_output_wrapper() {
        stream.rdbuf(old_buf);
    }
    void redirect(ostream& dest)
    {
        stream.rdbuf(dest.rdbuf());
    }
};

template <typename T>
T GetCorrectNumber(T min, T max)
{
    T x;
    while ((cin >> x).fail()	// check type
        || cin.peek() != '\n'	// is buffer empty (int/float check)
        || x < min || x > max)		// check range
    {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Type number (" << min << "-" << max << "):";
    }
    cerr << x << endl;
    return x;
}

int main() {
    redirect_output_wrapper cerr_out(cerr);

    auto now = chrono::system_clock::now();
    time_t time_now = chrono::system_clock::to_time_t(now);
    string time = to_string(time_now);

    ofstream logfile("log_" + time);
    if (logfile)
        cerr_out.redirect(logfile);
    Manager manager;
    cout << "=== Pipe and Compressor Station Manager ===\n";

    string logf;
    cout << "Enter log filename: ";
    INPUT_LINE(cin, logf);

    bool running = true;
    while (running) {
        cout << "\nMain Menu:\n";
        cout << "1) Add Pipe\n";
        cout << "2) Edit Pipe\n";
        cout << "3) Delete Pipe\n";
        cout << "4) Search and Batch Edit Pipes\n";
        cout << "5) List All Pipes\n";
        cout << "6) Add Compressor Station\n";
        cout << "7) Edit Compressor Station\n";
        cout << "8) Delete Compressor Station\n";
        cout << "9) Search Compressor Stations\n";
        cout << "10) List All Compressor Stations\n";
        cout << "11) Save to File\n";
        cout << "12) Load from File\n";
        cout << "0) Exit\n";

        cout << "Choose an option: ";

        switch (GetCorrectNumber(0, 12)) {
        case 1: { // Добавление новой трубы
            cout << "Pipe name: ";
            string name;
            INPUT_LINE(cin, name);
            cout << "Diameter (number): ";
            double diam = GetCorrectNumber(1.0, 10000.0);
            cout << "In repair? (1 - yes/0 - no): ";
            bool inrep=GetCorrectNumber(0, 1) == 1;
            int id = manager.addPipe(name, diam, inrep);
            cout << "Pipe added with ID=" << id << "\n";
            break;
        }
        case 2: { // Редактирование существующей 
            cout << "Pipe ID to edit: ";
            int id = GetCorrectNumber(1, 10000);
            Pipe& p = manager.getPipeById(id);
            if (p.getId() == 0) {
                cout << "Pipe with this ID not found\n";
                break;
            }
            cout << "Current data:\n" << p << "\n";

            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "New name: ";
            string newName;
            getline(cin, newName);
            if (!newName.empty()) {
                p.setName(newName);
                cout << "Name changed to: " << newName << endl;
            }

            cout << "New diameter: ";
            int d = GetCorrectNumber(1,10000);
            p.setDiameter(d);
                       
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
            break;
        }
        case 3: { // Удаление трубы
            cout << "Pipe ID to delete: ";
            int id = GetCorrectNumber(1,10000);
            if (manager.removePipeById(id)) cout << "Deleted.\n"; else cout << "Not found.\n";
            break;
        }
        case 4: { // Поиск и пакетное редактирование
            cout << "Search pipes for batch editing:\n";
            cout << "1) by name\n";
            cout << "2) by 'in repair' flag\n";
            cout << "3) enter IDs manually\n";
            cout << "Choose search method: ";
            int m = GetCorrectNumber(1,3);

            set<int> ids;

            if (m == 1) { // Поиск по имени
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
            else if (m == 2) { // Поиск по статусу ремонта
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
            else if (m == 3) { // Ручной ввод id
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
                break;
            }

            if (ids.empty()) {
                cout << "No pipes selected for editing.\n";
                break;
            }

            // Подтверждение выбора для пакетного редактирования
            cout << "You selected " << ids.size() << " pipes for batch editing.\n";
            cout << "Do you want to edit all of them? (1-yes/0-no): ";
            int confirm = GetCorrectNumber(0, 1);

            if (confirm==0) {
                // Пользователь хочет выбрать подмножество
                set<int> selectedIds;
                cout << "Enter the IDs you want to edit (separated by space): ";
                string idLine;
                INPUT_LINE(cin, idLine);
                istringstream idStream(idLine);
                int selectedId;
                while (idStream >> selectedId) {
                    if (ids.count(selectedId)>0) {
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
                break;
            }

            // Массовое редактирование
            cout << "Batch editing " << ids.size() << " pipes.\n";
            cout << "New name for selected: ";
            string newName;
            getline(cin, newName);
            cout << "New diameter for selected: ";
            string dstr;
            INPUT_LINE(cin, dstr);
            double newDiameter = -1.0;
            if (!dstr.empty()) {
                try { newDiameter = stod(dstr); }
                catch (...) { newDiameter = -1.0; }
            }
            cout << "Change 'in repair' flag? (0 = no change, 1 = set true, 2 = set false)\n";
            cout << "Choice: ";
            int rf = GetCorrectNumber(0,2);
            int changeFlag = -1;
            if (rf == 1) changeFlag = 1; // Установить 'в ремонте'
            else if (rf == 2) changeFlag = 0; // Установить 'не в ремонте'
            else changeFlag = -1; // Без изменений 

            vector<int> idVector(ids.begin(), ids.end());
            manager.batchEditPipes(idVector, newName, newDiameter, changeFlag);
            cout << "Batch editing completed for " << ids.size() << " pipes.\n";
            break;
        }
        case 5: { // Показать все трубы
            cout << "Total pipes: " << manager.getPipeCount() << "\n";
            for (const auto& pair : manager.getPipes()) cout << pair.second << "\n";
            break;
        }
        case 6: { // Добавление новой станции
            cout << "Compressor Station name: ";
            string name;
            INPUT_LINE(cin, name);
            cout << "Total workshops: ";
            int total = GetCorrectNumber(1,10000);
            cout << "Working workshops: ";
            int working = GetCorrectNumber(1,10000);
            cout << "Classification: ";
            string cls;
            INPUT_LINE(cin, cls);
            int id = manager.addStation(name, total, working, cls);
            cout << "Compressor Station added with ID=" << id << "\n";
            break;
        }
        case 7: { // Редактирование существующей
            cout << "Compressor Station ID to edit: ";
            int id = GetCorrectNumber(1,10000);
            CompressorStation& s = manager.getStationById(id);
            if (s.getId()==0) { cout << "Not found.\n"; break; }
            cout << s << "\n";

            cout << "New name: ";
            string n;
            INPUT_LINE(cin, n);
            cout << "New total: ";
            string tot;
            INPUT_LINE(cin, tot);
            cout << "New working: ";
            string work;
            INPUT_LINE(cin, work);
            cout << "New classification: ";
            string cls;
            INPUT_LINE(cin, cls);
            if (!n.empty()) s.setName(n);
            if (!tot.empty()) { try { s.setTotalWorkshops(stoi(tot)); } catch (...) {} }
            if (!work.empty()) { try { s.setWorkingWorkshops(stoi(work)); } catch (...) {} }
            if (!cls.empty()) s.setClassification(cls);
            cout << "Edited.\n";
            break;
        }
        case 8: { // Удаление станции
            cout << "Compressor Station ID to delete: ";
            int id = GetCorrectNumber(1,10000);
            if (manager.removeStationById(id)) cout << "Deleted.\n"; else cout << "Not found.\n";
            break;
        }
        case 9: { // Поиск станций
            cout << "Search Compressor Stations: 1) by name  2) by idle workshops percentage (>=)\n";
            cout << "Choice: ";
            int m = GetCorrectNumber(1,2);
            if (m == 1) { // по имени
                cout << "Name substring: ";
                string q;
                INPUT_LINE(cin, q);
                auto res = manager.findStationsByName(q);
                cout << "Found " << res.size() << " stations:\n";
                for (auto s : res) cout << s << "\n";
            }
            else if (m == 2) { // по проценту простоя
                cout << "Min idle percent (e.g., 50.0): ";
                double perc = GetCorrectNumber(0.0,100.0);
                auto res = manager.findStationsByIdlePercent(perc);
                cout << "Found " << res.size() << " stations:\n";
                for (auto s : res) cout << s << "\n";
            }
            else cout << "Invalid choice.\n";
            break;
        }
        case 10: { // Показать все станции
            cout << "Total stations: " << manager.getStationCount() << "\n";
            for (const auto& pair : manager.getStations()) cout << pair.second << "\n";
            break;
        }
        case 11: { // Сохранение в файл
            cout << "Enter filename to save: ";
            string fname;
            INPUT_LINE(cin, fname);
            if (manager.saveToFile(fname)) cout << "Saved.\n"; else cout << "Error saving.\n";
            break;
        }
        case 12: { // Загрузка из файла
            cout << "Enter filename to load: ";
            string fname;
            INPUT_LINE(cin, fname);
            if (manager.loadFromFile(fname)) cout << "Loaded.\n"; else cout << "Error loading.\n";
            break;
        }

        case 0: { // Выход из программы
            running = false;
            cout << "Goodbye!\n";
            break;
        }
        default: // Неверный выбор
            cout << "Invalid option.\n";
            break;
        }
    }
    return 0;
}
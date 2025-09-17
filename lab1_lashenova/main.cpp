#include <iostream>
#include <fstream>        // Для работы с файлами (ifstream, ofstream)
#include <string>         // Для работы со строками string
#include <limits> 

using namespace std;

struct Pipe {
    string name;          // Название трубы (километровая отметка)
    double length;        // Длина в километрах (double для дробных чисел)
    int diameter;         // Диаметр в миллиметрах (целое число)
    bool underRepair;     // Флаг ремонта: true - в ремонте, false - работает

    // Метод чтения данных с консоли
    void readFromConsole() {
        cout << "Enter pipe name: ";      // Вывод приглашения
        cin.ignore();                     // Очистка буфера от предыдущего ввода
        getline(cin, name);               // Чтение всей строки названия

        // Ввод длины с проверкой
        cout << "Enter pipe length (km): ";
        while (!(cin >> length) || length <= 0) {  // Пока ввод некорректен ИЛИ длина ≤ 0
            cout << "Error! Enter positive number: ";
            cin.clear();                          // Сброс флагов ошибок cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Очистка буфера
        }

        // Ввод диаметра с проверкой
        cout << "Enter pipe diameter (mm): ";
        while (!(cin >> diameter) || diameter <= 0) { // Аналогичная проверка
            cout << "Error! Enter positive integer: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        // Ввод статуса ремонта
        cout << "Is pipe under repair? (1 - yes, 0 - no): ";
        int repair;  // Временная переменная
        while (!(cin >> repair) || (repair != 0 && repair != 1)) { // Проверка на 0 или 1
            cout << "Error! Enter 1 or 0: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        underRepair = (repair == 1);  // Преобразование 1/0 в true/false
    }

    // Метод вывода информации на консоль
    void display() const {  // const - метод не меняет объект
        cout << "Pipe: " << name << endl;                    // Вывод названия
        cout << "Length: " << length << " km" << endl;       // Вывод длины
        cout << "Diameter: " << diameter << " mm" << endl;   // Вывод диаметра
        // Тернарный оператор: условие ? значение_если_true : значение_если_false
        cout << "Repair status: " << (underRepair ? "Under repair" : "Operational") << endl;
        cout << "------------------------" << endl;          // Разделитель
    }

    // Метод изменения статуса ремонта
    void toggleRepair() {
        cout << "Current status: " << (underRepair ? "Under repair" : "Operational") << endl;
        cout << "Change status? (1 - yes, 0 - no): ";
        int choice;
        while (!(cin >> choice) || (choice != 0 && choice != 1)) { // Проверка ввода
            cout << "Error! Enter 1 or 0: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        if (choice == 1) {              // Если пользователь выбрал изменить
            underRepair = !underRepair; // Инверсия: true становится false и наоборот
            cout << "Status changed to: " << (underRepair ? "Under repair" : "Operational") << endl;
        }
    }

    // Сохранение данных в файл
    void saveToFile(ofstream& file) const {  // ofstream - выходной файловый поток
        file << "PIPE" << endl;     // Метка для идентификации при загрузке
        file << name << endl;       // Сохранение названия
        file << length << endl;     // Сохранение длины
        file << diameter << endl;   // Сохранение диаметра
        file << underRepair << endl;// Сохранение статуса (1 или 0)
    }

    // Загрузка данных из файла
    void loadFromFile(ifstream& file) {  // ifstream - входной файловый поток
        cin.ignore();               // Очистка буфера консоли
        getline(file, name);        // Чтение названия из файла
        file >> length;             // Чтение длины
        file >> diameter;           // Чтение диаметра
        file >> underRepair;        // Чтение статуса
        file.ignore();              // Пропуск символа новой строки
    }
};

struct OilPumpingStation {
    string name;           // Название станции
    int totalPumps;        // Общее количество насосов
    int workingPumps;      // Количество работающих насосов
    double maxCapacity;    // Максимальная пропускная способность (м³/час)
    int stationClass;      // Класс станции (1-5)

    void readFromConsole() {
        cout << "Enter station name: ";
        cin.ignore();
        getline(cin, name);

        // Ввод общего количества насосов
        cout << "Enter total number of pumps: ";
        while (!(cin >> totalPumps) || totalPumps <= 0) {
            cout << "Error! Enter positive integer: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        // Ввод работающих насосов с проверкой диапазона
        cout << "Enter number of working pumps: ";
        while (!(cin >> workingPumps) || workingPumps < 0 || workingPumps > totalPumps) {
            cout << "Error! Enter number from 0 to " << totalPumps << ": ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        // Ввод пропускной способности
        cout << "Enter maximum capacity (m³/hour): ";
        while (!(cin >> maxCapacity) || maxCapacity <= 0) {
            cout << "Error! Enter positive number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        // Ввод класса станции (1-5)
        cout << "Enter station class (1-5): ";
        while (!(cin >> stationClass) || stationClass < 1 || stationClass > 5) {
            cout << "Error! Enter number from 1 to 5: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    void display() const {
        cout << "Oil Pumping Station: " << name << endl;
        cout << "Total pumps: " << totalPumps << endl;
        cout << "Working pumps: " << workingPumps << endl;
        cout << "Maximum capacity: " << maxCapacity << " m³/hour" << endl;
        cout << "Station class: " << stationClass << endl;
        cout << "Current efficiency: " << calculateEfficiency() << "%" << endl;
        cout << "------------------------" << endl;
    }

    // Расчет эффективности работы станции
    double calculateEfficiency() const {
        if (totalPumps == 0) return 0.0;  // Защита от деления на ноль
        // static_cast<double> - преобразование int в double для точного деления
        return (static_cast<double>(workingPumps) / totalPumps) * 100.0;
    }

    // Управление насосами (запуск/остановка)
    void managePump() {
        cout << "Current state: " << workingPumps << " of " << totalPumps << " pumps working" << endl;
        cout << "Efficiency: " << calculateEfficiency() << "%" << endl;
        cout << "1 - Start pump\n2 - Stop pump\n0 - Cancel\nChoose action: ";

        int choice;
        while (!(cin >> choice) || choice < 0 || choice > 2) { // Проверка 0,1,2
            cout << "Error! Enter 0, 1 or 2: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        if (choice == 1 && workingPumps < totalPumps) {  // Запуск насоса
            workingPumps++;
            cout << "Pump started. Now " << workingPumps << " pumps working" << endl;
            cout << "New efficiency: " << calculateEfficiency() << "%" << endl;
        }
        else if (choice == 2 && workingPumps > 0) {    // Остановка насоса
            workingPumps--;
            cout << "Pump stopped. Now " << workingPumps << " pumps working" << endl;
            cout << "New efficiency: " << calculateEfficiency() << "%" << endl;
        }
        else if (choice != 0) {
            cout << "Action not possible!" << endl;  // Невозможное действие
        }
    }

    void saveToFile(ofstream& file) const {
        file << "STATION" << endl;    // Метка для идентификации
        file << name << endl;
        file << totalPumps << endl;
        file << workingPumps << endl;
        file << maxCapacity << endl;
        file << stationClass << endl;
    }

    void loadFromFile(ifstream& file) {
        cin.ignore();
        getline(file, name);
        file >> totalPumps;
        file >> workingPumps;
        file >> maxCapacity;
        file >> stationClass;
        file.ignore();
    }
};

void displayMenu() {
    cout << "\n=== MENU ===" << endl;     // Заголовок меню
    cout << "1. Add pipe" << endl;        // Пункт 1 - добавить трубу
    cout << "2. Add oil pumping station" << endl; // Пункт 2 - добавить станцию
    cout << "3. View all objects" << endl;// Пункт 3 - просмотр
    cout << "4. Edit pipe" << endl;       // Пункт 4 - редактировать трубу
    cout << "5. Edit station" << endl;    // Пункт 5 - редактировать станцию
    cout << "6. Save" << endl;            // Пункт 6 - сохранить в файл
    cout << "7. Load" << endl;            // Пункт 7 - загрузить из файла
    cout << "0. Exit" << endl;            // Пункт 0 - выход
    cout << "Choose action: ";            // Приглашение к выбору
}

int main() {
    Pipe pipe;                     // Создание объекта трубы
    OilPumpingStation station;    // Создание объекта станции
    bool pipeExists = false;       // Флаг: была ли добавлена труба
    bool stationExists = false;    // Флаг: была ли добавлена станция

    int choice;                   // Переменная для выбора пункта меню
    do {                         // Начало цикла do-while
        displayMenu();           // Вывод меню

        // Проверка корректности ввода номера пункта
        while (!(cin >> choice)) {
            cout << "Error! Enter number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        // Обработка выбора пользователя
        switch (choice) {
        case 1: // Добавить трубу
            pipe.readFromConsole();
            pipeExists = true;        // Установка флага
            cout << "Pipe added successfully!" << endl;
            break;

        case 2: // Добавить станцию
            station.readFromConsole();
            stationExists = true;     // Установка флага
            cout << "Oil pumping station added successfully!" << endl;
            break;

        case 3: // Просмотр всех объектов
            if (pipeExists) {
                pipe.display();       // Вывод трубы если она существует
            }
            else {
                cout << "Pipe not added" << endl;
            }

            if (stationExists) {
                station.display();    // Вывод станции если она существует
            }
            else {
                cout << "Station not added" << endl;
            }
            break;

        case 4: // Редактировать трубу
            if (pipeExists) {
                pipe.toggleRepair();  // Изменение статуса ремонта
            }
            else {
                cout << "Add pipe first!" << endl; // Ошибка если трубы нет
            }
            break;

        case 5: // Редактировать станцию
            if (stationExists) {
                station.managePump(); // Управление насосами
            }
            else {
                cout << "Add station first!" << endl;
            }
            break;

        case 6: // Сохранение в файл
        {
            ofstream file("data.txt"); // Создание файла для записи
            if (file.is_open()) {      // Проверка открытия файла
                if (pipeExists) {
                    pipe.saveToFile(file); // Сохранение трубы
                }
                if (stationExists) {
                    station.saveToFile(file); // Сохранение станции
                }
                file.close();          // Закрытие файла
                cout << "Data saved to data.txt" << endl;
            }
            else {
                cout << "Error opening file for writing!" << endl;
            }
        }
        break;

        case 7: // Загрузка из файла
        {
            ifstream file("data.txt"); // Открытие файла для чтения
            if (file.is_open()) {
                string type;           // Для хранения метки "PIPE" или "STATION"
                while (file >> type) { // Чтение до конца файла
                    if (type == "PIPE") {
                        pipe.loadFromFile(file);
                        pipeExists = true;    // Установка флага
                    }
                    else if (type == "STATION") {
                        station.loadFromFile(file);
                        stationExists = true; // Установка флага
                    }
                }
                file.close();
                cout << "Data loaded from data.txt" << endl;
            }
            else {
                cout << "Error opening file for reading!" << endl;
            }
        }
        break;

        case 0: // Выход из программы
            cout << "Exiting program..." << endl;
            break;

        default: // Неверный выбор
            cout << "Invalid choice! Try again." << endl;
        }

    } while (choice != 0);  // Цикл продолжается пока choice не равен 0

    return 0;  // Завершение программы
}
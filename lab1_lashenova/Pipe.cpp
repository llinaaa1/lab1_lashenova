#include "Pipe.h"
#include <stdexcept>
#include <vector>
#include <iostream>
#include <limits>

// конструктор по умолчанию - инициализирует все поля нулевыми/пустыми значениями
Pipe::Pipe() : id(0), name(""), diameter(0.0), in_repair(false) {}
// параметризированный конструктор - инициализирует поля переданными значениями
Pipe::Pipe(int id_, const std::string& name_, double diameter_, bool in_repair_)
    : id(id_), name(name_), diameter(diameter_), in_repair(in_repair_) {
}

// Методы чтения полей
int Pipe::getId() const { return id; }
std::string Pipe::getName() const { return name; }
double Pipe::getDiameter() const { return diameter; }
bool Pipe::isInRepair() const { return in_repair; }

// Методы изменения
void Pipe::setName(const std::string& n) { name = n; }
void Pipe::setDiameter(double d) { diameter = d; }
void Pipe::setInRepair(bool r) { in_repair = r; }

//Преобразует объект в строку для сохранения в файл
std::string Pipe::serialize() const {
    std::ostringstream os;
    // id|name|diameter|in_repair
    os << id << '|' << name << '|' << diameter << '|' << (in_repair ? 1 : 0);
    return os.str();
}

// Создает объект из строки
Pipe Pipe::deserialize(const std::string& line) {
    // split by '|'
    std::vector<std::string> parts;
    std::string cur;
    for (char c : line) {
        if (c == '|') { parts.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    parts.push_back(cur);
    if (parts.size() != 4) throw std::runtime_error("Pipe::deserialize: wrong format");
    int id = std::stoull(parts[0]);
    std::string name = parts[1];
    double diameter = std::stod(parts[2]);
    bool in_repair = (parts[3] != "0");
    return Pipe(id, name, diameter, in_repair);
}

// Оператор вывода
std::ostream& operator<<(std::ostream& os, const Pipe& p) {
    os << "ID=" << p.id
        << " | Name=\"" << p.name << "\""
        << " | Diameter=" << p.diameter
        << " | InRepair=" << (p.in_repair ? "YES" : "NO");
    return os;
}

// Оператор ввода
std::istream& operator>>(std::istream& is, Pipe& p) {
    std::cout << "Enter pipe name: ";
    std::getline(is, p.name);
    std::cout << "Enter diameter: ";
    is >> p.diameter;
    std::cout << "Is in repair? (1 for yes, 0 for no): ";
    int repair;
    is >> repair;
    p.in_repair = (repair != 0);
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return is;
}
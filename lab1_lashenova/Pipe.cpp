#include "Pipe.h"
#include <iostream>
#include <limits>


Pipe::Pipe() : id(0), name(""), diameter(0.0), in_repair(false) {}

Pipe::Pipe(int id_, const std::string& name_,int diameter_, bool in_repair_)
    : id(id_), name(name_), diameter(diameter_), in_repair(in_repair_) {
}

int Pipe::getId() const { return id; }
std::string Pipe::getName() const { return name; }
double Pipe::getDiameter() const { return diameter; }
bool Pipe::isInRepair() const { return in_repair; }
void Pipe::setInRepair(bool r) { in_repair = r; }


std::ostream& operator<<(std::ostream& os, const Pipe& p) {
    os << "ID=" << p.id
        << " | Name=\"" << p.name << "\""
        << " | Diameter=" << p.diameter
        << " | InRepair=" << (p.in_repair ? "YES" : "NO");
    return os;
}

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
#include "CompressorStation.h"
#include <iostream>
#include <limits>

CompressorStation::CompressorStation() 
    : id(0), name(""), total_workshops(0), working_workshops(0), classification("") {
}

CompressorStation::CompressorStation(int id_, const std::string& name_, int total_, int working_, const std::string& classification_)
    : id(id_), name(name_), total_workshops(total_), working_workshops(working_), classification(classification_) {
}

int CompressorStation::getId() const { return id; }
std::string CompressorStation::getName() const { return name; }
int CompressorStation::getTotalWorkshops() const { return total_workshops; }
int CompressorStation::getWorkingWorkshops() const { return working_workshops; }
std::string CompressorStation::getClassification() const { return classification; }

void CompressorStation::setName(const std::string& n) { name = n; }
void CompressorStation::setTotalWorkshops(int t) { total_workshops = t; }
void CompressorStation::setWorkingWorkshops(int w) { working_workshops = w; }

double CompressorStation::percentIdle() const { // Вычисление процента простоя
    if (total_workshops <= 0) return 0.0;
    int idle = total_workshops - working_workshops;
    return (100.0 * idle) / total_workshops;
}

std::ostream& operator<<(std::ostream& os, const CompressorStation& cs) {
    os << "ID=" << cs.id
        << " | Name=\"" << cs.name << "\""
        << " | Total=" << cs.total_workshops
        << " | Working=" << cs.working_workshops
        << " | Idle%=" << cs.percentIdle()  
        << " | Class=\"" << cs.classification << "\"";
    return os;
}

std::istream& operator>>(std::istream& is, CompressorStation& cs) {
    std::cout << "Enter station name: ";
    std::getline(is, cs.name);
    std::cout << "Enter total workshops: ";
    is >> cs.total_workshops;
    std::cout << "Enter working workshops: ";
    is >> cs.working_workshops;

    //Очистка буфера ввода после ввода чисел
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Enter classification: ";
    std::getline(is, cs.classification);
    return is;
}
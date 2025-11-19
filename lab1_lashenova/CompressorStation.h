#ifndef COMPRESSORSTATION_H
#define COMPRESSORSTATION_H

#include <string>
#include <cstdint>
#include <iostream>

class CompressorStation {
private:
    int id;
    std::string name;
    int total_workshops;
    int working_workshops;
    std::string classification;

public:
    // Конструкторы
    CompressorStation();
    CompressorStation(int id_, const std::string& name_, int total_, int working_, const std::string& classification_);

   // Методы чтения полей
    int getId() const;
    std::string getName() const;
    int getTotalWorkshops() const;
    int getWorkingWorkshops() const;
    std::string getClassification() const;

    // Методы изменения
    void setName(const std::string& n);
    void setTotalWorkshops(int t);
    void setWorkingWorkshops(int w);
    void setClassification(const std::string& c);

    
    double percentIdle() const;  //Процент простоя станции

    std::string serialize() const;
    static CompressorStation deserialize(const std::string& line);

    // friend operators for input/output
    friend std::ostream& operator<<(std::ostream& os, const CompressorStation& cs);
    friend std::istream& operator>>(std::istream& is, CompressorStation& cs);
};

#endif // COMPRESSORSTATION_H

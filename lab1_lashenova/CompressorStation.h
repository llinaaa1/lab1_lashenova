#ifndef COMPRESSORSTATION_H
#define COMPRESSORSTATION_H

#include <string>
#include <iostream>

class CompressorStation {
private:
    int id;
    std::string name;
    int total_workshops;
    int working_workshops;
    std::string classification;

public:
    CompressorStation();
    CompressorStation(int id_, const std::string& name_, int total_, int working_, const std::string& classification_);

    int getId() const;
    std::string getName() const;
    int getTotalWorkshops() const;
    int getWorkingWorkshops() const;
    std::string getClassification() const;

    void setTotalWorkshops(int t);
    void setWorkingWorkshops(int w);

    
    double percentIdle() const;

    friend std::ostream& operator<<(std::ostream& os, const CompressorStation& cs);
    friend std::istream& operator>>(std::istream& is, CompressorStation& cs);
};

#endif // COMPRESSORSTATION_H

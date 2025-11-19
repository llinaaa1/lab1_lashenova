#ifndef PIPE_H
#define PIPE_H

#include <string>
#include <cstdint>
#include <sstream>
#include <iostream>

class Pipe {
private:
    int id;
    std::string name;
    double diameter; 
    bool in_repair;

public:
    Pipe();
    Pipe(int id_, const std::string& name_, double diameter_, bool in_repair_);

    
    int getId() const;
    std::string getName() const;
    double getDiameter() const;
    bool isInRepair() const;

    void setName(const std::string& n);
    void setDiameter(double d);
    void setInRepair(bool r);

    
    std::string serialize() const;
    static Pipe deserialize(const std::string& line);

    
    friend std::ostream& operator<<(std::ostream& os, const Pipe& p);
    friend std::istream& operator>>(std::istream& is, Pipe& p);
};

#endif // PIPE_H

#ifndef PIPE_H
#define PIPE_H

#include <string>
#include <iostream>

class Pipe {
private:
    int id;
    std::string name;
    int diameter; 
    bool in_repair;

public:
    Pipe();
    Pipe(int id_, const std::string& name_, int diameter_, bool in_repair_);

    
    int getId() const;
    std::string getName() const;
    double getDiameter() const;
    bool isInRepair() const;

    void setName(const std::string& n);
    void setDiameter(int d);
    void setInRepair(bool r);

    
    friend std::ostream& operator<<(std::ostream& os, const Pipe& p);
    friend std::istream& operator>>(std::istream& is, Pipe& p);

};

#endif // PIPE_H

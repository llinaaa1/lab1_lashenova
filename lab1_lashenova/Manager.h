#ifndef MANAGER_H
#define MANAGER_H

#include "Pipe.h"
#include "CompressorStation.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

class Manager {
private:
    std::unordered_map<int, Pipe> pipes; 
    std::unordered_map<int, CompressorStation> stations; 
    
    int next_pipe_id;
    int next_station_id;
    std::string log_filename;


    template<typename T, typename Predicate>
    std::vector<T> findEntities(const std::unordered_map<int, T>& container, Predicate pred);

public:
    Manager();
    Manager(const std::string& logFile);

    int makePipeId();
    int makeStationId();

    int addPipe(const std::string& name, double diameter, bool in_repair);
    int addPipe(const Pipe& pipe);
    bool removePipeById(int id);
    Pipe findPipeById(int id);
    std::vector<Pipe> findPipesByName(const std::string& substring);
    std::vector<Pipe> findPipesByRepairFlag(bool in_repair);
    const std::unordered_map<int, Pipe>& getPipes() const;
    size_t getPipeCount() const;

    int addStation(const std::string& name, int total, int working, const std::string& classification);
    int addStation(const CompressorStation& station);
    bool removeStationById(int id);
    CompressorStation findStationById(int id);
    std::vector<CompressorStation> findStationsByName(const std::string& substring);
    std::vector<CompressorStation> findStationsByIdlePercent(double minIdlePercent);
    const std::unordered_map<int, CompressorStation>& getStations() const;
    size_t getStationCount() const;

    bool saveToFile(const std::string& filename);
    bool loadFromFile(const std::string& filename);

    // Массовое редактирование труб
    void batchEditPipes(const std::vector<int>& ids, const std::string& newName, double newDiameter, int changeRepairFlag);
};

#endif // MANAGER_H

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
    std::unordered_map<uint64_t, Pipe> pipes;
    std::unordered_map<uint64_t, CompressorStation> stations;
    uint64_t next_pipe_id;
    uint64_t next_station_id;
    std::string log_filename;

    void logAction(const std::string& msg);

    template<typename T, typename Predicate>
    std::vector<T*> findEntities(const std::unordered_map<uint64_t, T>& container, Predicate pred);

public:
    Manager();
    Manager(const std::string& logFile);

    void setLogFilename(const std::string& filename);
    void writeLog(const std::string& msg);

    uint64_t makePipeId();
    uint64_t makeStationId();

    // Pipe methods
    uint64_t addPipe(const std::string& name, double diameter, bool in_repair);
    uint64_t addPipe(const Pipe& pipe);
    bool removePipeById(uint64_t id);
    Pipe* findPipeById(uint64_t id);
    std::vector<Pipe*> findPipesByName(const std::string& substring);
    std::vector<Pipe*> findPipesByRepairFlag(bool in_repair);
    const std::unordered_map<uint64_t, Pipe>& getPipes() const;
    size_t getPipeCount() const;

    // Station methods
    uint64_t addStation(const std::string& name, int total, int working, const std::string& classification);
    uint64_t addStation(const CompressorStation& station);
    bool removeStationById(uint64_t id);
    CompressorStation* findStationById(uint64_t id);
    std::vector<CompressorStation*> findStationsByName(const std::string& substring);
    std::vector<CompressorStation*> findStationsByIdlePercent(double minIdlePercent);
    const std::unordered_map<uint64_t, CompressorStation>& getStations() const;
    size_t getStationCount() const;

    // File operations
    bool saveToFile(const std::string& filename);
    bool loadFromFile(const std::string& filename);

    // Batch editing
    void batchEditPipes(const std::vector<uint64_t>& ids, const std::string& newName, double newDiameter, int changeRepairFlag);
};

#endif // MANAGER_H

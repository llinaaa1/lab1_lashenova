#include "Manager.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>

Manager::Manager() : next_pipe_id(1), next_station_id(1), log_filename("actions.log") {}
Manager::Manager(const std::string& logFile) : next_pipe_id(1), next_station_id(1), log_filename(logFile) {}

void Manager::setLogFilename(const std::string& filename) {
    log_filename = filename;
    logAction("Log filename changed to: " + filename);
}

void Manager::writeLog(const std::string& msg) {
    logAction(msg);
}

uint64_t Manager::makePipeId() {
    return next_pipe_id++;
}

uint64_t Manager::makeStationId() {
    return next_station_id++;
}

void Manager::logAction(const std::string& msg) {
    std::ofstream os(log_filename, std::ios::app);
    if (!os) return;
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif
    os << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " | " << msg << "\n";
    os.close();
}

template<typename T, typename Predicate>
std::vector<T*> Manager::findEntities(const std::unordered_map<uint64_t, T>& container, Predicate pred) {
    std::vector<T*> res;
    for (const auto& pair : container) {
        if (pred(pair.second)) {
            res.push_back(const_cast<T*>(&pair.second));
        }
    }
    return res;
}

// явна€ инстанциаци€ шаблонов
template std::vector<Pipe*> Manager::findEntities<Pipe, std::function<bool(const Pipe&)>>(const std::unordered_map<uint64_t, Pipe>&, std::function<bool(const Pipe&)>);
template std::vector<CompressorStation*> Manager::findEntities<CompressorStation, std::function<bool(const CompressorStation&)>>(const std::unordered_map<uint64_t, CompressorStation>&, std::function<bool(const CompressorStation&)>);

// Pipe methods
uint64_t Manager::addPipe(const std::string& name, double diameter, bool in_repair) {
    uint64_t id = makePipeId();
    pipes.emplace(id, Pipe(id, name, diameter, in_repair));
    logAction("Added pipe id=" + std::to_string(id) + " name=\"" + name + "\" diameter=" + std::to_string(diameter) + " in_repair=" + (in_repair ? "1" : "0"));
    return id;
}

uint64_t Manager::addPipe(const Pipe& pipe) {
    uint64_t id = makePipeId();
    pipes.emplace(id, Pipe(id, pipe.getName(), pipe.getDiameter(), pipe.isInRepair()));
    logAction("Added pipe id=" + std::to_string(id) + " name=\"" + pipe.getName() + "\"");
    return id;
}

bool Manager::removePipeById(uint64_t id) {
    auto it = pipes.find(id);
    if (it == pipes.end()) return false;
    logAction("Removed pipe id=" + std::to_string(it->first) + " name=\"" + it->second.getName() + "\"");
    pipes.erase(it);
    return true;
}

Pipe* Manager::findPipeById(uint64_t id) {
    auto it = pipes.find(id);
    return (it != pipes.end()) ? &it->second : nullptr;
}

std::vector<Pipe*> Manager::findPipesByName(const std::string& substring) {
    auto res = findEntities<Pipe>(pipes, [&substring](const Pipe& p) {
        return p.getName().find(substring) != std::string::npos;
        });
    logAction("Searched pipes by name=\"" + substring + "\" -> " + std::to_string(res.size()) + " found");
    return res;
}

std::vector<Pipe*> Manager::findPipesByRepairFlag(bool in_repair) {
    auto res = findEntities<Pipe>(pipes, [in_repair](const Pipe& p) {
        return p.isInRepair() == in_repair;
        });
    logAction("Searched pipes by in_repair=" + std::string(in_repair ? "1" : "0") + " -> " + std::to_string(res.size()) + " found");
    return res;
}

const std::unordered_map<uint64_t, Pipe>& Manager::getPipes() const { return pipes; }
size_t Manager::getPipeCount() const { return pipes.size(); }

// Station methods
uint64_t Manager::addStation(const std::string& name, int total, int working, const std::string& classification) {
    uint64_t id = makeStationId();
    stations.emplace(id, CompressorStation(id, name, total, working, classification));
    logAction("Added station id=" + std::to_string(id) + " name=\"" + name + "\" total=" + std::to_string(total) + " working=" + std::to_string(working));
    return id;
}

uint64_t Manager::addStation(const CompressorStation& station) {
    uint64_t id = makeStationId();
    stations.emplace(id, CompressorStation(id, station.getName(), station.getTotalWorkshops(), station.getWorkingWorkshops(), station.getClassification()));
    logAction("Added station id=" + std::to_string(id) + " name=\"" + station.getName() + "\"");
    return id;
}

bool Manager::removeStationById(uint64_t id) {
    auto it = stations.find(id);
    if (it == stations.end()) return false;
    logAction("Removed station id=" + std::to_string(it->first) + " name=\"" + it->second.getName() + "\"");
    stations.erase(it);
    return true;
}

CompressorStation* Manager::findStationById(uint64_t id) {
    auto it = stations.find(id);
    return (it != stations.end()) ? &it->second : nullptr;
}

std::vector<CompressorStation*> Manager::findStationsByName(const std::string& substring) {
    auto res = findEntities<CompressorStation>(stations, [&substring](const CompressorStation& s) {
        return s.getName().find(substring) != std::string::npos;
        });
    logAction("Searched stations by name=\"" + substring + "\" -> " + std::to_string(res.size()) + " found");
    return res;
}

std::vector<CompressorStation*> Manager::findStationsByIdlePercent(double minIdlePercent) {
    auto res = findEntities<CompressorStation>(stations, [minIdlePercent](const CompressorStation& s) {
        return s.percentIdle() >= minIdlePercent;
        });
    std::ostringstream oss;
    oss << "Searched stations by minIdlePercent=" << minIdlePercent << " -> " << res.size() << " found";
    logAction(oss.str());
    return res;
}

const std::unordered_map<uint64_t, CompressorStation>& Manager::getStations() const { return stations; }
size_t Manager::getStationCount() const { return stations.size(); }

// File operations
bool Manager::saveToFile(const std::string& filename) {
    std::ofstream os(filename);
    if (!os) {
        logAction("Failed to save to file: " + filename);
        return false;
    }
    os << "NEXT_PIPE_ID|" << next_pipe_id << "\n";
    os << "NEXT_STATION_ID|" << next_station_id << "\n";
    os << "#PIPES\n";
    for (const auto& p : pipes) os << p.second.serialize() << "\n";
    os << "#STATIONS\n";
    for (const auto& s : stations) os << s.second.serialize() << "\n";
    os.close();
    logAction("Saved to file: " + filename + " pipes=" + std::to_string(pipes.size()) + " stations=" + std::to_string(stations.size()));
    return true;
}

bool Manager::loadFromFile(const std::string& filename) {
    std::ifstream is(filename);
    if (!is) {
        logAction("Failed to load from file: " + filename);
        return false;
    }
    pipes.clear();
    stations.clear();
    std::string line;
    enum Section { NONE, PIPES, STATIONS } section = NONE;
    uint64_t loaded_next_pipe_id = 1;
    uint64_t loaded_next_station_id = 1;

    while (std::getline(is, line)) {
        if (line.empty()) continue;
        if (line.rfind("NEXT_PIPE_ID|", 0) == 0) {
            try { loaded_next_pipe_id = std::stoull(line.substr(13)); }
            catch (...) { loaded_next_pipe_id = 1; }
            continue;
        }
        if (line.rfind("NEXT_STATION_ID|", 0) == 0) {
            try { loaded_next_station_id = std::stoull(line.substr(16)); }
            catch (...) { loaded_next_station_id = 1; }
            continue;
        }
        if (line == "#PIPES") { section = PIPES; continue; }
        if (line == "#STATIONS") { section = STATIONS; continue; }

        try {
            if (section == PIPES) {
                Pipe p = Pipe::deserialize(line);
                pipes.emplace(p.getId(), p);
            }
            else if (section == STATIONS) {
                CompressorStation s = CompressorStation::deserialize(line);
                stations.emplace(s.getId(), s);
            }
        }
        catch (const std::exception& e) {
            logAction(std::string("Warning: failed to parse line during load: ") + e.what() + " line=[" + line + "]");
        }
    }
    is.close();

    //  орректировка next_id
    uint64_t max_pipe_id = 0;
    uint64_t max_station_id = 0;
    for (const auto& p : pipes) if (p.first > max_pipe_id) max_pipe_id = p.first;
    for (const auto& s : stations) if (s.first > max_station_id) max_station_id = s.first;
    next_pipe_id = std::max(loaded_next_pipe_id, max_pipe_id + 1);
    next_station_id = std::max(loaded_next_station_id, max_station_id + 1);

    logAction("Loaded from file: " + filename + " pipes=" + std::to_string(pipes.size()) + " stations=" + std::to_string(stations.size()));
    return true;
}

void Manager::batchEditPipes(const std::vector<uint64_t>& ids, const std::string& newName, double newDiameter, int changeRepairFlag) {
    std::ostringstream oss;
    oss << "Batch edit pipes count=" << ids.size() << " newName=\"" << newName << "\" newDiameter=" << newDiameter << " changeRepair=" << changeRepairFlag;
    logAction(oss.str());

    for (uint64_t id : ids) {
        Pipe* p = findPipeById(id);
        if (!p) {
            logAction("Batch edit: cannot find pipe id=" + std::to_string(id));
            continue;
        }
        if (!newName.empty()) p->setName(newName);
        if (newDiameter > 0.0) p->setDiameter(newDiameter);
        if (changeRepairFlag == 0) p->setInRepair(false);
        if (changeRepairFlag == 1) p->setInRepair(true);
        logAction("Batch edited pipe id=" + std::to_string(id));
    }
}
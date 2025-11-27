#include "Manager.h"
#include <fstream>
#include <iostream>
#include <sstream>

Manager::Manager() : next_pipe_id(1), next_station_id(1), log_filename("actions.log") {}

Manager::Manager(const std::string& logFile) : next_pipe_id(1), next_station_id(1), log_filename(logFile) {}

int Manager::makePipeId() {
    return next_pipe_id++;
}

int Manager::makeStationId() {
    return next_station_id++;
}

template<typename T, typename Predicate>
std::vector<T> Manager::findEntities(const std::unordered_map<int, T>& container, Predicate pred) {
    std::vector<T> res;
    for (const auto& pair : container) {
        if (pred(pair.second)) {
            res.push_back(pair.second);
        }
    }
    return res;
}
template std::vector<Pipe> Manager::findEntities<Pipe, std::function<bool(const Pipe&)>>(const std::unordered_map<int, Pipe>&, std::function<bool(const Pipe&)>);
template std::vector<CompressorStation> Manager::findEntities<CompressorStation, std::function<bool(const CompressorStation&)>>(const std::unordered_map<int, CompressorStation>&, std::function<bool(const CompressorStation&)>);

// Методы для труб
int Manager::addPipe(const std::string& name, double diameter, bool in_repair) {
    int id = makePipeId();
    pipes.emplace(id, Pipe(id, name, diameter, in_repair));
    return id;
}

int Manager::addPipe(const Pipe& pipe) {
    int id = makePipeId();
    pipes.emplace(id, Pipe(id, pipe.getName(), pipe.getDiameter(), pipe.isInRepair()));
    return id;
}

bool Manager::removePipeById(int id) {
    auto it = pipes.find(id);
    if (it == pipes.end()) return false;
    pipes.erase(it);
    return true;
}
Pipe Manager::findPipeById(int id) {
    auto it = pipes.find(id);
    return (it != pipes.end()) ? it->second : Pipe();
}

Pipe& Manager::getPipeById(int id) {
    auto it = pipes.find(id);
    if (it != pipes.end()) {
        return it->second;
    }
    static Pipe emptyPipe;
    return emptyPipe;
}

std::vector<Pipe> Manager::findPipesByName(const std::string& substring) {
    auto res = findEntities<Pipe>(pipes, [&substring](const Pipe& p) {
        return p.getName().find(substring) != std::string::npos;
        });
    return res;
}

std::vector<Pipe> Manager::findPipesByRepairFlag(bool in_repair) {
    auto res = findEntities<Pipe>(pipes, [in_repair](const Pipe& p) {
        return p.isInRepair() == in_repair;
        });
    return res;
}

const std::unordered_map<int, Pipe>& Manager::getPipes() const { return pipes; }
size_t Manager::getPipeCount() const { return pipes.size(); }

// Методы для станций
int Manager::addStation(const std::string& name, int total, int working, const std::string& classification) {
    if (working > total) {
        std::cout << "Error: Working workshops (" << working
            << ") cannot be more than total workshops (" << total << ")\n";
        return -1;
    }
    int id = makeStationId();
    stations.emplace(id, CompressorStation(id, name, total, working, classification));
    return id;
}

int Manager::addStation(const CompressorStation& station) {
    int id = makeStationId();
    stations.emplace(id, CompressorStation(id, station.getName(), station.getTotalWorkshops(), station.getWorkingWorkshops(), station.getClassification()));
    return id;
}

bool Manager::removeStationById(int id) {
    auto it = stations.find(id);
    if (it == stations.end()) return false;
    stations.erase(it);
    return true;
}

CompressorStation Manager::findStationById(int id) {
    auto it = stations.find(id);
    return (it != stations.end()) ? it->second : CompressorStation();
}
CompressorStation& Manager::getStationById(int id) {
    auto it = stations.find(id);
    if (it != stations.end()) {
        return it->second;
    }
    static CompressorStation emptyStation;
    return emptyStation;
}

std::vector<CompressorStation> Manager::findStationsByName(const std::string& substring) {
    auto res = findEntities<CompressorStation>(stations, [&substring](const CompressorStation& s) {
        return s.getName().find(substring) != std::string::npos;
        });
    return res;
}

std::vector<CompressorStation> Manager::findStationsByIdlePercent(double minIdlePercent) {
    auto res = findEntities<CompressorStation>(stations, [minIdlePercent](const CompressorStation& s) {
        return s.percentIdle() >= minIdlePercent;
        });
    std::ostringstream oss;
    oss << "Searched stations by minIdlePercent=" << minIdlePercent << " -> " << res.size() << " found";
    return res;
}

const std::unordered_map<int, CompressorStation>& Manager::getStations() const { return stations; }
size_t Manager::getStationCount() const { return stations.size(); }

// Операции с файлами
bool Manager::saveToFile(const std::string& filename) {
    std::ofstream os(filename);
    if (!os) return false;
    
    for (const auto& p : pipes) {
        os << p.second.getId() << "|"
            << p.second.getName() << "|"
            << p.second.getDiameter() << "|"
            << p.second.isInRepair() << "\n";
    }
    for (const auto& s : stations) {
        os << s.second.getId() << "|"
            << s.second.getName() << "|"
            << s.second.getTotalWorkshops() << "|"
            << s.second.getWorkingWorkshops() << "|"
            << s.second.getClassification() << "\n";
    }
    os.close();
    return true;
}

bool Manager::loadFromFile(const std::string& filename) {
    std::ifstream is(filename);
    if (!is) return false;

    pipes.clear();
    stations.clear();
    std::string line;

    while (std::getline(is, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string type;
        std::getline(iss, type, '|');

        if (type == "PIPE") {
            std::string id_str, name, diam_str, repair_str;
            std::getline(iss, id_str, '|');
            std::getline(iss, name, '|');
            std::getline(iss, diam_str, '|');
            std::getline(iss, repair_str, '|');

            int id = std::stoi(id_str);
            double diameter = std::stod(diam_str);
            bool in_repair = (repair_str == "1");

            pipes.emplace(id, Pipe(id, name, diameter, in_repair));
            if (id >= next_pipe_id) next_pipe_id = id + 1;
        }
        else if (type == "STATION") {
            std::string id_str, name, total_str, working_str, classification;
            std::getline(iss, id_str, '|');
            std::getline(iss, name, '|');
            std::getline(iss, total_str, '|');
            std::getline(iss, working_str, '|');
            std::getline(iss, classification, '|');

            int id = std::stoi(id_str);
            int total = std::stoi(total_str);
            int working = std::stoi(working_str);

            stations.emplace(id, CompressorStation(id, name, total, working, classification));
            if (id >= next_station_id) next_station_id = id + 1;
        }
    }

    is.close();
    return true;
}

// Массовое редактирование труб
void Manager::batchEditPipes(const std::vector<int>& ids, const std::string& newName, double newDiameter, int changeRepairFlag) {
    std::ostringstream oss;
    oss << "Batch edit pipes count=" << ids.size() << " newName=\"" << newName << "\" newDiameter=" << newDiameter << " changeRepair=" << changeRepairFlag;
    std::cout << oss.str() << std::endl;

    // Проходим по всем id труб для редактирования
    for (int id : ids) {
        auto it = pipes.find(id);
        if (it != pipes.end()) {
            Pipe& p = it->second; // Используем ссылку для изменения оригинала

            if (!newName.empty()) p.setName(newName);
            if (newDiameter > 0.0) p.setDiameter(newDiameter);
            if (changeRepairFlag == 0) p.setInRepair(false);
            if (changeRepairFlag == 1) p.setInRepair(true);
        }
    }
}
#ifndef UTILS_H
#define UTILS_H
#include <iostream>
#include <limits>
#include <string>
#include <chrono>
#include <fstream>
#include <set>
#include <unordered_map> 
#include "Pipe.h"
#include "CompressorStation.h"

#define INPUT_LINE(in, str) std::getline(in>>std::ws, str); \
						std::cerr << str << std::endl

class redirect_output_wrapper
{
    std::ostream& stream;
    std::streambuf* const old_buf;
public:
    redirect_output_wrapper(std::ostream& src)
        :old_buf(src.rdbuf()), stream(src)
    {
    }

    ~redirect_output_wrapper() {
        stream.rdbuf(old_buf);
    }
    void redirect(std::ostream& dest)
    {
        stream.rdbuf(dest.rdbuf());
    }
};

template <typename T>
T GetCorrectNumber(T min, T max)
{
    T x;
    while ((std::cin >> x).fail()	// check type
        || std::cin.peek() != '\n'	// is buffer empty (int/float check)
        || x < min || x > max)		// check range
    {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Type number (" << min << "-" << max << "):";
    }
    std::cerr << x << std::endl;
    return x;
}

template <typename Obj, typename Func, typename Param>
std::set<int> find_by_filter(const std::unordered_map<int, Obj>& objs, Func func, Param param) {
    std::set<int> result;
    for (const auto& obj : objs) {
        if (func(obj.second, param)) {
            std::cout << "ID: " << obj.first << std::endl;
            std::cout << obj.second;
            result.insert(obj.first);
        }
    }
    return result;
}
inline bool filter_pipe_by_name(const Pipe& pipe, const std::string& name_substr) {
    return pipe.getName().find(name_substr) != std::string::npos;
}
inline bool filter_pipe_by_repair(const Pipe& pipe, bool repair_status) {
    return pipe.isInRepair() == repair_status;
}

inline bool filter_station_by_name(const CompressorStation& station, const std::string& name_substr) {
    return station.getName().find(name_substr) != std::string::npos;
}
inline bool filter_station_by_idle_percent(const CompressorStation& station, double min_idle_percent) {
    return station.percentIdle() >= min_idle_percent;
}
#endif // UTILS_H


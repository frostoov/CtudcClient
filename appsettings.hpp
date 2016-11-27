#pragma once

#include "ui/qchambertable.hpp"

#include <json.hpp>

#include <string>
#include <cstdint>

struct AppSettings {
    std::string  addr;
    uint16_t port;
    std::string multicastAddr;
    uint16_t  multicastPort;
    QChamberTable::Config freqLevels;

    void load(const std::string& fileName);
    void save(const std::string& fileName);

    nlohmann::json marshal() const;
    void unMarshal(const nlohmann::json& doc);
};

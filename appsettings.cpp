#include "appsettings.hpp"

#include "configparser/appconfigparser.hpp"

using std::string;

void AppSettings::load(const std::string& fileName) {
    AppConfigParser parser;
    parser.load(fileName);

    unMarshal(parser.config());
}

void AppSettings::save(const std::string& fileName) {
    AppConfigParser parser(marshal());
    parser.save(fileName);
}

nlohmann::json AppSettings::marshal() const {
    return {
        {"address", addr },
        {"port", port },
        {"multicast_address", multicastAddr },
        {"multicast_port", multicastPort },
    };
}

void AppSettings::unMarshal(const nlohmann::json& doc) {
    addr = doc.at("address").get<string>();
    port = doc.at("port");
    multicastAddr = doc.at("multicast_address");
    multicastPort = doc.at("multicast_port");
}

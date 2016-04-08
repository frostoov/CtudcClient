#include "ui/mainwindow.hpp"

#include "configparser/appconfigparser.hpp"
#include <QApplication>
#include <memory>
#include <chrono>

using nlohmann::json;
using std::string;
using std::getenv;

auto readAppConfig() {
#ifdef _WIN32
    auto filename = "CtudcClient.conf";
#elif __unix__
    auto filename = getenv("HOME") + string("/.config/ctudc/CtudcClient.conf");
#endif
    AppConfigParser p;
    p.load(filename);
    return p.config();
}

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);

    QApplication a(argc, argv);
    console = new QConsole("CtudcClient");
    a.setStyle(QStyleFactory::create("Fusion"));

    auto conn = std::make_shared<CtudcConn>();
    json conf;
    try {
        conf = readAppConfig();
    } catch(std::exception& e) {
        QMessageBox mb(QMessageBox::Critical, "Failed read config", e.what());
        mb.show();
        return a.exec();
    }
    try {
        conn->connect(conf.at("address").get<std::string>(),
                      conf.at("port").get<uint16_t>());
    } catch(std::exception& e) {
        QMessageBox mb(QMessageBox::Critical, "Failed connect to host", e.what());
        mb.show();
        return a.exec();
    };

    MainWindow w(conn);
    w.show();
    return a.exec();
}

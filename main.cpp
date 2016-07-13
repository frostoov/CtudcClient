#include "ui/mainwindow.hpp"
#include "net/ctudcconn.hpp"
#include "appsettings.hpp"

#include <trek/net/multicastreceiver.hpp>

#include <QApplication>
#include <memory>
#include <chrono>

using trek::net::MulticastReceiver;

using nlohmann::json;
using std::string;
using std::getenv;

auto appConfigPath() {
#ifdef _WIN32
    auto filename = "CtudcClient.conf";
#elif __unix__
    auto filename = getenv("HOME") + string("/.config/ctudc/CtudcClient.conf");
#endif
    return filename;
}

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    
    QApplication a(argc, argv);
    console = new QConsole("CtudcClient");
    a.setStyle(QStyleFactory::create("Fusion"));

    AppSettings conf;
    try {
        conf.load(appConfigPath());
    } catch(std::exception& e) {
        QMessageBox mb(QMessageBox::Critical, "Failed read config", e.what());
        mb.show();
        return a.exec();
    }

    auto conn = std::make_shared<CtudcConn>(conf.addr, conf.port);
    auto recv = std::make_shared<MulticastReceiver>(conf.multicastAddr, conf.multicastPort);

    MainWindow w(conn, recv);
    w.show();
    return a.exec();
}

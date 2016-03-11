#include "ui/mainwindow.hpp"

#include "configparser/appconfigparser.hpp"
#include <QApplication>
#include <memory>

int main(int argc, char* argv[]) {
	std::ios_base::sync_with_stdio(false);
	QApplication a(argc, argv);
	auto conn = std::make_shared<CtudcConn>();
	try {
		AppConfigParser p;
		p.load("CtudcClient.conf");
		auto& c = p.config();
		conn->connectToHost(QString::fromStdString(c.at("address").get<std::string>()),
							c.at("port").get<quint16>());
	} catch(std::exception& e) {
		QMessageBox box;
		box.setText(e.what());
		box.show();
		return a.exec();
	};

	console = new QConsole("CtudcClient");
	a.setStyle(QStyleFactory::create("Fusion"));

	MainWindow w(conn);
	w.show();
	return a.exec();
}

#include "utils.hpp"
#include "ui/qconsole.hpp"

#include <QMessageBox>

using std::string;
using std::function;
using std::exception;

void doAction(const QString& action, function<void()>&& func) {
	try {
		func();
		console->printWithTime(action + " success");
	} catch(exception& e) {
		console->printWithTime(action + " failed: " + e.what());
	}
}

void errorMessage(const QString& title, const QString& message) {
	QMessageBox msg;
	msg.setText(title);
	msg.setInformativeText(message);
	msg.exec();
}

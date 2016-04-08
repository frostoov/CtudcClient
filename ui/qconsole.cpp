#include <sstream>
#include <iomanip>

#include "qconsole.hpp"

using std::unique_lock;
using std::mutex;
using std::ofstream;
using std::endl;
using std::ostringstream;
using std::string;
using std::setw;
using std::setfill;

QConsole* console;

QConsole::QConsole(const QString& title, QWidget* parent)
    : QPlainTextEdit(parent) {
    setMaximumBlockCount(2000);
    logStream.open((title + ".log").toStdString(), ofstream::binary | ofstream::app);
    setReadOnly(true);
    if(!title.isEmpty()) {
        printText("Starting " +  title);
        printText(QString::fromStdString( getDateTimeString(SystemClock::now())) );
    }
}

QConsole::~QConsole() {
    printText("Closing Session");
    if(logStream.is_open())
        logStream.close();
}

void QConsole::printText(const QString& text) {
    unique_lock<mutex> locker(printLock);
    appendPlainText(text);
    if(logStream.is_open())
        logStream << text.toUtf8().data() << endl;
}

void QConsole::printWithTime(const QString& text) {
    ostringstream stream;
    stream << "[" << getDateTimeString(SystemClock::now()) << "]   ";
    stream << text.toStdString();
    printText(QString::fromStdString(stream.str()));
}


std::string QConsole::getTimeString(const TimePoint& time) {
    auto cTime = TimePoint::clock::to_time_t(time);
    auto tm = std::localtime(&cTime);
    ostringstream stream;
    stream << setw(2) << setfill('0') << tm->tm_hour << ":"
           << setw(2) << setfill('0') << tm->tm_min  << ":"
           << setw(2) << setfill('0') << tm->tm_sec;
    return stream.str();
}

std::string QConsole::getDateString(const TimePoint& time) {
    auto cTime = TimePoint::clock::to_time_t(time);
    auto tm = std::localtime(&cTime);
    ostringstream stream;
    stream << setw(2) << setfill('0') << tm->tm_year + 1900 << "."
           << setw(2) << setfill('0') << tm->tm_mon  << "."
           << setw(2) << setfill('0') << tm->tm_mday;
    return stream.str();
}

std::string QConsole::getDateTimeString(const TimePoint& time) {
    return getDateString(time) + " | " + getTimeString(time);
}

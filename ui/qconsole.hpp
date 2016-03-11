#pragma once

#include <QPlainTextEdit>

#include <fstream>
#include <mutex>
#include <chrono>

class QConsole : public QPlainTextEdit {
	using SystemClock = std::chrono::system_clock;
	using TimePoint   = SystemClock::time_point;
public:
	QConsole(const QString& title, QWidget* parent = nullptr);
	~QConsole();
	void printText(const QString& text);
	void printWithTime(const QString& text);
protected:
	static std::string getTimeString(const TimePoint& time);
	static std::string getDateString(const TimePoint& time);
	static std::string getDateTimeString(const TimePoint& time);
private:
	std::mutex       printLock;
	std::ofstream    logStream;
};

extern QConsole* console;

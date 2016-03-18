#pragma once

#include <string>
#include <sstream>

class StringBuilder {
public:
	template<typename T>
	StringBuilder& operator<< (const T& arg) {
		mStream << arg;
		return *this;
	}
	operator std::string() const {
		return mStream.str();
	}
protected:
	std::ostringstream mStream;
};

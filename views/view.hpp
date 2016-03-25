#pragma once

#include <trek/net/response.hpp>

#include <QObject>
#include <functional>
#include <unordered_map>

class View : public QObject {
public:
    using Method = std::function<void(const trek::net::Response&)>;
	using Methods = std::unordered_map<std::string, Method>;
public:
    void handleReponse(const trek::net::Response& response);
    const std::string& name() const;
protected:
    View(const std::string& name, const Methods& method, QObject* parent = nullptr);
private:
    Methods     mMethods;
    std::string mName;
};

#pragma once

#include <trek/net/request.hpp>
#include <trek/net/response.hpp>

#include <memory>

class CtudcConn {
    class impl;
public:
    CtudcConn(const std::string& addr, uint16_t port);
    ~CtudcConn();
    trek::net::Response send(const trek::net::Request& request);
private:
    std::unique_ptr<impl> mImpl;

};

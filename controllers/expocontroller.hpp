#pragma once

#include "net/ctudcconn.hpp"

class ExpoController {
public:
    ExpoController(std::shared_ptr<CtudcConn> conn) : mConn(conn) { }

    void type() {
        mConn->send({"expo", __func__});
    }
    void run() {
        mConn->send({"expo", __func__});
    }
    void launchRead() {
        mConn->send({"expo", __func__});
    }
    void stopRead() {
        mConn->send({"expo", __func__});
    }
    void launchFreq(int delay) {
        mConn->send({"expo", __func__, {delay}});
    }
    void stopFreq() {
        mConn->send({"expo", __func__});
    }
    void freq() {
        mConn->send({"expo", __func__});
    }
    void triggerCount() const {
        mConn->send({"expo", __func__});
    }
    void packageCount() const {
        mConn->send({"expo", __func__});
    }
    void chambersCount() const {
        mConn->send({"expo", __func__});
    }
    void duration() const {
        mConn->send({"expo", __func__});
    }
private:
    std::shared_ptr<CtudcConn> mConn;
};

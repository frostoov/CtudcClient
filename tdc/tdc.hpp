#pragma once

#include <vector>
#include <string>
#include <cstddef>

class Tdc {
public:
    struct Hit {
        Hit(unsigned c, unsigned t) : channel(c), time(t) {}
        unsigned channel;
        unsigned time;
    };
    enum class EdgeDetection {
        leading  = 0,
        trailing = 1,
        leadingTrailing = 2,
    };
    enum class Mode {
        trigger    = 0,
        continuous = 1,
    };
    struct Settings {
        unsigned      windowWidth;
        int           windowOffset;
        EdgeDetection edgeDetection;
        unsigned      lsb;
    };
    using EventHits = std::vector<Hit>;
public:
    virtual ~Tdc() { }

    virtual void readEvents(std::vector<EventHits>& buffer) = 0;
    virtual void readRaw(std::vector<Hit>& buffer) = 0;
    virtual const std::string& name() const = 0;
    virtual Settings settings() = 0;
    virtual bool isOpen() const = 0;
    virtual void clear() = 0;
protected:
    Tdc() = default;
};

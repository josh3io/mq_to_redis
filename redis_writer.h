#pragma once
#include <time.h>
#include <string>
#include <redis3m/redis3m.hpp>

using namespace std;
class RedisWriter {
    public:
        RedisWriter();
        virtual ~RedisWriter();
        virtual void writeActivityFeed(string pwsid, string timestamp, string json);
        virtual void printTime();
        virtual void addTime(clock_t start, clock_t finish);
        virtual void flush();
    private:
        redis3m::simple_pool::ptr_t pool;
        double elapsed;
};


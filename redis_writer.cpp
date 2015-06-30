#include <time.h>
#include <string>
#include "redis_writer.h"
#include <iostream>

using namespace std;

RedisWriter::RedisWriter() {
    pool = redis3m::simple_pool::create("localhost");
}

RedisWriter::~RedisWriter() { }

void RedisWriter::writeActivityFeed(string key, string timestamp, string json) {
    pool->run_with_connection<void>([&](redis3m::connection::ptr_t conn) {
        conn->run(redis3m::command("ZADD")(key)(timestamp)(json));
    });
}

void RedisWriter::printTime() {
    cout << "time elapsed: " << elapsed << " seconds" << endl;
}

void RedisWriter::addTime(clock_t start, clock_t finish) {
    elapsed += ((double) (finish - start)/CLOCKS_PER_SEC);
}

void RedisWriter::flush() {
    pool->run_with_connection<void>([&](redis3m::connection::ptr_t conn) {
        conn->run(redis3m::command("FLUSHALL"));
    });
}

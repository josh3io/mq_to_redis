#include <time.h>
#include <string>
#include <sstream>
#include <vector>
#include <getopt.h>

#include "redis_writer.h"
#include "mq_reader.h"

#define no_argument 0
#define required_argument 1 
#define optional_argument 2

void help() {
    cout << "allowed arguments: \n\t--qmgr=<qm_name> \n\t--channel=<svrconn> \n\t--queue=<read_queue> \n\t--host=<qm_host> \n\t--port=<qm_port>" << endl;
}

auto main(int argc, char **argv) -> int
{
    string qmgr, channel, queue, host, port;
    char c;

    const struct option longopts[] = {
        { "qmgr",       optional_argument,  NULL,   'm' },
        { "channel",    optional_argument,  NULL,   'c' },
        { "queue",      optional_argument,  NULL,   'q' },
        { "host",       optional_argument,  NULL,   'h' },
        { "port",       optional_argument,  NULL,   'p' },
    };

    try {
        while ((c = getopt_long(argc, argv, "mcqhp:", longopts, NULL)) != -1) {
            switch (c) {
            case 'm':
                qmgr = std::string(optarg);
                break;
            case 'c':
                channel = std::string(optarg);
                break;
            case 'q':
                queue = std::string(optarg);
                break;
            case 'h':
                host = std::string(optarg);
                break;
            case 'p':
                port = std::string(optarg);
                break;
            }
        }
    } catch (std::logic_error e) {
        help();
        exit(1);
    }

    if (qmgr.empty()) {
        qmgr = MQReader::DEFAULT_QMGR;
    }
    if (channel.empty()) {
        channel = MQReader::DEFAULT_CHANNEL;
    }
    if (queue.empty()) {
        queue = MQReader::DEFAULT_QUEUE;
    }
    if (host.empty()) {
        host = MQReader::DEFAULT_HOST;
    }
    if (port.empty()) {
        port = MQReader::DEFAULT_PORT;
    }

    MQReader mq(qmgr,channel,host,port);
    RedisWriter writer;
    try {
        mq.connect();

        //mq.putMessage(queue,std::string("123456789_12345/987987987/test message test message test message test message test message test message test message test message test message test message test message test message test message test message test message"));
        //mq.putMessage(queue,std::string("123456789_12345/987987987/test message test message test message test message test message test message test message test message test message test message test message test message test message test message test message"));
    } catch (int e) {
        cout << "caught exception while connecting: " << e << "; mq reasonCode " << mq.errCode() << endl;
        return e;
    }

    cout << "connected to mq " << qmgr << " on conn " << channel << "/TCP/" << MQReader::makeConnName(host,port) << endl;
    cout << "going to read from queue " << queue << endl;

    try {
        while (1) {
            const string s = mq.getMessage(queue);
            if (s != "") {
                std::vector<std::string> msg = mq.parseMessage(s);
                cout << "write " << msg[0] << ":" << msg[1] << ":" << msg[2] << endl;
                writer.writeActivityFeed(msg[0],msg[1],msg[2]);
            }
        }
    } catch (int e) {
        cout << "caught exception: " << e << "; mq reasonCode " << mq.errCode() << endl;
        return e;
    }
    return 0;
}

#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <imqi.hpp>

class MQReader
{
    public:
        MQReader();
        MQReader(const std::string& qmgr, const std::string& channel, const std::string& host, const std::string& port);
        virtual ~MQReader();

        virtual void connect();

        virtual void putMessage(const std::string& queueName, const std::string& message);
        virtual std::string getMessage(const std::string& queueName);

        virtual std::vector<std::string> parseMessage(const std::string& s);

        virtual int errCode();

        static std::string makeConnName(const std::string& host, const std::string& port);

        const static int ERR_NO_QMGR = 0;
        const static int ERR_MSG_IS_NOT_STRING = 1;
        const static int ERR_NO_TEXT = 2;
        const static int ERR_PUT_FAIL = 3;
        const static int ERR_GET_FAIL = 4;
        const static char MQ_MSG_DELIM = '/';

        const static int GET_WAIT_INTERVAL = 15000;

        const static std::string DEFAULT_QMGR;
        const static std::string DEFAULT_CHANNEL;
        const static std::string DEFAULT_QUEUE;
        const static std::string DEFAULT_HOST;
        const static std::string DEFAULT_PORT;
    private:
        ImqQueueManager manager;
        ImqChannel* channel;
        ImqMessage* msg;
        ImqGetMessageOptions gmo;
        int rawErrCode;

        std::unordered_map<std::string,ImqQueue*> inQueues;
        std::unordered_map<std::string,ImqQueue*> outQueues;

        virtual void defChannel(const std::string& channelName, const std::string& connName);
        virtual std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
        virtual std::vector<std::string> split(const std::string &s, char delim);
};

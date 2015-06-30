#define NDEBUG
#include "mq_reader.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

const std::string MQReader::DEFAULT_QMGR = "my_qmgr";
const std::string MQReader::DEFAULT_CHANNEL = "SYSTEM.DEF.SVRCONN";
const std::string MQReader::DEFAULT_QUEUE = "TEST";
const std::string MQReader::DEFAULT_HOST = "localhost";
const std::string MQReader::DEFAULT_PORT = "1414";

MQReader::MQReader() {
    defChannel("MY.SVRCONN","localhost(1414)");
    manager.setChannelReference(channel);
    manager.setName("my_qmgr");
    gmo.setOptions(MQGMO_WAIT | MQGMO_FAIL_IF_QUIESCING);
    gmo.setWaitInterval(GET_WAIT_INTERVAL);
}

MQReader::MQReader(const std::string& qmgr, const std::string& channelName, const std::string& host, const std::string& port) {
    defChannel(channelName,makeConnName(host,port));
    manager.setChannelReference(channel);
    manager.setName(qmgr.c_str());

    gmo.setOptions(MQGMO_WAIT | MQGMO_FAIL_IF_QUIESCING);
    gmo.setWaitInterval(GET_WAIT_INTERVAL);
}

std::string MQReader::makeConnName(const std::string& host, const std::string& port) {
    return host + std::string("(") + port + std::string(")");
}

void MQReader::connect() {
    if (!manager.connect()) {
        rawErrCode = (int) manager.reasonCode();
        throw ERR_NO_QMGR;
    }
}
MQReader::~MQReader() {

    for (auto it = inQueues.begin(); it != inQueues.end(); ++it) {
        it->second->close();
    }
    for (auto it = outQueues.begin(); it != outQueues.end(); ++it) {
        it->second->close();
    }

    manager.setChannelReference();
    delete channel;
}

void MQReader::putMessage(const std::string& queueName, const std::string& message) {
    ImqQueue* queue = new ImqQueue;
    ImqMessage* msg = new ImqMessage;

    std::unordered_map<std::string,ImqQueue*>::const_iterator got = outQueues.find(queueName);
    if (got == inQueues.end()) {
        queue->setConnectionReference(manager);
        queue->setName(queueName.c_str());
        queue->setOpenOptions(MQOO_OUTPUT | MQOO_INQUIRE | MQOO_FAIL_IF_QUIESCING);
        outQueues[queueName] = queue;
    } else {
        queue = got->second;
    }
    
    msg->setFormat(MQFMT_STRING);
    msg->useFullBuffer(message.c_str(), message.size());

    bool retval = queue->put(*msg);


    delete msg;

    if (!retval) { 
        rawErrCode = (int) queue->reasonCode();
        throw ERR_PUT_FAIL; 
    }

}

string MQReader::getMessage(const string& queueName) {
    ImqQueue* queue = new ImqQueue;
    ImqMessage* msg = new ImqMessage;


    std::unordered_map<std::string,ImqQueue*>::const_iterator got = inQueues.find(queueName);
    if (got == inQueues.end()) {
        queue->setConnectionReference(manager);
        queue->setName(queueName.c_str());
        queue->setOpenOptions(MQOO_INPUT_SHARED | MQOO_INQUIRE | MQOO_FAIL_IF_QUIESCING);
        inQueues[queueName] = queue;
    } else {
        queue = got->second;
    }
    
    msg->setFormat(MQFMT_STRING);

    if (!queue->get(*msg,gmo)) {
        rawErrCode = (int)queue->reasonCode();
        if (rawErrCode != MQRC_NO_MSG_AVAILABLE) {
            throw ERR_GET_FAIL;
        } else {
            return std::string("");
        }
    }

    int len = (int)msg->dataLength();
    if (len == 0) {
        rawErrCode = -1;
        throw ERR_NO_TEXT;
    }
    if (!msg->formatIs(MQFMT_STRING)) {
        rawErrCode = -1;
        throw ERR_MSG_IS_NOT_STRING;
    }

    char* text  = msg->bufferPointer();

    return std::string(text);
}

void MQReader::defChannel(const string& channelName, const string& connName) {
    channel = new ImqChannel;
    channel->setHeartBeatInterval(1);
    channel->setChannelName(channelName.c_str());
    channel->setTransportType(MQXPT_TCP);
    channel->setConnectionName(connName.c_str());
}

int MQReader::errCode() {
    return rawErrCode;
}

std::vector<std::string> MQReader::parseMessage(const std::string& s) {
    return split(s,MQ_MSG_DELIM);
}


std::vector<std::string> &MQReader::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim) && elems.size() < 3) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> MQReader::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}



/*
 * sender.cc
 *
 *  Created on: May 16, 2020
 *      Author: hongt
 */
#include <omnetpp.h>
#include <queue>
#include <string>

using namespace omnetpp;
using namespace std;
class Senders: public cSimpleModule{
private:
    int EXB_SIZE;
    double TIMEOUT;
    double TIME_INTERVAL;
    double CHANNEL_DELAY;
    int BUFFER_COUNTER;
    int lastMessageId = -1;

    queue<int> SQ;
    queue<int> EXB;

    void generateMessage();
    void sendToExitBuffer();
    void sendToSwitch();
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Senders);

void Senders::initialize(){
    EXB_SIZE = par("EXB_SIZE").intValue();
    BUFFER_COUNTER = EXB_SIZE;
    TIMEOUT = par("TIMEOUT").doubleValue();
    TIME_INTERVAL = par("TIME_INTERVAL").doubleValue();
    CHANNEL_DELAY = par("CHANNEL_DELAY").doubleValue();

    scheduleAt(0, new cMessage("generate"));
    scheduleAt(0, new cMessage("send"));
}

void Senders::handleMessage(cMessage *msg){
    if(simTime() >= TIMEOUT)
        return;

    if(strcmp(msg->getName(), "generate") == 0){
        generateMessage();
        if(EXB.size() < EXB_SIZE)
            sendToExitBuffer();
        scheduleAt(simTime() + TIME_INTERVAL, msg);
    }

    if(strcmp(msg->getName(), "send") == 0){
        if(BUFFER_COUNTER > 0){
            sendToSwitch();
            sendToExitBuffer();
            --BUFFER_COUNTER;
        }
        scheduleAt(simTime() + CHANNEL_DELAY, msg);
    }

    if(strcmp(msg->getName(), "signal") == 0){
        ++BUFFER_COUNTER;
        delete msg;
    }
}

void Senders::generateMessage(){
    SQ.push(++lastMessageId);
    EV << "generated message id = " << lastMessageId << endl;
}

void Senders::sendToExitBuffer(){
    if( !SQ.empty()){
        int msgId = SQ.front();
        SQ.pop();
        EXB.push(msgId);
    }
}

void Senders::sendToSwitch(){
    int sendMsgId = EXB.front();
    EXB.pop();
    char nameMessage[10];
    strcpy(nameMessage, getName());

    cMessage *sendMsg = new cMessage(nameMessage);
    sendMsg->addPar("msgId");
    sendMsg->par("msgId").setLongValue(sendMsgId);

    //EV << getName() << ": " << sendMsg->par("msgId").longValue() << endl;
    send(sendMsg, "out");
}



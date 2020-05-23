/*
 * switch.cc
 *
 *  Created on: May 16, 2020
 *      Author: hongt
 */
#include <omnetpp.h>
#include <queue>
#include <limits>

using namespace omnetpp;
using namespace std;
class Switch: public cSimpleModule{
private:
    int EXB_SIZE;
    double TIMEOUT;
    double TIME_INTERVAL;
    double CHANNEL_DELAY;
    bool isChannelBussy;

    queue<int> ENB[3], EXB;

    void sendToExitBuffer();
    bool checkENB();
    void sendToReceiver();
    void sendSignalToSender(int);
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Switch);

void Switch::initialize(){
    EXB_SIZE = par("EXB_SIZE").intValue();
    TIMEOUT = par("TIMEOUT").doubleValue();
    TIME_INTERVAL = par("TIME_INTERVAL").doubleValue();
    CHANNEL_DELAY = par("CHANNEL_DELAY").doubleValue();
    isChannelBussy = false;

    //sự kiện gửi gói tin của switch được
    scheduleAt(simTime() + TIME_INTERVAL, new cMessage("send"));
}

void Switch::handleMessage(cMessage *msg){
    if(simTime() >= TIMEOUT){
        EV << "Time out" << endl;
        return;
    }

    /**
     * lấy id của gói tin mà các sender gửi lên
     * lưu các id vào ENB tương ứng
     * sinh sự kiện gửi gói tin từ ENB sang EXB sau 1 chu kỳ hoạt động của switch = chu kỳ sinh gói tin
     */
    if(strcmp(msg->getName(), "sender to receiver") == 0){
        int index = msg->getSenderModule()->getIndex();
        int payload = msg->par("msgId").longValue();
        ENB[index].push(payload);
        cMessage *newMsg = new cMessage("ENB to EXB");
        newMsg->addPar("senderId");
        newMsg->par("senderId").setLongValue(payload);
        scheduleAt(simTime() + TIME_INTERVAL, newMsg);
    }

    /**
     * kiểm tra xem gói tin yêu cầu chuyển từ ENB sang EXB
     * chọn gói tin có id bé nhất để gửi
     */

    if(strcmp(msg->getName(), "ENB to EXB") == 0){
        if(EXB.size() < EXB_SIZE){
            if(checkENB()){
                EV << "Send to Exit buffer!" << endl;
                sendToExitBuffer();
                delete msg;
            }
        }
    }

    //Set channel status if send success message
    if(strcmp(msg->getName(), "signal") == 0){
        isChannelBussy = false;
        delete msg;
    }

    //Send message to receiver
    if(strcmp(msg->getName(), "send") == 0){
        if(!EXB.empty()){
            if(!isChannelBussy){
                EV << "signal" << endl;
                sendToReceiver();
                isChannelBussy = true;
            }
        }
        scheduleAt(simTime() + TIME_INTERVAL, msg);
    }
}

/**
 * gửi thông báo ENB tương ứng có chỗ trống
 * @input số hiệu cổng gửi signal
 * @return không
 */

bool Switch::checkENB(){
    for (int i = 0; i < 3; i++){
        if(!ENB[i].empty())
            return true;
    }
    return false;
}

void Switch::sendSignalToSender(int port){
    send(new cMessage("signal"), "out", port);
}


/**
 * gửi gói tin từ ENB sang EXB
 * @input con chỏ char chứa thông tin tên ENB có gói tin gửi đến EXB
 * @return không
 */

void Switch::sendToExitBuffer(){
    int id = numeric_limits<int>::max();
    int port = 0;
    for ( int i = 0; i < 3; i++){
        if (!ENB[i].empty()){
            if(ENB[i].front() < id){
                id = ENB[i].front();
                ENB[i].pop();
                port = i;
            }
        }
    }
    EXB.push(id);
    sendSignalToSender(port);
}

/**
 * gửi gói tin đến receiver
 * @return không
 */

void Switch::sendToReceiver(){
    int sendMsgId = EXB.front();
    EXB.pop();

    cMessage *sendMsg = new cMessage("sender to receiver msg");

    cMsgPar *msgParam = new cMsgPar("msgId");
    msgParam->setLongValue(sendMsgId);
    sendMsg->addPar(msgParam);

    send(sendMsg, "out", 3);
}

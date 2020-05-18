/*
 * switch.cc
 *
 *  Created on: May 16, 2020
 *      Author: hongt
 */
#include <omnetpp.h>
#include <queue>

using namespace omnetpp;
class Switch: public cSimpleModule{
private:
    int EXB_SIZE;
    double TIMEOUT;
    double TIME_INTERVAL;
    double CHANNEL_DELAY;

    queue<int> ENB_A, ENB_B, ENB_C, EXB_E;

    //thời gian mà gói tin đầu tiên trong từng ENB muốn chuyển sang EXB
    simtime_t time_A, time_B, time_C;
    void sendToExitBuffer(char *);
    void sendToReceiver();
    void sendSignalToSender();
    int findMin_3(int, int, int);
    int findMin_2(int, int);
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
}

void Switch::handleMessage(cMessage *msg){
    if(simTime() >= TIMEOUT){
        EV << "Time out" << endl;
        return;
    }

    if(strcmp(msg->getName(), "sender to receiver msg") == 0){
        if(strcmp(getName(), "senderA") == 0){
            ENB_A.push(msg->par("msgId"));
            time_A = simTime() + TIME_INTERVAL;
            scheduleAt(time_A, new cMessage("ENB_A"));
        }else if(strcmp(getName(), "senderB") == 0){
            ENB_B.push(msg->par("msgId"));
            time_B = simTime() + TIME_INTERVAL;
            scheduleAt(time_B, new cMessage("ENB_B"));
        }else{
            ENB_C.push(msg->par("msgId"));
            time_C = simTime() + TIME_INTERVAL;
            scheduleAt(time_C, new cMessage("ENB_C"));
        }
    }

    if(EXB_E.size() == EXB_SIZE){
        if(time_A < simTime())
            time_A = simTime();
        if(time_B < simTime())
            time_B = simTime();
        if(time_C < simTime())
            time_C = simTime();
    }

    if(EXB_E.size() < EXB_SIZE){
        simtime_t time;
        int result = 0;
        if(time_A <= time && time_B <= time && time_C <= time){
            result = findMin_3(ENB_A.front(), ENB_A.front(), ENB_A.front());
            switch(result){
                case ENB_A.front():{
                    sendToExitBuffer("ENB_A");
                    sendSignalToSender("ENB_A");
                    break;
                }
                case ENB_B.front():{
                    sendToExitBuffer("ENB_B");
                    sendSignalToSender("ENB_B");
                    break;
                }
                default:{
                    sendToExitBuffer("ENB_C");
                    sendSignalToSender("ENB_C");
                    break;
                }
            }

        } else if (time_A <= time && time_B <= time){
            result = fineMin_2(ENB_A.front(), ENB_B.front());
            if (result == ENB_A.front()){
                sendToExitBuffer("ENB_A");
                sendSignalToSender("ENB_A");
            }else{
                sendToExitBuffer("ENB_B");
                sendSignalToSender("ENB_B");
            }
        } else if(time_A <= time && time_C <= time){
            result = fineMin_2(ENB_A.front(), ENB_c.front());
            if (result == ENB_A.front()){
                sendToExitBuffer("ENB_A");
                sendSignalToSender("ENB_A");
            }else{
                sendToExitBuffer("ENB_C");
                sendSignalToSender("ENB_C");
            }
        } else if(time_B <= time && time_C <= time){
            result = fineMin_2(ENB_C.front(), ENB_B.front());
            if (result == ENB_B.front()){
                sendToExitBuffer("ENB_B");
                sendSignalToSender("ENB_B");
            }else{
                sendToExitBuffer("ENB_C");
                sendSignalToSender("ENB_C");
            }
        } else if(time_A <= time){
            sendToExitBuffer("ENB_A");
            sendSignalToSender("ENB_A");
        } else if(time_B <= time){
            sendToExitBuffer("ENB_B");
            sendSignalToSender("ENB_B");
        } else{
            sendToExitBuffer("ENB_C");
            sendSignalToSender("ENB_C");
        }
    }

    if()

}

int Switch::findMin_3(int x, int y, int z){
    int min = x;
    if (y < min)
        min = y;
    if (z < min)
        min = z;
    return min;
}

int Switch::findMin_2(int x, int y){
    if (x < y)
        return x;
    else
        return y;
}

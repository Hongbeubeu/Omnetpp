/*
 * switch.cc
 *
 *  Created on: May 16, 2020
 *      Author: hongt
 */
#include <omnetpp.h>
#include <queue>

using namespace omnetpp;
using namespace std;
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
    void sendSignalToSender(char *);
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
    time_A = simTime();
    time_B = simTime();
    time_C = simTime();

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
     *
     */
    if(strcmp(msg->getName(), "senderA") == 0){
        ENB_A.push(msg->par("msgId").longValue());
        time_A = simTime() + TIME_INTERVAL;
        scheduleAt(time_A, new cMessage("ENB to EXB"));
    }else if(strcmp(msg->getName(), "senderB") == 0){
        ENB_B.push(msg->par("msgId").longValue());
        time_B = simTime() + TIME_INTERVAL;
        scheduleAt(time_B, new cMessage("ENB to EXB"));
    }else if(strcmp(msg->getName(), "senderC") == 0){
        ENB_C.push(msg->par("msgId").longValue());
        time_C = simTime() + TIME_INTERVAL;
        scheduleAt(time_C, new cMessage("ENB to EXB"));
    }

    /**
     * kiểm tra nếu EXB đầy thì chỉnh thời gian gói tin muốn chuyển từ ENB sang EXB thành thời điểm hiện tại
     */
    if(EXB_E.size() == EXB_SIZE){
        if(time_A < simTime())
            time_A = simTime();
        if(time_B < simTime())
            time_B = simTime();
        if(time_C < simTime())
            time_C = simTime();
    }

    /**
     * kiểm tra xem gói tin yêu cầu chuyển từ ENB sang EXB
     * chọn gói tin có id bé nhất để gửi
     */
    if(strcmp(msg->getName(), "ENB to EXB") == 0){
        if(EXB_E.size() < EXB_SIZE){
            simtime_t time = simTime();
            int result = 0;
            //trường hợp cả 3 ENB đều không rỗng
            if(!ENB_A.empty() && !ENB_B.empty() && !ENB_C.empty()){
                //trường hợp cả 3 ENB đều có gói tin muốn chuyển sang EXB
                if(time_A <= time && time_B <= time && time_C <= time){
                    result = findMin_3(ENB_A.front(), ENB_B.front(), ENB_C.front());
                    if(result == ENB_A.front()){
                        char s[6] = "ENB_A";
                        sendToExitBuffer(s);
                    }
                    else if(result == ENB_B.front()){
                        char s[6] = "ENB_B";
                        sendToExitBuffer(s);
                    }
                    else{
                        char s[6] = "ENB_C";
                        sendToExitBuffer(s);
                    }
                //trường hợp chỉ có 2 ENB có gói tin muốn chuyển lên EXB
                } else if (time_A <= time && time_B <= time){
                    result = findMin_2(ENB_A.front(), ENB_B.front());
                    if (result == ENB_A.front()){
                        char s[6] = "ENB_A";
                        sendToExitBuffer(s);
                    }else{
                        char s[6] = "ENB_B";
                        sendToExitBuffer(s);
                    }
                } else if(time_A <= time && time_C <= time){
                    result = findMin_2(ENB_A.front(), ENB_C.front());
                    if (result == ENB_A.front()){
                        char s[6] = "ENB_A";
                        sendToExitBuffer(s);
                    }else{
                        char s[6] = "ENB_C";
                        sendToExitBuffer(s);
                    }
                } else if(time_B <= time && time_C <= time){
                    result = findMin_2(ENB_C.front(), ENB_B.front());
                    if (result == ENB_B.front()){
                        char s[6] = "ENB_B";
                        sendToExitBuffer(s);
                    }else{
                        char s[6] = "ENB_C";
                        sendToExitBuffer(s);
                    }
                // trường hợp chỉ có 1 ENB có gói tin muốn chuyển lên EXB
                } else if(time_A <= time){
                    char s[6] = "ENB_A";
                    sendToExitBuffer(s);
                } else if(time_B <= time){
                    char s[6] = "ENB_B";
                    sendToExitBuffer(s);
                } else{
                    char s[6] = "ENB_C";
                    sendToExitBuffer(s);
                }
            //trường hợp ENB_C rỗng
            }else if(!ENB_A.empty() && !ENB_B.empty()){
                result = findMin_2(ENB_A.front(), ENB_B.front());
                if (result == ENB_A.front()){
                    char s[6] = "ENB_A";
                    sendToExitBuffer(s);
                }else{
                    char s[6] = "ENB_B";
                    sendToExitBuffer(s);
                }
            //trường hợp ENB_A rỗng
            }else if(!ENB_B.empty() && !ENB_C.empty()){
                result = findMin_2(ENB_C.front(), ENB_B.front());
                if (result == ENB_C.front()){
                    char s[6] = "ENB_C";
                    sendToExitBuffer(s);
                }else{
                    char s[6] = "ENB_B";
                    sendToExitBuffer(s);
                }
            //trường hợp ENB_B rỗng
            }else if(!ENB_A.empty() && !ENB_C.empty()){
                result = findMin_2(ENB_A.front(), ENB_C.front());
                if (result == ENB_A.front()){
                    char s[6] = "ENB_A";
                    sendToExitBuffer(s);
                }else{
                    char s[6] = "ENB_C";
                    sendToExitBuffer(s);
                }
            // trường hợp chỉ có ENB_A có gói tin cần gửi lên EXB
            }else if(!ENB_A.empty()){
                char s[6] = "ENB_A";
                sendToExitBuffer(s);
            // trường hợp chỉ có ENB_B có gói tin cần gửi lên EXB
            }else if(!ENB_B.empty()){
                char s[6] = "ENB_B";
                sendToExitBuffer(s);
            // trường hợp chỉ có ENB_C có gói tin cần gửi lên EXB
            }else if(!ENB_C.empty()){
                char s[6] = "ENB_C";
                sendToExitBuffer(s);
            }
        }
        delete msg;
    }

    /**
     * kiểm tra nếu đang ở sự kiện gửi mà EXB có gói tin cần gửi
     * gửi gói tin sang receiver
     * sinh sự kiện gửi sau 1 chu kỳ hoạt động
     */
    if(strcmp(msg->getName(), "send") == 0){
        if(!EXB_E.empty()){
            sendToReceiver();
        }
        scheduleAt(simTime() + TIME_INTERVAL, msg);
    }
}

/**
 * gửi thông báo ENB tương ứng có chỗ trống
 * @input con trỏ char chứa tên của sender cần được thông báo ENB có chỗ trống
 * @return không
 */
void Switch::sendSignalToSender(char *sender){
    if (strcmp(sender, "ENB_A") == 0)
        send(new cMessage("signal"), "out", 0);
    else if(strcmp(sender, "ENB_B") == 0)
        send(new cMessage("signal"), "out", 1);
    else
        send(new cMessage("signal"), "out", 2);
}


/**
 * gửi gói tin từ ENB sang EXB
 * @input con chỏ char chứa thông tin tên ENB có gói tin gửi đến EXB
 * @return không
 */
void Switch::sendToExitBuffer(char *ENB){
    if (strcmp(ENB, "ENB_A") == 0){
        EV << "signal to A: " << ENB << endl;
        int msgId = ENB_A.front();
        ENB_A.pop();
        EXB_E.push(msgId);
        sendSignalToSender(ENB);
    } else if (strcmp(ENB, "ENB_B") == 0){
        EV << "signal to B: " << ENB << endl;
        int msgId = ENB_B.front();
        ENB_B.pop();
        EXB_E.push(msgId);
        sendSignalToSender(ENB);
    } else{
        EV << "signal to C: " << ENB << endl;
        int msgId = ENB_C.front();
        ENB_C.pop();
        EXB_E.push(msgId);
        sendSignalToSender(ENB);
    }
}

/**
 * gửi gói tin đến receiver
 * @return không
 */
void Switch::sendToReceiver(){
    int sendMsgId = EXB_E.front();
    EXB_E.pop();

    cMessage *sendMsg = new cMessage("sender to receiver msg");

    cMsgPar *msgParam = new cMsgPar("msgId");
    msgParam->setLongValue(sendMsgId);
    sendMsg->addPar(msgParam);

    send(sendMsg, "out", 3);
}

/**
 * trả về giá trị bé nhất trong 3 số
 * @input là các giá trị số nguyên cần được so sánh
 * @return giá trị bé nhất trong 3 số đầu vào
 */
int Switch::findMin_3(int x, int y, int z){
    int min = x;
    if (y < min)
        min = y;
    if (z < min)
        min = z;
    return min;
}


/**
 * trả về giá trị bé nhất trong 2 số
 * @input là các giá trị số nguyên cần được so sánh
 * @return giá trị bé nhất trong 2 số đầu vào
 */
int Switch::findMin_2(int x, int y){
    if (x < y)
        return x;
    else
        return y;
}

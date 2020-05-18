simple Senders{
    parameters:
        @display("i=device/pc");
        double xpos;
        double ypos;
        @display("p=$xpos,$ypos");
        
        int EXB_SIZE;
        double TIMEOUT;
        double TIME_INTERVAL;
        double CHANNEL_DELAY;
    gates:
        input in;
    	output out;
}

simple Receiver{
    parameters:
        @display("i=device/pc2");
        double xpos;
        double ypos;
        @display("p=$xpos,$ypos");
        
        double TIMEOUT;
    	double TIME_INTERVAL;
    gates:
        input in;
        
}

simple Switch{
    parameters:
        @display("i=device/switch");
        double xpos;
        double ypos;
        @display("p=$xpos,$ypos");
        
        double TIMEOUT;
        int EXB_SIZE;
        double CREDIT_DELAY;
        double CHANNEL_DELAY;
        double TIME_INTERVAL;
    gates:
        input in[3];
        output out[4];
}

network SenderSwitchReceiver{
	parameters:
   	    senderA.xpos = 100;
   	    senderA.ypos = 100;
   	    
   	    senderB.xpos = 100;
   	    senderB.ypos = 200;
   	    
   	    senderC.xpos = 100;
   	    senderC.ypos = 300;
   	    
   	    switchD.xpos = 200;
   	    switchD.ypos = 200;
   	    
   	    receiverE.xpos = 300;
   	    receiverE.ypos = 200;
   	    
    types:
        channel Channel extends ned.DatarateChannel{
            delay = 15ms;
        }
   
    submodules:
        senderA: Senders;
        senderB: Senders;
        senderC: Senders;
        switchD: Switch;
        receiverE: Receiver;
    connections:
        senderA.out --> Channel --> switchD.in[0];
        senderB.out --> Channel --> switchD.in[1];
        senderC.out --> Channel --> switchD.in[2];
        
        senderA.in <-- Channel <-- switchD.out[0];
        senderB.in <-- Channel <-- switchD.out[1];
        senderC.in <-- Channel <-- switchD.out[2];
        
        receiverE.in <-- Channel <-- switchD.out[3];
        
}
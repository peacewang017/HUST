#include "../include/Global.h"

// struct  Packet {
// 	int seqnum;										//序号
// 	int acknum;										//确认号
// 	int checksum;									//校验和
// 	char payload[Configuration::PAYLOAD_SIZE];		//payload
	
// 	Packet();
// 	Packet(const Packet& pkt);
// 	virtual Packet & operator=(const Packet& pkt);
// 	virtual bool operator==(const Packet& pkt) const;
// 	virtual ~Packet();

// 	virtual void print();
// };

Packet makePkt(int seqNum,int ackNum,const char *data){
    Packet pkt;
    pkt.seqnum = seqNum;
    pkt.acknum = ackNum;
    memcpy(pkt.payload,data,Configuration::PAYLOAD_SIZE);
    pkt.checksum = pUtils->calculateCheckSum(pkt);
    return pkt;
}

Packet makeDataPkt(int seqNum,const char* data){
    return makePkt(seqNum,-1,data);
}

Packet makeAckPkt(int ackNum){
    char data[Configuration::PAYLOAD_SIZE] = "ACK";
    return makePkt(-1,ackNum,data);
}
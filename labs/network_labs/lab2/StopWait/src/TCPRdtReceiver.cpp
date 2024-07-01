#include "../include/TCPRdtReceiver.h"
#include "../include/utils.h"

TCPRdtReceiver::TCPRdtReceiver(int seqNumBits)
	: MAX_SEQ((seqNumBits > 0 && seqNumBits <= 16) ? (1 << seqNumBits) :
							 (1 << 16))
{
	expectedSeqNum = 0;
	lastAckPkt = makeAckPkt(-1);
}

TCPRdtReceiver::~TCPRdtReceiver()
{
}

void TCPRdtReceiver::receive(const Packet &packet)
{
	printf("---------------------------------------------------------------\n");
	printf("receiver window:\n");
	int checkSum = pUtils->calculateCheckSum(packet);
	if (checkSum == packet.checksum && expectedSeqNum == packet.seqnum) {
		pUtils->printPacket("receiver got data packet correctly",
				    packet);
		Message msg;
		memcpy(msg.data, packet.payload, sizeof(packet.payload));
		pns->delivertoAppLayer(RECEIVER, msg);
		lastAckPkt = makeAckPkt(expectedSeqNum);
		pUtils->printPacket("receiver sent ACK packet", lastAckPkt);
		expectedSeqNum = (expectedSeqNum + 1) % MAX_SEQ;
	} else {
		if (checkSum != packet.checksum)
			pUtils->printPacket(
				"receiver got data packet incorrectly due to incorrect data",
				packet);
		else
			pUtils->printPacket(
				"receiver got data packet incorrectly due to incorrect seqnum",
				packet);
		pUtils->printPacket("receiver resent ACK packet", lastAckPkt);
	}
	//调用模拟网络环境的sendToNetworkLayer，通过网络层发送上次的确认报文
	pns->sendToNetworkLayer(SENDER, lastAckPkt);
	printf("---------------------------------------------------------------\n\n");
}

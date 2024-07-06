#include "../include/Global.h"
#include "../include/GBNRdtSender.h"
#include "../include/utils.h"

GBNRdtSender::GBNRdtSender(int n, int seqNumBits)
	: MAX_SEQ((seqNumBits > 0 && seqNumBits <= 16) ? (1 << seqNumBits) :
							 (1 << 16))
	, N(n)
{
	base = 0;
	nextSeqNum = 0;
}

GBNRdtSender::~GBNRdtSender()
{
}

bool GBNRdtSender::getWaitingState()
{
	return (nextSeqNum == (base + N) % MAX_SEQ);
}

bool GBNRdtSender::send(const Message &message)
{
	if (getWaitingState()) {
		return false;
	}

	Packet pkt = makeDataPkt(nextSeqNum, message.data);
	pkts[nextSeqNum] = pkt;
	pns->sendToNetworkLayer(RECEIVER, pkt);
	pUtils->printPacket("sender sent data packet", pkt);

	nextSeqNum = (nextSeqNum + 1) % MAX_SEQ;
	if (nextSeqNum == ((base + 1) % MAX_SEQ)) {
		pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	}

	return true;
}

void GBNRdtSender::receive(const Packet &ackPkt)
{
	printf("---------------------------------------------------------------\n");
	printf("sender window:\n");
	for (int i = base; i != nextSeqNum; i = (i + 1) % MAX_SEQ) {
		printf("%d ", pkts[i].seqnum);
	}
	printf("\n");

	if (ackPkt.checksum == pUtils->calculateCheckSum(ackPkt)) {
		base = (ackPkt.acknum + 1) %
		       MAX_SEQ; //由于累计确认，后面的报文段收到能够说明前面的报文段运送正确
		if (pkts.count(ackPkt.acknum)) {
			pkts.erase(ackPkt.acknum);
		}
		pUtils->printPacket("sender got ACK packet correctly", ackPkt);
		if (base == nextSeqNum) {
			pns->stopTimer(SENDER, 0);
		}
	} else {
		pUtils->printPacket("sender got ACK packet incorrectly",
				    ackPkt);
	}
	printf("---------------------------------------------------------------\n\n");
}

void GBNRdtSender::timeoutHandler(int seqNum)
{
	int n = ((nextSeqNum - base) + MAX_SEQ) % MAX_SEQ;
	printf("timeout,resend %d packets,seqnum from %d to %d", n, base,
	       nextSeqNum - 1);
	pns->stopTimer(SENDER, 0);

	for (int i = base; i != nextSeqNum; i = (i + 1) % MAX_SEQ) {
		pns->sendToNetworkLayer(RECEIVER, pkts[i]);
		pUtils->printPacket("packet resent", pkts[i]);
	}

	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
}
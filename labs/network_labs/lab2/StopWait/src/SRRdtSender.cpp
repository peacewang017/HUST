#include "../include/utils.h"
#include "../include/SRRdtSender.h"

SRRdtSender::SRRdtSender(int n, int seqNumBits)
	: MAX_SEQ((seqNumBits > 0 && seqNumBits <= 16) ? (1 << seqNumBits) :
							 (1 << 16))
	, N(n)
{
	base = 0;
	nextSeqNum = 0;
}

SRRdtSender::~SRRdtSender()
{
}

inline bool SRRdtSender::inWindow(int ackNum)
{
	if (base == nextSeqNum) // base == nextSeqNum == 0
		return false;
	if (base < nextSeqNum)
		return base <= ackNum && ackNum < nextSeqNum;
	return nextSeqNum > ackNum || ackNum >= base; // 序号循环
}

bool SRRdtSender::getWaitingState()
{
	return nextSeqNum == (base + N) % MAX_SEQ;
}

bool SRRdtSender::send(const Message &message)
{
	if (getWaitingState()) { // 发送方处于等待确认状态
		return false;
	}
	Packet pkt = makeDataPkt(nextSeqNum, message.data);
	pkts[nextSeqNum] = PacketDocker(pkt, false);
	pUtils->printPacket("sender sent data packet", pkt);
	pns->sendToNetworkLayer(RECEIVER, pkt);
	// 启动发送方定时器
	pns->startTimer(SENDER, Configuration::TIME_OUT, nextSeqNum);
	nextSeqNum = (nextSeqNum + 1) % MAX_SEQ;
	fflush(stdout);
	return true;
}

void SRRdtSender::receive(const Packet &ackPkt)
{
	printf("---------------------------------------------------------------\n");
	printf("sender window:\n");
	for (int i = base; i != nextSeqNum; i = (i + 1) % MAX_SEQ)
		printf("%d ", pkts[i].first.seqnum);
	printf("\n");
	// 检查校验和是否正确
	int checkSum = pUtils->calculateCheckSum(ackPkt);
	// 如果校验和正确
	if (checkSum == ackPkt.checksum) {
		if (inWindow(ackPkt.acknum)) {
			pUtils->printPacket("sender got ACK packet correctly",
					    ackPkt);
			if (!pkts[ackPkt.acknum].second) {
				pkts[ackPkt.acknum].second = true;
				pns->stopTimer(SENDER, ackPkt.acknum);
			}
			if (ackPkt.acknum == base) {
				while (base != nextSeqNum &&
				       pkts[base].second) {
					pkts.erase(base);
					base = (base + 1) % MAX_SEQ;
				}
			}
		} else {
			pUtils->printPacket(
				"sender got ACK packet correctly,but not in the window",
				ackPkt);
		}
	} else {
		pUtils->printPacket("sender got ACK packet incorrectly",
				    ackPkt);
	}
	fflush(stdout);
	printf("---------------------------------------------------------------\n\n");
}

void SRRdtSender::timeoutHandler(int seqNum)
{
	pns->stopTimer(SENDER, seqNum);
	pns->sendToNetworkLayer(RECEIVER, pkts[seqNum].first);
	pUtils->printPacket("packet resent", pkts[seqNum].first);
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
}
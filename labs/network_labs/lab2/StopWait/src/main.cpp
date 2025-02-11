#include "../include/Global.h"
#include "../include/RdtSender.h"
#include "../include/RdtReceiver.h"
#include "../include/StopWaitRdtSender.h"
#include "../include/StopWaitRdtReceiver.h"
#include "../include/GBNRdtSender.h"
#include "../include/GBNRdtReceiver.h"
#include "../include/SRRdtSender.h"
#include "../include/SRRdtReceiver.h"
#include "../include/TCPRdtSender.h"
#include "../include/TCPRdtReceiver.h"

int main()
{
#ifdef GBN
	auto *ps = new GBNRdtSender(4, 3);
	auto *pr = new GBNRdtReceiver(3);
	printf("-*- This is GBN -*-\n\n");
#elif SR
	auto *ps = new SRRdtSender(4, 3);
	auto *pr = new SRRdtReceiver(4, 3);
	printf("-*- This is SR -*-\n\n");
#elif TCP
	auto *ps = new TCPRdtSender(4, 3);
	auto *pr = new TCPRdtReceiver(3);
	printf("-*- This is TCP -*-\n\n");
#else
	auto *ps = new StopWaitRdtSender();
	auto *pr = new StopWaitRdtReceiver();
	printf("-*- This is StopWait -*-\n\n");
#endif
	// pns->setRunMode(1);  //安静模式
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile("/home/peacewang/sources/network/lab2/input.txt");
	pns->setOutputFile("/home/peacewang/sources/network/lab2/output.txt");
	pns->start();
	delete ps;
	delete pr;
	delete pUtils; //指向唯一的工具类实例，只在main函数结束前delete
	delete pns; //指向唯一的模拟网络环境类实例，只在main函数结束前delete
	return 0;
}

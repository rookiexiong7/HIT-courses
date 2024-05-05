#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <stdlib.h>
#include <WinSock2.h>
#include <time.h>
#pragma comment(lib,"ws2_32.lib")
#define SERVER_PORT 12340 //接收数据的端口号
#define SERVER_IP "127.0.0.1" // 服务器的 IP 地址
const int BUFFER_LENGTH = 1026;
const int SEQ_SIZE = 32;//接收端序列号个数，为 1~20
const int SEQ_WINDOW_SIZE = 8;  //接收窗口大小

int recv_base = 0;

/****************************************************************/
/*
	-time 从服务器端获取当前时间
	-quit 退出客户端
	-testgbn [X] 测试 GBN 协议实现可靠数据传输
	[X] [0,1] 模拟数据包丢失的概率
	[Y] [0,1] 模拟 ACK 丢失的概率
*/
/****************************************************************/
void printTips() {
	printf("-----------------------------------------\n");
	printf("| -time to get current time |\n");
	printf("| -quit to exit client |\n");
	printf("| -testsr [X] [Y] to test the sr |\n");
	printf("-----------------------------------------\n");
}


//************************************
// Method: lossInLossRatio
// FullName: lossInLossRatio
// Access: public 
// Returns: BOOL
// Qualifier: 根据丢失率随机生成一个数字，判断是否丢失,丢失则返回TRUE，否则返回 FALSE
// Parameter: float lossRatio [0,1]
//************************************
BOOL lossInLossRatio(float lossRatio) {
	int lossBound = (int)(lossRatio * 100);
	int r = rand() % 101;
	if (r <= lossBound) {
		return TRUE;
	}
	return FALSE;
}


int main(int argc, char* argv[])
{
	//加载套接字库（必须）
	WORD wVersionRequested;
	WSADATA wsaData;
	//套接字加载时错误提示
	int err;
	//版本 2.2
	wVersionRequested = MAKEWORD(2, 2);
	//加载 dll 文件 Scoket 库
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		//找不到 winsock.dll
		printf("WSAStartup failed with error: %d\n", err);
		return 1;
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
	}
	else {
		printf("The Winsock 2.2 dll was found okay\n");
	}
	SOCKET socketClient = socket(AF_INET, SOCK_DGRAM, 0);
	SOCKADDR_IN addrServer;
	addrServer.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	//addrServer.sin_addr.S_un.S_addr = inet_pton(SERVER_IP)；
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(SERVER_PORT);
	//接收缓冲区
	char buffer[BUFFER_LENGTH] = { 0 };
	char recvMessage[1024 * 120];
	char cache[1024 * SEQ_WINDOW_SIZE];
	bool recvack[SEQ_SIZE] = { 0 };
	ZeroMemory(cache, sizeof(cache));
	ZeroMemory(recvMessage, sizeof(recvMessage));
	ZeroMemory(buffer, sizeof(buffer));
	int len = sizeof(SOCKADDR);
	//为了测试与服务器的连接，可以使用 -time 命令从服务器端获得当前时间
	//使用 -testgbn [X] [Y] 测试 GBN 其中[X]表示数据包丢失概率
	// [Y]表示 ACK 丢包概率
	printTips();
	int ret;
	int interval = 1;//收到数据包之后返回 ack 的间隔，默认为 1 表示每个都返回 ack，0 或者负数均表示所有的都不返回 ack
	char cmd[128] = { 0 };
	float packetLossRatio = 0.2; //默认包丢失率 0.2
	float ackLossRatio = 0.2; //默认 ACK 丢失率 0.2
	//用时间作为随机种子，放在循环的最外面
	srand((unsigned)time(NULL));
	int recvNum = 0;
	while (true) {
		gets_s(buffer);
		ret = sscanf(buffer, "%s%f%f", &cmd, &packetLossRatio, &ackLossRatio);
		//开始 GBN 测试，使用 GBN 协议实现 UDP 可靠文件传输
		if (!strcmp(cmd, "-testsr")) {
			printf("%s\n", "Begin to test SR protocol, please don't abort the process");
			printf("The loss ratio of packet is %.2f,the loss ratio of ack is % .2f\n", packetLossRatio, ackLossRatio);
			int waitCount = 0;
			int stage = 0;
			recv_base = 0;
			BOOL b;
			unsigned char u_code;//状态码
			unsigned short seq;//包的序列号
			unsigned short recvSeq;//接收窗口大小为 1，已确认的序列号
			unsigned short waitSeq;//等待的序列号
			sendto(socketClient, "-testsr", strlen("-testsr") + 1, 0, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
			while (true)
			{
				//等待 server 回复设置 UDP 为阻塞模式
				int recvSize = recvfrom(socketClient, buffer, BUFFER_LENGTH, 0, (SOCKADDR*)&addrServer, &len);
				//服务器状态传输完成状态码 255
				if ((unsigned char)buffer[0] == 245) {
					printf("| 数据接收成功, 接受的数据为 |\n");
					printf("当前接受窗口起始为：%d\n", recv_base);
					printf("%s\n", recvMessage);
					ZeroMemory(recvMessage, sizeof(recvMessage));
					recvNum = 0;
					break;
				}
				switch (stage) {
				case 0://等待握手阶段
					u_code = (unsigned char)buffer[0];
					if ((unsigned char)buffer[0] == 205)
					{
						printf("Ready for file transmission\n");
						buffer[0] = 200;
						buffer[1] = '\0';
						sendto(socketClient, buffer, 2, 0, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
						stage = 1;
						recvSeq = 0;
						waitSeq = 1;
					}
					break;
				case 1://等待接收数据阶段
					seq = (unsigned short)buffer[0];
					//随机法模拟包是否丢失
					b = lossInLossRatio(packetLossRatio);
					if (b) {
						printf("The packet with a seq of %d loss\n", seq);
						continue;
					}
					printf("recv a packet with a seq of %d\n", seq);
					// 接受数据包
					memcpy(&recvMessage[seq * 1024], &buffer[1], strlen(&buffer[1]));
					
					buffer[0] = seq;
					buffer[1] = '\0';
					//如果收到预期消息，接收窗口右移
					if (recv_base == seq) {
						recvack[seq] = false;
						recv_base++;
						for (int i = recv_base; i < recv_base + SEQ_WINDOW_SIZE; i++) {
							int m = i % SEQ_SIZE;
							if (recvack[m]) {
								recvack[m] = false;
								recv_base++;
							}	
						}
						recv_base %= SEQ_SIZE;
					}
						
					else {
						recvack[seq] = true;
					}
					recv_base %= SEQ_SIZE;
					b = lossInLossRatio(ackLossRatio);
					if (b) {
						printf("The ack of %d loss\n", (unsigned char)buffer[0]);
						continue;
					}
					sendto(socketClient, buffer, 2, 0, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
					printf("send a ack of %d\n", (unsigned char)buffer[0]);
					printf("当前接受窗口起始为：%d\n", recv_base);
					break;
				}
				Sleep(500);
			}
		}
		sendto(socketClient, buffer, strlen(buffer) + 1, 0, (SOCKADDR*)&addrServer, len);
		ret = recvfrom(socketClient, buffer, BUFFER_LENGTH, 0, (SOCKADDR*)&addrServer, &len);
		printf("%s\n", buffer);
		if (!strcmp(buffer, "Good bye!")) {
			break;
		}
		printTips();
	}
	//关闭套接字
	closesocket(socketClient);
	WSACleanup();
	return 0;
}

#include "stdafx.h"
#include "cClient.h"

Client::Client()
{
}


Client::~Client()
{
	closesocket(g_sock);//家南 摧扁    
	WSACleanup();//扩加 秦力拳
}

void Client::Setup()
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);//扩加 檬扁拳	
	g_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//家南 积己

	if (g_sock == -1)
	{
		printf("家南 积己 角菩...\n");
		return;
	}

	SOCKADDR_IN servaddr = { 0 };//家南 林家
	servaddr.sin_family = PF_INET;
	servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	servaddr.sin_port = htons(PORT_NUM);

	int re = 0;
	re = connect(g_sock, (struct sockaddr *)&servaddr, sizeof(servaddr));//楷搬 夸没
	
																	   
	if (re == -1)
	{
		printf("家南 楷搬 角菩...\n");
		return;
	}
	else
	{
		_beginthread(RecvThreadPoint, 0, (void *)g_sock);
		printf("楷搬 己傍\n");
	}
}

void Client::RecvThreadPoint(void * param)
{
	SOCKET sock = (SOCKET)param;
	char msg[MAX_MSG_LEN];

	SOCKADDR_IN cliaddr = { 0 };
	int len = sizeof(cliaddr);

	while (recv(sock, msg, sizeof(msg), 0) > 0)
	{
        EnterCriticalSection(&g_csMSG);
		//g_pMsgManager->PushWaitingQueue(msg);
		g_pMsgManager->PushScheduleList(msg);
        LeaveCriticalSection(&g_csMSG);
		//string aa = g_pMsgManager->GetMsg();
		//printf("%s\n", aa.c_str());
		//g_pMsgManager->testPrint();
	}
	closesocket(sock);
}
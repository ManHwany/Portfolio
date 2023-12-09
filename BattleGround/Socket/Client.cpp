#include "stdafx.h"
#include "cClient.h"

Client::Client()
{
}


Client::~Client()
{
	closesocket(g_sock);//���� �ݱ�    
	WSACleanup();//���� ����ȭ
}

void Client::Setup()
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);//���� �ʱ�ȭ	
	g_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//���� ����

	if (g_sock == -1)
	{
		printf("���� ���� ����...\n");
		return;
	}

	SOCKADDR_IN servaddr = { 0 };//���� �ּ�
	servaddr.sin_family = PF_INET;
	servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	servaddr.sin_port = htons(PORT_NUM);

	int re = 0;
	re = connect(g_sock, (struct sockaddr *)&servaddr, sizeof(servaddr));//���� ��û
	
																	   
	if (re == -1)
	{
		printf("���� ���� ����...\n");
		return;
	}
	else
	{
		_beginthread(RecvThreadPoint, 0, (void *)g_sock);
		printf("���� ����\n");
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
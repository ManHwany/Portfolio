#pragma once

#define PORT_NUM      8080//23000 // 10200
//#define SERVER_IP	  "211.106.191.161"
#define SERVER_IP     "127.0.0.1" //�׽�Ʈ IP �ּ�

class Client 
{
private:
	//SOCKET sock;
	char msg[MAX_MSG_LEN] = "";
	
public:
	Client();
	~Client();
	void Setup();

	// ������ �ְ�ޱ�
	static void RecvThreadPoint(void * param);
};
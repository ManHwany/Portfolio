#include "common.h"


#define NAME_NUM	  4
#define PORT_NUM      8080
#define BLOG_SIZE     5
#define MAX_MSG_LEN   256

#define PLAYER_ID	  0
#define NORMALOBJECT_ID 1000
#define SPECIALOBJECT_ID 10000

struct ID_Info {
	int PlayerID;
	string Name;
};

list<ID_Info> g_lIDList;
map<int, bool> m_mReadyCheck;
map<int, bool> m_mGameReadyFinish;
CRITICAL_SECTION cs;

const string nameList[NAME_NUM] =
{
	"����ȯ��",
	"��������",
	"��������",
	"�蹮�о�"
};


bool nameUse[NAME_NUM] =
{
	false,
};


SOCKET SetTCPServer(short pnum, int blog);	//��� ���� ����
void EventLoop(SOCKET sock);				//Event ó�� Loop

int P_ID = PLAYER_ID;
int NO_ID = NORMALOBJECT_ID;
int SO_ID = SPECIALOBJECT_ID;


// : >> �ڷᱸ��
list<string> m_sTotalSendMSG;
vector<string> m_scheduleQueue;
// : <<

int main()
{
	InitializeCriticalSection(&cs);
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);//���� �ʱ�ȭ	
	SOCKET sock = SetTCPServer(PORT_NUM, BLOG_SIZE);//��� ���� ����

	if (sock == -1)
	{
		perror("��� ���� ����");
	}
	else
	{
		EventLoop(sock);

	}
	WSACleanup();//���� ����ȭ

	return 0;
}
SOCKET SetTCPServer(short pnum, int blog)
{
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//���� ����
	if (sock == -1)
	{
		return -1;
	}

	SOCKADDR_IN servaddr = { 0 };//���� �ּ�
	servaddr.sin_family = AF_INET;
	//servaddr.sin_addr = GetDefaultMyIP();
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT_NUM);

	int re = 0;
	//���� �ּҿ� ��Ʈ��ũ �������̽� ����
	re = bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (re == -1)
	{
		return -1;
	}

	re = listen(sock, blog);//�� �α� ť ����
	if (re == -1)
	{
		return -1;
	}
	return sock;
}

SOCKET  sock_base[FD_SETSIZE];
HANDLE hev_base[FD_SETSIZE];
int cnt;

HANDLE AddNetworkEvent(SOCKET sock, long net_event)
{
	HANDLE hev = WSACreateEvent();

	sock_base[cnt] = sock;
	hev_base[cnt] = hev;
	cnt++;

	WSAEventSelect(sock, hev, net_event);
	return hev;
}

void AcceptProc(int index);
void ReadProc(int index);
void CloseProc(int index);

void EventLoop(SOCKET sock)
{
	AddNetworkEvent(sock, FD_ACCEPT);

	while (true)
	{
		//send(sock_base[1], "s", sizeof(1), 0);
		int index = WSAWaitForMultipleEvents(cnt, hev_base, false, INFINITE, false);

		WSANETWORKEVENTS net_events;
		WSAEnumNetworkEvents(sock_base[index], hev_base[index], &net_events);

		switch (net_events.lNetworkEvents)
		{
		case FD_ACCEPT: AcceptProc(index); break;
		case FD_READ: ReadProc(index); break;
		case FD_CLOSE: CloseProc(index); break;
		}


	}
	closesocket(sock);//���� �ݱ�


}



void AcceptProc(int index)
{
	SOCKADDR_IN cliaddr = { 0 };
	int len = sizeof(cliaddr);
	SOCKET dosock = accept(sock_base[0], (SOCKADDR *)&cliaddr, &len);

	if (cnt == FD_SETSIZE)
	{
		printf("ä�� �濡 �� ���� %s:%d �������� ���Ͽ���!\n",
			inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
		closesocket(dosock);
		return;
	}

	AddNetworkEvent(dosock, FD_READ | FD_CLOSE);

	//##########################################################
	// : >>  �������ڸ��� �����ڿ��� �ε�����ȣ, �̸� �����ֱ�


	char indexCh[MAX_MSG_LEN] = { "" };
	int playerID = cnt - 1;


	string name;
	while (true)
	{
		int nRandom = rand() % NAME_NUM;
		if (nameUse[nRandom]) continue;

		nameUse[nRandom] = true;
		name = nameList[nRandom];
		break;
	}

	sprintf(indexCh, "%d%s", playerID, name.c_str());
	send(sock_base[playerID], indexCh, sizeof(indexCh), 0);


	// : <<
	printf("%d's player connect\n", playerID);

	ID_Info info;
	info.Name = name;
	info.PlayerID = playerID;
	g_lIDList.push_back(info);
	m_mReadyCheck[playerID] = false;
	m_mGameReadyFinish[playerID] = false;

	//���� ���� join �޼��� ����
	auto it = g_lIDList.back();
	for (int i = 1; i < cnt - 1; i++)
	{
		printf("%d <- player %d connect\n", i, it.PlayerID);
		string MSG = "Join:" + to_string(it.PlayerID) + ":" + it.Name + "/";
		send(sock_base[i], MSG.c_str(), strlen(MSG.c_str()) + 1, 0);
	}

	//join�� ��󿡰Դ� ��� ������ ����
	string MSG;
	printf("===============================\n");
	for (auto listIter = g_lIDList.begin(); listIter != g_lIDList.end(); listIter++)
	{
		if (it.PlayerID == listIter->PlayerID) break;

		printf("%d <- player %d connect\n", cnt - 1, listIter->PlayerID);
		MSG += "Join:" + to_string(listIter->PlayerID) + ":" + listIter->Name + "/";
	}
	printf("===============================\n");
	if (!MSG.empty())
	{
		send(sock_base[cnt - 1], MSG.c_str(), strlen(MSG.c_str()) + 1, 0);
	}

	// : <<

}

void ReadProc(int index)
{


	int ID = 0;
	int K_N;
	char msg[MAX_MSG_LEN] = { "" };

	recv(sock_base[index], msg, MAX_MSG_LEN, 0);

	// >> :


	SOCKADDR_IN cliaddr = { 0 };
	int len = sizeof(cliaddr);
	getpeername(sock_base[index], (SOCKADDR *)&cliaddr, &len);
	// ������ �޽����� ���� ����
	char smsg[MAX_MSG_LEN];

	// << :
	char command = msg[0];
	// >> :

	// - ID ���� -
	// 0 ~ 999 : Player
	// 1000 ~ 1999 : Normal object
	// 2000 ~ 2999 : Special object

	// - Kinds of Number -
	// 100 : Player
	// 101 ~ 199 : Static Object
	// 200 ~ 299 : Dynamic Object

	// MSG ��� // " command / type / ID / KeyInput / vp , vr / : "

	// >> : 

	// c/index/p/K.N/0/0/0

	// c �� ���, ID �� K.N �Ҵ����ֱ�

	string sTotalMSG(msg);
	string sSendTotalMSG;

	if (sTotalMSG.empty()) {
		printf("error empty : �� �޽��� �ϳ� �Խ��ϴ�\n");
		return;
	}
	if (sTotalMSG.length() >= sTotalMSG.max_size()) {
		printf("error full : �� �� �޽��� �ϳ� �Խ��ϴ�\n");
		printf("%s\n", sTotalMSG.c_str());
	}
	string sMsg(msg);

	vector<string> vecS = split(sMsg, '/');
	vector<string> vecP;
	for (size_t i = 0; i < vecS.size(); ++i)
	{
		vecP = split(vecS[i], ':');
	}

	// > : ���»����� ���̾����� ������ ����
	if (vecP.size() <= 0) return;
	
	if (vecP.front() == "GameReset") {
		m_mReadyCheck.clear();
		g_lIDList.clear();
	}
	else if (vecP.front() == "Ready")
	{
		m_mReadyCheck[stoi(vecP[1])] = true;

		int check = 0;
		for each(auto it in m_mReadyCheck)
		{
			if (it.second) check++;
		}

		//��ü ����
		if (check == m_mReadyCheck.size())
		{
			string msg = "Start:" + string("/");
			for (int i = 1; i < cnt; ++i)
			{
				send(sock_base[i], msg.c_str(), sizeof(msg), 0);
			}
		}
	}
	else if (vecP.front() == "Cancel")
	{
		m_mReadyCheck[stoi(vecP[1])] = false;

		int check = 0;
		for each(auto it in m_mReadyCheck)
		{
			if (it.second) check++;
		}

		if (check != m_mReadyCheck.size())
		{
			string msg = "Stop:" + string("/");
			for (int i = 1; i < cnt; ++i)
			{
				send(sock_base[i], msg.c_str(), sizeof(msg), 0);
			}
		}
	}
	else if (vecP.front() == "GameSetting")
	{
		string msg = "Stop:" + string("/");
		for (int i = 1; i < cnt; ++i)
		{
			send(sock_base[i], msg.c_str(), sizeof(msg), 0);
		}
	}

	else if (vecP.front() == "GameReadyFinish")
	{
		m_mGameReadyFinish[stoi(vecP[1])] = true;

		int check = 0;
		for each(auto it in m_mGameReadyFinish)
		{
			if (it.second) check++;
		}

		//��ü ���ΰ��� �ε� �Ϸ�
		if (check == m_mGameReadyFinish.size())
		{
			string msg = "GameReadyFinish:" + string("/");
			for (int i = 1; i < cnt; ++i)
			{
				send(sock_base[i], msg.c_str(), sizeof(msg), 0);
			}
		}
	}

	else {
		// >> : ���� �Ľ�
		vector<string> vecSentence = split(sTotalMSG, '/');

		for (size_t i = 0; i < vecSentence.size(); ++i) {
			if (vecSentence[i].empty()) continue;
			sSendTotalMSG.clear();
			vector<string> vecPart = split(vecSentence[i], ':');
			if (vecPart.size() < 2) continue;

			//if (vecPart.front() != "Cr" || vecPart.front() != "De" || vecPart.front() != "Key" || vecPart.front() != "St" || vecPart.front() != "Die") continue;

			if (vecPart.front() == "Cr") {
				string sCreate;
				// Create / Ctype / PlayerID / K_N / EntityID / Category / PV / RV
				sCreate += vecPart[0] + ":";

				char cType;
				int nPlayerID;
				int nKindNum;
				int nEntityID;

				// split(vecPart[i], ',');

				// >> : CType �Ľ�
				if (vecPart[1] == "P") {
					cType = 'p';
				}
				else if (vecPart[1] == "SO") {
					cType = 's';
				}
				else if (vecPart[1] == "NO") {
					cType = 'n';
				}
				// <<

				sCreate += vecPart[1] + ":";

				// >> : Player ID
				nPlayerID = stoi(vecPart[2]);
				// <<
				sCreate += vecPart[2] + ":";

				// >> : K_N 
				nKindNum = stoi(vecPart[3]);
				// <<
				sCreate += vecPart[3] + ":";
				// >> : E_ID 
				nEntityID = stoi(vecPart[4]);

				if (cType == 'p') nEntityID = P_ID++;
				else if (cType == 'n') 	nEntityID = NO_ID++;
				else if (cType == 's') nEntityID = SO_ID++;

				sCreate += to_string(nEntityID) + ":";

				sCreate += vecPart[5] + ":";
				sCreate += vecPart[6] + ":";
				sCreate += vecPart[7] + ":";

				sSendTotalMSG += sCreate + "/";

				for (int i = 1; i<cnt; i++) {

					send(sock_base[i], sSendTotalMSG.c_str(), sizeof(smsg), 0);

				}
			}
			else {
				sSendTotalMSG += vecSentence[i] + "/";

				for (int i = 1; i<cnt; i++) {

					send(sock_base[i], sSendTotalMSG.c_str(), sizeof(smsg), 0);

				}
			}

		}

	}
}


void CloseProc(int index)
{
	SOCKADDR_IN cliaddr = { 0 };
	int len = sizeof(cliaddr);
	getpeername(sock_base[index], (SOCKADDR *)&cliaddr, &len);
	// >> : �������� ������ �տ� ���̸� �ٲ��ش�

	// >> : 1. Id Change Message
	string MSG;
	MSG = "Change:" + to_string(index) + "/";
	send(sock_base[cnt - 1], MSG.c_str(), MAX_MSG_LEN, 0);

	//����
	m_mReadyCheck.erase(index);


	// >> : 2. List Id Change 
	int tempIndex = index;
	string tempName;
	for (auto iter = g_lIDList.begin(); iter != g_lIDList.end();)
	{
		if (iter->PlayerID == index)
		{
			tempName = iter->Name;
			iter = g_lIDList.erase(iter);
		}
		else ++iter;
	}
	// >> : 3. Logout Id delete in list
	// >> : �������� ������ �տ� ���̸� �ٲ��ش�

	for (auto iter = g_lIDList.begin(); iter != g_lIDList.end(); ++iter)
	{
		if (iter->PlayerID == cnt - 1)
		{
			iter->PlayerID = tempIndex;
			iter->Name = tempName;
			break;
		}
	}

	// >> : send to Logout
	string Msg = "Logout:" + to_string(index) + string("/");
	for (int i = 1; i<cnt; i++)
	{
		send(sock_base[i], Msg.c_str(), strlen(Msg.c_str()) + 1, 0);
	}

	cnt--;
	closesocket(sock_base[index]);
	WSACloseEvent(hev_base[index]);
	sock_base[index] = sock_base[cnt];
	hev_base[index] = hev_base[cnt];
}
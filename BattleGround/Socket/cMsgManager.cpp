#include "stdafx.h"
#include "cMsgManager.h"

//원형큐
cMsgManager::cMsgManager()
{
	//원형큐
	m_scheduleQueue.resize(QUEUE_SIZE);
	m_front = m_rear = 0;
}

cMsgManager::~cMsgManager()
{
	m_scheduleQueue.clear();
}

void cMsgManager::PushScheduleQueue(char * msg)
{
	//원형큐
	//현재 위치에 데이터 추가
	m_scheduleQueue[m_rear] = string(msg);
	
	//rear 증가
	m_rear = (m_rear + 1) % QUEUE_SIZE;

	if (m_rear == m_front) printf("queue is Full\n.");

}

void cMsgManager::PushScheduleList(string msg) 
{
    //원형큐
    //현재 위치에 데이터 추가
    m_scheduleList.push_back(msg);
}

string cMsgManager::GetMsg()
{
	//원형큐
	//비었을 때

    if (m_front == m_rear) 
	{

        return "";
    }
	//front 값 전부 가져오기
	string temp = m_scheduleQueue[m_front];
	m_scheduleQueue[m_front] = "";

	//front 값 증가
	m_front = (m_front + 1) % QUEUE_SIZE;

	return temp;
}

string cMsgManager::GetMsgList() 
{
    //원형큐
    //비었을 때

    if (m_scheduleList.empty()) 
	{

        return "";
    }
    //front 값 전부 가져오기
    EnterCriticalSection(&g_csMSG);
    //g_pMsgManager->PushWaitingQueue(msg);
    string temp = m_scheduleList.front();
    m_scheduleList.pop_front();
    LeaveCriticalSection(&g_csMSG);

    return temp;
}

string cMsgManager::GetTotalMSGList() 
{
    string sTotalMSG;

    while (!m_scheduleList.empty()) 
	{
        string& sMsg = GetMsgList();

        if (sMsg.empty()) { continue; }

        sTotalMSG += sMsg;
    }

    return sTotalMSG;
}

void cMsgManager::testPrint()
{
	if (m_front == m_rear) return;
	printf("%s\n", m_scheduleQueue[m_front].c_str());
}

void cMsgManager::Destroy()
{
	m_scheduleList.clear();
}

string cMsgManager::GetTotalMSG() 
{
    string sTotalMSG;

    while (m_front != m_rear) 
	{
        string sMsg = GetMsg();

        if (sMsg.empty()) { continue; }
        if (sMsg.length() >= 1000) { continue; }       

		//printf("%s\n", sMsg.c_str());

        sTotalMSG += sMsg;
    }

    return sTotalMSG;
}

void cMsgManager::SendMSG() 
{
    if (m_sTotalSendMSG.empty()) 
	{ 
		return; 
	}
    if (m_sTotalSendMSG.front().empty()) 
	{ 
        m_sTotalSendMSG.pop_front();
        return;
    }
    send(g_sock, m_sTotalSendMSG.front().c_str(), m_sTotalSendMSG.front().length() + 1, 0);

    m_sTotalSendMSG.pop_front();
    // >> : 메세지 버퍼 비우기
}

void cMsgManager::PushSendMSG(string sMSG) 
{
    send(g_sock, sMSG.c_str(), MAX_MSG_LEN, 0);
    return;

	if (m_sTotalSendMSG.empty())
	{
		m_sTotalSendMSG.push_back(sMSG);
		return;
	} else if (m_sTotalSendMSG.back().length() >= 512) 
	{
        m_sTotalSendMSG.push_back(sMSG);
    } else 
	{
        m_sTotalSendMSG.back() += sMSG;
    }
}

void cMsgManager::SendToServer(IN string command,
                               IN string cType,
                               IN int PlayerID,
                               IN int kindNumber,
                               IN int nEntityID,
                               IN string sCategory,
                               IN vector<string>& vecInfo) 
{
    string sMSG;

    sMSG =
        command + string(":") +
        cType + string(":") +
        to_string(PlayerID) + string(":") +
        to_string(kindNumber) + string(":") +
        to_string(nEntityID) + string(":") +
        sCategory + string(":");

    for (size_t i = 0; i < vecInfo.size(); ++i) {
        sMSG += vecInfo[i];     
        sMSG += string(":");
    }

    sMSG += string("/");

    PushSendMSG(sMSG);
}
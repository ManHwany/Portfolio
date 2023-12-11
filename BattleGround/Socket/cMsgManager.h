#pragma once

#define g_pMsgManager  cMsgManager::GetInstance()  

//원형큐
#define QUEUE_SIZE 2000

class cMsgManager
{
private:
	
    list<string> m_sTotalSendMSG;

	//원형큐
	int m_front, m_rear;
	vector<string> m_scheduleQueue;
    list<string> m_scheduleList;

public:
	SINGLETONE(cMsgManager);

    static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }


    static std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        return split(s, delim, elems);
    }
	
	void PushScheduleQueue(char * msg);

	string GetMsg();
    string GetTotalMSG();

    void PushScheduleList(string msg);
    string GetMsgList();
    string GetTotalMSGList();
	void testPrint();
	void Destroy();

    void SendMSG();

    void PushSendMSG(string sMsg);

    void SendToServer(IN string command,
                      IN string cType,
                      IN int PlayerID,
                      IN int kindNumber,
                      IN int nEntityID,
                      IN string sCategory,
                      IN vector<string>& vecInfo);
};
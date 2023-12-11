#pragma once
#include "singletonBase.h"

//ini데이터 구조체
struct tagIniData
{
	tchar* section;
	tchar* key;
	tchar* value;
};

class iniData : public singletonBase <iniData>
{
private:
	typedef vector<tagIniData> arrIniData;
	typedef vector<tagIniData>::iterator arrIniDataIter;
	typedef vector<arrIniData> arrIniDatas;
	typedef vector<arrIniData>::iterator arrIniDatasIter;

private:
	arrIniDatas _vIniData;

public:
	HRESULT init();

	//데이터 추가하기(섹션,키,벨류)
	void addData(tchar* section, tchar* key, tchar* value);
	//세이브-> 파일이름만 넣어도 (파일이름.ini)파일로 자동 저장
	void iniSave(tchar* fileName);

	//문자값 로드하기(파일이름,섹션,키)
	tchar* loadDataString(tchar* fileName, tchar* section, tchar* key);
	//정수값 로드하기(파일이름,섹션,키)
	int loadDataInteger(tchar* fileName, tchar* section, tchar* key);

	iniData();
	~iniData();
};


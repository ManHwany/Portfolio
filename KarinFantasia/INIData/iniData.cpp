#include "stdafx.h"
#include "iniData.h"

iniData::iniData()
{
}

iniData::~iniData()
{
}
HRESULT iniData::init()
{
	return S_OK;
}

//데이터 추가하기(섹션,키,벨류)
void iniData::addData(tchar* section, tchar* key, tchar* value)
{
	//ini데이터 구조체 선언, 초기화
	tagIniData iniData;
	iniData.section = section;
	iniData.key = key;
	iniData.value = value;

	//ini데이터 구조체 벡터에 담기
	arrIniData vIniData;
	vIniData.push_back(iniData);

	//모든 데이터는 _vIniData에 구조체를 담을 벡터를 담기
	_vIniData.push_back(vIniData);
}

//세이브-> 파일이름만 넣어도 (파일이름.ini)파일로 자동 저장
void iniData::iniSave(tchar* fileName)
{
	tchar str[256];
	tchar dir[256];
	ZeroMemory(str, sizeof(str));
	ZeroMemory(dir, sizeof(dir));
	swprintf_s(dir, TEXT("\\%s.ini"), fileName);

	GetCurrentDirectory(256, str);
	wcscat_s(str, 256,dir);

	for (int i = 0; i < _vIniData.size(); i++)
	{
		arrIniData vData = _vIniData[i];

		WritePrivateProfileString(
			vData[0].section,		//대제목
			vData[0].key,			//키값(구성요소)
			vData[0].value,			//키값(구성요소)에 작성할 정보
			str);					//ini파일 경로
		vData.clear();
	}
	_vIniData.clear();
}

//문자값 로드하기(파일이름,섹션,키)
tchar* iniData::loadDataString(tchar* fileName, tchar* section, tchar* key)
{
	tchar str[256];
	tchar dir[256];
	ZeroMemory(str, sizeof(str));
	ZeroMemory(dir, sizeof(dir));
	swprintf_s(dir, TEXT("\\%s.ini"), fileName);

	GetCurrentDirectory(256, str);
	wcscat_s(str, 256,dir);

	tchar data[64] = { NULL };

	GetPrivateProfileString(
		section,			//대제목
		key,				//키값
		L"",					//섹션이름이나 키이름을 찾지 못하면 디폴트로 출력할 스트링
		data,				//키값에서 가르키는 값을 저장할 버퍼
		64,					//키값을 저장할 버퍼 사이즈
		str);				//ini파일 경로

	return data;
}

//정수값 로드하기(파일이름,섹션,키)
int iniData::loadDataInteger(tchar* fileName, tchar* section, tchar* key)
{
	tchar str[256];
	tchar dir[256];
	ZeroMemory(str, sizeof(str));
	ZeroMemory(dir, sizeof(dir));
	swprintf_s(dir, TEXT("\\%s.ini"), fileName);

	GetCurrentDirectory(256, str);
	wcscat_s(str, 256,dir);

	return GetPrivateProfileInt(section, key, 0, str);
}
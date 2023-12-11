#include "stdafx.h"
#include "cDoor.h"


cDoor::cDoor()
	: m_pDoor(nullptr),
	m_pDoorStatus(DOOR_CLOSE),
    m_fCurRotAngle(0.0f),
    m_fTarRotAngle(0.0f)
{
	D3DXMatrixIdentity(&matR);
    m_eEntityKind = eEntityKind::ENTITY_DOOR;
}


cDoor::~cDoor()
{
	SAFE_DELETE(m_pDoor);
}

void cDoor::Setup()
{
	D3DXMATRIXA16 matT, matR;
	D3DXMatrixIdentity(&matR);
	D3DXMatrixIdentity(&matT);

	D3DXMatrixTranslation(&matT, m_vPosition.x, m_vPosition.y, m_vPosition.z);
	D3DXMatrixRotationY(&matR, m_vRotAngle.y);
	
	m_vOriginRot = m_vRotAngle;

    m_fCurRotAngle = m_vOriginRot.y;
    m_fTarRotAngle = m_vOriginRot.y;
    D3DXMatrixIdentity(&m_matLocalR);
    m_vDir = { 0, 0, 1 };

	m_matWorld = matR * matT;

	D3DXVec3TransformNormal(&m_vDir, &m_vDir, &m_matWorld);
	D3DXVec3Normalize(&m_vDir, &m_vDir);
	

	m_pDoor = new cStaticSkinnedMesh("Xfiles/Door/", "Door.x");
	cOBB a;
	a.Setup(m_pDoor,"door", 0.5f, 0.5f, 1.0f);
	a.Update((D3DXMATRIXA16*)&(m_matWorld));
	m_vecOBB.push_back(a);
}


void cDoor::OBB_Setup()
{
}

void cDoor::Update(float dt)
{
	float fDeltaAngle = m_fTarRotAngle - m_fCurRotAngle;
	fDeltaAngle *= 0.05f;
	m_fCurRotAngle += fDeltaAngle;

	D3DXMATRIXA16 matT, matLocalR;
	D3DXMatrixIdentity(&matLocalR);
	D3DXMatrixIdentity(&matR);
	D3DXMatrixIdentity(&matT);
	D3DXMatrixTranslation(&matT, m_vPosition.x, m_vPosition.y, m_vPosition.z);
	D3DXMatrixRotationY(&matLocalR, m_fCurRotAngle);

	m_matWorld = m_matLocalR * matLocalR  * matT;
	for (int i = 0; i < m_vecOBB.size(); ++i) {
	    m_vecOBB[i].Update((D3DXMATRIXA16*)&(m_matWorld));

	}
}

void cDoor::Render()
{
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	D3DXMATRIXA16 matIdentity;
	D3DXMatrixIdentity(&matIdentity);

	if (m_pDoor) 
	{
		g_pD3DDevice->SetTransform(D3DTS_WORLD, &m_matWorld);
		g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		m_pDoor->UpdateAndRender();
		RenderOBB();
	}
}

void cDoor::RenderOBB()
{
	for (int i = 0; i < m_vecOBB.size(); ++i)
	{
		m_vecOBB[i].OBBBox_Render(D3DCOLOR_XRGB(0, 255, 0));
	}
}

bool cDoor::HandleMessage(const Telegram & telegram)
{
	if (telegram.key == '*')
	{
		D3DXVECTOR3 PlayerDir;
		PlayerDir = m_vPosition - D3DXVECTOR3(stof(telegram.vecExtraInfo[0]), 
											  stof(telegram.vecExtraInfo[1]), 
											  stof(telegram.vecExtraInfo[2]));
											  
		// 문연다
		if (m_pDoorStatus == eDoorStatus::DOOR_CLOSE) 
		{
			if (D3DXVec3Dot(&m_vDir, &PlayerDir) > 0)
			{
				m_pDoorStatus = eDoorStatus::DOOR_FRONT_OPEN;
                m_fTarRotAngle = m_vOriginRot.y - D3DX_PI / 2.0f;

				
			}
			else 
			{
				m_pDoorStatus = eDoorStatus::DOOR_BACK_OPEN;
                m_fTarRotAngle = m_vOriginRot.y + D3DX_PI / 2.0f;
			}
		}
		// 문 닫는다
        else 
		{			
            m_pDoorStatus = eDoorStatus::DOOR_CLOSE;
            m_fTarRotAngle = m_vOriginRot.y;
        }
	}
	return true;
}

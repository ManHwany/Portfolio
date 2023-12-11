#pragma once
#include "cGameObject.h"

enum eDoorStatus {
	DOOR_FRONT_OPEN,
	DOOR_BACK_OPEN,
	DOOR_CLOSE
};

class cDoor :
	public cGameObject
{
public:
	cDoor();
	~cDoor();

	void Setup() override;
	void OBB_Setup() override;
	void Update(float dt) override;
	void Render() override;
	void RenderOBB();

	virtual bool HandleMessage(const Telegram& telegram);

private:
	eDoorStatus			m_pDoorStatus;
	D3DXVECTOR3			m_vDir;
	D3DXVECTOR3			m_vOriginRot;
	D3DXMATRIXA16		matR;
    D3DXMATRIXA16       m_matLocalR;
	float				m_fCurRotAngle;
    float               m_fTarRotAngle;

	SYNTHESIZE(cStaticSkinnedMesh*, m_pDoor, Door);
};


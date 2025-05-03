#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "Player.h"
#include "Scene.h"
#include "GameObject.h"

class TitleScene : public CScene
{
public:
	TitleScene();
	virtual ~TitleScene();

private:
	CGameObject ** m_ppObjects = NULL;

	CWallsObject * m_pWallsObject = NULL;


public:
	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);
};


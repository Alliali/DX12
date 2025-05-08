#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "SceneManager.h"


class TitleScene : public SceneManager
{
public:
	TitleScene(CPlayer* pPlayer);
	virtual ~TitleScene();

private:
	CGameObject ** m_ppObjects = NULL;

	CWallsObject * m_pWallsObject = NULL;

	CExplosiveObject* m_pExplosiveObject = NULL;

public:
	void BuildObjects() override;
	void Animate(float fElapsedTime) override;
	void Render(HDC hDCFrameBuffer, CCamera* pCamera) override;
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;

	void OnObjectByCursorCollision(CGameObject* pObject) override;

	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera);

};


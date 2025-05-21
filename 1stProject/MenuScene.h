#pragma once

#include "GameObject.h"
#include "Camera.h"
#include "SceneManager.h"

class MenuScene : public SceneManager
{
public:
	MenuScene(CPlayer* pPlayer);
	virtual ~MenuScene();

private:
	int			m_nObjects = 0;
	CGameObject** m_ppObjects = NULL;

	CWallsObject* m_pWallsObject = NULL;

	CExplosiveObject* m_pExplosiveObject = NULL;

public:
	// SceneManager을(를) 통해 상속됨
	void BuildObjects() override;
	void Animate(float fElapsedTime) override;
	void Render(HDC hDCFrameBuffer, CCamera* pCamera) override;
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) override;
	void OnObjectByCursorCollision(CGameObject* pObject) override;
	CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera) override;
};


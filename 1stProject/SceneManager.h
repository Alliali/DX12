#pragma once
#include "stdafx.h"

class CPlayer;
class CCamera;
class CGameObject;
class CGameFramework;

class SceneManager
{
protected:
	CPlayer* m_pPlayer = nullptr;
	CGameFramework* m_pFramework = nullptr;

public:
	SceneManager(CPlayer* pPlayer) : m_pPlayer(pPlayer) {}
	virtual ~SceneManager()	{}

	void SetFramework(CGameFramework* pFramework) { m_pFramework = pFramework; }
	CGameFramework* GetFramework() { return m_pFramework; }

	virtual void BuildObjects() = 0;
	virtual void Animate(float fElapsedTime) = 0;
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera) = 0;
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) = 0;
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) = 0;
	virtual CGameObject* PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera) = 0;
};


#include "stdafx.h"
#include "TitleScene.h"
#include "GraphicsPipeline.h"
#include "GameFramework.h"

TitleScene::TitleScene(CPlayer* pPlayer) : SceneManager(pPlayer)
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::BuildObjects()
{
	CExplosiveObject::PrepareExplosion();

	float fHalfWidth = 45.0f, fHalfHeight = 45.0f, fHalfDepth = 200.0f;
	CWallMesh* pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 30);

	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetPosition(0.0f, 0.0f, 0.0f);
	m_pWallsObject->SetMesh(pWallCubeMesh);
	m_pWallsObject->SetColor(RGB(0, 0, 0));
	m_pWallsObject->m_pxmf4WallPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmf4WallPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmf4WallPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth);
	m_pWallsObject->m_pxmf4WallPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth);
	m_pWallsObject->m_xmOOBBPlayerMoveCheck = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth * 0.05f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	CCubeMesh* pCubeMesh = new CCubeMesh(20.0f, 4.0f, 4.0f);

	m_ppObjects = new CGameObject * [2];

	m_ppObjects[0] = new CExplosiveObject();
	m_ppObjects[0]->SetMesh(pCubeMesh);
	m_ppObjects[0]->SetColor(RGB(255, 0, 0));
	m_ppObjects[0]->SetPosition(0.0f, 0.0f, 14.0f);
	m_ppObjects[0]->SetRotationAxis(XMFLOAT3(1.0f, 0.0f, 0.0f));
	m_ppObjects[0]->SetRotationSpeed(90.0f);
	m_ppObjects[0]->SetNextScene(SceneType::Menu);

	NameMesh* pNameMesh = new NameMesh(4.0f, 4.0f, 4.0f);
	m_ppObjects[1] = new CGameObject();
	m_ppObjects[1]->SetMesh(pNameMesh);
	m_ppObjects[1]->SetColor(RGB(255, 0, 0));
	m_ppObjects[1]->SetPosition(-10.0f, 0.0f, 14.0f);
	m_ppObjects[1]->SetRotationAxis(XMFLOAT3(1.0f, 0.0f, 0.0f));
	m_ppObjects[1]->SetRotationSpeed(90.0f);

	m_pExplosiveObject = static_cast<CExplosiveObject*>(m_ppObjects[0]);

#ifdef _WITH_DRAW_AXIS
	m_pWorldAxis = new CGameObject();
	CAxisMesh* pAxisMesh = new CAxisMesh(0.5f, 0.5f, 0.5f);
	m_pWorldAxis->SetMesh(pAxisMesh);
#endif
}

void TitleScene::Animate(float fElapsedTime)
{
	m_pWallsObject->Animate(fElapsedTime);
	m_ppObjects[1]->Animate(fElapsedTime);
	m_ppObjects[0]->Animate(fElapsedTime);

	if (m_pExplosiveObject->m_bBlowingUp) {
		m_pExplosiveObject->Animate(fElapsedTime);
		if (m_pExplosiveObject->IsExplosionFinished()) {
			m_pFramework->ChangeScene(m_pExplosiveObject->GetNextScene());
		}
	}
}

void TitleScene::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);

	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);
	m_pWallsObject->Render(hDCFrameBuffer, pCamera);
	m_ppObjects[0]->Render(hDCFrameBuffer, pCamera);
	m_ppObjects[1]->Render(hDCFrameBuffer, pCamera);

#ifdef _WITH_DRAW_AXIS
	CGraphicsPipeline::SetViewOrthographicProjectTransform(&pCamera->m_xmf4x4ViewOrthographicProject);
	m_pWorldAxis->SetRotationTransform(&m_pPlayer->m_xmf4x4World);
	m_pWorldAxis->Render(hDCFrameBuffer, pCamera);
#endif
}

void TitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

}

void TitleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void TitleScene::OnObjectByCursorCollision(CGameObject* pObject)
{
	if (pObject == m_ppObjects[0]) {
		m_pExplosiveObject->m_bBlowingUp = true;
	}
}

CGameObject* TitleScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / (float)pCamera->m_Viewport.m_nWidth) - 1) / pCamera->m_xmf4x4PerspectiveProject._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / (float)pCamera->m_Viewport.m_nHeight) - 1) / pCamera->m_xmf4x4PerspectiveProject._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&pCamera->m_xmf4x4View);

	int nIntersected = 0;
	float fNearestHitDistance = FLT_MAX;
	CGameObject* pNearestObject = NULL;
	
		float fHitDistance = FLT_MAX;
		nIntersected = m_ppObjects[0]->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance);
		if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance))
		{
			fNearestHitDistance = fHitDistance;
			pNearestObject = m_ppObjects[0];
		}
	
	return(pNearestObject);
}
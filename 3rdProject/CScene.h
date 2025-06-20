#pragma once
#include "CGameTimer.h"
#include "CShader.h"

class CScene
{
public:
	CScene();
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseObjects();

	bool ProcessInput(UCHAR* pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	void ReleaseUploadBuffers();

	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }

//그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature();

protected:
////씬은 셰이더들의 집합이다. 셰이더들은 게임 객체들의 집합이다.
//	CShader** m_ppShaders = NULL;
//	int m_nShaders = 0;
	//씬은 게임 객체들의 집합이다. 게임 객체는 셰이더를 포함한다.

	/*CGameObject **m_ppObjects = NULL;
	int m_nObjects = 0;*/
	//따라하기 11번삭제 객체별로 셰이더를 설정하는것이 아닌 셰이더객체별로 메쉬와 객체를 설정하기 위함
	
//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다.
	CObjectsShader *m_pShaders = NULL;
	int m_nShaders = 0;

	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;
	//루트 시그너처를 나타내는 인터페이스 포인터이다.

	CHeightMapTerrain* m_pTerrain = NULL;	//따라하기 15 추가
};


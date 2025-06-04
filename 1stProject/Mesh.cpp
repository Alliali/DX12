#include "stdafx.h"
#include "Mesh.h"
#include "GraphicsPipeline.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPolygon::CPolygon(int nVertices)
{
	m_nVertices = nVertices;
	m_pVertices = new CVertex[nVertices];
}

CPolygon::~CPolygon()
{
	if (m_pVertices) delete[] m_pVertices;
}

void CPolygon::SetVertex(int nIndex, CVertex& vertex)
{
	if ((0 <= nIndex) && (nIndex < m_nVertices) && m_pVertices)
	{
		m_pVertices[nIndex] = vertex;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMesh::CMesh(int nPolygons)
{
	m_nPolygons = nPolygons;
	m_ppPolygons = new CPolygon*[nPolygons];
}

CMesh::~CMesh()
{
	if (m_ppPolygons)
	{
		for (int i = 0; i < m_nPolygons; i++) if (m_ppPolygons[i]) delete m_ppPolygons[i];
		delete[] m_ppPolygons;
	}
}

void CMesh::SetPolygon(int nIndex, CPolygon *pPolygon)
{
	if ((0 <= nIndex) && (nIndex < m_nPolygons)) m_ppPolygons[nIndex] = pPolygon;
}

void Draw2DLine(HDC hDCFrameBuffer, XMFLOAT3& f3PreviousProject, XMFLOAT3& f3CurrentProject)
{
	XMFLOAT3 f3Previous = CGraphicsPipeline::ScreenTransform(f3PreviousProject);
	XMFLOAT3 f3Current = CGraphicsPipeline::ScreenTransform(f3CurrentProject);
	::MoveToEx(hDCFrameBuffer, (long)f3Previous.x, (long)f3Previous.y, NULL);
	::LineTo(hDCFrameBuffer, (long)f3Current.x, (long)f3Current.y);
}

void CMesh::Render(HDC hDCFrameBuffer)
{
	XMFLOAT3 f3InitialProject, f3PreviousProject;
	bool bPreviousInside = false, bInitialInside = false, bCurrentInside = false, bIntersectInside = false;

	for (int j = 0; j < m_nPolygons; j++)
	{
		int nVertices = m_ppPolygons[j]->m_nVertices;
		CVertex* pVertices = m_ppPolygons[j]->m_pVertices;

		f3PreviousProject = f3InitialProject = CGraphicsPipeline::Project(pVertices[0].m_xmf3Position);
		bPreviousInside = bInitialInside = (-1.0f <= f3InitialProject.x) && (f3InitialProject.x <= 1.0f) && (-1.0f <= f3InitialProject.y) && (f3InitialProject.y <= 1.0f);
		for (int i = 1; i < nVertices; i++)
		{
			XMFLOAT3 f3CurrentProject = CGraphicsPipeline::Project(pVertices[i].m_xmf3Position);
			bCurrentInside = (-1.0f <= f3CurrentProject.x) && (f3CurrentProject.x <= 1.0f) && (-1.0f <= f3CurrentProject.y) && (f3CurrentProject.y <= 1.0f);
			if (((0.0f <= f3CurrentProject.z) && (f3CurrentProject.z <= 1.0f)) && ((bCurrentInside || bPreviousInside))) ::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
			f3PreviousProject = f3CurrentProject;
			bPreviousInside = bCurrentInside;
		}
		if (((0.0f <= f3InitialProject.z) && (f3InitialProject.z <= 1.0f)) && ((bInitialInside || bPreviousInside))) ::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3InitialProject);
	}
}

BOOL CMesh::RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance)
{
	float fHitDistance;
	BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
	if (bIntersected && (fHitDistance < *pfNearHitDistance)) *pfNearHitDistance = fHitDistance;

	return(bIntersected);
}

int CMesh::CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance)
{
	int nIntersections = 0;
	bool bIntersected = m_xmOOBB.Intersects(xmvPickRayOrigin, xmvPickRayDirection, *pfNearHitDistance);
	if (bIntersected)
	{
		for (int i = 0; i < m_nPolygons; i++)
		{
			switch (m_ppPolygons[i]->m_nVertices)
			{
			case 3:
			{
				XMVECTOR v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				XMVECTOR v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[1].m_xmf3Position));
				XMVECTOR v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				break;
			}
			case 4:
			{
				XMVECTOR v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				XMVECTOR v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[1].m_xmf3Position));
				XMVECTOR v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				v0 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[0].m_xmf3Position));
				v1 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[2].m_xmf3Position));
				v2 = XMLoadFloat3(&(m_ppPolygons[i]->m_pVertices[3].m_xmf3Position));
				bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
				if (bIntersected) nIntersections++;
				break;
			}
			}
		}
	}
	return(nIntersections);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMesh::CCubeMesh(float fWidth, float fHeight, float fDepth) : CMesh(6)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;

	CPolygon *pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(0, pFrontFace);

	CPolygon *pTopFace = new CPolygon(4);
	pTopFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pTopFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	SetPolygon(1, pTopFace);

	CPolygon *pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(2, pBackFace);

	CPolygon *pBottomFace = new CPolygon(4);
	pBottomFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBottomFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(3, pBottomFace);

	CPolygon *pLeftFace = new CPolygon(4);
	pLeftFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pLeftFace->SetVertex(1, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(4, pLeftFace);

	CPolygon *pRightFace = new CPolygon(4);
	pRightFace->SetVertex(0, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pRightFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(5, pRightFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CWallMesh::CWallMesh(float fWidth, float fHeight, float fDepth, int nSubRects) : CMesh((4 * nSubRects * nSubRects) + 2)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;
	float fCellWidth = fWidth * (1.0f / nSubRects);
	float fCellHeight = fHeight * (1.0f / nSubRects);
	float fCellDepth = fDepth * (1.0f / nSubRects);

	int k = 0;
	CPolygon* pLeftFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pLeftFace = new CPolygon(4);
			pLeftFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pLeftFace->SetVertex(1, CVertex(-fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pLeftFace);
		}
	}

	CPolygon* pRightFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pRightFace = new CPolygon(4);
			pRightFace->SetVertex(0, CVertex(+fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pRightFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + (j * fCellDepth)));
			pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight + ((i + 1) * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight + (i * fCellHeight), -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pRightFace);
		}
	}

	CPolygon* pTopFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pTopFace = new CPolygon(4);
			pTopFace->SetVertex(0, CVertex(-fHalfWidth + (i * fCellWidth), +fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pTopFace->SetVertex(1, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), +fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pTopFace->SetVertex(2, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), +fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			pTopFace->SetVertex(3, CVertex(-fHalfWidth + (i * fCellWidth), +fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pTopFace);
		}
	}

	CPolygon* pBottomFace;
	for (int i = 0; i < nSubRects; i++)
	{
		for (int j = 0; j < nSubRects; j++)
		{
			pBottomFace = new CPolygon(4);
			pBottomFace->SetVertex(0, CVertex(-fHalfWidth + (i * fCellWidth), -fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pBottomFace->SetVertex(1, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), -fHalfHeight, -fHalfDepth + (j * fCellDepth)));
			pBottomFace->SetVertex(2, CVertex(-fHalfWidth + ((i + 1) * fCellWidth), -fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			pBottomFace->SetVertex(3, CVertex(-fHalfWidth + (i * fCellWidth), -fHalfHeight, -fHalfDepth + ((j + 1) * fCellDepth)));
			SetPolygon(k++, pBottomFace);
		}
	}

	CPolygon* pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(k++, pFrontFace);

	CPolygon* pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(k++, pBackFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth, fHalfHeight, fHalfDepth), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAirplaneMesh::CAirplaneMesh(float fWidth, float fHeight, float fDepth) : CMesh(24)
{
	float fx = fWidth*0.5f, fy = fHeight*0.5f, fz = fDepth*0.5f;

	float x1 = fx * 0.2f, y1 = fy * 0.2f, x2 = fx * 0.1f, y3 = fy * 0.3f, y2 = ((y1 - (fy - y3)) / x1)*x2 + (fy - y3);
	int i = 0;

	//Upper Plane
	CPolygon *pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(2, CVertex(0.0f, 0.0f, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(+fx, -y3, -fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, -fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, -fz));
	SetPolygon(i++, pFace);

	//Lower Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(0.0f, 0.0f, +fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(0.0f, 0.0f, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(-fx, -y3, +fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, +fz));
	SetPolygon(i++, pFace);

	//Right Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(1, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(2, CVertex(+x2, +y2, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(2, CVertex(+x2, +y2, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x2, +y2, -fz));
	pFace->SetVertex(1, CVertex(+x2, +y2, +fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+fx, -y3, -fz));
	pFace->SetVertex(1, CVertex(+x2, +y2, +fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, +fz));
	SetPolygon(i++, pFace);

	//Back/Right Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(+fx, -y3, -fz));
	pFace->SetVertex(2, CVertex(+fx, -y3, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(+fx, -y3, +fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, -fz));
	pFace->SetVertex(2, CVertex(+x1, -y1, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(+x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(0.0f, 0.0f, +fz));
	SetPolygon(i++, pFace);

	//Left Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(0.0f, +(fy + y3), -fz));
	pFace->SetVertex(2, CVertex(-x2, +y2, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, +(fy + y3), +fz));
	pFace->SetVertex(1, CVertex(-x2, +y2, -fz));
	pFace->SetVertex(2, CVertex(-x2, +y2, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(-x2, +y2, -fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x2, +y2, +fz));
	pFace->SetVertex(1, CVertex(-fx, -y3, -fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, +fz));
	SetPolygon(i++, pFace);

	//Back/Left Plane
	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(0.0f, 0.0f, +fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(0.0f, 0.0f, -fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(-x1, -y1, -fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(-x1, -y1, +fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, +fz));
	SetPolygon(i++, pFace);

	pFace = new CPolygon(3);
	pFace->SetVertex(0, CVertex(-x1, -y1, -fz));
	pFace->SetVertex(1, CVertex(-fx, -y3, +fz));
	pFace->SetVertex(2, CVertex(-fx, -y3, -fz));
	SetPolygon(i++, pFace);

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fx, fy, fz), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CAxisMesh::CAxisMesh(float fWidth, float fHeight, float fDepth) : CMesh(3)
{
	float fHalfWidth = fWidth * 0.5f;
	float fHalfHeight = fHeight * 0.5f;
	float fHalfDepth = fDepth * 0.5f;

	CPolygon* pxAxis = new CPolygon(2);
	pxAxis->SetVertex(0, CVertex(-fHalfWidth, 0.0f, 0.0f));
	pxAxis->SetVertex(1, CVertex(+fHalfWidth, 0.0f, 0.0f));
	SetPolygon(0, pxAxis);

	CPolygon* pyAxis = new CPolygon(2);
	pyAxis->SetVertex(0, CVertex(0.0f, -fHalfWidth, 0.0f));
	pyAxis->SetVertex(1, CVertex(0.0f, +fHalfWidth, 0.0f));
	SetPolygon(1, pyAxis);

	CPolygon* pzAxis = new CPolygon(2);
	pzAxis->SetVertex(0, CVertex(0.0f, 0.0f, -fHalfWidth));
	pzAxis->SetVertex(1, CVertex(0.0f, 0.0f, +fHalfWidth));
	SetPolygon(2, pzAxis);
}

void CAxisMesh::Render(HDC hDCFrameBuffer)
{
	XMFLOAT3 f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[0]->m_pVertices[0].m_xmf3Position);
	XMFLOAT3 f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[0]->m_pVertices[1].m_xmf3Position);
	HPEN hPen = ::CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);

	f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[1]->m_pVertices[0].m_xmf3Position);
	f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[1]->m_pVertices[1].m_xmf3Position);
	hPen = ::CreatePen(PS_SOLID, 0, RGB(0, 0, 255));
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);

	f3PreviousProject = CGraphicsPipeline::Project(m_ppPolygons[2]->m_pVertices[0].m_xmf3Position);
	f3CurrentProject = CGraphicsPipeline::Project(m_ppPolygons[2]->m_pVertices[1].m_xmf3Position);
	hPen = ::CreatePen(PS_SOLID, 0, RGB(0, 255, 0));
	hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
	::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
	::SelectObject(hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
}

StartMesh::StartMesh(float fWidth, float fHeight, float fDepth) : CMesh(16)
{
	CPolygon* pFace = nullptr;
	int i = 0;
	float unit = 1.0f;
	float thickness = unit * 0.3f;
	float depth = fDepth * 0.5f;
	float spacing = unit * 1.4f;

	// ���� A�� x=0�� �������� ��� ���� ����
	float startX = -2.0f * spacing;

	// === S ===
	{
		float x = startX;
		// Top bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + 0, unit, -depth));
		pFace->SetVertex(1, CVertex(x + unit, unit, -depth));
		pFace->SetVertex(2, CVertex(x + unit, unit - thickness, -depth));
		pFace->SetVertex(3, CVertex(x + 0, unit - thickness, -depth));
		SetPolygon(i++, pFace);

		// Middle bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + 0, 0.5f * unit + thickness * 0.5f, -depth));
		pFace->SetVertex(1, CVertex(x + unit, 0.5f * unit + thickness * 0.5f, -depth));
		pFace->SetVertex(2, CVertex(x + unit, 0.5f * unit - thickness * 0.5f, -depth));
		pFace->SetVertex(3, CVertex(x + 0, 0.5f * unit - thickness * 0.5f, -depth));
		SetPolygon(i++, pFace);

		// Bottom bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + 0, 0, -depth));
		pFace->SetVertex(1, CVertex(x + unit, 0, -depth));
		pFace->SetVertex(2, CVertex(x + unit, thickness, -depth));
		pFace->SetVertex(3, CVertex(x + 0, thickness, -depth));
		SetPolygon(i++, pFace);

		// Left top vertical
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + 0, unit, -depth));
		pFace->SetVertex(1, CVertex(x + thickness, unit, -depth));
		pFace->SetVertex(2, CVertex(x + thickness, 0.5f * unit + thickness * 0.5f, -depth));
		pFace->SetVertex(3, CVertex(x + 0, 0.5f * unit + thickness * 0.5f, -depth));
		SetPolygon(i++, pFace);

		// Right bottom vertical
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + unit - thickness, 0.5f * unit - thickness * 0.5f, -depth));
		pFace->SetVertex(1, CVertex(x + unit, 0.5f * unit - thickness * 0.5f, -depth));
		pFace->SetVertex(2, CVertex(x + unit, 0, -depth));
		pFace->SetVertex(3, CVertex(x + unit - thickness, 0, -depth));
		SetPolygon(i++, pFace);
	}

	// === T ===
	{
		float x = startX + spacing;
		// Top bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + 0, unit, -depth));
		pFace->SetVertex(1, CVertex(x + unit, unit, -depth));
		pFace->SetVertex(2, CVertex(x + unit, unit - thickness, -depth));
		pFace->SetVertex(3, CVertex(x + 0, unit - thickness, -depth));
		SetPolygon(i++, pFace);

		// Middle vertical
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + 0.5f * unit - thickness * 0.5f, unit, -depth));
		pFace->SetVertex(1, CVertex(x + 0.5f * unit + thickness * 0.5f, unit, -depth));
		pFace->SetVertex(2, CVertex(x + 0.5f * unit + thickness * 0.5f, 0, -depth));
		pFace->SetVertex(3, CVertex(x + 0.5f * unit - thickness * 0.5f, 0, -depth));
		SetPolygon(i++, pFace);
	}

	// === A ===
	{
		float x = startX + spacing * 2;
		// Left slant
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + 0, 0, -depth));
		pFace->SetVertex(1, CVertex(x + thickness, 0, -depth));
		pFace->SetVertex(2, CVertex(x + 0.5f * unit, unit, -depth));
		pFace->SetVertex(3, CVertex(x + 0.5f * unit - thickness, unit, -depth));
		SetPolygon(i++, pFace);

		// Right slant
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + 0.5f * unit + thickness, unit, -depth));
		pFace->SetVertex(1, CVertex(x + 0.5f * unit, unit, -depth));
		pFace->SetVertex(2, CVertex(x + unit - thickness, 0, -depth));
		pFace->SetVertex(3, CVertex(x + unit, 0, -depth));
		SetPolygon(i++, pFace);

		// Middle bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + thickness, 0.5f * unit, -depth));
		pFace->SetVertex(1, CVertex(x + unit - thickness, 0.5f * unit, -depth));
		pFace->SetVertex(2, CVertex(x + unit - thickness, 0.5f * unit + thickness, -depth));
		pFace->SetVertex(3, CVertex(x + thickness, 0.5f * unit + thickness, -depth));
		SetPolygon(i++, pFace);
	}

	// === R ===
	{
		float x = startX + spacing * 3;
		// Vertical bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + 0, 0, -depth));
		pFace->SetVertex(1, CVertex(x + thickness, 0, -depth));
		pFace->SetVertex(2, CVertex(x + thickness, unit, -depth));
		pFace->SetVertex(3, CVertex(x + 0, unit, -depth));
		SetPolygon(i++, pFace);

		// Top horizontal
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + thickness, unit, -depth));
		pFace->SetVertex(1, CVertex(x + unit, unit, -depth));
		pFace->SetVertex(2, CVertex(x + unit, unit - thickness, -depth));
		pFace->SetVertex(3, CVertex(x + thickness, unit - thickness, -depth));
		SetPolygon(i++, pFace);

		// Middle horizontal
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + thickness, 0.5f * unit, -depth));
		pFace->SetVertex(1, CVertex(x + unit, 0.5f * unit, -depth));
		pFace->SetVertex(2, CVertex(x + unit, 0.5f * unit + thickness, -depth));
		pFace->SetVertex(3, CVertex(x + thickness, 0.5f * unit + thickness, -depth));
		SetPolygon(i++, pFace);

		// Diagonal leg
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + thickness, 0.5f * unit, -depth));
		pFace->SetVertex(1, CVertex(x + unit, 0, -depth));
		pFace->SetVertex(2, CVertex(x + unit - thickness, 0, -depth));
		pFace->SetVertex(3, CVertex(x + thickness, 0.5f * unit - thickness, -depth));
		SetPolygon(i++, pFace);
	}

	// === T (������) ===
	{
		float x = startX + spacing * 4;
		// Top bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + 0, unit, -depth));
		pFace->SetVertex(1, CVertex(x + unit, unit, -depth));
		pFace->SetVertex(2, CVertex(x + unit, unit - thickness, -depth));
		pFace->SetVertex(3, CVertex(x + 0, unit - thickness, -depth));
		SetPolygon(i++, pFace);

		// Middle vertical
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(x + 0.5f * unit - thickness * 0.5f, unit, -depth));
		pFace->SetVertex(1, CVertex(x + 0.5f * unit + thickness * 0.5f, unit, -depth));
		pFace->SetVertex(2, CVertex(x + 0.5f * unit + thickness * 0.5f, 0, -depth));
		pFace->SetVertex(3, CVertex(x + 0.5f * unit - thickness * 0.5f, 0, -depth));
		SetPolygon(i++, pFace);
	}
}

NameMesh::NameMesh(float fWidth, float fHeight, float fDepth) : CMesh(26) // 9���� ���� �� ��� 4~5�� polygon
{
	CPolygon* pFace = nullptr;
	int i = 0;
	float unit = 2.0f; // �������� 2�� Ű�� ũ��
	float thickness = unit * 0.3f;
	float depth = fDepth * 0.5f;

	float spacing = unit * 1.2f;
	float offsetX = 0;

	// 'L'
	{
		float baseX = offsetX;
		// Vertical bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX, 0, -depth));
		pFace->SetVertex(1, CVertex(baseX + thickness, 0, -depth));
		pFace->SetVertex(2, CVertex(baseX + thickness, unit, -depth));
		pFace->SetVertex(3, CVertex(baseX, unit, -depth));
		SetPolygon(i++, pFace);

		// Bottom bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX, 0, -depth));
		pFace->SetVertex(1, CVertex(baseX + unit, 0, -depth));
		pFace->SetVertex(2, CVertex(baseX + unit, thickness, -depth));
		pFace->SetVertex(3, CVertex(baseX, thickness, -depth));
		SetPolygon(i++, pFace);

		offsetX += spacing;
	}

	// 'e'
	{
		float baseX = offsetX;
		// Horizontal bars (top, middle, bottom)
		for (int j = 0; j < 3; j++) {
			float y = unit - j * unit * 0.5f;
			pFace = new CPolygon(4);
			pFace->SetVertex(0, CVertex(baseX, y, -depth));
			pFace->SetVertex(1, CVertex(baseX + unit, y, -depth));
			pFace->SetVertex(2, CVertex(baseX + unit, y - thickness, -depth));
			pFace->SetVertex(3, CVertex(baseX, y - thickness, -depth));
			SetPolygon(i++, pFace);
		}
		// Left vertical bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX, 0, -depth));
		pFace->SetVertex(1, CVertex(baseX + thickness, 0, -depth));
		pFace->SetVertex(2, CVertex(baseX + thickness, unit, -depth));
		pFace->SetVertex(3, CVertex(baseX, unit, -depth));
		SetPolygon(i++, pFace);

		offsetX += spacing;
	}

	// �� ��° 'e' (���� ���)
	{
		float baseX = offsetX;
		// Horizontal bars (top, middle, bottom)
		for (int j = 0; j < 3; j++) {
			float y = unit - j * unit * 0.5f;
			pFace = new CPolygon(4);
			pFace->SetVertex(0, CVertex(baseX, y, -depth));
			pFace->SetVertex(1, CVertex(baseX + unit, y, -depth));
			pFace->SetVertex(2, CVertex(baseX + unit, y - thickness, -depth));
			pFace->SetVertex(3, CVertex(baseX, y - thickness, -depth));
			SetPolygon(i++, pFace);
		}
		// Left vertical bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX, 0, -depth));
		pFace->SetVertex(1, CVertex(baseX + thickness, 0, -depth));
		pFace->SetVertex(2, CVertex(baseX + thickness, unit, -depth));
		pFace->SetVertex(3, CVertex(baseX, unit, -depth));
		SetPolygon(i++, pFace);

		offsetX += spacing;
	}

	// 'y'
	{
		float baseX = offsetX;

		// ��� �¿� �밢��
		pFace = new CPolygon(4); // Left diagonal
		pFace->SetVertex(0, CVertex(baseX, unit, -depth));
		pFace->SetVertex(1, CVertex(baseX + thickness, unit, -depth));
		pFace->SetVertex(2, CVertex(baseX + unit * 0.5f + thickness, unit * 0.5f, -depth));
		pFace->SetVertex(3, CVertex(baseX + unit * 0.5f, unit * 0.5f, -depth));
		SetPolygon(i++, pFace);

		pFace = new CPolygon(4); // Right diagonal
		pFace->SetVertex(0, CVertex(baseX + unit - thickness, unit, -depth));
		pFace->SetVertex(1, CVertex(baseX + unit, unit, -depth));
		pFace->SetVertex(2, CVertex(baseX + unit * 0.5f + thickness, unit * 0.5f, -depth));
		pFace->SetVertex(3, CVertex(baseX + unit * 0.5f, unit * 0.5f, -depth));
		SetPolygon(i++, pFace);

		// Bottom vertical
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX + unit * 0.5f, 0, -depth));
		pFace->SetVertex(1, CVertex(baseX + unit * 0.5f + thickness, 0, -depth));
		pFace->SetVertex(2, CVertex(baseX + unit * 0.5f + thickness, unit * 0.5f, -depth));
		pFace->SetVertex(3, CVertex(baseX + unit * 0.5f, unit * 0.5f, -depth));
		SetPolygon(i++, pFace);

		offsetX += spacing;
	}

	// 'u'
	{
		float baseX = offsetX;

		// Left bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX, thickness, -depth));
		pFace->SetVertex(1, CVertex(baseX + thickness, thickness, -depth));
		pFace->SetVertex(2, CVertex(baseX + thickness, unit, -depth));
		pFace->SetVertex(3, CVertex(baseX, unit, -depth));
		SetPolygon(i++, pFace);

		// Right bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX + unit - thickness, thickness, -depth));
		pFace->SetVertex(1, CVertex(baseX + unit, thickness, -depth));
		pFace->SetVertex(2, CVertex(baseX + unit, unit, -depth));
		pFace->SetVertex(3, CVertex(baseX + unit - thickness, unit, -depth));
		SetPolygon(i++, pFace);

		// Bottom bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX + thickness, 0, -depth));
		pFace->SetVertex(1, CVertex(baseX + unit - thickness, 0, -depth));
		pFace->SetVertex(2, CVertex(baseX + unit - thickness, thickness, -depth));
		pFace->SetVertex(3, CVertex(baseX + thickness, thickness, -depth));
		SetPolygon(i++, pFace);

		offsetX += spacing;
	}

	// 'n'
	{
		float baseX = offsetX;

		// Left bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX, 0, -depth));
		pFace->SetVertex(1, CVertex(baseX + thickness, 0, -depth));
		pFace->SetVertex(2, CVertex(baseX + thickness, unit, -depth));
		pFace->SetVertex(3, CVertex(baseX, unit, -depth));
		SetPolygon(i++, pFace);

		// Right bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX + unit - thickness, 0, -depth));
		pFace->SetVertex(1, CVertex(baseX + unit, 0, -depth));
		pFace->SetVertex(2, CVertex(baseX + unit, unit, -depth));
		pFace->SetVertex(3, CVertex(baseX + unit - thickness, unit, -depth));
		SetPolygon(i++, pFace);

		// Diagonal middle bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX + thickness, unit, -depth));
		pFace->SetVertex(1, CVertex(baseX + thickness + thickness, unit, -depth));
		pFace->SetVertex(2, CVertex(baseX + unit - thickness, 0, -depth));
		pFace->SetVertex(3, CVertex(baseX + unit - thickness - thickness, 0, -depth));
		SetPolygon(i++, pFace);

		offsetX += spacing;
	}

	// 'G'
	{
		float baseX = offsetX;

		// Top bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX + thickness, unit, -depth));
		pFace->SetVertex(1, CVertex(baseX + unit, unit, -depth));
		pFace->SetVertex(2, CVertex(baseX + unit, unit - thickness, -depth));
		pFace->SetVertex(3, CVertex(baseX + thickness, unit - thickness, -depth));
		SetPolygon(i++, pFace);

		// Left vertical bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX, unit, -depth));
		pFace->SetVertex(1, CVertex(baseX + thickness, unit, -depth));
		pFace->SetVertex(2, CVertex(baseX + thickness, 0, -depth));
		pFace->SetVertex(3, CVertex(baseX, 0, -depth));
		SetPolygon(i++, pFace);

		// Bottom bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX + thickness, 0, -depth));
		pFace->SetVertex(1, CVertex(baseX + unit, 0, -depth));
		pFace->SetVertex(2, CVertex(baseX + unit, thickness, -depth));
		pFace->SetVertex(3, CVertex(baseX + thickness, thickness, -depth));
		SetPolygon(i++, pFace);

		// Inner middle bar (right)
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX + 0.5f * unit, 0.5f * unit - 0.5f * thickness, -depth));
		pFace->SetVertex(1, CVertex(baseX + unit, 0.5f * unit - 0.5f * thickness, -depth));
		pFace->SetVertex(2, CVertex(baseX + unit, 0.5f * unit + 0.5f * thickness, -depth));
		pFace->SetVertex(3, CVertex(baseX + 0.5f * unit, 0.5f * unit + 0.5f * thickness, -depth));
		SetPolygon(i++, pFace);

		offsetX += spacing;
	}

	// 'I'
	{
		float baseX = offsetX;

		// Top bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX, unit - thickness, -depth));
		pFace->SetVertex(1, CVertex(baseX + unit, unit - thickness, -depth));
		pFace->SetVertex(2, CVertex(baseX + unit, unit, -depth));
		pFace->SetVertex(3, CVertex(baseX, unit, -depth));
		SetPolygon(i++, pFace);

		// Bottom bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX, 0, -depth));
		pFace->SetVertex(1, CVertex(baseX + unit, 0, -depth));
		pFace->SetVertex(2, CVertex(baseX + unit, thickness, -depth));
		pFace->SetVertex(3, CVertex(baseX, thickness, -depth));
		SetPolygon(i++, pFace);

		// Vertical bar
		pFace = new CPolygon(4);
		pFace->SetVertex(0, CVertex(baseX + 0.5f * unit - 0.5f * thickness, thickness, -depth));
		pFace->SetVertex(1, CVertex(baseX + 0.5f * unit + 0.5f * thickness, thickness, -depth));
		pFace->SetVertex(2, CVertex(baseX + 0.5f * unit + 0.5f * thickness, unit - thickness, -depth));
		pFace->SetVertex(3, CVertex(baseX + 0.5f * unit - 0.5f * thickness, unit - thickness, -depth));
		SetPolygon(i++, pFace);

		offsetX += spacing;
	}


}



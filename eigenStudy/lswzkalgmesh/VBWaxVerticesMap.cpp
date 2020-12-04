#include "stdafx.h"
#include "VBWaxVerticesMap.h"

void VBWaxVerticesMap::WaxMap(const VSSimpleMeshF & gm1, const VSSimpleMeshF & gm2, const float & theta)
{
	InitPoint(theta, gm1, gm2);
	CreateMatrix();
	CreateKDTree(gm1, gm2);

	for (unsigned i = 0; i < 10; i++)
	{
		FindNNPoint(gm1, gm2);
		SmoothNNPoint();
		CalPoint(theta);
	}
	FindNNPoint(gm1, gm2);
	m_vOut.resize(2);
	m_vOut[0] = VD_V2CB(m_vMeshNNVertices1);
	m_vOut[1] = VD_V2CB(m_vMeshNNVertices2);
}

VFVECTOR3 VBWaxVerticesMap::MeshBarycenter(const VSSimpleMeshF & gm)
{
	VFVECTOR3 retVal(0.f, 0.f, 0.f);
	for (unsigned i = 0; i < gm.nVertCount; i++)
	{
		retVal += gm.pVertices[i];
	}

	retVal /= static_cast<float>(gm.nVertCount);

	return retVal;
}

void VBWaxVerticesMap::InitPoint(const float & theta, const VSSimpleMeshF & gm1, const VSSimpleMeshF & gm2)
{
	m_vCirclePoint.resize(num);

	VFVECTOR3 v1 = MeshBarycenter(gm1), v2 = MeshBarycenter(gm2);
	VFVECTOR3 midPoint = (v1 + v2) / 2.f, temp(0.f, 0.f, 1.f);
	float radius = 5.f;//(v1 - v2).Magnitude() / 2.f * tanf(theta);
	VFVECTOR3  axis = v1 - v2;
	axis.Normalize();
	VFVECTOR3  direct = axis.Cross(temp);
	direct.Normalize();
	VFVECTOR3 initPointDirect = radius * direct;

	m_vCirclePoint[0] = initPointDirect + midPoint;
	float delta = 2.f * VF_PI / static_cast<float>(num);
	VFVECTOR3  vtemp(0.f, 0.f, 0.f);
	for (unsigned i = 1; i < num; i++)
	{
		float vsin = sinf(static_cast<float>(i) * delta / 2.f);
		vtemp.x = axis.x * vsin;
		vtemp.y = axis.y * vsin;
		vtemp.z = axis.z * vsin;
		float w = cosf(static_cast<float>(i) * delta / 2.f);

		Quaternion  rot(w, vtemp), vert(0, initPointDirect);
		rot.normalize();
		vert = rot * vert * rot.inv();           //quaternion interpolate
		//initPoint = vert.im();
		m_vCirclePoint[i] = vert.im() + midPoint;
	}
}

void VBWaxVerticesMap::CreateKDTree(const VSSimpleMeshF & gm1, const VSSimpleMeshF & gm2)
{
	m_vMeshVert1.resize(gm1.nVertCount);
	m_vMeshVert2.resize(gm2.nVertCount);
	memcpy_s(&m_vMeshVert1.front(), sizeof(VFVECTOR3) * m_vMeshVert1.size(), gm1.pVertices, sizeof(VFVECTOR3) * m_vMeshVert1.size());
	memcpy_s(&m_vMeshVert2.front(), sizeof(VFVECTOR3) * m_vMeshVert2.size(), gm2.pVertices, sizeof(VFVECTOR3) * m_vMeshVert2.size());
	cbf1 = VD_V2CB(m_vMeshVert1);
	cbf2 = VD_V2CB(m_vMeshVert2);
	tree1.Build(out1, cbf1);
	tree2.Build(out2, cbf2);
}

void VBWaxVerticesMap::FindNNPoint(const VSSimpleMeshF & gm1, const VSSimpleMeshF & gm2)
{
	std::pair<unsigned, float>  res1, res2;
	m_vMeshNNVertices1.resize(num);
	m_vMeshNNVertices2.resize(num);
	for (unsigned i = 0; i < m_vCirclePoint.size(); i++)
	{
		knn1.Build(res1, out1, cbf1, m_vCirclePoint[i]);
		knn2.Build(res2, out2, cbf2, m_vCirclePoint[i]);

		m_vMeshNNVertices1[i] = res1.first;
		m_vMeshNNVertices2[i] = res2.first;
	}
}

void VBWaxVerticesMap::SmoothNNPoint()
{
	for (unsigned i = 0; i < num; i++)
	{
		m_vMeshSmoothVert1[i] = VFVECTOR3::ZERO;
		m_vMeshSmoothVert2[i] = VFVECTOR3::ZERO;
		for (unsigned j = 0; j < num; j++)
		{
			m_vMeshSmoothVert1[i] += m_vSmoothMatrix[i * num + j] * m_vMeshVert1[ m_vMeshNNVertices1[j] ];
			m_vMeshSmoothVert2[i] += m_vSmoothMatrix[i * num + j] * m_vMeshVert2[ m_vMeshNNVertices2[j] ];
		}
	}
}

void VBWaxVerticesMap::CalPoint(const float & theta)
{
	for (unsigned i = 0; i < num; i++)
	{
		VFVECTOR3 v1p = m_vCirclePoint[i] - m_vMeshSmoothVert1[i];
		VFVECTOR3 v1v2 = m_vMeshSmoothVert2[i] - m_vMeshSmoothVert1[i];
		v1v2.Normalize();
		float len = v1p.Dot(v1v2);
		VFVECTOR3 p = m_vMeshSmoothVert1[i] + len * v1v2;   //p点在v1 v2直线上的投影点
		VFVECTOR3 pDirect = m_vCirclePoint[i] - p;
		pDirect.Normalize();

		VFVECTOR3 midPoint = (m_vMeshSmoothVert1[i] + m_vMeshSmoothVert2[i]) / 2.f;
		len = (midPoint - m_vMeshSmoothVert1[i]).Magnitude() *tanf(theta);

		m_vCirclePoint[i] = len * pDirect + midPoint;
	}
}

void VBWaxVerticesMap::CreateMatrix()
{
	m_vMeshSmoothVert1.resize(num);
	m_vMeshSmoothVert2.resize(num);
	float val[] = { .1f, -.4f, 1.6f, -.4f, .1f };
	m_vSmoothMatrix.resize(num * num);

	for (unsigned i = 0; i < 3; i++)
	{
		m_vSmoothMatrix[i] = val[i + 2];     //第一行
		m_vSmoothMatrix[num  * num - 3 + i] = val[i];   //最后一行
	}
	m_vSmoothMatrix[num - 2] = m_vSmoothMatrix[(num - 1)* num + 1] = val[0];
	m_vSmoothMatrix[num - 1] = m_vSmoothMatrix[(num - 1)* num + 0] = val[1];

	for (unsigned i = 0; i < 4; i++)
	{
		m_vSmoothMatrix[num + i] = val[i + 1];      //第二行
		m_vSmoothMatrix[(num - 2) * num + (num - 4 + i)] = val[i];   //倒数第二行
	}
	m_vSmoothMatrix[2 * num - 1] = m_vSmoothMatrix[(num - 2) * num + 0] = val[0];

	for (unsigned i = 2; i < num - 2; i++)
		memcpy_s(&m_vSmoothMatrix[i * num + i - 2], sizeof(float) * 5, val, sizeof(float) * 5);

	VSDnsMatrixF   m_matrix, m_inverse;
	m_matrix.row = m_matrix.col = num;
	m_matrix.pData = &m_vSmoothMatrix.front();

	VBDnsMatrixInverseF   m_MatInv;
	m_MatInv.Build(m_inverse, m_matrix);

	unsigned len = m_inverse.row * m_inverse.col;
	assert(len == num * num);
	memcpy_s(&m_vSmoothMatrix.front(), sizeof(float) * len, m_inverse.pData, sizeof(float) * len);
}

//////////////////////////////////////////////////////////////////////

void VBWaxVerticesMap::WaxMapFillWax(const VSSimpleMeshF & gm1, const VSSimpleMeshF & gm2,
	const VFRay & tAxis1, const VFRay & tAxis2, const VFPlane &tSurf1, const VFPlane &tSurf2, const float & theta)
{
	VFVECTOR3 tAxisAveDir = tAxis1.GetDirection() + tAxis2.GetDirection();
	tAxisAveDir.Normalize();
	InitPointFillWax(theta, gm1, gm2, tAxisAveDir);
	CreateMatrix();
	CreateKDTree(gm1, gm2);

	for (unsigned i = 0; i < 10; i++)
	{
		FindNNPoint(gm1, gm2);
		SmoothNNPoint();
		CalPoint(theta);
	}
	FindNNPoint(gm1, gm2);
	//分别判断在牙面上方的点是否超过5个,并做优化
	OptimalPointFillWax1(gm1, tSurf1, tAxis1);
	OptimalPointFillWax2(gm2, tSurf2, tAxis2);

	m_vOut.resize(2);
	m_vOut[0] = VD_V2CB(m_vMeshNNVertices1);
	m_vOut[1] = VD_V2CB(m_vMeshNNVertices2);
}
void VBWaxVerticesMap::InitPointFillWax(const float & theta, const VSSimpleMeshF & gm1, const VSSimpleMeshF & gm2, const VFVECTOR3 & tAxisAveDir)
{
	m_vCirclePoint.resize(num);

	VFVECTOR3 v1 = MeshBarycenter(gm1), v2 = MeshBarycenter(gm2);
	VFVECTOR3 midPoint = (v1 + v2) / 2.f + 4 * (-tAxisAveDir);// , temp(0.f, 0.f, 1.f);
	float radius = 5.f;//(v1 - v2).Magnitude() / 2.f * tanf(theta);
	VFVECTOR3  axis = v1 - v2;
	axis.Normalize();
	VFVECTOR3  direct = axis.Cross(tAxisAveDir);
	direct.Normalize();
	VFVECTOR3 initPointDirect = radius * direct;

	m_vCirclePoint[0] = initPointDirect + midPoint;
	float delta = 2.f * VF_PI / static_cast<float>(num);
	VFVECTOR3  vtemp(0.f, 0.f, 0.f);
	for (unsigned i = 1; i < num; i++)
	{
		float vsin = sinf(static_cast<float>(i) * delta / 2.f);
		vtemp.x = axis.x * vsin;
		vtemp.y = axis.y * vsin;
		vtemp.z = axis.z * vsin;
		float w = cosf(static_cast<float>(i) * delta / 2.f);

		Quaternion  rot(w, vtemp), vert(0, initPointDirect);
		rot.normalize();
		vert = rot * vert * rot.inv();           //quaternion interpolate
												 //initPoint = vert.im();
		m_vCirclePoint[i] = vert.im() + midPoint;
	}
}
void VBWaxVerticesMap::OptimalPointFillWax1(const VSSimpleMeshF & gm1, const VFPlane &tSurf1, const VFRay & tAxis1)
{
	std::vector<VFVECTOR3> vCirclePoint(m_vCirclePoint);
	unsigned count = 1;
	while (1)
	{
		unsigned flag = 0;
		for (unsigned i = 0; i < num; i++)
		{
			float d = tSurf1.getDistance(gm1.pVertices[m_vMeshNNVertices1[i]]);       //
			if (d > 0)
			{
				flag++;
			}
		}
		if (flag >= 5)
		{
			for (unsigned j = 0; j < m_vCirclePoint.size(); j++)
			{
				vCirclePoint[j] = m_vCirclePoint[j] + count * (-tAxis1.GetDirection());
			}
			//findNNpoint
			std::pair<unsigned, float>  res1;
			m_vMeshNNVertices1.resize(num);
			for (unsigned i = 0; i < vCirclePoint.size(); i++)
			{
				knn1.Build(res1, out1, cbf1, vCirclePoint[i]);

				m_vMeshNNVertices1[i] = res1.first;
			}
			//smoothNNpoint
			for (unsigned i = 0; i < num; i++)
			{
				m_vMeshSmoothVert1[i] = VFVECTOR3::ZERO;
				for (unsigned j = 0; j < num; j++)
				{
					m_vMeshSmoothVert1[i] += m_vSmoothMatrix[i * num + j] * m_vMeshVert1[m_vMeshNNVertices1[j]];
				}
			}
		}
		else
		{
			break;
		}
		if (count == 4)
		{
			break;
		}
		count++;
	}

		
}
void VBWaxVerticesMap::OptimalPointFillWax2(const VSSimpleMeshF & gm2, const VFPlane &tSurf2, const VFRay & tAxis2)
{
	std::vector<VFVECTOR3> vCirclePoint(m_vCirclePoint);
	unsigned count = 1;
	while (1)
	{
		unsigned flag = 0;
		for (unsigned i = 0; i < num; i++)
		{
			float d = tSurf2.getDistance(gm2.pVertices[m_vMeshNNVertices2[i]]);       //
			if (d > 0)
			{
				flag++;
			}
		}
		if (flag >= 5)
		{
			for (unsigned j = 0; j < m_vCirclePoint.size(); j++)
			{
				vCirclePoint[j] = m_vCirclePoint[j] + count * (-tAxis2.GetDirection());
			}
			//findNNpoint
			std::pair<unsigned, float>  res2;
			m_vMeshNNVertices2.resize(num);
			for (unsigned i = 0; i < vCirclePoint.size(); i++)
			{
				knn2.Build(res2, out2, cbf2, vCirclePoint[i]);

				m_vMeshNNVertices2[i] = res2.first;
			}
			//smoothNNpoint
			for (unsigned i = 0; i < num; i++)
			{
				m_vMeshSmoothVert2[i] = VFVECTOR3::ZERO;
				for (unsigned j = 0; j < num; j++)
				{
					m_vMeshSmoothVert2[i] += m_vSmoothMatrix[i * num + j] * m_vMeshVert2[m_vMeshNNVertices2[j]];
				}
			}
		}
		else
		{
			break;
		}
		if (count == 4)
		{
			break;
		}
		count++;
	}
	
}

VD_EXPORT_SYSTEM_SIMPLE(VBWaxVerticesMap, VNALGMESH::VRWaxVerticesMap);
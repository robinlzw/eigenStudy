/*******************************************************************
 *  Copyright(c) 2017-2020 Smartee
 *  All rights reserved.
 *
 *  File Name:
 *  Author: Aries Wang
 *  Create time: 2018-03-16
 ******************************************************************/
 
#include "stdafx.h"
#include "VBAACollisionSence.h"

VBAACollisionSence::VBAACollisionSence(const VSESS< VNCollisionTest::VRMergeCollisionTest > & sysCollisionTest)
	: m_sysCollision(sysCollisionTest), m_pMergeCollision(NULL), m_blCreated(false)
{
	m_sysCollision.Build(m_essOut);

	m_essOut.pSysProv->Apply(VD_P2U(m_sysProxy));

	m_sysProxy.UseServer([this](auto & pSys) {
		m_pMergeCollision = &pSys;
	});
	
	m_hubSceneCombain.Create(*m_pMergeCollision);

	m_hubResult.Create(*m_pMergeCollision, m_hubSceneCombain);
	m_hubResult.Output(m_datResult);
}

VBAACollisionSence::~VBAACollisionSence()
{
	unsigned nSize = m_vSrcOrtho.size();
	for (unsigned i = 0; i < nSize; i++)
	{
		delete m_vSrcOrtho[i];
	}
	m_vSrcOrtho.clear();
	m_hubScenes.clear();
	m_vTeethIntervalInfo.clear();	
	//m_sysProxy.Destory();
}

void VBAACollisionSence::InitResource()
{
	unsigned nTCount = m_vMeshHubList.size();
	unsigned nSize = nTCount - 1;
	VASSERT(nSize <= 16);
	for (unsigned i = 0; i < m_vSrcOrtho.size(); i++)
	{
		delete m_vSrcOrtho[i];
	}
	m_hubScenes.resize(nSize);
	m_vSrcOrtho.resize(nSize);
	m_vTeethIntervalInfo.resize(nSize);
	for (unsigned i = 0; i < nSize; ++i)
	{
		VFMatrix4 ortho;
		INNER_WXJ::VSMeshHub* toothA = m_vMeshHubList[i].pMeshHub;
		INNER_WXJ::VSMeshHub* toothB = m_vMeshHubList[i + 1].pMeshHub;
		INNER_WXJ::VSLocSrc* locSrcA = m_vMeshHubList[i].pLocSrc;
		INNER_WXJ::VSLocSrc* locSrcB = m_vMeshHubList[i + 1].pLocSrc;
		GetOrtho(ortho, toothA, toothB, (m_vInitLocList[i]), (m_vInitLocList[i + 1]));

		m_vSrcOrtho[i] = new INNER_WXJ::VSSceneOrtho(*toothA, *toothB,
			*locSrcA, *locSrcB, *m_pMergeCollision, ortho);

		m_hubScenes[i].reset(new TVHub< VNCollisionTest::VRMergeScene >(m_hubSceneCombain,
			m_vSrcOrtho[i]->srcOrtho, m_vSrcOrtho[i]->hubObjA, m_vSrcOrtho[i]->hubObjB));
		//std::cout << "[" << i << "]: scene\n";
		//std::cout << "lid: " << m_vSrcOrtho[i]->lId.id.pData << " vcount: " << (*(*m_vSrcOrtho[i]->psrcMeshA)).nVertCount << std::endl;
		//std::cout << "rid: " << m_vSrcOrtho[i]->rId.id.pData << " vcount: " << (*(*m_vSrcOrtho[i]->psrcMeshB)).nVertCount << std::endl;
	}

	m_collisionSenceInfo.pHubResult = &m_hubResult;
	m_collisionSenceInfo.pDatResult = &m_datResult;
	m_collisionSenceInfo.pHubSceneCombain = &m_hubSceneCombain;
	m_collisionSenceInfo.pvSrcOrtho = &m_vSrcOrtho;
	m_collisionSenceInfo.pvTeethIntervalInfo = &m_vTeethIntervalInfo;
}



bool VBAACollisionSence::ResetMesh(const VSConstBuffer<VSSimpleMeshF>& cbTeeth)
{
	bool blRet = true;
	unsigned nTCount = cbTeeth.len;
	if (m_vMeshHubList.size() != nTCount)
	{
		for (size_t i = 0; i < m_vMeshHubList.size(); i++)
		{
			delete m_vMeshHubList[i].pMeshHub;
			delete m_vMeshHubList[i].pLocSrc;
		}
		m_vMeshHubList.clear();
		m_vInitLocList.clear();
	}
	if (0 == m_vMeshHubList.size())
	{
		m_vMeshHubList.resize(nTCount);
		m_vInitLocList.resize(nTCount);
		for (size_t i = 0; i < nTCount; i++)
		{
			m_vMeshHubList[i].pMeshHub = new INNER_WXJ::VSMeshHub(*m_pMergeCollision, cbTeeth.pData[i]);
			m_vInitLocList[i] = VFLocation::ORIGIN;
			m_vMeshHubList[i].pLocSrc = new INNER_WXJ::VSLocSrc(VFLocation::ORIGIN);
		}
		blRet = false;
	}
	else
	{
		for (size_t i = 0; i < nTCount; i++)
		{
			m_vMeshHubList[i].pMeshHub->Reset(cbTeeth.pData[i]);
		}
	}
	return blRet;
}
inline void VBAACollisionSence::GetOrtho(VFMatrix4& ortho,
	const INNER_WXJ::VSMeshHub* toothA, const INNER_WXJ::VSMeshHub* toothB,
	const VFLocation& stateA, const VFLocation& stateB)
{
	// 具体牙齿具体计算	
	unsigned nCount1 = (*toothA->srcMesh).nVertCount;
	unsigned nCount2 = (*toothB->srcMesh).nVertCount;
	std::vector<VFVector3> vertices1;
	std::vector<VFVector3> vertices2;
	vertices1.resize(nCount1);
	vertices2.resize(nCount2);
	VFVector3 totalV1 = VFVector3::ORIGIN;
	VFVector3 totalV2 = VFVector3::ORIGIN;
	for (unsigned i = 0; i < nCount1; i++)
	{
		vertices1[i] = stateA.TransLocalVertexToGlobal((*toothA->srcMesh).pVertices[i]);
		totalV1 += vertices1[i];
	}
	for (unsigned i = 0; i < nCount2; i++)
	{
		vertices2[i] = stateB.TransLocalVertexToGlobal((*toothB->srcMesh).pVertices[i]);
		totalV2 += vertices2[i];
	}

	VFVector3 center1 = totalV1 / ((float)nCount1);
	VFVector3 center2 = totalV2 / ((float)nCount2);
	//Smartee_WriteLog(g_pLogFile, "tooth center1: %f\t%f\t%f\n", center1.x, center1.y, center1.z);
	//Smartee_WriteLog(g_pLogFile, "tooth center2: %f\t%f\t%f\n", center2.x, center2.y, center2.z);
	VFLocation locat;
	locat.position = -((center1 + center2) / 2.0f);
	//Smartee_WriteLog(g_pLogFile, "position: %f\t%f\t%f\n", locat.position.x, locat.position.y, locat.position.z);	
	VFVector3 newZ = center2 - center1;
	//Smartee_WriteLog(g_pLogFile, "direction: %f\t%f\t%f\n", newZ.x, newZ.y, newZ.z);
	newZ.Normalize();
	VFVector3 axis = ((newZ + VFVector3(0, 0, 1.0f)) / 2);
	//Smartee_WriteLog(g_pLogFile, "orientation axis: %f\t%f\t%f\n", axis.x, axis.y, axis.z);
	axis.Normalize();
	locat.orientation = VFQuaternion::FromAxisAngle(axis, SMT_PI);
	//VFVector3 axis = ((newZ + VFVector3(0, 0, 1.0f)) / 2);
	//Smartee_WriteLog(g_pLogFile, "orientation axis: %f\t%f\t%f\n", axis.x, axis.y, axis.z);
	//axis.Normalize();
	//locat.orientation = VFQuaternion::FromAxisAngle(axis, SMT_PI);
	VFVector3 vMax(-1.0f, -1.0f, -1.0f);

	VFMatrix4 trans = VFMatrix4::CreateTrans(locat.position);
	VFMatrix3 rotate3;
	locat.orientation.ToRotationMatrix(rotate3);
	VFMatrix4 rotate(rotate3);
	VFMatrix4 stateMatr = rotate * trans;
	center1 = stateMatr *center1;
	center2 = stateMatr *center2;
	float flMax = (abs(center1.z) > abs(center2.z)) ? abs(center1.z) : abs(center2.z);
	for (unsigned i = 0; i < nCount1; i++)
	{
		vertices1[i] = (locat.orientation * (vertices1[i] + locat.position));
		//if (abs(vertices1[i].z) < flMax)
		{
			for (unsigned j = 0; j < 3; j++)
			{
				float flVal = abs(vertices1[i][j]);
				if (flVal > vMax[j])
					vMax[j] = flVal;
			}
		}
	}
	for (unsigned i = 0; i < nCount2; i++)
	{
		vertices2[i] = (locat.orientation * (vertices2[i] + locat.position));
		//if (abs(vertices2[i].z) < flMax)
		{
			for (unsigned j = 0; j < 3; j++)
			{
				float flVal = abs(vertices2[i][j]);
				if (flVal > vMax[j])
					vMax[j] = flVal;
			}
		}
	}
	for (unsigned j = 0; j < 3; j++)
	{
		vMax[j] = 1.0f / vMax[j];
	}

	VFMatrix4 scale = VFMatrix4::CreateScale(vMax);

	ortho = scale * stateMatr;
}


// 碰撞检测
void VBAACollisionSence::CollisionDetection(std::vector<std::pair<float, std::pair<VFVECTOR3, VFVECTOR3> > >& out,
	const VSConstBuffer< NMALG_TEETHPATCHING::VSTeethPatchingTopoTooth >& cbTeeth)
{
	/*
	碰撞检测方向为相邻牙齿重心连线方向，检测完之后返回相邻牙齿之间碰撞点和碰撞距离
	*/
	std::vector<VSSimpleMeshF> vMesh(cbTeeth.len);
	for (size_t i = 0; i < cbTeeth.len; i++)
	{
		vMesh[i] = cbTeeth.pData[i].topoToothMesh;
	}
	_CollisionDetection(out, VD_V2CB(vMesh));
}


// 碰撞检测
void VBAACollisionSence::CollisionDetection(std::vector<std::pair<float, std::pair<VFVECTOR3, VFVECTOR3> > >& out,
	const VSConstBuffer< NMALG_TEETHPATCHING::VSPatchedTooth >& cbTeeth)
{
	/*
	碰撞检测方向为相邻牙齿重心连线方向，检测完之后返回相邻牙齿之间碰撞点和碰撞距离
	*/
	std::vector<VSSimpleMeshF> vMesh(cbTeeth.len);
	for (size_t i = 0; i < cbTeeth.len; i++)
	{
		vMesh[i] = cbTeeth.pData[i].mesh;
	}
	_CollisionDetection(out, VD_V2CB(vMesh));
}


// 碰撞检测
inline void VBAACollisionSence::_CollisionDetection(std::vector<std::pair<float, std::pair<VFVECTOR3, VFVECTOR3> > >& out,
	const VSConstBuffer<VSSimpleMeshF>& cbTeeth)
{
	/*
	碰撞检测方向为相邻牙齿重心连线方向，检测完之后返回相邻牙齿之间碰撞点和碰撞距离
	*/
	bool blRet = ResetMesh(cbTeeth);
	if (false == blRet)
	{
		InitResource();
	}
	UpdateCollision();
	m_pMergeCollision->Elapse();

	m_collisionSenceInfo.pDatResult->Peer([this, &out](const VNCollisionTest::VSMergeResult & result) {
		unsigned nSize = (m_collisionSenceInfo.pvTeethIntervalInfo)->size();
		out.resize(nSize);
		for (unsigned i = 0; i < nSize; i++)
		{
			(*m_collisionSenceInfo.pvTeethIntervalInfo)[i].pointA = result.rslts[i].seg.mOrigin;
			(*m_collisionSenceInfo.pvTeethIntervalInfo)[i].pointB = result.rslts[i].seg.mTarget;
			VFVECTOR3 dir = result.rslts[i].seg.mTarget - result.rslts[i].seg.mOrigin;
			dir.Normalize();
			VFRay ray(result.rslts[i].seg.mOrigin, dir);
			(*m_collisionSenceInfo.pvTeethIntervalInfo)[i].flInterval = ray.GetUnitsLenth((*m_collisionSenceInfo.pvTeethIntervalInfo)[i].pointB);
			out[i].first = (*m_collisionSenceInfo.pvTeethIntervalInfo)[i].flInterval;
			out[i].second.first = result.rslts[i].seg.mOrigin;
			out[i].second.second = result.rslts[i].seg.mTarget;
		}
	});
}

inline void VBAACollisionSence::UpdateCollision()
{
	unsigned nSize = m_vMeshHubList.size();
	for (unsigned i = 0; i < nSize; i++)
	{

		INNER_WXJ::VSLocSrc* pLocSrc = m_vMeshHubList[i].pLocSrc;
		VFMatrix4 ortho = m_vInitLocList[i].ToMatrix4();
		pLocSrc->srcMatrix.Reset(ortho);
		if (i > 0)
		{
			const INNER_WXJ::VSMeshHub* pToothA = m_vMeshHubList[i - 1].pMeshHub;
			const INNER_WXJ::VSMeshHub* pToothB = m_vMeshHubList[i].pMeshHub;

			GetOrtho((*m_collisionSenceInfo.pvSrcOrtho)[i - 1]->ortho, pToothA, pToothB, m_vInitLocList[i - 1], m_vInitLocList[i]);
			(*m_collisionSenceInfo.pvSrcOrtho)[i - 1]->srcOrtho.Reset((*m_collisionSenceInfo.pvSrcOrtho)[i - 1]->ortho);
		}
	}
	m_collisionSenceInfo.pHubResult->BatchConnect(*m_collisionSenceInfo.pHubSceneCombain);
}

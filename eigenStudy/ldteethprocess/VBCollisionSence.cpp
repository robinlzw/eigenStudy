/*******************************************************************
 *  Copyright(c) 2017-2020 Smartee
 *  All rights reserved.
 *
 *  File Name:
 *  Author: Aries Wang
 *  Create time: 2018-03-16
 ******************************************************************/
 
#include "stdafx.h"
#include "VBCollisionSence.h"


// 貌似是提供了检测牙齿间距的功能。




VBCollisionSence::VBCollisionSence(const VSESS< VNCollisionTest::VRCollisionExt > & sysCollisionTest)
	: m_pMergeCollision(NULL), m_sysCollision(sysCollisionTest), m_srcOrder(5),
	m_srcMatA(MAT4_IDENTITY), m_srcMatB(MAT4_IDENTITY), m_srcMatScene(MAT4_IDENTITY),
	m_srcMeshB(VSSimpleMeshF()), m_srcMeshA(VSSimpleMeshF())	
{
	InitResource();
}



VBCollisionSence::~VBCollisionSence()
{	
}



void VBCollisionSence::Build(VSCollisionSenceResult& out, const VSSimpleMeshF& objA, const VSSimpleMeshF& objB)
{
	UpdateCollision(objA, objB);
	m_pMergeCollision->Elapse();

	std::vector<unsigned> vFlags(objA.nVertCount, VD_INVALID_INDEX);

	m_datResult.Peer([this, &out, &vFlags](const VNCollisionTest::VSExtResult & result) {
		//m_vDistancePair.resize(result.distances.len);
		m_vDistancePair.clear();
		for (size_t i = 0; i < result.distances.len; i++)
		{
			//if (VD_INVALID_INDEX == vFlags[result.distances.pData[i].meshABack])
			{
				//vFlags[result.distances.pData[i].meshABack] = 0;
				VSCollisionPointPair pair;
				pair.nMeshAPoint = result.distances.pData[i].meshABack;
				pair.nMeshBPoint = result.distances.pData[i].meshBFront;
				m_vDistancePair.push_back(pair);
			}
		}
		out.blCollision = result.bCollision;
		out.flDeepestDis = result.deepestDis;
		out.gapPointPairs = VD_V2CB(m_vDistancePair);
	});	
	
	out.dir = m_dir;
}



void VBCollisionSence::InitResource()
{
	m_sysCollision.Build(m_essOut);

	m_essOut.pSysProv->Apply(VD_P2U(m_sysProxy));

	m_sysProxy.UseServer([this](auto & pSys) {
		m_pMergeCollision = &pSys;
	});
	m_hubMeshA.Create(*m_pMergeCollision, m_srcMeshA);
	m_hubMeshB.Create(*m_pMergeCollision, m_srcMeshB);
	m_hubObjA.Create(*m_pMergeCollision, m_srcMatA, m_hubMeshA);
	m_hubObjB.Create(*m_pMergeCollision, m_srcMatB, m_hubMeshB);
	m_hubScene.Create(*m_pMergeCollision, m_srcOrder, m_srcMatScene);
	m_hubResult.Create(*m_pMergeCollision, m_hubScene, m_hubObjA, m_hubObjB);
	m_hubResult.Output(m_datResult);
}



inline void VBCollisionSence::UpdateCollision(const VSSimpleMeshF& objA, const VSSimpleMeshF& objB)
{
	m_srcMeshA.Reset(objA);
	m_srcMeshB.Reset(objB);
	VFMatrix4 ortho;
	GetOrtho(ortho, objA, objB);
	m_srcMatScene.Reset(ortho);	
}


inline void VBCollisionSence::GetOrtho(VFMatrix4& ortho,
	const VSSimpleMeshF& toothA, const VSSimpleMeshF& toothB)
{
	// 具体牙齿具体计算	
	unsigned nCount1 = toothA.nVertCount;
	unsigned nCount2 = toothB.nVertCount;
	std::vector<VFVector3> vertices1;
	std::vector<VFVector3> vertices2;
	vertices1.resize(nCount1);
	vertices2.resize(nCount2);
	VFVector3 totalV1 = VFVector3::ORIGIN;
	VFVector3 totalV2 = VFVector3::ORIGIN;
	for (unsigned i = 0; i < nCount1; i++)
	{
		vertices1[i] = toothA.pVertices[i];
		totalV1 += vertices1[i];
	}
	for (unsigned i = 0; i < nCount2; i++)
	{
		vertices2[i] = toothB.pVertices[i];
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
	m_dir = newZ;	// 保存检测方向
	VFVector3 axis = ((newZ + VFVector3(0, 0, 1.0f)) / 2);
	//Smartee_WriteLog(g_pLogFile, "orientation axis: %f\t%f\t%f\n", axis.x, axis.y, axis.z);
	axis.Normalize();
	locat.orientation = VFQuaternion::FromAxisAngle(axis, VF_PI);
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
#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <obj/alg/stdnotdef.h>
#include <pkg/ldck4cce.h>
#include <pkg/ldwzkupdater.h>
#include "lsalgteethprocess.h"

class VCToothObj
{
public:
	VCToothObj(const unsigned nToothLabel, const double dbAvgEdgeLen, IVTracer& tracer);
	VCToothObj(const unsigned nToothLabel, const double dbAvgEdgeLen, IVTracer& tracer,
		const NMALG_TEETHPATCHING::VSTeethPatchingTopoTooth& topo);
	~VCToothObj();

	void AppendKeyPoint(const VFVECTOR3& keyPoint);
	std::vector<VFVECTOR3>& GetVertRef();
	std::vector<VNVECTOR3UI>& GetSurfRef();
	std::vector<unsigned>& GetVertOnJawMeshRef();

	bool Init();
	void FindStrawHatSideVerts();
	void FindNbrBndryToPatch(const NMALG_TEETHPATCHING::VSSegmentedJaw& segmentedJaw);
	void PatchToothNbrSide();
	void PatchBottom();
	void ExtendBottomCtrlPoints();
	void SetToothAxis(const NMALG_TEETHPATCHING::VSTeethPatchingTopoTooth & topo, const NMALG_TEETHPATCHING::VSToothInfo& info);
	void MakeMatrixL();
	void MakeMatrixR();
	void SolveEquation();
	void SmoothGumLine(const double dbW = 0.1);
private:
	void Reset();	
	void UpdateMesh(const VSConstBuffer<std::vector<VFVECTOR3> >& cbVertices,
		const VSConstBuffer<std::vector<VNVECTOR3UI> >& cbSurfaces, 
		const VSConstBuffer<unsigned>& cbCtrlPoints);
	void UpdateMeshWithBtm(const VSConstBuffer<VFVECTOR3>& cbVertices,
		const VSConstBuffer<VNVECTOR3UI>& cbSurfaces,
		const VSConstBuffer<unsigned>& cbBndryLine);
	void CalcToothAxis();
	void MakeGumLineMatrix(std::vector<VSTripletD>& vMatrixL, const double dbW);
	void UpdatePerfMesh();
	void FillVertType();
	IVTracer &							m_tracer;
public:
	unsigned							m_nToothLabel;	
	double								m_dbAvgEdgeLen;
	
	std::vector<VFVECTOR3>				m_vVertices;
	std::vector<VNVECTOR3UI>			m_vSurfaces;
	std::vector<unsigned>				m_vBndryLine;			// 原始边界
	std::vector<unsigned>				m_vGumLine;			// 牙龈线边界
	std::vector<unsigned>				m_vVertOnJawMesh;		// 牙齿网格顶点在原始牙颌网格顶点的索引
	std::vector<unsigned>				m_vFixedVerts;			// 固定的顶点	
	std::vector<unsigned>				m_vCrownVerts;			// 牙冠顶点	
	std::vector<unsigned>				m_vStrawHatSideVerts;
	std::vector<unsigned>				m_vStrawHatSideVertsOnJawMesh;
	std::vector<unsigned>				m_vSideAddPoints;
	std::vector<unsigned>				m_vBtmCtrlPoints;
	unsigned							m_nBottomCenter;
	std::vector<NMALG_TEETHPATCHING::VEToothVertType>		m_vVertType;
	VSPerfectMesh						m_perfMesh;
	
	NMALG_TEETHPATCHING::VSToothAxis	m_toothAxis;
	std::vector<VFVECTOR3>				m_vNbrKeyPoints;
	std::vector<VSToothNbrEdgeLine>		m_vNbrEdgeLine;
	std::vector<VSToothNbrPatchedVert>	m_vNbrPatchedVert;

	std::vector< VSTripletD >	m_vMatrixL;
	std::shared_ptr<double>		m_pMatrixR;
	std::vector< VSTripletD >	m_vMatrixR;
	VNALGMESH::VSMeshGeodic				m_meshGeodic;
	VNALGMESH::VSMeshVertLine			m_edgeLine;
	double						m_dbW0;
	double						m_dbW1;
	double						m_dbW2;
	TVExtSource< VNALGMESH::VRPerfMesh >	m_extVRPerfMesh;
	TVExtSource< VNALGMESH::VRGeodicMesh >	m_extVRGeodicMesh;
	TVExtSource< VNALGMESH::VRSliceMeshBoundary >	m_extVRSliceMeshBndry;	
	TVExtSource< VNALGMESH::VRFindHole >	m_extVRFildHole;

	std::shared_ptr<TVR2B< VNALGMESH::VRPerfMesh > >	m_ptrVRPerfMesh;
	std::shared_ptr<TVR2B< VNALGMESH::VRGeodicMesh > >	m_ptrVRGeodicMesh;
	std::shared_ptr<TVR2B< VNALGMESH::VRSliceMeshBoundary > >	m_ptrVRSliceMeshBndry;	// 单个的洞：一圈顶点表示的边界（牙侧面、底面）
	std::shared_ptr<TVR2B< VNALGMESH::VRFindHole > >	m_ptrVRFindHole;				// 多个洞
};



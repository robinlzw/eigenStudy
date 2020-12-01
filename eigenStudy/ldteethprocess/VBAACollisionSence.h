#pragma once
#include <obj/tisysbak.h>
#include <obj/alg/ialgteethpatching.h>
#include <sys/alg/isysparallel.h>
#include "inner.h"

class VBAACollisionSence
{
public:
	VBAACollisionSence(const VSESS< VNCollisionTest::VRMergeCollisionTest > & sysCollisionTest);
	~VBAACollisionSence();
		
	void CollisionDetection(std::vector<std::pair<float, std::pair<VFVECTOR3, VFVECTOR3> > >& out, const VSConstBuffer< NMALG_TEETHPATCHING::VSTeethPatchingTopoTooth >& cbTeeth);
	void CollisionDetection(std::vector<std::pair<float, std::pair<VFVECTOR3, VFVECTOR3> > >& out, const VSConstBuffer<NMALG_TEETHPATCHING::VSPatchedTooth>& cbTeeth);
	
private:	
	void _CollisionDetection(std::vector<std::pair<float, std::pair<VFVECTOR3, VFVECTOR3> > >& out, const VSConstBuffer<VSSimpleMeshF>& cbTeeth);
	void InitResource();
	bool ResetMesh(const VSConstBuffer< VSSimpleMeshF >& cbTeeth);
	void UpdateCollision();
	void GetOrtho(VFMatrix4& ortho,
		const INNER_WXJ::VSMeshHub* toothA, const INNER_WXJ::VSMeshHub* toothB,
		const VFLocation& stateA, const VFLocation& stateB);
	
	INNER_WXJ::IVDSCollisionMerge* m_pMergeCollision;
	TVData< VNCollisionTest::VSMergeResult >	m_datResult;
	TVHub< VNCollisionTest::VRMergeResult >		m_hubResult;
	TVHub< VNCollisionTest::VRMergeSceneCombain > m_hubSceneCombain;
	std::vector< std::shared_ptr< TVHub< VNCollisionTest::VRMergeScene > > > m_hubScenes;
	std::vector<INNER_WXJ::VSSceneOrtho*> m_vSrcOrtho;
	std::vector<INNER_WXJ::VSCollisionCheckInfo>	m_vTeethIntervalInfo;	// —¿≥›º‰œ∂
	VSESS< VNCollisionTest::VRMergeResult, VNCollisionTest::VRMergeSceneCombain, VNCollisionTest::VRTestObjA,
		VNCollisionTest::VRTestObjB, VNCollisionTest::VRMesh > m_essOut;
	TVR2B< VNCollisionTest::VRMergeCollisionTest  > m_sysCollision;
	TVServiceProxy< IVSysDynamic<VNCollisionTest::VRMergeResult,
		VNCollisionTest::VRMergeSceneCombain,
		VNCollisionTest::VRTestObjA,
		VNCollisionTest::VRTestObjB,
		VNCollisionTest::VRMesh > > m_sysProxy;	


	std::vector<INNER_WXJ::VSMeshHubInfo>	m_vMeshHubList;
	std::vector<VFLocation>					m_vInitLocList;
	INNER_WXJ::VSCollisionSenceInfo			m_collisionSenceInfo;
	bool m_blCreated;
};
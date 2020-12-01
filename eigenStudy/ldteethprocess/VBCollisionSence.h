#pragma once
#include <sys/alg/isysparallel.h>
#include <obj/tisysbak.h>






// VBCollisionSence类——貌似是提供了检测牙齿间距的功能。
class VBCollisionSence
{
public:
	VBCollisionSence(const VSESS< VNCollisionTest::VRCollisionExt > & sysCollisionTest);
	~VBCollisionSence();


	struct VSCollisionPointPair
	{
		unsigned nMeshAPoint;
		unsigned nMeshBPoint;
	};
	
	
	struct VSCollisionSenceResult
	{
		VFVECTOR3 dir;
		bool blCollision;
		float flDeepestDis;	// 大于0表示碰撞
		VSConstBuffer<VSCollisionPointPair> gapPointPairs;
	};


	void Build(VSCollisionSenceResult& out,const VSSimpleMeshF& objA, const VSSimpleMeshF& objB);    

private:
	void InitResource();

private:
	using IVDSCollisionExt = IVSysDynamic< VNCollisionTest::VRExtResult, VNCollisionTest::VRTestScene,
									VNCollisionTest::VRTestObjA, VNCollisionTest::VRTestObjB, VNCollisionTest::VRMesh >;

	std::vector<VSCollisionPointPair>		m_vDistancePair;

	TVR2B< VNCollisionTest::VRCollisionExt  > m_sysCollision;

	VSESS< VNCollisionTest::VRExtResult, VNCollisionTest::VRTestScene, VNCollisionTest::VRTestObjA,
					VNCollisionTest::VRTestObjB, VNCollisionTest::VRMesh > m_essOut;

	TVServiceProxy< IVSysDynamic<VNCollisionTest::VRExtResult,
							VNCollisionTest::VRTestScene,
							VNCollisionTest::VRTestObjA,
							VNCollisionTest::VRTestObjB,
							VNCollisionTest::VRMesh > > m_sysProxy;

	IVDSCollisionExt* m_pMergeCollision;

	TVData< VNCollisionTest::VSExtResult >	m_datResult;

	TVHub< VNCollisionTest::VRExtResult >		m_hubResult;
	
	
	TVHub< VNCollisionTest::VRMesh > m_hubMesh;
	TVHub< VNCollisionTest::VRTestObjA > m_hubObjA;
	TVHub< VNCollisionTest::VRTestObjB > m_hubObjB;
	TVHub< VNCollisionTest::VRTestScene > m_hubScene;

	TVHub< VNCollisionTest::VRMesh > m_hubMeshA;
	TVHub< VNCollisionTest::VRMesh > m_hubMeshB;

	TVSource< VSSimpleMeshF >	m_srcMeshA;
	TVSource< VSSimpleMeshF >	m_srcMeshB;
	TVSource< VFMatrix4 >	m_srcMatA;
	TVSource< VFMatrix4 >	m_srcMatB;
	TVSource< VFMatrix4 >	m_srcMatScene;
	TVSource< unsigned >	m_srcOrder;				// 2 ^ n	射线对，

	VFVECTOR3		m_dir;

private:
	void UpdateCollision(const VSSimpleMeshF& objA, const VSSimpleMeshF& objB);

	void GetOrtho(VFMatrix4& ortho,	const VSSimpleMeshF& toothA, const VSSimpleMeshF& toothB);
};
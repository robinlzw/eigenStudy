#pragma once
#include <vmath/vmath.h>
#include <sys/alg/isysparallel.h>

namespace INNER_WXJ
{
#define AA_USE_INVERSE		1
#define AA_WEIGHT_FIX		100
#define AA_WEIGHT_MOVE		20
#define SMT_PI				3.1415926f

#define AA_WEIGHT_DIRWEIGHT 10.0

	using IVDSCollisionMerge = IVSysDynamic< VNCollisionTest::VRMergeResult, VNCollisionTest::VRMergeSceneCombain,
		VNCollisionTest::VRTestObjA, VNCollisionTest::VRTestObjB, VNCollisionTest::VRMesh >;



		
	struct VSToothLocCoord
	{
		VFVECTOR3 xAxis;
		VFVECTOR3 yAxis;
	};





	struct VSMeshHub
	{
		VSMeshHub(IVDSCollisionMerge & sys, const VSSimpleMeshF& mesh) : srcMesh(mesh)
		{	
			hubMesh.Create(sys, srcMesh);
		}
		
		~VSMeshHub()
		{
			hubMesh.Destory();
		}
		
		void Reset(const VSSimpleMeshF& mesh)
		{
			srcMesh.Reset(mesh);
		}
		void Destroy()
		{
			hubMesh.Destory();
		}
		TVSource< VSSimpleMeshF >	srcMesh;
		TVHub< VNCollisionTest::VRMesh > hubMesh;
	};




	struct VSLocSrc
	{
		VSLocSrc(const VFLocation& location): srcMatrix(location.ToMatrix4())
		{
		}		
		TVSource< VFMatrix4 >		srcMatrix;		
	};



	
	
	typedef struct VSSceneOrtho
	{
		VSSceneOrtho(VSMeshHub& toothA, 
			VSMeshHub& toothB,
			VSLocSrc& locA,
			VSLocSrc& locB,
			IVDSCollisionMerge& sys, const VFMatrix4& orth)
			: nReset(0), ortho(orth), srcOrtho(orth)
		{			
			psrcMeshA = &(toothA.srcMesh);
			psrcMeshB = &(toothB.srcMesh);
			
			hubObjA.Create(sys, locA.srcMatrix, toothA.hubMesh);
			hubObjB.Create(sys, locB.srcMatrix, toothB.hubMesh);
			
		}
		~VSSceneOrtho()
		{
			hubObjA.Destory();
			hubObjB.Destory();
		}
		void Reset( VSMeshHub& toothA, 
			VSMeshHub& toothB,
			VSLocSrc& locA,
			VSLocSrc& locB,
			IVDSCollisionMerge& sys, const VFMatrix4& orth)
		{
			nReset = 0;
			ortho = orth;
			srcOrtho.Reset(orth);			
			hubObjA.Destory();			
			hubObjA.Create(sys, locA.srcMatrix, toothA.hubMesh);
			psrcMeshA = &(toothA.srcMesh);			
			hubObjB.Destory();
			hubObjB.Create(sys, locB.srcMatrix, toothB.hubMesh);
			psrcMeshB = &(toothB.srcMesh);			
		}
		
		VFMatrix4	ortho;
		unsigned	nReset;
		TVSource<VFMatrix4>	srcOrtho;
		TVHub< VNCollisionTest::VRTestObjA > hubObjA;
		TVHub< VNCollisionTest::VRTestObjB > hubObjB;
		const TVSource< VSSimpleMeshF >*	psrcMeshA;
		const TVSource< VSSimpleMeshF >*	psrcMeshB;
	}VS_SCENEORTHO, *PVS_SCENEORTHO;



	
	VD_PERSISTCLASS_BEGIN(VSCollisionCheckInfo)
		VD_DEFMEMBER(float, flInterval)		
		VD_DEFMEMBER(VFVECTOR3, pointA)
		VD_DEFMEMBER(VFVECTOR3, pointB)
	VD_PERSISTCLASS_END();

	struct VSCollisionSenceInfo
	{
		TVData< VNCollisionTest::VSMergeResult >*	pDatResult;
		TVHub< VNCollisionTest::VRMergeResult >*		pHubResult;
		TVHub< VNCollisionTest::VRMergeSceneCombain >*	pHubSceneCombain;
		std::vector<VSSceneOrtho*>*	pvSrcOrtho;
		std::vector<INNER_WXJ::VSCollisionCheckInfo>* pvTeethIntervalInfo;	// —¿≥›º‰œ∂
	};



	
	struct VSMeshHubInfo
	{
		VSMeshHub* pMeshHub;
		VSLocSrc* pLocSrc;
	};
}

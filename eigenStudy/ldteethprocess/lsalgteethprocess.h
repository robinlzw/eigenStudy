#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "obj/alg/stdnotdef.h"
#include "vmath/vflocation.h"
#include "vmath/vfmesh.h"
#include "vstd/tisys.h"
#include "obj/ialgmesh/ialgmesh.h"
#include "sys/alg/isysmesh.h"
#include "sys/trn50/isyscut.h"
#include "obj/alg/ialgteethpatching.h"
#include "pkg/ldck4cce.h"
#include "pkg/ldwzkupdater.h"
#include "pkg/ldwxlupdater.h"
#include "obj/iglnew/VGlEnvCreator.h"
#include "obj/tiwxltmp.h"
#include "obj/lsalgtools/VDSparseMatrix.h"
#include "obj/lsalgtools/ilsalgtoolsperfmesh.h"
#include "VBCollisionSence.h"
#include "VBAACollisionSence.h"

// lsalgteethprocess.h	
/*
	ò���Ƕ�ldteethprocess.h�Ĳ��䡣
	�����������ṹ�塪��������pair�ṹ��������VSCollisionPointPair, VSGapPointPairs
	�����������ࣺVCToothObj, VCGapObj
	

*/




class VCToothObj;
class VCGapObj;

//// �Զ�����


	// VSKeyPoints�ࡪ���µĲ����㷨
VD_PERSISTCLASS_BEGIN(VSKeyPoints)
	VD_DEFMEMBER(VSConstBuffer<VFVECTOR3>, cbKeyPoints)
VD_PERSISTCLASS_END()

VD_PERSISTCLASS_BEGIN(VSLine)
	VD_DEFMEMBER(VSConstBuffer<unsigned>, cbLine)
VD_PERSISTCLASS_END()




	// VSToothNbrEdgeLine�ࡪ������������Ҫ�޲��ı߽���
VD_PERSISTCLASS_BEGIN(VSToothNbrEdgeLine)
	VD_DEFMEMBER(unsigned, nNbrTooth)
	VD_DEFMEMBER(std::vector<unsigned>, vBndryLine)	// ������ԭʼ����߽������	
VD_PERSISTCLASS_END()




	// VSToothNbrPatchedVert�ࡪ�������������޲�����Ϣ
VD_PERSISTCLASS_BEGIN(VSToothNbrPatchedVert)
	VD_DEFMEMBER(unsigned, nNbrTooth)
	VD_DEFMEMBER(std::vector<unsigned>, vVertices)	// ������ԭʼ���񶥵��������ֻ��ʾ���ӵĵ㣬������ԭʼ�߽��
	VD_DEFMEMBER(unsigned, ctrlVert) //
VD_PERSISTCLASS_END()



	// VSToothPatched��
VD_PERSISTCLASS_BEGIN(VSToothPatched)
	VD_DEFMEMBER(VSPerfectMesh, pm)
	VD_DEFMEMBER(NMALG_TEETHPATCHING::VSToothAxis, toothAxis)
	VD_DEFMEMBER(VSConstBuffer< unsigned >, cbLableSurface)  // ���ȵ���nToothCount , ÿ������һ���������Ƭ����ʾ������Ƭ�϶�Ϊ�����ڲ�����Ϊ�������ݱ���ʱ����ʼλ��
	VD_DEFMEMBER(VSConstBuffer< unsigned >, cbVertexLable)  // ÿ��������������ݱ�ʶ�� ֵΪVD_INVALID_INDEX��ʾΪ����
VD_PERSISTCLASS_END()




	// VSCollisionPointPair��
struct VSCollisionPointPair
{
	std::pair<unsigned, unsigned> pointPair;
};




	// VSGapPointPairs��
struct VSGapPointPairs
{
	float flDeepestDis;				// С��0��ʾ��ײ
	std::pair<unsigned, unsigned>	pairToothLabel;
	std::pair<VFVECTOR3, std::vector<VSCollisionPointPair> > gapPointPairs;
};






namespace NMALG_TEETHPATCHING
{
#define COLLISIONSENCEAA_ENABLE


	// VBTeethPatching�ࡪ������
	class VBTeethPatching: public IVBTeethPatching, IVTracer
	{
	public:
		VBTeethPatching(const unsigned guid);
		~VBTeethPatching();

		virtual void InitCollisionSence(const VSESS< VNCollisionTest::VRCollisionExt > & essCollisionExt);
		virtual void InitCollisionSenceAA(const VSESS< VNCollisionTest::VRMergeCollisionTest > & sysCollisionTest);


		// ���ɲ��������м���
		virtual void BuildTopoTeeth(VSTeethPatchingTopoResult & topoRslt, const VSTeethPatchingInput & topoInput);
		
		// ���ɲ��������м���
		virtual void BuildInitInfo(VSTeethPatchInfo& info, const VSTeethPatchingTopoResult& topo, CRITICAL_SECTION* pCsOpenGl = NULL);
		
		// ���������������ݣ����ɲ����������ս��
		virtual void BuildResult(VSTeethPatchingResult& rslt, const VSTeethPatchingTopoResult& topo,
			const VSTeethPatchInfo& info, CRITICAL_SECTION* pCsOpenGl = NULL);


		// ����������û�о������������ݣ����ɲ����������ս����
		virtual void Build(VSTeethPatchingResult& result, const VSSegmentedJaw& segmentedJaw);

		virtual void UpdateToothAxis(VSTeethPatchingResult& result, const VSConstBuffer<VSToothAxisContext>& toothAxises);
		virtual void UpdateToothGap(VSTeethPatchingResult& result, const VSConstBuffer<VSGapContext>& gaps);

		unsigned GetGuid() const { return m_guid; };
	
	
	protected:
		void OnChanged();		 // ���ݱ仯�Զ�����    
		
	private:		
		void Init(const VSSegmentedJaw& segmentedJaw);		
		void Reset();		
		void MergePatchedTooth();		
		void MergePatchedTooth(const VSTeethPatchingTopoResult& topoMesh);
		void MergePatchedToothEx(const VSTeethPatchingTopoResult& topoMesh);
		double CollisionCheck(VSGapPointPairs& gapPointPair, const unsigned nToothIdx1, 
			const unsigned nToothIdx2, CRITICAL_SECTION* pCsOpenGl = NULL);
		double CollisionCheck(VBCollisionSence::VSCollisionSenceResult& ret, 
			std::pair<VFVECTOR3, std::vector<VSCollisionPointPair> >& gapPointPairs,
			const VSSimpleMeshF& mesh1, const VSSimpleMeshF& mesh2, CRITICAL_SECTION* pCsOpenGl = NULL);
		void CalcCollisionPointPair(std::vector<VSGapPointPairs>& vGapPointPairs);
		void CalcCollisionPointPair(std::vector<VSGapPointPairs>& vGapPointPairs,
			const VSTeethPatchingTopoResult& topoJaw, CRITICAL_SECTION* pCsOpenGl = NULL);
		void CalcCollisionPointPair(std::vector<VSGapPointPairs>& vGapPointPairs,
			const VSConstBuffer<VSPatchedTooth>& cbPatchTooth, CRITICAL_SECTION* pCsOpenGl = NULL);
		void MakeQuadProgramMatrixs();
		void MakeQuadProgramMatrixsAieq(const VSConstBuffer<VSGapPointPairs>& cbGapPointPairs);
		void MakeMatrixL2(NMALG_MATRIX::VSSparseMatrix& L2);
		void MakeMatrixL(std::vector<VSTripletD >& L);
		void MakeMatrixM(std::vector<VSTripletD >& M);
		void MakeMatrixX0();
		void MakeMatrixAeq();
		void MakeMatrixAeqUidAndKid(std::vector<VSTripletD>& vAeqKid);
		void MakeMatrixAieq(const VSConstBuffer<VSGapPointPairs>& cbGapPointPairs);
		void MakeMatrixAieqUidAndKid(std::vector<VSTripletD>& vAieqKid);
		void MakeMatrixBeq();
		void MakeMatrixBieq();
		void MakeHUidUid(const VSConstBuffer<VSTripletD>& L2);
		void MakeHKidUidTranspose(std::vector<VSTripletD>& matHKidUidTranspose, const VSConstBuffer<VSTripletD>& L2);
		void SolveQuadProgram(std::vector<VSTripletD>& X, const VSSprsMatrixD& sprsMatHUidUid,
			const VSSprsMatrixD& sprsMatHKidUid, const VSSprsMatrixD& sprsMatAeqUid,
			const VSSprsMatrixD& sprsMatAeqKid, const VSSprsMatrixD& sprsMatAieqUid,
			const VSSprsMatrixD& sprsMatAieqKid);
		void QuadProgFixed(std::vector<VSTripletD>& X, std::vector<VSTripletD>& vLambda,
			const VSSprsMatrixD& sprsMatHUidUid, const VSSprsMatrixD& sprsMatHKidUid,
			const VSSprsMatrixD& sprsMatAeqUidTmp, const VSSprsMatrixD& sprsMatAeqKidTmp);
		void _ConfirmStartSurface(std::vector< unsigned > & vsurf,
			const VSPerfectMesh & mesh, const VSConstBuffer< unsigned > & orgLable, unsigned TC) const;
		void _MakeMatrixKidAndVid(std::vector<VSTripletD>& vMatKid,
			std::vector<VSTripletD>& vMatUid, VSConstBuffer<VSTripletD>& cbMat);
		void UpdateJawMesh(const VSConstBuffer<VSTripletD>& cbX);
		void UpdateBtmCtrlPoints(const VSTeethPatchingTopoResult& topo, const VSTeethPatchInfo& info);
		void CheckZeroTriangle(std::vector<VFVECTOR3> vVerts, const VSConstBuffer<VNVECTOR3UI>& cbSurfs);
		void GetHardPalateInfo(VSTeethPatchingTopoResult & topoRslt);
		void Remove1VertConn2Bndry(NM_PMMESH::VSMesh& meshInOut);
		void RemoveSurfWith3Bndry(NM_PMMESH::VSMesh& meshInOut);
	public:
		VSPerfectMesh	m_jawMesh;
		std::vector<VFVECTOR3> m_vKeyPoints;
		std::vector<std::shared_ptr<VCToothObj> > m_vObjTooth;
		std::vector<std::shared_ptr<VCGapObj> > m_vObjGap;

	private:
		struct VSToothVertInfo
		{
			unsigned nIdxBase;
			unsigned nVertCount;
		};
		

		// ���ݲ����ϲ�Ϊ��������
		std::vector<VFVECTOR3> m_vVerts;
		std::vector<VNVECTOR3UI> m_vSurfs;
		VSPerfectMesh	m_jawMeshPatched;
		std::vector< VEToothVertType >	m_vVertTypes;
		std::vector<std::vector<unsigned> > m_vvToothVOnJawMesh;
		std::vector<VSToothVertInfo>	m_vToothVertInfo;
		std::vector<std::pair<unsigned, unsigned> >	m_vSideCtrlPointPair;
		std::vector<unsigned>		m_vVarVertOnJawMesh;
		std::vector<unsigned>		m_vFixVertOnJawMesh;
		std::vector<unsigned>		m_vVertFlag;
		std::vector<unsigned>		m_vVertMapKid;
		std::vector<unsigned>		m_vVertMapUid;
		unsigned					m_nKidCount;
		unsigned					m_nUidCount;
		TVExtSource< VNALGMESH::VRPerfMesh >	m_extVRPerfMesh;
		std::shared_ptr<TVR2B< VNALGMESH::VRPerfMesh > >	m_ptrVRPerfMesh;

		std::vector<VSTripletD> m_matHUidUid;
		NMALG_MATRIX::VSSparseMatrix m_matHKidUidTrans;
		VSSprsMatrixD			m_sprsMatHUidUid;
		VSSprsMatrixD			m_sprsMatHKidUidTrans;
		
		std::vector<VSTripletD> m_Aieq;
		std::vector<VSTripletD> m_AieqUid;
		NMALG_MATRIX::VSSparseMatrix m_AieqKid;
		std::vector<VSTripletD>	m_Bieq;
		VSSprsMatrixD			m_sprsMatAieq;		
		VSSprsMatrixD			m_sprsMatAieqUid;
		VSSprsMatrixD			m_sprsMatAieqKid;
		VSSprsMatrixD			m_sprsMatBieq;

		std::vector<VSTripletD> m_Aeq;
		std::vector<VSTripletD> m_AeqUid;
		NMALG_MATRIX::VSSparseMatrix m_AeqKid;
		std::vector<VSTripletD>	m_Beq;
		VSSprsMatrixD			m_sprsMatAeq;
		VSSprsMatrixD			m_sprsMatAeqUid;
		VSSprsMatrixD			m_sprsMatAeqKid;
		VSSprsMatrixD			m_sprsMatBeq;
		
				
		std::vector<VSTripletD> m_vX0;
		VSSprsMatrixD			m_sprsMatX0;
		VSSegmentedJaw m_segmentedJaw;

		//VGlEnvCreator<4, 5> m_GLCreator;
		//std::shared_ptr< TVExtSysUser< VNCollisionTest::VRCollisionExtCreator > > m_SysUserCollisionTestCreator;
		//VSESS< VNCollisionTest::VRCollisionExt >  m_EssSysCollisionTest;
		VBCollisionSence* m_pCollisionSence;

		VBAACollisionSence* m_pCollisionSenceAA;


	private:
		// BuildTopoTeeth ����ڴ�
		std::vector< VSTeethPatchingTopoTooth >		m_teeth;
		std::vector< VSPatchPair >					m_patchPairs;


		// BuildInitInfo ����ڴ�
		std::vector<VSToothInfo>					m_teethInfo;
		std::vector<float>							m_pairGapInfo;


		// BuildResult ����ڴ�
		std::vector< VSPatchedTooth >				m_teethData;
		std::vector<std::vector<VNVECTOR3UI> >		m_teethSurfs;	
		NM_PMMESH::VSMesh m_meshHardPalate;
		std::vector<std::vector<unsigned>> m_vvMeshHPVertType;
		std::vector<VSConstBuffer<unsigned>> m_vCbMeshHPVertType;

		std::vector<VFVECTOR3> m_vVertNormals;
		unsigned m_nOutput;
	private:
		unsigned m_guid;
	};




	// VSysManager�ࡪ�����������������ࣿ
	class VSysManager
	{
	public:
		VSysManager();
		~VSysManager();

		VBTeethPatching* CreateSys();

		void DeleteSys(IVBTeethPatching* teethPathing);

	private:
		std::vector<VBTeethPatching*> m_vSysTeethPatching;
	};
}

#pragma once
#include <vmath/vfmesh.h>
#include <erp/erp.h>
#include <sys/alg/isysmesh.h>
#include "VBEDCmpCombHandle.h"
#include "VBFindHole.h"

//Ԥ��������ѹ��

class VBPreProcCompress
{
public:
	VBPreProcCompress(const VSESS<VNALGMESH::VRPerfMesh> & gm) : m_Graph(gm){}
	~VBPreProcCompress() {}

	void Build(VNWZKALG::VSEdgeBreakerDecInfo & cmp, const VSSimpleMeshF & smesh)
	{
		Process(smesh);
		cmp = data.Get();
	}

private:
	void       Process(const VSSimpleMeshF & smesh);
	VFVECTOR3  AddVert(const VSSimpleMeshF & smesh, const VNWZKALG::VSHoleBoundary & Bndry);
	void       AddSurf(const VSSimpleMeshF & smesh, const VNWZKALG::VSHoleBoundary & Bndry);
	void       MeshFixed(VSSimpleMeshF & out, const VSSimpleMeshF & in, const VNWZKALG::VSHoleBoundary & Bndry);
	void       RelationMapManage(VNWZKALG::VSEdgeBreakerDecInfo & res, const VSSimpleMeshF & smesh);   //ֻ���޸���������������
	//void  DataCopy();

private:
	TVPersist<VNWZKALG::VSEdgeBreakerDecInfo>   data;
	TVR2B<VNALGMESH::VRPerfMesh>                m_Graph;
	VBEDCmpCombHandle                           m_objCmp;
	VBFindHole                                  m_objFindHole;

	VNMesh::VSCompressedMesh                    mesh;

private:
	std::vector<VFVECTOR3>    m_vVert;
	std::vector<VNVECTOR3UI>  m_vSurf;
	std::vector<unsigned>     m_vBndryV;
	VSSimpleMeshF             m_FixedMesh;

	//���¹�ϵ��Ϊɾ����ӵĵ������Ƭ֮��Ķ�Ӧ��ϵ
	std::vector<unsigned>     m_vVertMap;       //�Ǳ�Ϊ��ѹ�㣬������Ϊ��Ӧ��ԭʼ��������
	std::vector<unsigned>     m_vTriMap;        //�Ǳ��Ӧ��ѹ����Ƭ��������Ϊ��Ӧ��ԭʼ����Ƭ����
	std::vector<unsigned>     m_vOldV2NewV;     //�Ǳ��Ӧԭʼ�㣬������Ϊ��Ӧ���µ�����
	std::vector<unsigned>     m_vOldTri2NewTri; //�Ǳ�Ϊԭʼ����Ƭ��������Ϊ��Ӧ���µ�����Ƭ����
};


class VBCompressMeshGetter
{
public:
	VBCompressMeshGetter() {}
	~VBCompressMeshGetter() {}

	void Build(VNMesh::VSCompressedMesh & out, const VNWZKALG::VSEdgeBreakerDecInfo & cmp);
	/*{
		out.clers = cmp.TopoBin;
		out.firstVert = cmp.firstVert;
		out.triCount = cmp.triLen;
		out.vertCount = cmp.vertLen;
		out.handles = cmp.handles;
		out.bndryV = cmp.bndryV;
		assert(cmp.compRes.len == 3);
		SubBuild(out.xVert, cmp.compRes.pData[0]);
		SubBuild(out.yVert, cmp.compRes.pData[1]);
		SubBuild(out.zVert, cmp.compRes.pData[2]);
	}*/

private:
	void SubBuild(VNMesh::VSCompressedFloats & out, const VNWZKALG::VSCompressRes & in);
	/*{
		out.minValue = in.min;
		out.lowBitLen = in.bitLen.y;
		out.lowBitData = in.lowBitVal;
		out.highBitLen = in.bitLen.x;
		out.highBitData = in.highBitCompVal;
	}*/
};
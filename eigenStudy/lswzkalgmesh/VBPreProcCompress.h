#pragma once
#include <vmath/vfmesh.h>
#include <erp/erp.h>
#include <sys/alg/isysmesh.h>
#include "VBEDCmpCombHandle.h"
#include "VBFindHole.h"

//预处理网格压缩

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
	void       RelationMapManage(VNWZKALG::VSEdgeBreakerDecInfo & res, const VSSimpleMeshF & smesh);   //只对修复过的网格做处理
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

	//以下关系均为删除添加的点和三角片之后的对应关系
	std::vector<unsigned>     m_vVertMap;       //角标为解压点，里面存的为对应的原始顶点索引
	std::vector<unsigned>     m_vTriMap;        //角标对应解压三角片，里面存的为对应的原始三角片索引
	std::vector<unsigned>     m_vOldV2NewV;     //角标对应原始点，里面存的为对应的新点坐标
	std::vector<unsigned>     m_vOldTri2NewTri; //角标为原始三角片，里面存的为对应的新的三角片索引
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
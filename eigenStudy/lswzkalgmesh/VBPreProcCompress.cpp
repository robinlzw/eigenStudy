#include "stdafx.h"
#include "VBPreProcCompress.h"

void WriteSimpleMesh(const char* pszFileName, const VSSimpleMeshF& mesh)
{
	std::ofstream objFile(pszFileName);
	for (unsigned i = 0; i < mesh.nVertCount; i++)
	{
		const VFVECTOR3& vert = mesh.pVertices[i];
		objFile << "v " << vert.x << " " << vert.y << " " << vert.z << std::endl;
	}

	for (unsigned i = 0; i < mesh.nTriangleCount; i++)
	{
		const VNVECTOR3UI& surf = mesh.pTriangles[i];

		objFile << "f " << (surf.x + 1) << " "
			<< (surf.y + 1) << " "
			<< (surf.z + 1) << std::endl;
	}
	objFile.close();
}
void VBPreProcCompress::Process(const VSSimpleMeshF & smesh)
{
	//std::ofstream logFile("D:/testdata/preprocompress.log", std::ofstream::app);
    double delta = 1e-5;
#if 0
	{
		std::ofstream objFile("D:/testdata/preproccompress.obj");
		for (size_t i = 0; i < smesh.nVertCount; i++)
		{
			const VFVECTOR3& v = smesh.pVertices[i];
			objFile << "v " << v.x << " " << v.y << " " << v.z << std::endl;
		}
		for (size_t i = 0; i < smesh.nTriangleCount; i++)
		{
			const VNVECTOR3UI& f = smesh.pTriangles[i];
			objFile << "f " << f.x + 1 << " " << f.y + 1 << " " << f.z + 1 << std::endl;
		}
	}
#endif
	VSPerfectMesh  gm = m_Graph.Run(smesh).Get<VSPerfectMesh>();
	VNWZKALG::VSHoleBoundary Bndry;
	m_objFindHole.Build(Bndry, gm);
	bool flag = false;
	if (Bndry.bndry.len > 0)
	{
		MeshFixed(m_FixedMesh, smesh, Bndry);
#if 0
		std::ofstream fp1("D:/testdata/preproccompress1.obj");
		for (unsigned i = 0; i < m_FixedMesh.nVertCount; i++)
		{
			fp1 << "v " << m_FixedMesh.pVertices[i].x << "  " << m_FixedMesh.pVertices[i].y << "  " << m_FixedMesh.pVertices[i].z << std::endl;
		}
		fp1 << std::endl;
		for (unsigned i = 0; i < m_FixedMesh.nTriangleCount; i++)
		{
			fp1 << "f " << m_FixedMesh.pTriangles[i].x + 1 << "  " << m_FixedMesh.pTriangles[i].y + 1 << "  " << m_FixedMesh.pTriangles[i].z + 1 << std::endl;
		}
		fp1.close();
#endif
		gm = m_Graph.Run(m_FixedMesh).Get<VSPerfectMesh>();
		m_objFindHole.Build(Bndry, gm);
		flag = true;
		VASSERT(Bndry.bndry.len == 0);
	}

	VNWZKALG::VSEdgeBreakerDecInfo res;
	if (Bndry.bndry.len > 0)
		m_objCmp.Build(res, gm, VD_INVALID_INDEX, delta);
	else
		m_objCmp.Build(res, gm, 0, delta);
	if (flag)
	{
		res.delVertID = res.vtMapIdUC2C.pData[m_FixedMesh.nVertCount - 1];     //添加点在解压后网格中的ID

		RelationMapManage(res, m_FixedMesh);
		res.vertMapID = VD_V2CB(m_vVertMap);
		res.triMapID = VD_V2CB(m_vTriMap);
		res.vtMapIdUC2C = VD_V2CB(m_vOldV2NewV);
		res.triMapIdUC2C = VD_V2CB(m_vOldTri2NewTri);
	}
	data.Build(res);
	//DataCopy();
}

VFVECTOR3 VBPreProcCompress::AddVert(const VSSimpleMeshF & smesh, const VNWZKALG::VSHoleBoundary & Bndry)
{
	unsigned bndryNum = Bndry.bndry.pData[0].orderedVert.len;
	m_vBndryV.resize(bndryNum);
	memcpy_s(&m_vBndryV.front(), sizeof(unsigned) * bndryNum, Bndry.bndry.pData[0].orderedVert.pData, sizeof(unsigned) * bndryNum);

	VFVECTOR3  vert(0.f, 0.f, 0.f);
	for (unsigned i = 0; i < bndryNum; i++)
	{
		vert += smesh.pVertices[m_vBndryV[i]];
	}

	vert = vert / static_cast<float>(bndryNum);
	return vert;
}

void VBPreProcCompress::AddSurf(const VSSimpleMeshF & mesh, const VNWZKALG::VSHoleBoundary & Bndry)
{
	VFVECTOR3 vert = AddVert(mesh, Bndry);

	unsigned vNum = mesh.nVertCount;
	unsigned sNum = mesh.nTriangleCount;
	m_vVert.resize(vNum + 1);
	m_vSurf.resize(sNum + m_vBndryV.size());
	memcpy_s(&m_vVert.front(), sizeof(VFVECTOR3) * vNum, mesh.pVertices, sizeof(VFVECTOR3) * vNum);
	memcpy_s(&m_vSurf.front(), sizeof(VNVECTOR3UI) * sNum, mesh.pTriangles, sizeof(VNVECTOR3UI) * sNum);

	m_vVert[vNum] = vert;

	unsigned bNum = m_vBndryV.size();
	VNVECTOR3UI temp;
	temp.x = vNum;
	for (unsigned i = 0; i < bNum - 1; i++)
	{
		temp.y = m_vBndryV[i];
		temp.z = m_vBndryV[i + 1];
		m_vSurf[sNum] = temp;
		sNum++;
	}

	temp.y = m_vBndryV[bNum - 1];
	temp.z = m_vBndryV[0];
	m_vSurf[sNum] = temp;
}

void VBPreProcCompress::MeshFixed(VSSimpleMeshF & out, const VSSimpleMeshF & in, const VNWZKALG::VSHoleBoundary & Bndry)
{
	AddSurf(in, Bndry);

	out.nTriangleCount = m_vSurf.size();
	out.nVertCount = m_vVert.size();
	out.pTriangles = &m_vSurf.front();
	out.pVertices = &m_vVert.front();
}

void VBPreProcCompress::RelationMapManage(VNWZKALG::VSEdgeBreakerDecInfo & res, const VSSimpleMeshF & smesh)
{
	//解压点与原始点对应关系
	m_vVertMap.resize(res.vertMapID.len - 1);
	if (res.delVertID == 0)
		memcpy_s(&m_vVertMap.front(), sizeof(unsigned) * m_vVertMap.size(), &res.vertMapID.pData[1], sizeof(unsigned) * m_vVertMap.size());
	else
	{
		memcpy_s(&m_vVertMap.front(), sizeof(unsigned) * res.delVertID, res.vertMapID.pData, sizeof(unsigned) * res.delVertID);
		memcpy_s(&m_vVertMap[res.delVertID], sizeof(unsigned) * (m_vVertMap.size() - res.delVertID), &res.vertMapID.pData[res.delVertID + 1], sizeof(unsigned) * (m_vVertMap.size() - res.delVertID));
	}

	//解压三角片与原始三角片对应关系
	unsigned count = 0, orgDelID = res.vertMapID.pData[ res.delVertID ];
	m_vTriMap.resize(res.triLen);  //空间有多余，须删除部分三角片
	for (unsigned i = 0; i < res.triMapID.len; i++)
	{
		VNVECTOR3UI  temp = smesh.pTriangles[ res.triMapID.pData[i] ];   //修补后的三角片
		if (temp.x == orgDelID || temp.y == orgDelID || temp.z == orgDelID)
			continue;
		m_vTriMap[ count ++ ] = res.triMapID.pData[i];
	}
	m_vTriMap.resize(count);

	//原始顶点与解压点对应关系
	m_vOldV2NewV.resize(m_vVertMap.size());
	for (unsigned i = 0; i < m_vVertMap.size(); i++)
		m_vOldV2NewV[m_vVertMap[i]] = i;  //UC2C

	//原始三角片与解压三角片对应关系
	m_vOldTri2NewTri.resize(m_vTriMap.size());
	for (unsigned i = 0; i < m_vTriMap.size(); i++)
		m_vOldTri2NewTri[m_vTriMap[i]] = i;  //UC2C
}

//void VBPreProcCompress::DataCopy()
//{
//	mesh.clers     = data.Get().TopoBin;
//	mesh.firstVert = data.Get().firstVert;
//	mesh.triCount  = data.Get().triLen;
//	mesh.vertCount = data.Get().vertLen;
//	mesh.handles   = data.Get().handles;
//	mesh.bndryV    = data.Get().bndryV;
//	//x
//	mesh.xVert.minValue = data.Get().compRes.pData[0].min;
//	mesh.xVert.lowBitLen = data.Get().compRes.pData[0].bitLen.y;
//	mesh.xVert.lowBitData = data.Get().compRes.pData[0].lowBitVal;
//	mesh.xVert.highBitLen = data.Get().compRes.pData[0].bitLen.x;
//	mesh.xVert.highBitData = data.Get().compRes.pData[0].highBitCompVal;
//	//y
//	mesh.yVert.minValue = data.Get().compRes.pData[1].min;
//	mesh.yVert.lowBitLen = data.Get().compRes.pData[1].bitLen.y;
//	mesh.yVert.lowBitData = data.Get().compRes.pData[1].lowBitVal;
//	mesh.yVert.highBitLen = data.Get().compRes.pData[1].bitLen.x;
//	mesh.yVert.highBitData = data.Get().compRes.pData[1].highBitCompVal;
//	//z
//	mesh.zVert.minValue = data.Get().compRes.pData[2].min;
//	mesh.zVert.lowBitLen = data.Get().compRes.pData[2].bitLen.y;
//	mesh.zVert.lowBitData = data.Get().compRes.pData[2].lowBitVal;
//	mesh.zVert.highBitLen = data.Get().compRes.pData[2].bitLen.x;
//	mesh.zVert.highBitData = data.Get().compRes.pData[2].highBitCompVal;
//}

//VD_EXPORT_SYSTEM_SIMPLE_L2(VBPreProcCompress, VNALGMESH::VRPreProcCompressCreator);

void VBCompressMeshGetter::Build(VNMesh::VSCompressedMesh & out, const VNWZKALG::VSEdgeBreakerDecInfo & cmp)
{
	out.clers = cmp.TopoBin;
	out.firstVert = cmp.firstVert;
	out.triCount = cmp.triLen;
	out.vertCount = cmp.vertLen;
	out.delVertID = cmp.delVertID;
	out.handles = cmp.handles;
	out.bndryV = cmp.bndryV;
	assert(cmp.compRes.len == 3);
	SubBuild(out.xVert, cmp.compRes.pData[0]);
	SubBuild(out.yVert, cmp.compRes.pData[1]);
	SubBuild(out.zVert, cmp.compRes.pData[2]);
}

void VBCompressMeshGetter::SubBuild(VNMesh::VSCompressedFloats & out, const VNWZKALG::VSCompressRes & in)
{
	out.minValue = in.min;
	out.lowBitLen = in.bitLen.y;
	out.lowBitData = in.lowBitVal;
	out.highBitLen = in.bitLen.x;
	out.highBitData = in.highBitCompVal;
}

class VBCompressInfoGetter
{
public:
    VBCompressInfoGetter() {}
    ~VBCompressInfoGetter() {}

    void Build(VNMesh::VSCompressInfo & out, const VNWZKALG::VSEdgeBreakerDecInfo & cmp)
    {
		if (cmp.bndryLen > 0)
			out.vertMapC2UC = cmp.vertMapIDwithHole;
		else
			out.vertMapC2UC = cmp.vertMapID;
        out.triMapC2UC = cmp.triMapID;

		out.vertMapUC2C = cmp.vtMapIdUC2C;
		out.triMapUC2C = cmp.triMapIdUC2C;
    }
};

VD_BEGIN_PIPELINE(VPLPreProcCompress, VSESS<VNALGMESH::VRPerfMesh>)
    VDDP_NOD2(Compress, VBPreProcCompress);
    VDDP_NOD(CMGetter, VBCompressMeshGetter);
    VDDP_NOD(CIGetter, VBCompressInfoGetter);
VD_END_PIPELINE()

class VMPreProcCompress
{
public:
    VMPreProcCompress(VPLPreProcCompress & pl
        , IVSlot<VSSimpleMeshF> & slt 
        , IVInputPort<VNMesh::VSCompressedMesh> & ipCM
        , IVInputPort<VNMesh::VSCompressInfo> & ipCI) 
    {
        pl.CreateCompress(m_Compress, pl.GetEnv().Get<VSESS<VNALGMESH::VRPerfMesh>>(), slt);
        pl.CreateCMGetter(m_CMGetter, m_Compress);
        pl.CreateCIGetter(m_CIGetter, m_Compress);
        VLNK(ipCM, m_CMGetter);
        VLNK(ipCI, m_CIGetter);
    }
    ~VMPreProcCompress() {}

private:
    VPLPreProcCompress::Compress    m_Compress;
    VPLPreProcCompress::CMGetter    m_CMGetter;
    VPLPreProcCompress::CIGetter    m_CIGetter;
};

struct VSYSTraitPreProcCompress
{
    typedef VPLPreProcCompress PIPELINE;

    template< typename TR > struct TRTrait;
    template<> struct TRTrait< VNALGMESH::VRPreProcCompress > { typedef VMPreProcCompress mngr_type; };
};

class VBPreProcCompressCreator
{
public:
    VBPreProcCompressCreator() {}
    ~VBPreProcCompressCreator() {}

    void Build(VSESS<VRPreProcCompress> & out, const VSESS<VNALGMESH::VRPerfMesh> & in)
    {
        m_svr.reset(new TVSystemProviderImp< VSYSTraitPreProcCompress, IVSYS< VNALGMESH::VRPreProcCompress > >(in));
        out.pSysProv = m_svr.get();
    }

private:
    std::shared_ptr< TVSystemProviderImp< VSYSTraitPreProcCompress, IVSYS< VNALGMESH::VRPreProcCompress > > > m_svr;
};

VD_BEGIN_PIPELINE(VPLPPCC)
    VDDP_NOD(PPCC, VBPreProcCompressCreator);
VD_END_PIPELINE()

class VMPreProcCompressCreator
{
public:
    VMPreProcCompressCreator(VPLPPCC & pl, IVSlot<VSESS<VNALGMESH::VRPerfMesh>> & slt, IVInputPort<VSESS<VRPreProcCompress>> & ip)
    {
        pl.CreatePPCC(m_PPCC, slt);
        VLNK(ip, m_PPCC);
    }
    ~VMPreProcCompressCreator() {}

private:
    VPLPPCC::PPCC m_PPCC;
};

struct VSYSTraitPPCC
{
    typedef VPLPPCC PIPELINE;

    template< typename TR > struct TRTrait;
    template<> struct TRTrait< VNALGMESH::VRPreProcCompressCreator > { typedef VMPreProcCompressCreator mngr_type; };
};

void VLIBEXPFUNCNAME(VSESS<VNALGMESH::VRPreProcCompressCreator> & extsys)
{ 
    static TVSystemProviderImp< VSYSTraitPPCC, IVSYS< VNALGMESH::VRPreProcCompressCreator > > svrPPCC;
    extsys.pSysProv = &svrPPCC;
}

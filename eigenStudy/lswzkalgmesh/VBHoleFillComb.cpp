#include "stdafx.h"
#include "VBHoleFillComb.h"

void VBHoleFillComb::CombFun(const VSSimpleMeshF & msh)
{
	VSPerfectMesh pm = m_PerfMesh.Run(msh).Get<VSPerfectMesh>();

	VSConstBuffer<VFVECTOR3, VFVECTOR3>    vInfo;
	vInfo.len = pm.GetVertexCount();
	vInfo.pData1 = pm.pVertices;
	vInfo.pData2 = pm.pVertNormals;
		
	//VSConstBuffer<VSConstBuffer<unsigned>>    bndryV;
	VNWZKALG::VSHoleBoundary   bndryV;
	m_FindBndry.Build(bndryV, pm);     //查找所有边界点	

	unsigned vNum = pm.GetVertexCount();
	for (unsigned i = 0; i < bndryV.bndry.len; i++)
	{		
		VFVECTOR3  centerV = CenterVert(msh, bndryV.bndry.pData[i].orderedVert);
		m_vAddVert.push_back(centerV);

		VNVECTOR3UI tri;
		tri.x = vNum;
		unsigned bndyVnum = bndryV.bndry.pData[i].orderedVert.len;
		for (unsigned j = 0; j < bndyVnum - 1; j++)
		{
			tri.y = bndryV.bndry.pData[i].orderedVert.pData[j];
			tri.z = bndryV.bndry.pData[i].orderedVert.pData[j + 1];
			m_vAddTri.push_back(tri);
		}

		//the last triangle		
		tri.y = bndryV.bndry.pData[i].orderedVert.pData[bndyVnum - 1];
		tri.z = bndryV.bndry.pData[i].orderedVert.pData[0];
		m_vAddTri.push_back(tri);
		vNum++;
	}
	
	vNum = pm.GetVertexCount();
	m_vVertices.resize(vNum + m_vAddVert.size());
	memcpy(&m_vVertices.front().x, &pm.pVertices[0].x, sizeof(VFVECTOR3) * vNum);
    if(m_vAddVert.size() != 0)
	    memcpy(&m_vVertices[vNum].x, &m_vAddVert.front().x, sizeof(VFVECTOR3) * m_vAddVert.size());
	unsigned sNum = pm.GetSurfCount();
	m_vSurfaces.resize(sNum + m_vAddTri.size());
	memcpy(&m_vSurfaces.front().x, &pm.pSurfaces[0].x, sizeof(VNVECTOR3UI) * sNum);
    if (m_vAddTri.size() != 0)
	    memcpy(&m_vSurfaces[sNum].x, &m_vAddTri.front().x, sizeof(VNVECTOR3UI) * m_vAddTri.size());

	/*unsigned vNumTotal = vNum;
	for (unsigned i = 0; i < bndryV.len; i++)
	{
		VNWZKALG::VSFillResult  addTopo = m_HoleFill.Run(bndryV.pData[i], vInfo).Get<VNWZKALG::VSFillResult>();

		vNum = m_vVertices.size();
		//unsigned vNumTotal = vNum + addTopo.newvertice.len;
		vNumTotal = vNum + addTopo.newvertice.len;
		m_vVertices.resize(vNumTotal);	
		if (addTopo.newvertice.len > 0)
			memcpy(&m_vVertices[vNum].x, &addTopo.newvertice.pData[0].x, sizeof(VFVECTOR3) * addTopo.newvertice.len);

		sNum = m_vSurfaces.size();
		unsigned sNumTotal = sNum + addTopo.triangles.len;
		m_vSurfaces.resize(sNumTotal);
		unsigned vLen = addTopo.newvertice.len;
		for (unsigned i = 0; i < addTopo.triangles.len; i++)
		{
			unsigned orgID = addTopo.triangles.pData[i].x;
			m_vSurfaces[sNum + i].x = (orgID > vNumTotal ? orgID + vLen : orgID);
			orgID = addTopo.triangles.pData[i].y;
			m_vSurfaces[sNum + i].y = (orgID > vNumTotal ? orgID + vLen : orgID);
			orgID = addTopo.triangles.pData[i].z;
			m_vSurfaces[sNum + i].z = (orgID > vNumTotal ? orgID + vLen : orgID);
		}		
	}	*/
}

VFVECTOR3 VBHoleFillComb::CenterVert(const VSSimpleMeshF & sm, const VSConstBuffer<unsigned>& bndryV)
{
	VFVECTOR3  sum(0.f, 0.f, 0.f), val;
	for (unsigned i = 0; i < bndryV.len; i++)
	{
		sum.x += sm.pVertices[bndryV.pData[i]].x;
		sum.y += sm.pVertices[bndryV.pData[i]].y;
		sum.z += sm.pVertices[bndryV.pData[i]].z;
	}
	
	val.x = sum.x / static_cast<float>(bndryV.len);
	val.y = sum.y / static_cast<float>(bndryV.len);
	val.z = sum.z / static_cast<float>(bndryV.len);
	return val;
}

VD_BEGIN_PIPELINE( VPLHoleFillCombCreator , VSESS< VNALGMESH::VRHoleFillCombDep > );
    VDDP_ARG( ESDep , VNALGMESH::VRHoleFillCombDep ) ;
    VDDP_EXT( ESPerfMesh , VNALGMESH::VRPerfMesh , ESDep );
    VDDP_EXT( ESHoleFill , VNALGMESH::VRHoleFill , ESDep );
    VDDP_NOD2( HoleFillComb , VBHoleFillComb );
VD_END_PIPELINE();

class VMHoleFillComb
{
public:
    VMHoleFillComb( VPLHoleFillCombCreator & pl 
                    , IVSlot< VSSimpleMeshF > & sltHoleMesh
                    , IVInputPort< VSSimpleMeshF > & ipFullMesh )
    {
        pl.CreateHoleFillComb( m_HoleFillComb , pl.GetExtESPerfMesh() , pl.GetExtESHoleFill() , sltHoleMesh );
        VLNK( ipFullMesh , m_HoleFillComb );
    }

    ~VMHoleFillComb(){}

private:
    VPLHoleFillCombCreator::HoleFillComb m_HoleFillComb;
};

struct VTraitSysHoleFillCombCreator
{
    typedef VPLHoleFillCombCreator PIPELINE ;  
    template< typename TR > struct TRTrait ; 
    template<> struct TRTrait< VNALGMESH::VRHoleFillComb > { typedef VMHoleFillComb mngr_type ; } ; 
} ; 

VD_EXPORT_SYSTEM_SYSTEM_SIMPLE( VTraitSysHoleFillCombCreator , VNALGMESH::VRHoleFillCombCreator ) ;
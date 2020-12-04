#include "stdafx.h"
#include "VBCurveToMesh.h"
 
void VBCurveToMesh::AddVertInfo(const VSGraphMesh & mesh, const Curve & cv)
{
    const unsigned & vtNum = mesh.GetVertexCount();
    m_vVert.resize(vtNum + cv.len );
    for (unsigned i = 0; i < vtNum; i ++)
    {
        m_vVert[i] = mesh.pVertices[i];        
    }

    for (unsigned i = 0; i < cv.len ; i ++)
    {
        unsigned k = i + vtNum;
        const VNVECTOR2UI &vi = mesh.GetVertIndxOfEdge( cv.pData[i].EdgeIndx );

        m_vVert[k] = (mesh.pVertices[vi[1]] - mesh.pVertices[vi[0]]) * cv.pData[i].Alpha + mesh.pVertices[vi[0]];
    }
}

void VBCurveToMesh::DelSurf(const VSGraphMesh & mesh, const Curve & cv)
{
    const unsigned & sNum = mesh.GetSurfCount();
	const unsigned & vNum = mesh.GetVertexCount();

	m_vTemp.resize(sNum);     //三角片初始全部标记为0
	//m_vCurveVert.resize(cv.len);
    
    for (unsigned i = 0; i < cv.len; i ++)
    {
        const VNVECTOR2UI & et = mesh.GetSurfIndxOfEdge(cv.pData[i].EdgeIndx);
		m_vTemp[et[0]]  = 1;
		m_vTemp[et[1]]  = 1;
		//m_vCurveVert[i] = vNum + i;
    }
    
    //删除标记为1的面片
    for (unsigned k = 0; k < sNum; k ++)
    {
        if (m_vTemp[k] == 0)
        {
			VNVECTOR3UI vidxs = mesh.pSurfaces[k];
            m_vSurfVert.push_back(vidxs);
        }
    }
}

void VBCurveToMesh::TopoSurf(const VSGraphMesh & mesh, const Curve & cv)
{
	VNVECTOR3UI temp;

	const unsigned &nu =m_vTemp.size();
    for (unsigned i = 0; i < nu; i ++)
    {
		if (m_vTemp[i] == 1)
		{
			const VNVECTOR3UI & se = mesh.GetEdgeIndxOfSurf(i);
			unsigned e0, eidx1, eidx2;

			//标记三条边的位置			
			for (unsigned j = 0; j < 3; j++)
			{
				bool flag = 1;
				for (unsigned k = 0; k < cv.len; k++)
				{
					if (se[j] == cv.pData[k].EdgeIndx)
					{
						flag = 0;
						break;
					}
				}
				if (flag)
				{
					e0 = j;
				}				
			}

			eidx1 = se[(e0 + 1) % 3];
			eidx2 = se[(e0 + 2) % 3];			

			//标记曲线所在的两条边
			unsigned v1, v2;
			for (unsigned j = 0; j < cv.len; j++)
			{
				if (cv.pData[j].EdgeIndx == eidx1)
				{
					v1 = j;
				}
				if (cv.pData[j].EdgeIndx == eidx2)
				{
					v2 = j;
				}
			}
			
			const unsigned & vNum = mesh.GetVertexCount();

			temp[0] = mesh.pSurfaces[i][(e0 + 1) % 3];
			temp[1] = mesh.pSurfaces[i][(e0 + 2) % 3];
			temp[2] = v1 + vNum;
			m_vSurfVert.push_back(temp);

			temp[0] = v1 + vNum;
			temp[1] = v2 + vNum;
			temp[2] = mesh.pSurfaces[i][(e0 + 1) % 3];
			m_vSurfVert.push_back(temp);

			temp[0] = v1 + vNum;
			temp[1] = mesh.pSurfaces[i][e0];
			temp[2] = v2 + vNum;
			m_vSurfVert.push_back(temp);			
		}    
    }
}

void VBCurveToMesh::UpdateTopo(const VSGraphMesh & mesh, const Curve & cv)
{
    AddVertInfo(mesh, cv);
    DelSurf(mesh, cv);
    TopoSurf(mesh, cv); 
}

VD_EXPORT_SYSTEM_SIMPLE(VBCurveToMesh, VNALGMESH::VRCurveToMesh);
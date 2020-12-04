#include "StdAfx.h"
#include "VBGeodicMesh.h"
#include "VGraphAndHeap.h"

/////////////////////////////////////////////
//输入
//   * msh       : 网格数据
//   * cd.len    : 边界点（初始距离为零的点）的个数
//   * cd.pData  ：边界点位于网格顶点列表中的下标
/////////////////////////////////////////////////
//输出
//   * 将所有顶点的测地距离（float型）放到数组m_lstDistance中
///////////////////////////////////////////////////////////////
void VBGeodicMesh::Build( VNALGMESH::VSMeshGeodic & gm , const VSGraphMesh & msh , const VSConstBuffer< unsigned > & cd )
{  
    m_lstDistance   .clear() ;
    m_lstGeodDisNode.clear() ;    
    m_lstNeigbNode  .clear() ;
    m_lstEqCoef     .clear() ; 
	//m_lstTemp.clear();

    Init(msh, cd);
#if 0
	{
		std::ofstream objFile("D:/testdata/geodicmesh.obj");
		for (unsigned i = 0; i < msh.GetVertexCount(); i++)
		{
			const VFVECTOR3& v = msh.pVertices[i];
			objFile << "v " << v.x << " " << v.y << " " << v.z << std::endl;
		}
		for (unsigned i = 0; i < msh.GetSurfCount(); i++)
		{
			const VNVECTOR3UI& v = msh.pSurfaces[i];
			objFile << "f " << v.x + 1 << " " << v.y + 1 << " " << v.z + 1 << std::endl;
		}
	}
#endif
    CalGeodDist(msh, cd);

    for (unsigned i = 0; i < m_lstGeodDisNode.size(); i ++)
    {
        //m_lstDistance[i] = m_lstTemp[i].dist;
		m_lstDistance[i] = m_lstGeodDisNode[i].dist;
    }

    gm.lstDist = VD_V2CB( m_lstDistance ) ;	
}

//自定义排序，升序排列(必须为全局函数，不能为类成员)
//bool MySort(const geodDisNode & v1, const geodDisNode & v2)
//{
//    return v1.Index < v2.Index;
//}

void VBGeodicMesh::Init( const VSGraphMesh & msh, const VSConstBuffer< unsigned > & cd )
{
    geodDisNode temp;
    
    m_lstDistance.resize( msh.GetVertexCount() ) ;
	m_lstGeodDisNode.resize( msh.GetVertexCount() );

    for (unsigned i = 0; i < msh.GetVertexCount(); i ++)
    {
        temp.Index = i;
		temp.path  = -1;

        for (unsigned j = 0; j < cd.len; j ++)
        {
            if (cd.pData[j] == i)
            {
                temp.dist     = 0;
                temp.nodeType = 1;
				break;
            }
            else
            {
                temp.dist     = static_cast<float>(VD_INVALID_INDEX);
                temp.nodeType = 0;
            }
        }
        m_lstGeodDisNode[i] = temp;
    }
}

void VBGeodicMesh::CalEqCoef(const VSGraphMesh & msh, const int &Indx0, const int &Indx1, const int &Indx2)
{
    float Mtr1, Mtr2, Mtr3, Mtr4, Mdet;
    const float &u1 = m_lstGeodDisNode[Indx1].dist;
    const float &u2 = m_lstGeodDisNode[Indx2].dist;   

	m_lstEqCoef.clear();
    m_lstEqCoef.resize(4);		

	Mtr1 = (msh.pVertices[Indx2] - msh.pVertices[Indx0]).SqrMagnitude();
	Mtr2 = (msh.pVertices[Indx0] - msh.pVertices[Indx2]).Dot(msh.pVertices[Indx0] - msh.pVertices[Indx1]);
    Mtr3 =  Mtr2;
	Mtr4 = (msh.pVertices[Indx1] - msh.pVertices[Indx0]).SqrMagnitude();
    Mdet = Mtr1 * Mtr4 - Mtr2 * Mtr3;

	float n1 = (msh.pVertices[Indx0] - msh.pVertices[Indx1]).Magnitude();
	float n2 = (msh.pVertices[Indx0] - msh.pVertices[Indx2]).Magnitude();
    m_lstEqCoef[0] = (Mtr1 + Mtr4 - Mtr2 - Mtr3) / Mdet;                        //a
    m_lstEqCoef[1] = 2 * ( - (Mtr1 - Mtr3) * u1 - (Mtr4 - Mtr2) * u2 ) / Mdet;   //b
    m_lstEqCoef[2] = (u1 * (u1 * Mtr1 - u2 * Mtr3) + u2 * ( u2 * Mtr4 - u1 * Mtr2)) / Mdet - 1;  //c
	m_lstEqCoef[3] = (msh.pVertices[Indx0] - msh.pVertices[Indx2]).Dot(msh.pVertices[Indx0] - msh.pVertices[Indx1]) / (n1 * n2);  //P1*P2T
}

bool VBGeodicMesh::UpWind(const VSGraphMesh & msh, const float & solution, const int & Indx0, const int &Indx1, const int &Indx2)
{
	float len1 = (msh.pVertices[Indx0] - msh.pVertices[Indx1]).Magnitude();
	float len2 = (msh.pVertices[Indx0] - msh.pVertices[Indx2]).Magnitude();

	float upwind1 = (solution - m_lstGeodDisNode[Indx1].dist) / len1;
	float upwind2 = (solution - m_lstGeodDisNode[Indx2].dist) / len2;

	if (upwind1 >= m_lstEqCoef[3] * upwind2 &&
		upwind2 >= m_lstEqCoef[3] * upwind1)
		return true;
	else
		return false;
}

void VBGeodicMesh::CalQuadEq(const VSGraphMesh & msh, const int &Indx0, const int &Indx1, const int &Indx2)
{
    float u = 0.f, u1 = 0.f, mDist = 100.f;
    float len1 = (msh.pVertices[Indx0] - msh.pVertices[Indx1]).Magnitude();
    float len2 = (msh.pVertices[Indx0] - msh.pVertices[Indx2]).Magnitude();

	float delta = m_lstEqCoef[1] * m_lstEqCoef[1] - 4 * m_lstEqCoef[0] * m_lstEqCoef[2];
	float geodis = std::max(m_lstGeodDisNode[Indx1].dist, m_lstGeodDisNode[Indx2].dist);

    if (delta >= 0)
    {
        u  = (sqrtf(delta) - m_lstEqCoef[1] ) / (2 * m_lstEqCoef[0]);
		u1 = ( - sqrtf(delta) - m_lstEqCoef[1]) / (2 * m_lstEqCoef[0]);

		//如果负根满足upwind条件，则不考虑正根
		bool flag = UpWind(msh, u1, Indx0, Indx1, Indx2);
		if(flag && u1 > geodis)
		{
			m_lstGeodDisNode[Indx0].dist = m_lstGeodDisNode[Indx0].dist > u1 ? u1 : m_lstGeodDisNode[Indx0].dist;
		}
		else
		{
			flag = UpWind(msh, u, Indx0, Indx1, Indx2);
			if(flag && u > geodis)
			{
				m_lstGeodDisNode[Indx0].dist = m_lstGeodDisNode[Indx0].dist > u ? u : m_lstGeodDisNode[Indx0].dist;
			}
			else
			{
				mDist = std::min(m_lstGeodDisNode[Indx2].dist + len2, m_lstGeodDisNode[Indx1].dist + len1);
				m_lstGeodDisNode[Indx0].dist = m_lstGeodDisNode[Indx0].dist > mDist ? mDist : m_lstGeodDisNode[Indx0].dist;
			}
		}			
    }

    if (delta < 0)
    {
        mDist = std::min(m_lstGeodDisNode[Indx2].dist + len2, m_lstGeodDisNode[Indx1].dist + len1);                  
        m_lstGeodDisNode[Indx0].dist = m_lstGeodDisNode[Indx0].dist > mDist ? mDist : m_lstGeodDisNode[Indx0].dist;
    }  
}

void VBGeodicMesh::CalEq(const VSGraphMesh & msh, const int &Indx0, const int &Indx1, const int &Indx2) 
{
    float ux = 0.f, minDist = 100.f;
	float len1 = (msh.pVertices[Indx0] - msh.pVertices[Indx1]).Magnitude();
	float len2 = (msh.pVertices[Indx0] - msh.pVertices[Indx2]).Magnitude();

    CalEqCoef(msh, Indx0, Indx1, Indx2);

    if (m_lstEqCoef[0] != 0)
    {
        CalQuadEq(msh, Indx0, Indx1, Indx2);       
    }
    if (m_lstEqCoef[0] == 0 && m_lstEqCoef[1] != 0)
    {
        ux = - m_lstEqCoef[2] / m_lstEqCoef[1];

		bool flag = UpWind(msh, ux, Indx0, Indx1, Indx2);
		float geodis = std::max(m_lstGeodDisNode[Indx1].dist, m_lstGeodDisNode[Indx2].dist);

		if(flag && ux > geodis)
        {
            m_lstGeodDisNode[Indx0].dist = m_lstGeodDisNode[Indx0].dist > ux ? ux : m_lstGeodDisNode[Indx0].dist;
        }
		else
        {
            minDist = std::min(m_lstGeodDisNode[Indx2].dist + len2, m_lstGeodDisNode[Indx1].dist + len1);                  
            m_lstGeodDisNode[Indx0].dist = m_lstGeodDisNode[Indx0].dist > minDist ? minDist : m_lstGeodDisNode[Indx0].dist;
        }
    }
    if (m_lstEqCoef[0] == 0 && m_lstEqCoef[1] == 0)
    {
        minDist = std::min(m_lstGeodDisNode[Indx2].dist + len2, m_lstGeodDisNode[Indx1].dist + len1);                  
        m_lstGeodDisNode[Indx0].dist = m_lstGeodDisNode[Indx0].dist > minDist ? minDist : m_lstGeodDisNode[Indx0].dist;
    }
}

void VBGeodicMesh::UpdateType(const VSGraphMesh & msh, const unsigned & vidx)
{
    //更新dist最小的顶点的相邻状态为far的顶点为open
    unsigned m = msh.GetEdgeCountOfVetex(vidx);
    m_lstNeigbNode.clear();
    for (unsigned j = 0; j < m; j ++)
    {
        unsigned k = msh.GetEdgeIndxOfVertex(vidx, j);

        const VNVECTOR2UI ei = msh.GetVertIndxOfEdge(k);
        for (unsigned n = 0; n < 2; n ++)
        {
            if (m_lstGeodDisNode[ei[n]].nodeType != 2)
            {
                m_lstNeigbNode.push_back(ei[n]);
            }
        }
    }
}

void VBGeodicMesh::CalGeodDist( const VSGraphMesh & msh, const VSConstBuffer< unsigned > & cd )
{
	std::vector<geodDisNode> p;
	p.clear();
	p.resize(msh.GetVertexCount());
	m_lstGeodDisNode[cd.pData[0]].path = cd.pData[0];    //起始点的源记录为其本身索引
	p[0] = m_lstGeodDisNode[cd.pData[0]];
	heap<geodDisNode, minfloatCompare> H(&p[0], 1, msh.GetVertexCount());	
	
    if (cd.len > 1)
    {
        for (unsigned i = 1; i < cd.len; i ++)
        {
			m_lstGeodDisNode[cd.pData[i]].path = cd.pData[i];
            H.insert(m_lstGeodDisNode[cd.pData[i]]);            
        }
    }

	unsigned count = 0;
    while (count != m_lstGeodDisNode.size())
    {
        //更新dist最小的顶点状态为dead
		if (p[0].nodeType != 2)
        {
            p[0].nodeType = 2;
			
            m_lstGeodDisNode[p[0].Index].nodeType = 2;
            if (p[0].dist > m_lstGeodDisNode[p[0].Index].dist)
            {
                p[0].dist = m_lstGeodDisNode[p[0].Index].dist;
				p[0].path = m_lstGeodDisNode[p[0].Index].path;
            }
        }      

		UpdateType(msh, p[0].Index);

        //更新状态为open的顶点的距离值
        if (m_lstNeigbNode.size() > 0)
        {
            for (unsigned i = 0; i < m_lstNeigbNode.size(); i ++)
            {
				unsigned vidx0 = VD_INVALID_INDEX, vidx1 = VD_INVALID_INDEX, vidx2 = VD_INVALID_INDEX;
				int pos = VD_INVALID_INDEX;
                vidx0 = m_lstNeigbNode[i];

                if (vidx0 != p[0].Index)
                {
                    unsigned sNum = msh.GetSurfCountOfVetex(vidx0);
                    for (unsigned j = 0; j < sNum; j ++)
                    {
                        unsigned sidx = msh.GetSurfIndxOfVertex(vidx0, j);
                        const VNVECTOR3UI &sivt = msh.pSurfaces[sidx];

						if (m_lstGeodDisNode[ msh.pSurfaces[sidx][0] ].nodeType != 2 &&
							m_lstGeodDisNode[ msh.pSurfaces[sidx][1] ].nodeType != 2 &&
							m_lstGeodDisNode[ msh.pSurfaces[sidx][2] ].nodeType != 2)
							continue;
                        else
                        {
                            for (unsigned k = 0; k < 3; k ++)
                            {
                                if (vidx0 == msh.pSurfaces[sidx][k])
                                {
                                    pos = k;
									break;
                                }
                            }
                        }
						vidx1 = msh.pSurfaces[sidx][(pos + 1) % 3];
						vidx2 = msh.pSurfaces[sidx][(pos + 2) % 3];

						bool vState1 = (m_lstGeodDisNode[vidx1].nodeType == 2 && m_lstGeodDisNode[vidx1].path == m_lstGeodDisNode[p[0].Index].path);
						bool vState2 = (m_lstGeodDisNode[vidx2].nodeType == 2 && m_lstGeodDisNode[vidx2].path == m_lstGeodDisNode[p[0].Index].path);

						float minDist = 100.f;
						if (vState1 && vState2)
                        {
							minDist = m_lstGeodDisNode[vidx0].dist;

                            CalEq(msh, vidx0, vidx2, vidx1);	
							if (m_lstGeodDisNode[vidx0].dist < minDist)
							{
								m_lstGeodDisNode[vidx0].path = (m_lstGeodDisNode[vidx0].dist == 0.f ? m_lstGeodDisNode[vidx0].path : p[0].path);
								m_lstGeodDisNode[vidx0].nodeType = 1;
							}														
                        }
						else if (vState1 && !vState2)
						{
							minDist = m_lstGeodDisNode[vidx1].dist +
								     (msh.pVertices[vidx1] - msh.pVertices[vidx0]).Magnitude();
							if (m_lstGeodDisNode[vidx0].dist > minDist)
							{
								m_lstGeodDisNode[vidx0].dist = minDist;
								m_lstGeodDisNode[vidx0].path = (m_lstGeodDisNode[vidx0].dist == 0.f ? m_lstGeodDisNode[vidx0].path : p[0].path);
								m_lstGeodDisNode[vidx0].nodeType = 1;
							}
						}
						else if (!vState1 && vState2)
						{
							minDist = m_lstGeodDisNode[vidx2].dist +
								     (msh.pVertices[vidx2] - msh.pVertices[vidx0]).Magnitude();
							if (m_lstGeodDisNode[vidx0].dist > minDist)
							{
								m_lstGeodDisNode[vidx0].dist = minDist;
								m_lstGeodDisNode[vidx0].path = (m_lstGeodDisNode[vidx0].dist == 0.f ? m_lstGeodDisNode[vidx0].path : p[0].path);
								m_lstGeodDisNode[vidx0].nodeType = 1;
							}							
						}
                    }					
                }
                //避免在堆中重复插入元素
				pos = VD_INVALID_INDEX;
				for (int i = 0; i < H.size(); i++)
				{
					if (p[i].Index == vidx0)
					{
						pos = i;
						break;
					}
				}
				if (pos >= 0 && pos < H.size())
					H.remove(pos);
				H.insert(m_lstGeodDisNode[vidx0]);             
            } 
        }		
        //m_lstTemp.push_back(H.removefirst());
		H.removefirst();
		count++;
    }   
   //sort(m_lstTemp.begin(), m_lstTemp.end(), MySort);
}

VD_EXPORT_SYSTEM_SIMPLE(VBGeodicMesh, VNALGMESH::VRGeodicMesh);
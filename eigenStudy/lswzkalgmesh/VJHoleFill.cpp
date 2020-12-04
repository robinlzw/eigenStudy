#include "stdafx.h"
#include "VJHoleFill.h"
#include "VGraphAndHeap.h" 

//struct FillResult
//{
//	VSConstBuffer< VFVECTOR3   > newvertice;
//	VSConstBuffer< VNVECTOR3UI > triangles ;
//};
//
//void VJHoleFill::Build( FillResult & meshOut, const VSConstBuffer< unsigned > & cbf, const VSConstBuffer< VFVECTOR3 , VFVECTOR3 > & vInfo ) 
//{
//	
//}

//void VJHoleFill::Build(VSSimpleMeshF & meshOut, const VSConstBuffer< unsigned > & cbf, const VSPerfectMesh & mesh)
void VJHoleFill::Build(VSFillResult & meshOut, const VSConstBuffer< unsigned > & cbf, const VSConstBuffer< VFVECTOR3, VFVECTOR3 > & vInfo)
{	 
    m_vVertices.clear();
    m_vSurf    .clear();
    m_vNbrVert .clear();
    m_vVertHeap.clear();
    m_vTemp    .clear();

	Init(cbf, vInfo);
	FillBoundHole(cbf, vInfo);

	//�������һ�£�������Ƭ������ת����
	/*
	for (unsigned i = 0; i < m_vSurf.size(); i++)
	{
	    std::swap(m_vSurf[i][0], m_vSurf[i][2]);		
	}*/		
	
	meshOut.newvertice.len   = m_vVertices.size() - vInfo.len ;
	if (meshOut.newvertice.len == 0)
		meshOut.newvertice.pData = nullptr                    ;
	else
		meshOut.newvertice.pData = &m_vVertices[vInfo.len]    ;
	meshOut.triangles.len    = m_vSurf.size()                 ;
	meshOut.triangles.pData  = &m_vSurf[0]                    ;
	//SmoothNewVert(mesh, cbf);  	
}

void VJHoleFill::Init(const VSConstBuffer<unsigned> & cbf, const VSConstBuffer< VFVECTOR3, VFVECTOR3 > & vInfo) //, const VSPerfectMesh & mesh
{
	m_vNbrVert.resize(cbf.len);
	//��һ������	
	m_vNbrVert[0].x = cbf.pData[1];    //x����ǰ�ڵ�i����һ���ڽӵ�  y����ǰ�ڵ�i��ǰһ���ڽӵ�
	m_vNbrVert[0].y = cbf.pData[cbf.len - 1];

	for (unsigned i = 1; i < cbf.len - 1; i++)
	{
		m_vNbrVert[i].x = cbf.pData[i + 1];
		m_vNbrVert[i].y = cbf.pData[i - 1];
	}

	//���һ������
	m_vNbrVert[cbf.len - 1].x = cbf.pData[0];
	m_vNbrVert[cbf.len - 1].y = cbf.pData[cbf.len - 2];
	
	m_vVertices.resize(vInfo.len);
	memcpy(&m_vVertices.front(), vInfo.pData1, sizeof(VFVECTOR3) * vInfo.len);
	//for (unsigned i = 0; i < vInfo.len; i++)
	//{
	//	m_vVertices.push_back(vInfo.pData1[i]);
	//}
}

float VJHoleFill::CalcAverLen(const VSConstBuffer<unsigned> & cbf, const VSConstBuffer< VFVECTOR3, VFVECTOR3 > & vInfo)
{
    float sum = 0.f;
	VFVector3 temp1, temp2;
	for (unsigned i = 0; i < cbf.len - 1; i++)
	{
		temp1 = vInfo.pData1[cbf.pData[i]];
		temp2 = vInfo.pData1[cbf.pData[i + 1]];

		sum += (temp2 - temp1).Magnitude();
	}
	temp1 = vInfo.pData1[cbf.pData[cbf.len - 1]];
	temp2 = vInfo.pData1[cbf.pData[0]];
	sum += (temp2 - temp1).Magnitude();
    
	return sum / static_cast<float>(cbf.len);
}

float VJHoleFill::CalcVertAngle(const VSConstBuffer<unsigned> & cbf, const VSConstBuffer< VFVECTOR3, VFVECTOR3 > & vInfo, const unsigned & n)       //�������Ϊn�Ķ��������ڽӵ�ļн�
{
	VFVector3 Vec0 = vInfo.pData1[ m_vNbrVert[n][0] ] - vInfo.pData1[ cbf.pData[n] ];
	VFVector3 Vec1 = vInfo.pData1[ m_vNbrVert[n][1] ] - vInfo.pData1[ cbf.pData[n] ];

    VFVector3 vCross = Vec0.Cross(Vec1);            //���˳���밼͹����ж����
	VFVector3 vNormal = vInfo.pData2[cbf.pData[n]];

	float vDot = Vec0.Dot(Vec1);
	float vMagnitude = Vec0.Magnitude() * Vec1.Magnitude();
	float acosValue = vDot / vMagnitude;
	/*acosValue = (acosValue > 1.f) ? 1.f : acosValue;
	acosValue = (acosValue < -1.f) ? -1.f : acosValue;*/

    /*float vAngl = VFMath::ACos(acosValue).valueRadians();	
    if (vCross.Dot(vNormal) > 0)
    {
        vAngl = VF_DBL_PI - vAngl;
    }*/
	float vAngl = VFMath::ACos(acosValue).valueDegrees();
	if (vCross.Dot(vNormal) < 0)
	{
		vAngl = 360.f - vAngl;
	}

    return vAngl;
}

unsigned VJHoleFill::FindPos(const unsigned & vidx, const unsigned & size)
{
	unsigned pos = VD_INVALID_INDEX;
	for (unsigned i = 0; i < size; i++)
	{
		if (m_vVertHeap[i].vidx == vidx)
		{
			pos = i;
			return pos;
		}
	}
	return pos;
}

void VJHoleFill::UpdateNbrVertAngle(const unsigned & m, const unsigned & vidx0, const unsigned & vidx1, const unsigned & vidx2)
{
	VFVector3 Vec0 = m_vVertices[vidx1] - m_vVertices[vidx0];
	VFVector3 Vec1 = m_vVertices[vidx2] - m_vVertices[vidx0];
	float v0 = Vec0.Dot(Vec1);
	float v1 = Vec0.Magnitude() * Vec1.Magnitude();
	float value = v0 / v1;
	//value = value > 1.f ? 1.f : value;
	//value = value < -1.f ? -1.f : value;     //Խ����
	//float vexAngl = VFMath::ACos(value).valueRadians() ; 
	float vexAngl = VFMath::ACos(value).valueDegrees();

	m_vVertHeap[m].angle = m_vVertHeap[m].angle - vexAngl;
}

void VJHoleFill::FillBoundHole(const VSConstBuffer<unsigned> & cbf, const VSConstBuffer< VFVECTOR3, VFVECTOR3 > & vInfo)
{
    VertAngle temp;

	m_vVertHeap.clear();
	m_vVertHeap.resize(5 * cbf.len);

    heap<VertAngle, DComp>  H(&m_vVertHeap[0], 0, 5 * cbf.len);

    for (unsigned i = 0; i < cbf.len; i ++)
    {
        temp.angle  = CalcVertAngle(cbf, vInfo, i);
        temp.nbrVet = m_vNbrVert[i];
        temp.vidx   = cbf.pData[i];
        H.insert(temp);
    }

	float averDis = CalcAverLen(cbf, vInfo);
    VNVECTOR3UI tri;
	//unsigned vCount = mesh.GetVertexCount();
	unsigned vCount = vInfo.len;
    while (H.size() > 3)
    {
		unsigned vidx0 = m_vVertHeap[0].nbrVet[0];
		unsigned vidx1 = m_vVertHeap[0].nbrVet[1];
        float d = (m_vVertices[vidx0] - m_vVertices[vidx1]).Magnitude();
		unsigned vidx2 = VD_INVALID_INDEX;

        if (d < 2 * averDis)
        {
            tri[0] = m_vVertHeap[0].vidx;
            tri[1] = m_vVertHeap[0].nbrVet[0];
            tri[2] = m_vVertHeap[0].nbrVet[1];
            m_vSurf.push_back(tri);

            //���¶�����ڽӵ�
			unsigned v0 = VD_INVALID_INDEX, v1 = VD_INVALID_INDEX;
			v0 = FindPos(tri[1], H.size());
			m_vVertHeap[v0].nbrVet[1] = m_vVertHeap[0].nbrVet[1];
			v1 = FindPos(tri[2], H.size());
			m_vVertHeap[v1].nbrVet[0] = m_vVertHeap[0].nbrVet[0];
            //���������ڽӵ�н�			
            UpdateNbrVertAngle(v0, tri[1], tri[0], tri[2]);			
            UpdateNbrVertAngle(v1, tri[2], tri[0], tri[1]);
			vidx0 = m_vVertHeap[v0].vidx;
			vidx1 = m_vVertHeap[v1].vidx;
			vidx2 = m_vVertHeap[0].vidx;
			v0 = FindPos(vidx0, H.size());
			temp = H.remove(v0);
			H.insert(temp);
			v1 = FindPos(vidx1, H.size());			
			temp = H.remove(v1);
			H.insert(temp);
			v0 = FindPos(vidx2, H.size());
			H.remove(v0);
            //H.removefirst();
        }

        if (d >= 2 * averDis)
        {           
			VFVector3 vtmp = (m_vVertices[vidx0] + m_vVertices[vidx1]) / 2.f;
            m_vVertices.push_back(vtmp);

            tri[0] = m_vVertHeap[0].vidx;
			tri[1] = m_vVertHeap[0].nbrVet[0];
			tri[2] = vCount;                      
            m_vSurf.push_back(tri);
			//���¶�����ڽӵ�
			unsigned v0 = VD_INVALID_INDEX, v1 = VD_INVALID_INDEX;
			v0 = FindPos(tri[1], H.size());
			m_vVertHeap[v0].nbrVet[1] = vCount;
			UpdateNbrVertAngle(v0, tri[1], tri[0], tri[2]);

            tri[0] = m_vVertHeap[0].vidx;
			tri[1] = vCount;
			tri[2] = m_vVertHeap[0].nbrVet[1];                       
            m_vSurf.push_back(tri);
            //���¶�����ڽӵ�
			v1 = FindPos(tri[2], H.size());
			m_vVertHeap[v1].nbrVet[0] = vCount;
            //���������ڽӵ�н�
			UpdateNbrVertAngle(v1, tri[2], tri[0], tri[1]);
			
			vidx0 = m_vVertHeap[v0].vidx;
			vidx1 = m_vVertHeap[v1].vidx;
			vidx2 = m_vVertHeap[0].vidx;
			v0 = FindPos(vidx0, H.size());
			temp = H.remove(v0);
			H.insert(temp);
			v1 = FindPos(vidx1, H.size());
			temp = H.remove(v1);
			H.insert(temp);
			v0 = FindPos(vidx2, H.size());			

            temp.angle = 180.f;
            temp.vidx  = vCount;
            temp.nbrVet = m_vVertHeap[v0].nbrVet;
			H.remove(v0);                             //ȡ���ڽӵ����ɾ��
            H.insert(temp);
			vCount++;
        }
    }
    //���һ��������
    tri[0] = m_vVertHeap[0].vidx;
    tri[1] = m_vVertHeap[0].nbrVet[0];
    tri[2] = m_vVertHeap[0].nbrVet[1];
    m_vSurf.push_back(tri);	
}

//void VJHoleFill::SmoothNewVert(const VSPerfectMesh & mesh, const VSConstBuffer<unsigned> & cbf)
//{
//    //���¹������˹�ϵ
//    Rebuild(m_vVertices.size(), m_vSurf.size(), &m_vSurf[0]);
//
//    //const unsigned & nu = m_vVertices.size();
//    for (unsigned i = cbf.len; i < m_vVertices.size(); i ++)
//    {
//        unsigned en = mesh.GetEdgeCountOfVetex(i);
//        m_vTemp.clear();
//        for (unsigned j = 0; j < en; j ++)
//        {
//            unsigned ei = mesh.GetEdgeIndxOfVertex(i, j);
//            VNVECTOR2UI ev = mesh.GetVertIndxOfEdge(ei);
//            if (ev[0] != i)
//            {
//                m_vTemp.push_back(ev[0]);
//            }
//            else
//            {
//                m_vTemp.push_back(ev[1]);
//            }
//        }
//
//        unsigned tn = m_vTemp.size();
//        VFVector3 sum, vt;
//        for (unsigned k = 0; k < tn; k ++)
//        {
//            vt = m_vVertices[m_vTemp[k]];
//            sum += vt;
//        }
//        m_vVertices[i] = sum / static_cast<float>(tn) - m_vVertices[i];
//    }
//}

VD_EXPORT_SYSTEM_SIMPLE(VJHoleFill, VNALGMESH::VRHoleFill);
#pragma once

#include <vstd/mem.h>
#include <vmath/vfvector3.h>
//#include "VTopoMesh.h"
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
using namespace VNWZKALG;

/********************************************************/
/*                    �㷨����
1���õ��׶��߽���Ϣ������߽�ߵ�ƽ������aver_l
2������ÿ���߽��������ڱߵļнǴ�С
3���ҳ�������С�нǵı߽�㣬�������������ڱ߽��ľ���s��
��s>2*aver_l���������������������Σ���������һ��������
4�����±߽����Ϣ
5���жϿն��Ƿ��޲�����������ת2�����������㷨����
6������Ĺ�˳����*/
/********************************************************/

//struct FillResult
//{
//	VSConstBuffer< VFVECTOR3   > newvertice; // ������
//	VSConstBuffer< VNVECTOR3UI > triangles;  // ��������Ƭ�����ж��������Ϊ�����ɵ����������Ƭ
//};

class VJHoleFill //: public VTopoMesh
{
public:
    VJHoleFill() {}
    ~VJHoleFill(){}

public : 
	//void Build(VSSimpleMeshF & meshOut, const VSConstBuffer< unsigned > & cbf, const VSPerfectMesh & mesh);   //����Ϊ����߽������������
    // cbf �߽��λ��vInfo�е��±�
    // vInfo ������Ϣ
    // meshOut  �����㼰��������
	void VJHoleFill::Build(VSFillResult & meshOut, const VSConstBuffer< unsigned > & cbf, const VSConstBuffer< VFVECTOR3, VFVECTOR3 > & vInfo);

private : 
    struct VertAngle
    {
        unsigned     vidx;
        float        angle;
        VNVECTOR2UI  nbrVet;
    };
    
    class DComp 
    {
    public:
        static bool prior(VertAngle x, VertAngle y)
        { return x.angle < y.angle; }
    };

private:
    void   Init(const VSConstBuffer<unsigned> & cbf, const VSConstBuffer< VFVECTOR3, VFVECTOR3 > & vInfo);
	unsigned FindPos(const unsigned & vidx, const unsigned & size);
    float  CalcAverLen(const VSConstBuffer<unsigned> & cbf, const VSConstBuffer< VFVECTOR3, VFVECTOR3 > & vInfo);    
    float  CalcVertAngle(const VSConstBuffer<unsigned> & cbf, const VSConstBuffer< VFVECTOR3, VFVECTOR3 > & vInfo, const unsigned & n);
    void   UpdateNbrVertAngle(const unsigned & m, const unsigned & vidx0, const unsigned & vidx1, const unsigned & vidx2);
    void   FillBoundHole(const VSConstBuffer<unsigned> & cbf, const VSConstBuffer< VFVECTOR3, VFVECTOR3 > & vInfo);
    //void   SmoothNewVert(const VSPerfectMesh & mesh, const VSConstBuffer<unsigned> & cbf);   //

private:
    std::vector< VFVector3 >    m_vVertices;
    std::vector<VNVECTOR3UI>    m_vSurf; 
    std::vector<VNVECTOR2UI>    m_vNbrVert;
    std::vector< VertAngle >    m_vVertHeap;
    std::vector< unsigned >     m_vTemp;
};


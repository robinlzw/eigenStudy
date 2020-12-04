#pragma once

#include <vstd/mem.h>
#include <vmath/vfvector3.h>
//#include "VTopoMesh.h"
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
using namespace VNWZKALG;

/********************************************************/
/*                    算法流程
1、得到孔洞边界信息，计算边界边的平均长度aver_l
2、计算每个边界点的两相邻边的夹角大小
3、找出具有最小夹角的边界点，计算它的两相邻边界点的距离s，
若s>2*aver_l成立，则增加两个三角形，否则增加一个三角形
4、更新边界点信息
5、判断空洞是否修补完整，若否转2，若完整则算法结束
6、曲面的光顺处理*/
/********************************************************/

//struct FillResult
//{
//	VSConstBuffer< VFVECTOR3   > newvertice; // 新增点
//	VSConstBuffer< VNVECTOR3UI > triangles;  // 新增三角片，其中顶点的索引为包含旧点的所有三角片
//};

class VJHoleFill //: public VTopoMesh
{
public:
    VJHoleFill() {}
    ~VJHoleFill(){}

public : 
	//void Build(VSSimpleMeshF & meshOut, const VSConstBuffer< unsigned > & cbf, const VSPerfectMesh & mesh);   //输入为有序边界点索引和网格
    // cbf 边界点位于vInfo中的下标
    // vInfo 顶点信息
    // meshOut  新增点及网格数据
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


#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <type_traits>
#include <memory>



// ��Ŀ�����а����Ŀ⣺
/*
	eigen�ľ�̬�⣺....
	��������õ��Ĳ�֪������ľ�̬�⣺lsalgsuperlu.lib

*/




#include "vstd.h"
#pragma comment(lib, "vstd.lib")


#include "vmath.h"
#pragma comment(lib, "vmath.lib")							// vmath.lib���������ܶ��Զ���Ļ�����ѧ���͡�������Ԫ���࣬������ȡ�



#include "./Common/pkg/ldck4cce.h"
#pragma comment(lib, "ldck4cce.lib")


#include "./Common/pkg/ldwzkupdater.h"
#pragma comment(lib, "ldwzkupdater.lib")

#include "./Common/pkg/ldwxlupdater.h"
#pragma comment(lib, "ldwxlupdater.lib")


#include "./Common/obj/lsalgtools/ilsalgtools.h"
#include "./Common/obj/lsalgtools/ilsalgtoolsperfmesh.h"
#include "./Common/obj/lsalgtools/lsalgobjfile.h"
#include "./Common/obj/lsalgtools/profile.h"
#include "./Common/obj/lsalgtools/triangle.h"
#include "./Common/obj/lsalgtools/VDDenseMatrix.h"
#include "./Common/obj/lsalgtools/VDSparseMatrix.h"
#pragma comment(lib, "lsalgtools.lib")							// lsalgtools.lib�����ṩ��һЩOBJ�ļ���д�Ľӿڣ�VSMesh�࣬



#include "./Common/obj/alg/ialgobjectcontainerproxy.h"			
#pragma comment(lib, "lsalgobjectcontainerproxy.lib")			// lsalgobjectcontainerproxy.lib����ldobjectcontainer�Ĵ���⣬����VCProxy��


#include "./Common/pkg/ldobjectcontainer.h"
#pragma comment(lib, "ldobjectcontainer.lib")					// ldobjectcontainer��̬�� 





#include "./lsalgmatrix/lsalgmatrix.h"							// lsalgmatrix.lib�������Դ�����ص���Ŀ�Զ�������ݽṹ���㷨��
//#include "./common/obj/alg/ialgmatrix.h"
//#include "./lsalgmatrix/inner.h"
#pragma comment(lib, "lsalgmatrix.lib")



#include "lswzkalgmesh/VBGeodicMesh.h"
#pragma comment(lib, "lswzkalgmesh.lib")					// lswzkalgmesh.lib������������ά�����ѧ������صĺܶ�ӿڣ�����laplace����VBLaplace2DeformMat����ؾ��빦����VBGeodicMesh������



#include "lsalgcutpathgen/VCCalcLevelSet.h"
#include "lsalgcutpathgen/VCFindPointPairPath.h"
#include "lsalgcutpathgen/lsalgcutpathgen.h"					// lsalgcutpathgen.lib���������и�·����
#include "obj/alg/ialgcutpathgen.h"								// �����и�·����
#pragma comment(lib, "lsalgcutpathgen.lib")






//#include "./Common/obj/alg/ialgmeshrayintersection.h"			// �ṩ�����߲����IVMeshRayIntersection������Ŀǰ��֪�����������ĸ����
#include "./lsalgmeshrayintersection/lsalgmeshrayintersection.h"
#pragma comment(lib, "lsalgmeshrayintersection.lib")



//#include "ldteethprocess/lsalgteethprocess.h"
//#include "./Common/pkg/ldteethprocess.h"
//#pragma comment(lib, "ldteethprocess.lib")						// �����⣬�ṩ��VBCollisionSence
//



//#include "lsalgmeshsimplify2/lsalgmeshsimplify2.h"
//#pragma comment(lib, "lsalgmeshsimplify2.lib")
//



// �о�һ���ú�ʵ�ֵĶ�Persistclass�Ķ��壺
/*
		VD_PERSISTCLASS_BEGIN(persistSample)
		VD_DEFMEMBER(VSConstBuffer<VFVECTOR3>, cbCutVertices) 	
		VD_PERSISTCLASS_END();
		չ��֮��Ĵ��룺
*/
struct persistSample 
{
		static const unsigned LOOPBGN = 999;			// ������
		typedef persistSample MYTYPE;
		VSConstBuffer<VFVECTOR3> cbCutVertices;


		template< typename TP >  
		static void LoopMap(const TVHelperCounter<999> *, TP & mapper, const MYTYPE & v)			// TVHelperCounter�ǿ��ࡣ
		{
			VBMarshalling< VSConstBuffer<VFVECTOR3>, TV_HasExternalMember< VSConstBuffer<VFVECTOR3>, TP >::value >::SaveExtData(mapper, v.name); 
			LoopMap(reinterpret_cast< TVHelperCounter<999> * >(0), mapper, v); 
		}


		template< typename CP > 
		static void LoopCorrect(const TVHelperCounter<999> *, CP & r, unsigned & posEnd, const VSConstBuffer< char > & pbuff, unsigned objPos, unsigned curPos) 
		{
				VBMarshalling< VSConstBuffer<VFVECTOR3>, TV_HasPtrMember< VSConstBuffer<VFVECTOR3>, CP >::value >::CorrectPtr(r, posEnd, pbuff, objPos + offsetof(MYTYPE, cbCutVertices), curPos);
				LoopCorrect(reinterpret_cast< TVHelperCounter<999> * >(0), r, posEnd, pbuff, objPos, posEnd); 
		}

			
		template< typename TP > 
		static void MapTo(TP & mapper, const MYTYPE & ti) 
		{
				LoopMap(reinterpret_cast< TVHelperCounter< 0 > * >(0), mapper, ti); 
		}
			

		template< typename CP >
		static void CorrectPtr(CP & r, unsigned & posEnd, const VSConstBuffer< char > & pbuff, unsigned objPos, unsigned curPos) 
		{
				LoopCorrect(reinterpret_cast< TVHelperCounter< 0 > * >(0), r, posEnd, pbuff, objPos, curPos);	
		}; 
};





#define MAXLEN 1024


// ���������������������Լ�д��һЩ��������

template <typename T>
void vndisp(const VNVector3<T>& v)
{
	std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}


void v3disp(const VFVECTOR3& v);
void writeData2D(const Eigen::VectorXd& x, const Eigen::VectorXd& y, const char* filename);
void readData(Eigen::VectorXd& x, const char* filename);
void writeData3D();
void writeOBJ();
void readOBJ();

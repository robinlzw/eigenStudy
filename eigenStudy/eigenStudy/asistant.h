#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <type_traits>
#include <memory>



// 项目属性中包含的库：
/*
	eigen的静态库：....
	矩阵计算用到的不知道哪里的静态库：lsalgsuperlu.lib

*/




#include "vstd.h"
#pragma comment(lib, "vstd.lib")


#include "vmath.h"
#pragma comment(lib, "vmath.lib")							// vmath.lib——包含很多自定义的基础数学类型——如四元数类，向量类等。



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
#pragma comment(lib, "lsalgtools.lib")							// lsalgtools.lib——提供了一些OBJ文件读写的接口，VSMesh类，



#include "./Common/obj/alg/ialgobjectcontainerproxy.h"			
#pragma comment(lib, "lsalgobjectcontainerproxy.lib")			// lsalgobjectcontainerproxy.lib——ldobjectcontainer的代理库，包含VCProxy类


#include "./Common/pkg/ldobjectcontainer.h"
#pragma comment(lib, "ldobjectcontainer.lib")					// ldobjectcontainer动态库 





#include "./lsalgmatrix/lsalgmatrix.h"							// lsalgmatrix.lib——线性代数相关的项目自定义的数据结构和算法。
//#include "./common/obj/alg/ialgmatrix.h"
//#include "./lsalgmatrix/inner.h"
#pragma comment(lib, "lsalgmatrix.lib")



#include "lswzkalgmesh/VBGeodicMesh.h"
#pragma comment(lib, "lswzkalgmesh.lib")					// lswzkalgmesh.lib——包含了三维网格科学计算相关的很多接口，包括laplace矩阵VBLaplace2DeformMat，测地距离功能类VBGeodicMesh。。。



#include "lsalgcutpathgen/VCCalcLevelSet.h"
#include "lsalgcutpathgen/VCFindPointPairPath.h"
#include "lsalgcutpathgen/lsalgcutpathgen.h"					// lsalgcutpathgen.lib——计算切割路径。
#include "obj/alg/ialgcutpathgen.h"								// 计算切割路径。
#pragma comment(lib, "lsalgcutpathgen.lib")






//#include "./Common/obj/alg/ialgmeshrayintersection.h"			// 提供了射线测距类IVMeshRayIntersection？？？目前不知道被包含在哪个库里。
#include "./lsalgmeshrayintersection/lsalgmeshrayintersection.h"
#pragma comment(lib, "lsalgmeshrayintersection.lib")



//#include "ldteethprocess/lsalgteethprocess.h"
//#include "./Common/pkg/ldteethprocess.h"
//#pragma comment(lib, "ldteethprocess.lib")						// 切牙库，提供了VBCollisionSence
//



//#include "lsalgmeshsimplify2/lsalgmeshsimplify2.h"
//#pragma comment(lib, "lsalgmeshsimplify2.lib")
//



// 研究一下用宏实现的对Persistclass的定义：
/*
		VD_PERSISTCLASS_BEGIN(persistSample)
		VD_DEFMEMBER(VSConstBuffer<VFVECTOR3>, cbCutVertices) 	
		VD_PERSISTCLASS_END();
		展开之后的代码：
*/
struct persistSample 
{
		static const unsigned LOOPBGN = 999;			// ？？？
		typedef persistSample MYTYPE;
		VSConstBuffer<VFVECTOR3> cbCutVertices;


		template< typename TP >  
		static void LoopMap(const TVHelperCounter<999> *, TP & mapper, const MYTYPE & v)			// TVHelperCounter是空类。
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


// 函数声明——————自己写的一些辅助函数

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

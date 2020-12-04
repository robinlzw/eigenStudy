#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <type_traits>
#include <memory>



#include "vstd.h"
#pragma comment(lib, "vstd.lib")


#include "vmath.h"
#pragma comment(lib, "vmath.lib")							// vmath.lib���������ܶ��Զ���Ļ�����ѧ���͡�������Ԫ���࣬������ȡ�



#include "./Common/pkg/ldck4cce.h"
#pragma comment(lib, "ldck4cce.lib")


#include "./Common/pkg/ldwzkupdater.h"
#pragma comment(lib, "ldwzkupdater.lib")


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


#include "./lsalgcutpathgen/lsalgcutpathgen.h"					// lsalgcutpathgen.lib���������и�·����
#pragma comment(lib, "lsalgcutpathgen.h")

//#include "./Common/obj/alg/ialgmeshrayintersection.h"			// �ṩ�����߲����IVMeshRayIntersection������Ŀǰ��֪�����������ĸ����
#include "./lsalgmeshrayintersection/lsalgmeshrayintersection.h"
#pragma comment(lib, "lsalgmeshrayintersection.lib")



//#include "ldteethprocess/lsalgteethprocess.h"
//#include "./Common/pkg/ldteethprocess.h"
//#pragma comment(lib, "ldteethprocess.lib")						// �����⣬�ṩ��VBCollisionSence
//
//
//
//#include "lsalgmeshsimplify2/lsalgmeshsimplify2.h"
//#pragma comment(lib, "lsalgmeshsimplify2.lib")
//




//#define VD_DEFRELATION( relation_name , ... )  class VIID_##relation_name ; \
//    template<> struct IVRLTN< VIID_##relation_name > \
//    {\
//        typedef TVRelationDesc< VIID_##relation_name , __VA_ARGS__ >   DESC  ;\
//        typedef typename DESC::ID                                      ID    ;\
//        typedef typename DESC::NAME                                    NAME  ;\
//        typedef typename DESC::VALUE                                   VALUE ;\
//        typedef typename DESC::HUB                                     HUB   ;\
//    };\
//    using relation_name = IVRLTN< VIID_##relation_name >
//
//
//
//#define VD_DEFRELATION_NS( ns_name , relation_name , ... )  class VIID_##relation_name ; \
//    struct IVRLTN_##relation_name \
//    {\
//        typedef TVRelationDesc< VIID_##relation_name , __VA_ARGS__ >   DESC  ;\
//        typedef typename DESC::ID                                      ID    ;\
//        typedef typename DESC::NAME                                    NAME  ;\
//        typedef typename DESC::VALUE                                   VALUE ;\
//        typedef typename DESC::HUB                                     HUB   ;\
//    };};\
//    template<> struct IVRLTN< ns_name::VIID_##relation_name > : ns_name::IVRLTN_##relation_name {} ;\
//    namespace ns_name { using relation_name = IVRLTN< VIID_##relation_name >
//
//
//VD_DEFRELATION_NS(VNALGMESH, VRSliceMeshBoundary, VSPerfTopoGraph, VSMeshVertLine);





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

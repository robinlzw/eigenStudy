#pragma once

#include <iostream>
#include <fstream>
#include <Eigen\Dense>		// ��Ҫ��eigen��Ŀ¼��ӵ�����Ŀ¼�С�
#include <vector>
#include <string>
#include <type_traits>
#include <memory>

#include "vfvector3.h"		// �ṩ����ά�ռ�������VFVECTOR3
#include "vfmesh.h"			// �ṩ����ά����ļ������ݽṹ��
#include "vfquaternion.h"	// �ṩ����Ԫ����
#include "vflocation.h"		// �ṩ��ʵʩ�ֲ�/ȫ������ϵת������VFLocation
#include "obj.h"			// �ṩ��һ�ѽӿ��ࡣ
#include "mem.h"			// �ṩ��VSConstBuffer��
#include "ticmn.h"			// �ṩ�˴���VSConstBuffer���������ݵĺ�����
#include "pttrn.h"			// һЩ���õ�ģʽ
#include "strm.h"			// �ṩ��TVPersist, TVFilePersist, TVBuilderArgTuple����ģ��
#include "./Common/obj/tisysbak.h"		// �ṩ��TVR2BBase<>��ģ�壬TVR2B<>��ģ���
#include "./Common/obj/alg/ialgmeshrayintersection.h"		// �ṩ�����߲����IVMeshRayIntersection
#include "vfray.h"			// VFRay��
#include "./Common/obj/lsalgtools/ilsalgtoolsperfmesh.h"	// �ṩ��NM_PMMESH::VSMesh��
#include "./Common/obj/alg/ialgobjectcontainerproxy.h"		// VCProxy��
#include "./Common/pkg/ldobjectcontainer.h"



#pragma comment(lib, "lsalgtools.lib")							// �ṩ��һЩOBJ�ļ���д�Ľӿ�
#pragma comment(lib, "lsalgobjectcontainerproxy.lib")			// lsalgtools.lib��������
#pragma comment(lib, "ldobjectcontainer.lib")					// ��̬�⵼��lib




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

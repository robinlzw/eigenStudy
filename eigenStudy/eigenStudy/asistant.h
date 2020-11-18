#pragma once

#include <iostream>
#include <fstream>
#include <Eigen\Dense>		// 需要把eigen的目录添加到包含目录中。
#include <vector>
#include <string>
#include <type_traits>
#include <memory>

#include "vfvector3.h"		// 提供了三维空间向量类VFVECTOR3
#include "vfmesh.h"			// 提供了三维网格的几种数据结构。
#include "vfquaternion.h"	// 提供了四元数类
#include "vflocation.h"		// 提供了实施局部/全局坐标系转换的类VFLocation
#include "obj.h"			// 提供了一堆接口类。
#include "mem.h"			// 提供了VSConstBuffer类
#include "ticmn.h"			// 提供了处理VSConstBuffer等类型数据的函数。
#include "pttrn.h"			// 一些常用的模式
#include "strm.h"			// 提供了TVPersist, TVFilePersist, TVBuilderArgTuple等类模板
#include "./Common/obj/tisysbak.h"		// 提供了TVR2BBase<>类模板，TVR2B<>类模板等



#define VD_DEFRELATION( relation_name , ... )  class VIID_##relation_name ; \
    template<> struct IVRLTN< VIID_##relation_name > \
    {\
        typedef TVRelationDesc< VIID_##relation_name , __VA_ARGS__ >   DESC  ;\
        typedef typename DESC::ID                                      ID    ;\
        typedef typename DESC::NAME                                    NAME  ;\
        typedef typename DESC::VALUE                                   VALUE ;\
        typedef typename DESC::HUB                                     HUB   ;\
    };\
    using relation_name = IVRLTN< VIID_##relation_name >








#define MAXLEN 1024

void v3disp(const VFVECTOR3& v);

void writeData2D(const Eigen::VectorXd& x, const Eigen::VectorXd& y, const char* filename);
void readData(Eigen::VectorXd& x, const char* filename);
void writeData3D();
void writeOBJ();
void readOBJ();

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



#define MAXLEN 1024

void v3disp(const VFVECTOR3& v);

void writeData2D(const Eigen::VectorXd& x, const Eigen::VectorXd& y, const char* filename);
void readData(Eigen::VectorXd& x, const char* filename);
void writeData3D();
void writeOBJ();
void readOBJ();

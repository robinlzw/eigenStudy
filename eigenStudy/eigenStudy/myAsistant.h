#pragma once
#include <iostream>
#include <Eigen\Dense>		// ��Ҫ��eigen��Ŀ¼��ӵ�����Ŀ¼�С�
#include <vector>
#include <string>

#include "vfvector3.h"		// �ṩ����ά�ռ�������VFVECTOR3
#include "vfmesh.h"			// �ṩ����ά����ļ������ݽṹ��
#include "vfquaternion.h"	// �ṩ����Ԫ����
#include "vflocation.h"		// �ṩ��ʵʩ�ֲ�/ȫ������ϵת������VFLocation
#include "obj.h"			// �ṩ��һ�ѽӿ��ࡣ
#include "mem.h"			// �ṩ��VSConstBuffer��
#include "ticmn.h"			// �ṩ�˴���VSConstBuffer���������ݵĺ�����
#include "pttrn.h"			// һЩ���õ�ģʽ
#include "strm.h"			// �ṩ��TVPersist, TVFilePersist����ģ��

// �Լ�д��һЩ��������

inline void v3disp(const VFVECTOR3&);



#include "myAsistant.inl"





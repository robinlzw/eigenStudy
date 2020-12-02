#pragma once
#include <vector>
#include <string>
#include <iostream>



#ifdef LDOBJECTCONTAINER_EXPORTS
#define LDOBJECTCONTAINER_API  __declspec(dllexport)
#else
#define LDOBJECTCONTAINER_API  __declspec(dllimport)
#endif

#include <obj/alg/ialgobjectcontainer.h>

// �ṩ�����ɶ��ֹ��������Ľӿڣ��������߲����VSMeshRayIntersection������������VSMeshsimplify2��
/*
	�������е�caller���������˱�������Ϊ�ص�������OBJ_Create(), OBJ_Release()������
	OBJ_Create()�������ɸ��ֹ��������
	����������ɵĹ��������ο�ö������enum E_OBJ_TYPE

*/



#ifdef __cplusplus
	extern "C" {
#endif	
		LDOBJECTCONTAINER_API void* OBJ_Create(const unsigned nObjEnum);

		LDOBJECTCONTAINER_API void OBJ_Release(IVObject* pObj);
#ifdef __cplusplus
	}
#endif	

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

// 提供了生成多种功能类对象的接口，包括射线测距类VSMeshRayIntersection、精简网格类VSMeshsimplify2等
/*
	代理类中的caller函数调用了本库中作为回调函数的OBJ_Create(), OBJ_Release()函数。
	OBJ_Create()可以生成各种功能类对象。
	具体可以生成的功能类对象参考枚举类型enum E_OBJ_TYPE

*/



#ifdef __cplusplus
	extern "C" {
#endif	
		LDOBJECTCONTAINER_API void* OBJ_Create(const unsigned nObjEnum);

		LDOBJECTCONTAINER_API void OBJ_Release(IVObject* pObj);
#ifdef __cplusplus
	}
#endif	

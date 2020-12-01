#pragma once
#include "LibStatic/include/objreference.h"
#include "pmp/visualization/SurfaceMeshGL.h"
#include "pmp/algorithms/SurfaceSimplification.h"
#include "obj/lsalgtools/ilsalgtoolsperfmesh.h"
#include "../include/algmeshsimplify2.h"

//#define USETHREAD_ENABLE

namespace NMALG_MESHSIMPLIFY2
{
	enum MyTask
	{
		E_INITRESOURCE,
		E_WORK,
		E_EXIT,
	};



	// 精简网格操作类
	class VSMeshsimplify2: public NM_COMMON::VObjRef, public IVMeshsimplify2
	{
	public:
		VSMeshsimplify2(const unsigned nGuid);
		virtual ~VSMeshsimplify2(); 

		virtual void Gen(VSMeshSimplifyOut& out, const VSMeshSimplifyInput& input);

	public:	
		void InitResource();
		void DestoryResource();
		void Work();


		// simplemesh类型的网格转换为第三方库pmp库中的网格类型。
		void LoadMesh(pmp::SurfaceMeshGL& meshGL, const VSSimpleMeshF& simpMesh);



		void GetResult(const pmp::SurfaceMeshGL& mesh);
		
		NM_PMMESH::VSMesh m_outMesh;
		
		VSMeshSimplifyInput m_input;
		VSMeshSimplifyOut m_out;

		HWND    m_hWnd;
		HDC     m_hDC;
		HGLRC   m_hRC;



#ifdef USETHREAD_ENABLE
		bool m_blExit;
		HANDLE  m_Work;
		HANDLE  m_WorkDone;
		unsigned m_nTask;
	private:
		HANDLE  m_Thread;
		DWORD   m_ThreadID;
#endif
	};

}

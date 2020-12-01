#include "stdafx.h"
#include <interface/iobjreference.h>
#include "lsalgmeshsimplify2.h"

using namespace pmp;

namespace NMALG_MESHSIMPLIFY2
{
	IVObject* GetGenerator(const unsigned nGuid)
	{
		return static_cast<IVMeshsimplify2*>(new VSMeshsimplify2(nGuid));
	}

	static LRESULT CALLBACK DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		//switch ( uMsg )
		//{
		//case WM_USER:
		//    OnInit( hDlg );
		//    return TRUE;
		//default:
		//    break;
		//}
		return DefWindowProc(hDlg, uMsg, wParam, lParam);
	}
#ifdef USETHREAD_ENABLE
	static DWORD WINAPI ThreadFunc(LPVOID lpThreadParameter)
	{
		VSMeshsimplify2 & obj = *(VSMeshsimplify2 *)lpThreadParameter;
		
		while (false == obj.m_blExit)
		{
			::WaitForSingleObject(obj.m_Work, INFINITE);
			switch (obj.m_nTask)
			{
				case E_INITRESOURCE:
					obj.InitResource();
					break;
				case E_WORK:
					obj.Work();					
					break;
				case E_EXIT:
					obj.DestoryResource();
					break;
				default:
					return 1;
			}
			SetEvent(obj.m_WorkDone);
		}
		return 1;
	}
#endif // 
	
	
	VSMeshsimplify2::VSMeshsimplify2(const unsigned nGuid) : NM_COMMON::VObjRef(nGuid)
	{
#ifdef USETHREAD_ENABLE
		m_blExit = false;
		m_Work = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_WorkDone = CreateEvent(NULL, FALSE, FALSE, NULL);

		m_Thread = CreateThread(NULL, 0, ThreadFunc, this, 0, &m_ThreadID);
		m_nTask = E_INITRESOURCE;
		SetEvent(m_Work);
		::WaitForSingleObject(m_WorkDone, INFINITE);
#else
		InitResource();
#endif		
	}


	VSMeshsimplify2::~VSMeshsimplify2()
	{
#ifdef USETHREAD_ENABLE
		m_blExit = true;
		m_nTask = E_EXIT;
		DWORD dwExitCode = VD_INVALID_INDEX;
		GetExitCodeThread(m_Thread, &dwExitCode);
		if (0 == dwExitCode)
		{
			dwExitCode = GetLastError();
		}
		else
		{
			SetEvent(m_Work);
			::WaitForSingleObject(m_WorkDone, INFINITE);
		}
		CloseHandle(m_Work);
		CloseHandle(m_WorkDone);
#else
		DestoryResource();
#endif
	}
	void VSMeshsimplify2::InitResource()
	{
		HINSTANCE hInstance = GetModuleHandle(nullptr);
		std::wstring szTitle = L"SimplifyMesh";
		std::wstring szWindowClass = L"SimplifyMeshClass";		

		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DialogProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = nullptr;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = L"";
		wcex.lpszClassName = szWindowClass.c_str();
		wcex.hIconSm = nullptr; 
		RegisterClassEx(&wcex);

		m_hWnd = CreateWindow(szWindowClass.c_str(), szTitle.c_str(), WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
																																							//SetWindowLongPtr( m_hWnd , GWL_USERDATA , ( LONG_PTR ) ( this ) );
																																							//SendMessage( m_hWnd , WM_USER , 0 , 0 );

		m_hDC = GetDC(m_hWnd);

		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),	                           // size of this pfd
			1,								                               // version number
			PFD_DRAW_TO_WINDOW |			                               // support window
			PFD_SUPPORT_OPENGL |			                               // support OpenGL
			PFD_SWAP_EXCHANGE |
			PFD_SUPPORT_COMPOSITION |
			//PFD_DOUBLEBUFFER ,				                               // double buffered
			PFD_TYPE_RGBA,                                                 // RGBA type
			24,                                                            // 24-bit color depth
			0, 0, 0, 0, 0, 0,                                              // color bits ignored
			8,                                                             // no alpha buffer
			0,                                                             // shift bit ignored
			0,                                                             // no accumulation buffer
			0, 0, 0, 0,                                                    // accum bits ignored
			16,                                                            // 16-bit z-buffer
			8,                                                             // 8-bit Stencil buffer
			0,                                                             // no auxiliary buffer
			PFD_MAIN_PLANE,                                                // main layer
			0,                                                             // reserved
			0, 0, 0                                                        // layer masks ignored
		};
		DWORD dwThreadID = GetCurrentThreadId();
		int pixelformat = ChoosePixelFormat(m_hDC, &pfd);
		SetPixelFormat(m_hDC, pixelformat, &pfd);

		m_hRC = wglCreateContext(m_hDC);
		wglMakeCurrent(m_hDC, m_hRC);

		// now that we have a GL context, initialize GLEW
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			std::string strTxt = "Error initializing GLEW: ";
			strTxt += (char*)glewGetErrorString(err);
			MessageBoxA(NULL, strTxt.c_str(), "Smartee", MB_OK);
			exit(1);
		}

		// debug: print GL and GLSL version
		//std::cout << "GLEW   " << glewGetString(GLEW_VERSION) << std::endl;
		//std::cout << "GL     " << glGetString(GL_VERSION) << std::endl;
		//std::cout << "GLSL   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
		// call glGetError once to clear error queue
		glGetError();
	}


	void VSMeshsimplify2::DestoryResource()
	{
		DWORD dwThreadID = GetCurrentThreadId();
		wglMakeCurrent(nullptr, nullptr);
		if (m_hRC)
		{
			wglDeleteContext(m_hRC);
		}
		if (m_hDC)
		{
			::ReleaseDC(m_hWnd, m_hDC);
		}
		if (m_hWnd)
		{
			DestroyWindow(m_hWnd);
		}
	}


	void VSMeshsimplify2::Gen(VSMeshSimplifyOut& out, const VSMeshSimplifyInput& input)
	{
		if (input.simpMesh.nVertCount <= input.nTargVertCount)
		{
			PMCopyMesh(m_outMesh, input.simpMesh);
			PMGetSimpMesh(out.simpMesh, m_outMesh);
			return;
		}
		m_input = input;
#ifdef USETHREAD_ENABLE
		m_nTask = E_WORK;
		SetEvent(m_Work);
		::WaitForSingleObject(m_WorkDone, INFINITE);
#else
		Work();
#endif
		out = m_out;
	}

	void VSMeshsimplify2::Work()
	{	
		int normal_deviation = 180;		
		int aspect_ratio = 10;
		SurfaceMeshGL mesh_;
		mesh_.clear();
		LoadMesh(mesh_, m_input.simpMesh);
		SurfaceSimplification ss(mesh_);
		ss.initialize(aspect_ratio, 0.0, 0.0, normal_deviation, 0.0);
		ss.simplify(m_input.nTargVertCount);
		// re-compute face and vertex normals
		mesh_.update_opengl_buffers();
		GetResult(mesh_);
		// 运来第一个参数是m_input.simpMesh，应该是写错了。
		PMGetSimpMesh(m_out.simpMesh, m_outMesh);		
	}

	void VSMeshsimplify2::LoadMesh(SurfaceMeshGL& mesh, const VSSimpleMeshF& simpMesh)
	{
		for (size_t nVIdx = 0; nVIdx < simpMesh.nVertCount; nVIdx++)
		{
			const VFVECTOR3& v = simpMesh.pVertices[nVIdx];
			mesh.add_vertex(Point(v.x, v.y, v.z));
		}
					
		for (size_t nSIdx = 0; nSIdx < simpMesh.nTriangleCount; nSIdx++)
		{
			const VNVECTOR3UI& s = simpMesh.pTriangles[nSIdx];
			std::vector<Vertex> vertices;
			for (size_t i = 0; i < 3; i++)
			{
				vertices.emplace_back(s[i]);
			}
			mesh.add_face(vertices);
		}	
		mesh.update_opengl_buffers();
	}

	void VSMeshsimplify2::GetResult(const SurfaceMeshGL& mesh)
	{		
		//vertices		
		VertexProperty<Point> points = mesh.get_vertex_property<Point>("v:point");
		m_outMesh.vVertice.resize(mesh.n_vertices());
		unsigned nCount = 0;
		for (SurfaceMesh::VertexIterator vit = mesh.vertices_begin();
			vit != mesh.vertices_end(); ++vit)
		{
			const Point& p = points[*vit];			
			m_outMesh.vVertice[nCount++] = VFVECTOR3(p[0], p[1], p[2]);
		}
		
		//faces
		m_outMesh.vSurface.resize(mesh.n_faces());
		nCount = 0;
		for (SurfaceMesh::FaceIterator fit = mesh.faces_begin();
			fit != mesh.faces_end(); ++fit)
		{
			
			SurfaceMesh::VertexAroundFaceCirculator fvit = mesh.vertices(*fit);
			for (size_t i = 0; i < 3; i++)
			{
				m_outMesh.vSurface[nCount][i] = (*fvit).idx();
				++fvit;
			}
			nCount++;
		}
	}
}

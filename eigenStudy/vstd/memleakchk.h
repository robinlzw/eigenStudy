#pragma once

#ifdef _V_WITHOUT_USEMEM_CHECK

#else
#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#ifdef WIN32
#include <crtdbg.h>
#endif
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif

#ifdef  _DEBUG
#ifndef _DLL
#define _V_NEED_MEM_LEAK_CHECK_CODE
#endif
#endif

#ifdef _V_NEED_MEM_LEAK_CHECK_CODE
    #define VM_ENABLE_MEM_LEAK_CHECK() ( _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF) ) 
    #define VM_BREAK_MEM_ALLOC( a ) ( _CrtSetBreakAlloc(a) )
    //#define VM_TRACE_MEM_LEAK() atexit( vfExitWithMemoryLeakCheck ) ;
#else // _DEBUG
    #define VM_ENABLE_MEM_LEAK_CHECK() 
    #define VM_BREAK_MEM_ALLOC( a ) 
    //#define VM_TRACE_MEM_LEAK() 
#endif // _DEBUG

#endif
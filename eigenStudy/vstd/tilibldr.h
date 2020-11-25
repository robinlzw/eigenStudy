#pragma once

// 定义了一堆和库输出相关的宏和函数

#include "ticmn.h" 
#include "memleakchk.h"
#ifdef WIN32




class VLibLoader 
{
public :
    template< class F >
    static void UseStdLiberary( const wchar_t * lpLibName , F func )
    {
        std::wstring path( lpLibName ) ;
		path += L".dll";
        UseLiberary<F>( path.c_str() , func ) ;
    }

private :
    template< class F >
    static void UseLiberary( const wchar_t * lpLibName , F func )
    {
        typedef typename VCMN::function_traits< decltype( func ) >::arg_type argreftype  ;
        typedef typename std::remove_reference< argreftype >::type argtype ;
        typedef void  ( MV_STDCALL * USELIB )( IVUser< argtype > & usr ) ;

        HMODULE hlib = LoadLibrary( lpLibName ) ;

        argtype * p = 0;
        if( hlib )
        {
            USELIB proc = (USELIB)GetProcAddress( hlib , "UseLiberary" ) ;
            
            if( proc )
                proc( VCMN::TVLambdaUser< F >( func ) ) ;

            FreeLibrary( hlib ) ;
        } 
		else
		{
			DWORD dwErr = GetLastError();
			HLOCAL LocalAddress = NULL;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL, dwErr, 0, (PTSTR)&LocalAddress, 0, NULL);
			
			TCHAR szBuf[256] = { 0 };
			wsprintf(szBuf, L"Load library %s failed\r\nerror code: %d, err description: %s", lpLibName, dwErr, (PTSTR)LocalAddress);
			MessageBox(NULL, szBuf, L"Smartee", MB_OK);
		}
    }
} ;



class VLibLoader2
{
public:
    template< class F , typename GParam >
    static void UseStdLiberary( const wchar_t * lpLibName , F func , GParam & gparam )
    {
        std::wstring path( lpLibName );
//#ifdef _DEBUG 
//        path += L"d.dll";
//#else 
//        path += L".dll";
//#endif 
		path += L".dll";
        UseLiberary<F>( path.c_str() , func , gparam );
    }

private:
    template< class F , typename GParam >
    static void UseLiberary( const wchar_t * lpLibName , F func , GParam & gparam )
    {
        typedef typename VCMN::function_traits< decltype( func ) >::arg_type argreftype;
        typedef typename std::remove_reference< argreftype >::type argtype;
        typedef void  ( MV_STDCALL * USELIB )( IVUser< argtype > & , GParam & );

        HMODULE hlib = LoadLibrary( lpLibName );

        argtype * p = 0;
        if ( hlib )
        {
            USELIB proc = ( USELIB ) GetProcAddress( hlib , "UseLiberary" );

            if ( proc )
                proc( VCMN::TVLambdaUser< F >( func ) , gparam );

            FreeLibrary( hlib );
        }
    }
};




template< class FUse , class F >
inline void VFUseStaticLiberary( FUse fu , F func )
{
    //typedef VCMN::function_traits< decltype( func ) >::arg_type argreftype  ;
    //typedef std::remove_reference< argreftype >::type argtype ;
    //typedef void  ( * USELIB )( IVUser< argtype > & usr ) ;

    //auto a = VCMN::TVLambdaUser< F >( func ) ;
    fu( VCMN::TVLambdaUser< F >( func ) ) ;
}




template< class FUse , class F , typename GParam >
inline void VFUseStaticLiberary2( FUse fu , F func , GParam & gparam )
{
    fu( VCMN::TVLambdaUser< F >( func ) , gparam );
}
#endif

	


#ifdef _WINDLL
#define VD_DECLEARDLIBERARY( libName , interfName )  
#define VD_DECLEARDLIBERARY2( libName , interfName , gloName )


#ifdef _V_WITHOUT_USEMEM_CHECK
#define VD_IMPLEMENTLIBERARY( libName ) \
BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved ) \
{ \
    switch (ul_reason_for_call) \
    { \
    case DLL_PROCESS_ATTACH: \
    case DLL_THREAD_ATTACH: \
    case DLL_THREAD_DETACH: \
    case DLL_PROCESS_DETACH: \
        break; \
    } \
    return TRUE; \
}  
#else
#define VD_IMPLEMENTLIBERARY( libName ) \
BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved ) \
{ \
    switch (ul_reason_for_call) \
    { \
    case DLL_PROCESS_ATTACH: \
        VM_ENABLE_MEM_LEAK_CHECK();  \
        break; \
    case DLL_THREAD_ATTACH: \
    case DLL_THREAD_DETACH: \
    case DLL_PROCESS_DETACH: \
        break; \
    } \
    return TRUE; \
}
#endif



//	动态库导入\导出函数的头部——extern"C" __declspec(dllexport) void __stdcall UseLiberary(IVUser<参数2>& usr)
#define VD_LIBERARYEXPORT( libName , interfName ) extern "C" MV_EXPORT_API void MV_STDCALL UseLiberary( IVUser< interfName > & usr ) 



#define VD_LIBERARYEXPORT2( libName , interfName , gloName ) extern "C" MV_EXPORT_API void MV_STDCALL UseLiberary( IVUser< interfName > & usr , gloName & gparam ) 


#ifdef _DEBUG
#define VD_GETLIBMODULE( libName ) GetModuleHandle( L#libName L"d.dll" )
#else
#define VD_GETLIBMODULE( libName ) GetModuleHandle( L#libName L".dll" )
#endif
#else
#define VD_LIBERARYEXPORT( libName , interfName ) void UseLiberary_##libName( IVUser< interfName > & usr )
#define VD_LIBERARYEXPORT2( libName , interfName , gloName ) void UseLiberary_##libName( IVUser< interfName > & usr , gloName & gparam )
#define VD_DECLEARDLIBERARY( libName , interfName ) extern void UseLiberary_##libName( IVUser< interfName > & usr ) ; 
#define VD_DECLEARDLIBERARY2( libName , interfName , gloName ) extern void UseLiberary_##libName( IVUser< interfName > & usr , gloName & gparam ) ; 
#ifdef WIN32
#define VD_IMPLEMENTLIBERARY( libName ) __declspec( dllexport ) void getRidOfLNK4221_##libName(){} 
#else
#define VD_IMPLEMENTLIBERARY( libName ) void getRidOfLNK4221_##libName(){} 
#endif
#define VD_GETLIBMODULE( libName ) GetModuleHandle( 0 )  
#endif





#ifdef VUSESTATICLIB
#define VD_USESTDLIBERRARY( libName , ... ) VFUseStaticLiberary( UseLiberary_##libName , __VA_ARGS__ )
#define VD_USESTDLIBERRARY2( libName , ... ) VFUseStaticLiberary2( UseLiberary_##libName , __VA_ARGS__ )
#else
#define VD_USESTDLIBERRARY( libName , ... ) VLibLoader::UseStdLiberary( L###libName , __VA_ARGS__ ) 
#define VD_USESTDLIBERRARY2( libName , ... ) VLibLoader2::UseStdLiberary( L###libName , __VA_ARGS__ ) 
#endif
 



template< typename TLib , const wchar_t * libname >
struct VSLibInfo
{
    typedef TLib LibType;
};




template< typename TLoadLibInfo , typename ... TNeedLibs >
struct VSLib
{
    //static_assert( 0 , "TLoadLibInfo must be VSLibInfo< TLib , libname > style." );
};




template< typename TLib , const wchar_t * libname , typename ... TNeedLibs >
struct VSLib< VSLibInfo< TLib , libname > , TNeedLibs... >
{
    typedef VSLibInfo< TLib , libname > InfoType;
};

template< typename TLoadLibInfo , typename TNeedLib , typename ... TNeedLibs >
struct VSLib< TLoadLibInfo , TNeedLib , TNeedLibs... > : public VSLib< TLoadLibInfo , TNeedLibs... >
{
    //static_assert( 0 , "TLoadLibInfo must be VSLibInfo< TLib , libname > style." );
};

template< typename TLib , const wchar_t * libname , typename TNeedLib , typename ... TNeedLibs >
struct VSLib< VSLibInfo< TLib , libname > , TNeedLib , TNeedLibs... > : public VSLib< VSLibInfo< TLib , libname > , TNeedLibs... >
{
};




class VLibLoaderLevel
{
public:
    template< typename TLib >
    static void LoadLib( TLib & libinfo , IVUser< typename TLib::InfoType::LibType > & usr )
    {
        _LoadLib( libinfo , usr );
    }

private:
    template< typename TLib , const wchar_t * libname , typename ... TDepLibs >
    static void _RealLoad( VSLibInfo< TLib , libname > & libinfo , IVUser< TLib > & usr , TDepLibs&... deplibs )
    {
        std::wstring path( libname );
#ifdef _DEBUG 
        path += L"d.dll";
#else 
        path += L".dll";
#endif 
        typedef void  ( MV_STDCALL * USELIB )( IVUser< TLib > & usr );
        HMODULE hlib = LoadLibrary( path.c_str() );
        if ( hlib )
        {
            USELIB proc = ( USELIB ) GetProcAddress( hlib , "UseLiberary" );

            if ( proc )
                proc( usr , deplibs... );

            FreeLibrary( hlib );
        }
    }

    template< typename TLoadLibInfo , typename ... TNeedLibs , typename ... TLibs >
    static void _LoadLib( VSLib< TLoadLibInfo , TNeedLibs... > & libinfo
                          , IVUser< typename TLoadLibInfo::LibType > & usr
                          , TLibs& ... loadedlibs )
    {
        TLoadLibInfo info;
        _RealLoad( info , usr );
    }

    template< typename TLoadLibInfo , typename TNeedLib , typename ... TNeedLibs , typename ... TLibs >
    static void _LoadLib( VSLib< TLoadLibInfo , TNeedLib , TNeedLibs... > & libinfo
                          , IVUser< typename TLoadLibInfo::LibType > & usr
                          , TLibs& ... loadedlibs )
    {
        TNeedLib needInfo;
        _LoadLib( needInfo , VD_L2U( [ this , &loadedlibs ]( typename TNeedLib::InfoType::LibType & needlib ) {
            VSLib< TLoadLibInfo , TNeedLibs... > nextInfo;
            _LoadLib( nextInfo , loadedlibs... , needlib );
        } ) );
    }
};
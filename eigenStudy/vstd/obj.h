#pragma once
#include <type_traits>
#ifndef VINTERFACE 
#ifdef WIN32
#define VPUREINTERFACE  struct __declspec(novtable)
#else
#define VPUREINTERFACE  struct
#endif
#define VINTERFACE struct
#endif 

#ifdef _WIN32
#define MV_STDCALL     __stdcall
#define MV_EXPORT_API  __declspec(dllexport)
#define MV_IMPORT_API  __declspec(dllimport)
#else
#define MV_STDCALL
#define MV_EXPORT_API
#define MV_IMPORT_API
#endif

#define VD_DECLARE_NO_COPY_CLASS(classname)      \
    private:                                    \
        classname(const classname&);            \
        classname& operator=(const classname&)

#define VD_DECLARE_NO_COPY_TEMPLATE_CLASS(classname, arg)  \
        private:                                              \
            classname(const classname<arg>&);                 \
            classname& operator=(const classname<arg>&)

#define VD_DECLARE_NO_COPY_TEMPLATE_CLASS2(classname, arg1, arg2 )  \
        private:                                              \
            classname(const classname<arg1,arg2>&);                 \
            classname& operator=(const classname<arg1,arg2>&)

#define VD_DECLARE_NO_COPY_TEMPLATE_CLASS3(classname, arg1, arg2 , arg3 )  \
        private:                                              \
            classname(const classname<arg1,arg2, arg3>&);                 \
            classname& operator=(const classname<arg1,arg2, arg3>&)

#define VD_DECLARE_NO_COPY_TEMPLATE_CLASS4(classname, arg1, arg2 , arg3 , arg4 )  \
        private:                                              \
            classname(const classname<arg1,arg2, arg3 , arg4 >&);                 \
            classname& operator=(const classname<arg1,arg2, arg3, arg4 >&)

#define VD_DECLARE_NO_COPY_TEMPLATE_CLASS5(classname, arg1, arg2 , arg3 , arg4 , arg5 )  \
        private:                                              \
            classname(const classname<arg1,arg2, arg3 , arg4 , arg5 >&);                 \
            classname& operator=(const classname<arg1,arg2, arg3, arg4 , arg5 >&)



// 结构体定义头——struct className : public IVOobject
#define VD_DECLAREOBJECT( className )     VINTERFACE className : public IVObject

#define VD_DECLAREINTERFACE( interfName ) VPUREINTERFACE interfName

#define VD_ARG(...) __VA_ARGS__  

// 基础接口，所有接口都从此接口继承,  现在不在使用虚的析构函数
VINTERFACE IVObject
{ 
    IVObject(){}
    virtual ~IVObject(){}
} ;   



// External ralation to IVComponent::Participate
VPUREINTERFACE IVAgent
{
    virtual void Present() = 0 ;
} ;



VPUREINTERFACE IVComponent
{
    virtual void Participate() = 0 ; 
} ;


template< typename ID >
VPUREINTERFACE IVDelegate : IVAgent
{
} ;
  
template< typename ID >
VPUREINTERFACE IVDelegateArr : IVAgent
{
} ;

template< typename ID , unsigned COUNT >
VPUREINTERFACE IVDelegateSet : IVAgent
{
} ;

template< typename ID >
VPUREINTERFACE IVElement : IVComponent
{
} ;


template< typename TID  >  
using IVClassifiable = IVElement< TID > ;

 
template< class T >
struct TVDataTrait
{  
    //typedef typename std::conditional< std::is_abstract< T >::value  , T & , const T & >::type rawRef ;
    typedef typename std::conditional< std::is_scalar< T >::value  , T          , typename std::conditional< std::is_abstract< T >::value  , T & , const T & >::type >::type reference    ; 
} ;    
 



template<>
struct TVDataTrait< void >
{
    typedef void reference ;
} ;




template< class T = void , class TRef = typename TVDataTrait< T >::reference >
VPUREINTERFACE IVUser
{
    virtual void Visit( TRef t ) = 0 ;
} ;   



template<>
VPUREINTERFACE IVUser< void >
{
    virtual void Visit() = 0 ;
};     
   
template< class T = void , class TRef = typename TVDataTrait< T >::reference >
VINTERFACE IVOwner
{
    virtual void Use( IVUser< T , TRef > & user ) = 0 ;
} ;  






///////////////////////////////////////////////////////////////////////////////////////////////////////////
// 不满足标准使用方式的结构体
// 此结构体必须需要使用提供的模板类库：TVServiceProxy, TVService, TVServiceProvider来使用，不应直接访问此结构体
// 参见"tisvr.h"
template< class T > VPUREINTERFACE IVSpace
{ 
    typedef void    FreeFuncType ( IVSpace< T > & spc ) ;

    T             * pService     ; 
    IVSpace< T  > * pPreRef      ;
    IVSpace< T  > * pNxtRef      ;
    FreeFuncType  * procFreeClnt ;
    FreeFuncType  * procFreeSrvc ;

    IVSpace(){}
    ~IVSpace(){} 

    VD_DECLARE_NO_COPY_TEMPLATE_CLASS( IVSpace , T ) ;
} ;  

template< class T >
struct TVDataTrait< IVSpace<T> >
{  
    typedef IVSpace<T> & reference ; 
} ;

template< class T >
using VI = IVSpace< T > ;

template< class T >
using VO = IVUser< IVSpace< T > > ;

#define VD_DECLAREREFCLASS( cls )  template<> struct TVDataTrait< cls > { typedef cls & reference ; } ;
#define VD_DECLAREREFCLASS_T1( cls )  template< class T > struct TVDataTrait< cls< T > > { typedef cls< T > & reference ; } ;
#define VD_DECLAREREFCLASS_T2( cls )  template< class T1 , class T2 > struct TVDataTrait< cls< T1 , T2 > > { typedef cls< T1 , T2 > & reference ; } ;
#define VD_DECLAREREFCLASS_T3( cls )  template< class T1 , class T2  , class T3 > struct TVDataTrait< cls< T1 , T2 , T3 > > { typedef cls< T1 , T2 , T3 > & reference ; } ;

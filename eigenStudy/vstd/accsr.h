#pragma once

#include "obj.h"

// 用于Trigger模式
VPUREINTERFACE IVTriggerable 
{
    virtual void Fire() = 0 ;
} ;



// 用于Trigger模式
template< class T >
VPUREINTERFACE IVPipe
{
    virtual void Flow( T v ) = 0 ;
} ;


/*
#include <type_traits>

template< class T >
VINTERFACE IVObjAccessor : public IVClonable< IVObjAccessor< T > >
{
    VINTERFACE Observer : public IVObject
    {
        virtual void OnTargetCreate ( T * pObj ) = 0 ; 
        virtual void OnTargetDestory( T * pObj ) = 0 ;
    } ;

    virtual T *  GetTarget()                         = 0 ; 
    virtual bool HasObserver   ( Observer * pObsvr ) = 0 ; 
    virtual void AttachObserver( Observer * pObsvr ) = 0 ; 
    virtual void DetachObserver( Observer * pObsvr ) = 0 ; 
} ;

//
//template< class T , class TRef = VAccParaTypeTrait< T >::CnstUseType > 
//VINTERFACE IVFlowAccessor : public IVClonable< IVFlowAccessor< T , DT > >
//{
//    VINTERFACE Observer : public IVObject
//    {
//        virtual void OnFlowData( TRef newValue ) = 0 ;
//    } ;
//
//    virtual bool   HasChecker   ( Observer * pChecker ) = 0 ; 
//    virtual void   AttachChecker( Observer * pChecker ) = 0 ; 
//    virtual void   DetachChecker( Observer * pChecker ) = 0 ; 
//} ; 



template< class T >
VINTERFACE IVObjWrapper : public IVObject
{
    virtual IVObjAccessor< T > & GetAccContent() = 0 ;
} ;

template< class T >
struct TVAccDataTrait
{
    typedef const T & ConstRef ;
} ;

template< class T >
struct TVAccNormTrait
{
    typedef T & ConstRef ;
};

template< class T >
struct TVAccPtrTrait
{
    typedef T * ConstRef ;
} ;

template< class T >
struct TVAccCnstPtrTrait
{
    typedef const T * ConstRef ;
} ;

template< class T >
struct TVAccValueTrait
{
    typedef T ConstRef ;
} ;

template<> struct TVAccDataTrait< bool             > { typedef bool             ConstRef ; } ;
template<> struct TVAccDataTrait< int              > { typedef int              ConstRef ; } ;
template<> struct TVAccDataTrait< unsigned         > { typedef unsigned         ConstRef ; } ;
template<> struct TVAccDataTrait< char             > { typedef char             ConstRef ; } ;
template<> struct TVAccDataTrait< short            > { typedef short            ConstRef ; } ;
template<> struct TVAccDataTrait< long             > { typedef long             ConstRef ; } ;
template<> struct TVAccDataTrait< __int64          > { typedef __int64          ConstRef ; } ;
template<> struct TVAccDataTrait< unsigned char    > { typedef unsigned char    ConstRef ; } ;
template<> struct TVAccDataTrait< unsigned short   > { typedef unsigned short   ConstRef ; } ;
template<> struct TVAccDataTrait< unsigned long    > { typedef unsigned long    ConstRef ; } ;
template<> struct TVAccDataTrait< unsigned __int64 > { typedef unsigned __int64 ConstRef ; } ;
template<> struct TVAccDataTrait< float            > { typedef float            ConstRef ; } ;
template<> struct TVAccDataTrait< double           > { typedef double           ConstRef ; } ;

// 声明一个IVObject对象的Trait
#define VM_DECLEAR_OBJECT_DATA( className ) template<> struct TVAccDataTrait< className > { typedef className & ConstRef ; } ;

template< class T  , class DT = TVAccDataTrait< T > >
VINTERFACE IVDataAccessor : public IVClonable< IVDataAccessor< T , DT > >
{
    VINTERFACE Observer : public IVObject
    {
        virtual void  OnDataChanged( typename DT::ConstRef newValue ) = 0 ;
    } ;

    virtual typename DT::ConstRef GetValue()                          = 0 ; 
    virtual bool                  HasObserver   ( Observer * pObsvr ) = 0 ; 
    virtual void                  AttachObserver( Observer * pObsvr ) = 0 ; 
    virtual void                  DetachObserver( Observer * pObsvr ) = 0 ; 
} ;

VINTERFACE IVTriggerAccessor : public IVClonable< IVTriggerAccessor >
{
    VINTERFACE Observer : public IVObject
    {
        virtual void OnNotify()     = 0 ;
    } ;
    
    virtual bool HasHandler   ( Observer * pHandler ) = 0 ; 
    virtual void AttachHandler( Observer * pHandler ) = 0 ; 
    virtual void DetachHandler( Observer * pHandler ) = 0 ; 
} ; 

template< class T , class DT = TVAccDataTrait< T > > 
VINTERFACE IVFlowAccessor : public IVClonable< IVFlowAccessor< T , DT > >
{
    VINTERFACE Observer : public IVObject
    {
        virtual void OnFlowData( typename DT::ConstRef newValue ) = 0 ;
    } ;

    virtual bool   HasChecker   ( Observer * pChecker ) = 0 ; 
    virtual void   AttachChecker( Observer * pChecker ) = 0 ; 
    virtual void   DetachChecker( Observer * pChecker ) = 0 ; 
} ; 

typedef IVTriggerAccessor IVAccTrigger ; 
 
template< class T > 
VINTERFACE IVAccPipe : public IVClonable< IVAccPipe< T > >
{
    VINTERFACE Observer : public IVObject
    {
        virtual void OnFlowData( T msg ) = 0 ;
    } ;

    virtual bool   HasObserver    ( Observer * pChecker ) = 0 ; 
    virtual void   AttachObserver ( Observer * pChecker ) = 0 ; 
    virtual void   DetachObserver ( Observer * pChecker ) = 0 ; 
} ; 

template< class T >
VINTERFACE IVAccProperty : public IVClonable< IVAccProperty<T> >
{
    VINTERFACE  Observer : public IVObject
    {
        virtual void  OnDataChanged( T valueRef ) = 0 ;
    } ;

    virtual T        GetValue          ()                    = 0 ; 
    virtual bool     HasObserver   ( Observer * pObsvr ) = 0 ; 
    virtual void     AttachObserver( Observer * pObsvr ) = 0 ; 
    virtual void     DetachObserver( Observer * pObsvr ) = 0 ;  
} ;
*/
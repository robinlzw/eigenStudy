#pragma once

#include "obj.h"

template< class T >
VINTERFACE IVXEnumerator : public IVObject
{
    virtual bool IsEnd()                       = 0 ;
    virtual void Reset()                       = 0 ;
    virtual void Next( IVUser< T > & visitor ) = 0 ;
} ;

/*
// 遍历通用接口
template< class T >
VINTERFACE IVEnumerator : public IVClonable< IVEnumerator< T > >
{
    virtual unsigned int        Next( unsigned int celt, T * pBuff )   = 0 ; 
    virtual bool                Skip( unsigned int celt )              = 0 ;
    virtual bool                Reset()                                = 0 ;
} ;

template< class T >
VINTERFACE IVEnumerable : public IVObject
{
    virtual IVEnumerator< T > * CreateEnumerator() = 0 ;
} ;

template< class T >
VINTERFACE IVCollection : public IVObject
{
    virtual unsigned int GetCount() = 0 ;
    virtual unsigned int CopyTo( T * buff , unsigned int buffSize ) = 0 ;
} ;

template< class K , class T >
struct DictionaryEntry
{
    K m_key   ;
    T m_value ;
} ;

template< class K , class T >
VINTERFACE IVDictionary : public IVCollection< T > , public IVEnumerable< T >
{
    virtual void Clear()                                = 0 ;   //清空字典
    virtual bool Contains( const K & key )              = 0 ;   // 判断是否包含有关键字为key的元素
    virtual bool Remove( const K & key )                = 0 ;   // 移除关键字为key的元素，出错返回false

    virtual bool GetItem( const K & key , T * pItem )       = 0 ; // 获取关键字为key的元素到 pItem
                                                                  // 不存在时返回false ；
    virtual void SetItem( const K & key , const T & item )  = 0 ; // 设置关键字为key的元素值为item
    virtual IVCollection< K > * CreateKeyList()             = 0 ; // 创建一个关键字的集合
    virtual IVCollection< T > * CreateVauleList()           = 0 ; // 创建一个所有元素在一起的集合

    virtual IVEnumerator< DictionaryEntry< K , T > > * CreateDictEnerator() = 0 ; // 返回一个可以成对访问元素的遍历器
} ;

template< class T >
VINTERFACE IVList : public IVCollection< T > , public IVEnumerable< T >
{
    virtual void     Clear()                                    = 0 ;
    virtual bool     GetItem( unsigned indx  , T * pItem )      = 0 ;
    virtual unsigned Add( const T & item )                      = 0 ;
    virtual void     Insert( unsigned indx  , const T & item )  = 0 ;
    virtual bool     Remove( unsigned indx  )                   = 0 ;
} ;
*/
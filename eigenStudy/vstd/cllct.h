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
// ����ͨ�ýӿ�
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
    virtual void Clear()                                = 0 ;   //����ֵ�
    virtual bool Contains( const K & key )              = 0 ;   // �ж��Ƿ�����йؼ���Ϊkey��Ԫ��
    virtual bool Remove( const K & key )                = 0 ;   // �Ƴ��ؼ���Ϊkey��Ԫ�أ�������false

    virtual bool GetItem( const K & key , T * pItem )       = 0 ; // ��ȡ�ؼ���Ϊkey��Ԫ�ص� pItem
                                                                  // ������ʱ����false ��
    virtual void SetItem( const K & key , const T & item )  = 0 ; // ���ùؼ���Ϊkey��Ԫ��ֵΪitem
    virtual IVCollection< K > * CreateKeyList()             = 0 ; // ����һ���ؼ��ֵļ���
    virtual IVCollection< T > * CreateVauleList()           = 0 ; // ����һ������Ԫ����һ��ļ���

    virtual IVEnumerator< DictionaryEntry< K , T > > * CreateDictEnerator() = 0 ; // ����һ�����ԳɶԷ���Ԫ�صı�����
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
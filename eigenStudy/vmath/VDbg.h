#pragma once

#define VD_RUN_WITH_DEBUG

#ifdef VD_RUN_WITH_DEBUG

#include <string>
#include <fstream> 
#include <sstream>
#include "vfvector3.h"
#include "vfvector2.h"
#include "vnvector.h"

class VMathDbg
{
public: 
    VMathDbg()
    {
    }

    ~VMathDbg()
    {
    }

private :
    template< typename T >
    static void _writeItem( std::ofstream & os , const VNVector3< T > & v )
    {
        os << v.x << "  " << v.y <<  "  "  << v.z << "  " ;
    } 
    static void _writeItem( std::ofstream & os , const VFVECTOR3 & v )
    {
        os << v.x << "  " << v.y <<  "  "  << v.z << "  " ;
    } 
    static void _writeItem( std::ofstream & os , const VFVECTOR2 & v )
    {
        os << v.x << "  " << v.y <<  "  " ;
    } 
    template< typename T >
    static void _writeItem( std::ofstream & os , T v )
    {
        os << v ;
    } 
private :
    template< typename T >
    static void _readItem( std::ifstream & os , VNVector3< T > & v )
    {
        os >> v.x >> v.y >> v.z ;
    } 
    static void _readItem( std::ifstream & os , VFVECTOR3 & v )
    {
        os >> v.x >> v.y >> v.z ;
    } 
    static void _readItem( std::ifstream & os , VFVECTOR2 & v )
    {
        os >> v.x >> v.y ;
    } 
    template< typename T >
    static void _readItem( std::ifstream & os , T & v )
    {
        os >> v ;
    } 

public :
    template< typename T , typename F >
    static void WriteArray( const std::string & strFileName , unsigned len , const T * v , F f )
    {
        std::ofstream  fdest( strFileName ) ;
        
        //fdest << len << std::endl ;

        for( unsigned i = 0 ; i < len ; i ++ )
        {
            f( fdest , v[i] ) ;
            fdest << std::endl ;
        }
    }
    template< typename T , typename F >
    static void ReadArray( const std::string & strFileName , std::vector< T > & v , F f )
    {
        std::ifstream  fdest( strFileName ) ;
        
        unsigned len ;
        fdest >> len ;
        v.resize( len ) ;

        for( unsigned i = 0 ; i < len ; i ++ )
        {
            T & item = v[i] ; 
            f( fdest , item ) ;
        }
    }

    template< typename T >
    static void WriteArray( const std::string & strFileName , unsigned len , const T * v )
    {
        WriteArray( strFileName , len , v , []( std::ofstream  & f , const T & v ){
            _writeItem( f , v ) ;
        } ) ; 
    }
    
    template< typename T >
    static void WriteArray( const std::string & strFileName , const VSConstBuffer< T > &  cb ) 
    {
        WriteArray( strFileName , cb.len , cb.pData ) ;
    }

    template< typename T >
    static void ReadArray( const std::string & strFileName , std::vector< T > & v )
    {
        ReadArray( strFileName , v , []( std::ifstream  & f , T & v ){
            _readItem( f , v ) ;
        } ) ;  
    }

    template< typename F >
    static bool CheckHitCount( unsigned nCount , F f )
    { 
        static unsigned counter(0) ;

        if( nCount != ( counter ++ ) ) 
        {
            f() ;
            return false ;
        }

        return true ;
    }
};  

#endif
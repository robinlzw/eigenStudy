#pragma once

#include <vstd/obj.h"
#include <vstd/cllct.h"
#include <vector>
//
//template< class T >
//class VVectorEnumImp : public IVEnum< T >
//{
//public :
//    VVectorEnumImp( const std::vector< T > & data )
//        : m_data( data )
//        , m_position(0)
//    {
//    }
//    ~VVectorEnumImp()
//    {
//    }
//
//public :
//    virtual void        Reset()                             
//    {
//        m_position = 0 ;
//    }
//
//    virtual unsigned    Next( T * buffer , unsigned count ) 
//    {
//        unsigned copied ;
//
//        if( m_position + count >= m_data.size() )
//            copied = m_data.size() - m_position ;
//        else
//            copied = count ;
//
//        for( unsigned i = 0 ; i < copied ; i ++ ) 
//        {
//            buffer[ i ] = m_data[ m_position + i ] ;
//        }
//
//        m_position += count ;
//
//        return copied ;
//    }
//
//protected :
//    unsigned                    m_position  ;
//    std::vector< T >            m_data      ;
//} ;

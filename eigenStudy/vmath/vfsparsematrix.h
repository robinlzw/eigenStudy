#pragma once

#include "vflargevector.h"

class VFSparseMatrix
{
public:
    VFSparseMatrix( unsigned row );
    ~VFSparseMatrix(void);

public :
    unsigned GetSize() const ;
    void     Clear() ;

public :
    void Multi( const VFLargeVector & src , VFLargeVector & rslt ) const ;
    void SetCellValue( unsigned row , unsigned col , double v ) ;
    template< class F > void UsingCell(  unsigned row , unsigned col , F f ) ;

protected :
    class VRow 
    {
    public :
        VRow(){}
        ~VRow(){}
    public :
        void   Clear() ;
        double Dot( const VFLargeVector & v ) const ;
        void   SetValue( unsigned idx , double v ) ;
        template< class F > void UsingValue(  unsigned idx , F f ) ;

        struct Item
        {
            Item(){}
            Item( unsigned i, double v ):idx(i),value(v){}
            unsigned idx   ;
            double   value ;
        } ;
    protected :
        std::vector< Item > m_data; 
    } ;

public :
    std::vector< VRow >  m_content ;
};

inline 
VFSparseMatrix::VFSparseMatrix( unsigned row )
{
    m_content.resize( row ) ;
}

inline 
VFSparseMatrix::~VFSparseMatrix(void)
{
}
   
inline 
unsigned VFSparseMatrix::GetSize() const 
{
    return m_content.size() ; 
}

inline
void VFSparseMatrix::Clear() 
{
    std::for_each( m_content.begin() , m_content.end() , []( VRow & r ){
    }) ;
}

inline 
void VFSparseMatrix::Multi( const VFLargeVector & src , VFLargeVector & rslt ) const 
{
    assert( GetSize() == src.GetLength() && GetSize() == rslt.GetLength() ) ;
    unsigned sz = GetSize() ;

    for( unsigned i = 0 ; i < sz ; i++ )
    {
        rslt[i] = m_content[i].Dot( src ) ;
    }
}


inline
void VFSparseMatrix::SetCellValue( unsigned row , unsigned col , double v ) 
{
    assert( row < GetSize() && col < GetSize() ) ;

    VRow & vr = m_content[row] ;

    vr.SetValue( col , v ) ;
}

template< class F > inline void VFSparseMatrix::UsingCell(  unsigned row , unsigned col , F f ) 
{
    assert( row < GetSize() && col < GetSize() ) ;
    VRow & vr = m_content[row] ;
    vr.UsingValue( col , f ) ;
}

inline 
double VFSparseMatrix::VRow::Dot( const VFLargeVector & v ) const
{
    double rslt(0) ;

    std::for_each( m_data.begin() , m_data.end() , [&v,&rslt]( const VFSparseMatrix::VRow::Item & i ){
        rslt += v[i.idx] * i.value ;    
    }) ;

    return rslt ;
}      

inline
    void   VFSparseMatrix::VRow::Clear() 
{
    m_data.clear() ;
}

inline 
void VFSparseMatrix::VRow::SetValue( unsigned idx , double v ) 
{ 
    Item item( idx , v ) ;

    assert(0) ; // 这代码尚未验证

    auto it = std::lower_bound( m_data.begin() , m_data.end() , item , []( const Item & mid , const Item & targ ){
        return mid.idx < targ.value ;
    }) ;

    if( it == m_data.end() )
        m_data.push_back( item ) ;
    else if( it->idx == idx )
        it->value = v ;
    else
        m_data.insert( it , item ) ;


    //    for ( unsigned i = 0 ; i < m_data.size() ; i ++ )
    //{
    //    if( idx == m_data[i].idx )
    //    {
    //        m_data[i].value = v ;
    //        return ;
    //    }

    //    if( idx < m_data[i].idx )
    //    {
    //        m_data.insert( m_data.begin() + i , Item( idx , v ) ) ;
    //        return ;
    //    }
    //}  

    //m_data.push_back( Item( idx , v ) ) ;
}

template< class F >
inline void VFSparseMatrix::VRow::UsingValue( unsigned idx , F f )
{
    Item item( idx , 0 ) ;

    assert(0) ; // 这代码尚未验证

    auto it = std::lower_bound( m_data.begin() , m_data.end() , item , []( const Item & mid , const Item & targ ){
        return mid.idx < targ.value ;
    }) ;

    if( it == m_data.end() )
    {
        m_data.push_back( item ) ;
        f( m_data.back().value ) ;
    }
    else if( it->idx == idx )
    {
        it->value = v ;
        f( it->value ) ;
    }
    else
    {
        std::vector< Item >::iterator itNew = m_data.insert( it , item ) ;
        f( itNew->value ) ;
    }
}

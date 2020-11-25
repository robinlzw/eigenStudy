
/*
// Implementation of hstmplt.h


// TVVectorEnumImp
template< class T >
inline
TVVectorEnumImp<T>::TVVectorEnumImp()
: m_nCurIndx( 0 )
{
}

template< class T >
inline
TVVectorEnumImp<T>::TVVectorEnumImp( const std::vector< T > & src  )
: m_nCurIndx( 0 )
, m_itemList( src )
{ 
}

template< class T >
inline
TVVectorEnumImp<T>::~TVVectorEnumImp()
{
}


template< class T >
inline
unsigned int  
TVVectorEnumImp<T>::Next( unsigned int celt, T * pBuff )
{
    unsigned int upper = (std::min)( m_nCurIndx + celt , m_itemList.size() ) ;
    unsigned int count = upper - m_nCurIndx ;

    for( unsigned int i = m_nCurIndx ; i < upper ; i ++ )
        pBuff[ i - m_nCurIndx ] = m_itemList[i] ;

    m_nCurIndx = upper ;

    return count ;
}

template< class T >
inline
bool 
TVVectorEnumImp<T>::Skip( unsigned int celt )    
{
    if( m_nCurIndx + celt <= m_itemList.size() )
    {
        m_nCurIndx += celt ;
        return true ;
    }

    return false ;
}

template< class T >
inline
bool 
TVVectorEnumImp<T>::Reset()
{
    m_nCurIndx = 0 ;
    return true ;
}

template< class T >
inline
IVEnumerator< T > * 
TVVectorEnumImp<T>::Clone()            
{
    return new TVVectorEnumImp<T>( *this ) ;
}
*/
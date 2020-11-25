#pragma once
 
#include "vnvector.h"
#include "../vstd/ticmn.h" 

class VTopoGraph
{    
public :
    VTopoGraph()
    {
        _lstVertInfo.len = 0 ;
        _lstSurfInfo.len = 0 ;
        _lstEdgeInfo.len = 0 ;
        _lstEdgeRef .len = 0 ;
        _lstSurfRef .len = 0 ;  
    }

    // Vertex Nebour of surface
    struct VOS
    {
        unsigned surfIndx   ;
        unsigned vertInSurf ;

        VOS(){}
        VOS( unsigned s , unsigned vi ):surfIndx(s),vertInSurf(vi){}
        bool operator==( const VOS & eOther ) const { return eOther.surfIndx == surfIndx && eOther.vertInSurf == vertInSurf ; } 
        bool operator!=( const VOS & eOther ) const { return eOther.surfIndx != surfIndx || eOther.vertInSurf != vertInSurf ; } 
    } ; 

    // VertexNebour of Edge
    struct VOE
    {
        unsigned edgeIndx     ;
        unsigned vertInEdge   ; // 0: 起始点，1：终止点

        VOE(){}
        VOE( unsigned e , unsigned vi ):edgeIndx(e),vertInEdge(vi){ VASSERT( vi == 0 || vi == 1 ) ;}
        bool operator==( const VOE & eOther ) const { return eOther.edgeIndx == edgeIndx && eOther.vertInEdge == vertInEdge ; } 
        bool operator!=( const VOE & eOther ) const { return eOther.edgeIndx != edgeIndx || eOther.vertInEdge != vertInEdge ; }  
        bool IsBeginning() const { return vertInEdge == 0 ; }
        bool IsEnding   () const { return vertInEdge == 1 ; }
        VOE Opposite() const { return VOE( edgeIndx , vertInEdge ^ 0x01 ); }
    } ;

    static const unsigned INVALID_INDX = ~(unsigned(0)) ;
 
    //unsigned GetVerticesCount() const ;
    unsigned GetSurfCount()     const ;
    unsigned GetEdgeCount()     const ; 
    unsigned GetVertexCount()   const ;

    //const VNVECTOR3UI * GetTriangleBuffer()   const ;

    // 点-〉面
    unsigned  GetSurfCountOfVetex( unsigned indx ) const ;
    unsigned  GetSurfIndxOfVertex( unsigned vIndx , unsigned sIndx ) const ; 
    VOS       GetSurfOfVertex    ( unsigned vIndx , unsigned sIndx ) const   ; 

    // 点-〉边
    unsigned  GetEdgeCountOfVetex( unsigned indx ) const ; 
    unsigned  GetEdgeIndxOfVertex( unsigned vIndx , unsigned eIndx ) const ;
    VOE       GetEdgeOfVertex    ( unsigned vIndx , unsigned eIndx ) const ;
    
    // 面-〉点
    //const VNVECTOR3UI & GetVertIndxOfSurf   ( unsigned sIndx ) const ;
    const VNVECTOR3UI & GetVertNbrIndxOfSurf( unsigned sIndx ) const ; 

    // 面-〉边
    const VNVECTOR3UI & GetEdgeIndxOfSurf   ( unsigned sIndx ) const ;
    const VNVECTOR3US & GetEdgeNbrIndxOfSurf( unsigned sIndx ) const ;

    // 边-> 面
    const VNVECTOR2UI & GetSurfIndxOfEdge   ( unsigned eIndx ) const ;
    const VNVECTOR2UI & GetSurfNbrIndxOfEdge( unsigned eIndx ) const ;

    // 边-> 点
    const VNVECTOR2UI & GetVertIndxOfEdge   ( unsigned eIndx ) const ; 
    //const VNVECTOR2UI & GetVertNbrIndxOfEdge( unsigned eIndx ) const ;  

protected :
    // Data Struct
    struct VertInfo
    {
        unsigned   edgeCount    ;
        unsigned   edgeOffset   ;  // 相对_lstEdgeRef.pData的偏移
        unsigned   surfCount    ;
        unsigned   surfOffset   ;  // 相对_lstSurfRef.pData的偏移
    } ;

    struct SurfInfo 
    {
        // pVert[i]的对边为pEdge[i]
        //VNVECTOR3UI vert ;
        VNVECTOR3UI edge ;
        VNVECTOR3UI idxInVertNbr ;
        VNVECTOR3US idxInEdgeNbr ;
    } ;

    struct EdgeInfo
    {
        VNVECTOR2UI vert      ; // 0: start , 1: end
        VNVECTOR2UI nbrSurf   ; // 0：left  , 1: right
        VNVECTOR2UI idxInSurf ;
    } ;
 
    struct SurfRef
    {
        unsigned  crd ; // 点在三角片种的下标
        unsigned  pre ; // 前一个三角片，绕点逆时针旋转后的临面位于点的面列表中的下标
        unsigned  nxt ;
        unsigned  idx ; // 三角片下标
    } ;

    struct EdgeRef
    {
        unsigned  crd ; 
        unsigned  idx ;
    } ; 

    VSConstBuffer< VertInfo > _lstVertInfo ;
    VSConstBuffer< SurfInfo > _lstSurfInfo ;
    VSConstBuffer< EdgeInfo > _lstEdgeInfo ;
    VSConstBuffer< EdgeRef  > _lstEdgeRef  ;
    VSConstBuffer< SurfRef  > _lstSurfRef  ; 

    template< class F > void _useVertInfo( const VTopoGraph & src , F f ) { f( src._lstVertInfo ) ; }
    template< class F > void _useSurfInfo( const VTopoGraph & src , F f ) { f( src._lstSurfInfo ) ; }
    template< class F > void _useEdgeInfo( const VTopoGraph & src , F f ) { f( src._lstEdgeInfo ) ; }
    template< class F > void _useEdgeRef ( const VTopoGraph & src , F f ) { f( src._lstEdgeRef  ) ; }
    template< class F > void _useSurfRef ( const VTopoGraph & src , F f ) { f( src._lstSurfRef  ) ; } 
     
    template< typename TP > static void MapTo( TP & mapper , const VTopoGraph & ti ){ 
        VBMarshalling< VSConstBuffer< VertInfo > , true >::SaveExtData( mapper , ti._lstVertInfo ) ;
        VBMarshalling< VSConstBuffer< SurfInfo > , true >::SaveExtData( mapper , ti._lstSurfInfo ) ;
        VBMarshalling< VSConstBuffer< EdgeInfo > , true >::SaveExtData( mapper , ti._lstEdgeInfo ) ;
        VBMarshalling< VSConstBuffer< EdgeRef  > , true >::SaveExtData( mapper , ti._lstEdgeRef  ) ;
        VBMarshalling< VSConstBuffer< SurfRef  > , true >::SaveExtData( mapper , ti._lstSurfRef  ) ;
    }
    template< typename CP >
    static void CorrectPtr( CP & r , unsigned & posEnd , const VSConstBuffer< char > & pbuff , unsigned objPos , unsigned curPos ){
        const VTopoGraph * pmesh = reinterpret_cast< const VTopoGraph * >( pbuff.pData + objPos ) ;
        posEnd = curPos ;
        VBMarshalling< VSConstBuffer< VertInfo > , true >::CorrectPtr( r , posEnd , pbuff , objPos + offsetof( VTopoGraph , _lstVertInfo ), posEnd );
        VBMarshalling< VSConstBuffer< SurfInfo > , true >::CorrectPtr( r , posEnd , pbuff , objPos + offsetof( VTopoGraph , _lstSurfInfo ), posEnd );
        VBMarshalling< VSConstBuffer< EdgeInfo > , true >::CorrectPtr( r , posEnd , pbuff , objPos + offsetof( VTopoGraph , _lstEdgeInfo ), posEnd );
        VBMarshalling< VSConstBuffer< EdgeRef  > , true >::CorrectPtr( r , posEnd , pbuff , objPos + offsetof( VTopoGraph , _lstEdgeRef  ), posEnd );
        VBMarshalling< VSConstBuffer< SurfRef  > , true >::CorrectPtr( r , posEnd , pbuff , objPos + offsetof( VTopoGraph , _lstSurfRef  ), posEnd );
    };
}; 

template< class TBase >
class TVGraphAdaptor : public TBase 
{
public :
    TVGraphAdaptor(){}
    TVGraphAdaptor( const VTopoGraph & gm )
    {
        static_assert( std::is_base_of< VTopoGraph , TBase >::value , "TBase mast derived from VTopoGraph " ) ;

        _useVertInfo( gm , [ this ]( const VSConstBuffer< typename TBase::VertInfo > & cb ){ this->_lstVertInfo = cb ; } ) ;
        _useSurfInfo( gm , [ this ]( const VSConstBuffer< typename TBase::SurfInfo > & cb ){ this->_lstSurfInfo = cb ; } ) ;
        _useEdgeInfo( gm , [ this ]( const VSConstBuffer< typename TBase::EdgeInfo > & cb ){ this->_lstEdgeInfo = cb ; } ) ;
        _useEdgeRef ( gm , [ this ]( const VSConstBuffer< typename TBase::EdgeRef  > & cb ){ this->_lstEdgeRef  = cb ; } ) ;
        _useSurfRef ( gm , [ this ]( const VSConstBuffer< typename TBase::SurfRef  > & cb ){ this->_lstSurfRef  = cb ; } ) ;
    }
    void Redirect( const VTopoGraph & gm )
    {
        static_assert( std::is_base_of< VTopoGraph , TBase >::value , "TBase mast derived from VTopoGraph " ) ;

        _useVertInfo( gm , [ this ]( const VSConstBuffer< typename TBase::VertInfo > & cb ){ this->_lstVertInfo = cb ; } ) ;
        _useSurfInfo( gm , [ this ]( const VSConstBuffer< typename TBase::SurfInfo > & cb ){ this->_lstSurfInfo = cb ; } ) ;
        _useEdgeInfo( gm , [ this ]( const VSConstBuffer< typename TBase::EdgeInfo > & cb ){ this->_lstEdgeInfo = cb ; } ) ;
        _useEdgeRef ( gm , [ this ]( const VSConstBuffer< typename TBase::EdgeRef  > & cb ){ this->_lstEdgeRef  = cb ; } ) ;
        _useSurfRef ( gm , [ this ]( const VSConstBuffer< typename TBase::SurfRef  > & cb ){ this->_lstSurfRef  = cb ; } ) ;
    }
} ;

class VIndepTopoGraph : public VTopoGraph
{
public : 
    VIndepTopoGraph(){}
    VIndepTopoGraph( const VTopoGraph & srcGraph )
    {
        _copy( srcGraph ) ;
    }

    void operator = ( const VTopoGraph & srcGraph )
    {
        _copy( srcGraph ) ;
    }

    void Copy( const VTopoGraph & srcGraph )
    {
        _copy( srcGraph ) ;
    }

private :
    void _copy( const VTopoGraph & srcGraph )
    {
        _useVertInfo( srcGraph , [ this ]( const VSConstBuffer< VertInfo > & cb ){ VCMN::Copy( cb , m_lstVertInfo ) ; } ) ;
        _useSurfInfo( srcGraph , [ this ]( const VSConstBuffer< SurfInfo > & cb ){ VCMN::Copy( cb , m_lstSurfInfo ) ; } ) ;
        _useEdgeInfo( srcGraph , [ this ]( const VSConstBuffer< EdgeInfo > & cb ){ VCMN::Copy( cb , m_lstEdgeInfo ) ; } ) ;
        _useEdgeRef ( srcGraph , [ this ]( const VSConstBuffer< EdgeRef  > & cb ){ VCMN::Copy( cb , m_lstEdgeRef  ) ; } ) ;
        _useSurfRef ( srcGraph , [ this ]( const VSConstBuffer< SurfRef  > & cb ){ VCMN::Copy( cb , m_lstSurfRef  ) ; } ) ;  

        _lstVertInfo = VD_V2CB( m_lstVertInfo ) ;
        _lstSurfInfo = VD_V2CB( m_lstSurfInfo ) ;
        _lstEdgeInfo = VD_V2CB( m_lstEdgeInfo ) ;
        _lstEdgeRef  = VD_V2CB( m_lstEdgeRef  ) ;
        _lstSurfRef  = VD_V2CB( m_lstSurfRef  ) ;   
    }

private : 
    std::vector< VertInfo > m_lstVertInfo  ;
    std::vector< SurfInfo > m_lstSurfInfo  ;
    std::vector< EdgeInfo > m_lstEdgeInfo  ;
    std::vector< EdgeRef  > m_lstEdgeRef   ;
    std::vector< SurfRef  > m_lstSurfRef   ;  
} ;

inline unsigned  VTopoGraph::GetSurfCount() const
{
    return _lstSurfInfo.len ;
}

inline unsigned VTopoGraph::GetVertexCount() const
{
    return _lstVertInfo.len;
} 

//inline const VNVECTOR3UI * VTopoGraph::GetTriangleBuffer()   const 
//{
//    return _Trangles ;
//}

inline unsigned VTopoGraph::GetEdgeCount()     const 
{
    return _lstEdgeInfo.len ;
} 

inline unsigned VTopoGraph::GetSurfCountOfVetex( unsigned indx ) const
{
    return _lstVertInfo.pData[ indx ].surfCount ;
}
    
inline unsigned  VTopoGraph::GetSurfIndxOfVertex( unsigned vIndx , unsigned sIndx ) const
{
    const SurfRef & sr = _lstSurfRef.pData[ _lstVertInfo.pData[ vIndx ].surfOffset + sIndx ] ;

    return sr.idx ;
}

//
inline VTopoGraph::VOS VTopoGraph::GetSurfOfVertex( unsigned vIndx , unsigned sIndx ) const
{
    const SurfRef & sr = _lstSurfRef.pData[ _lstVertInfo.pData[ vIndx ].surfOffset + sIndx ] ;

    return VOS( sr.idx , sr.crd ) ;
} ; 

inline unsigned  VTopoGraph::GetEdgeCountOfVetex( unsigned indx ) const 
{
    return _lstVertInfo.pData[ indx ].edgeCount ;
} 

inline unsigned VTopoGraph::GetEdgeIndxOfVertex( unsigned vIndx , unsigned eIndx ) const 
{
    unsigned off = _lstVertInfo.pData[ vIndx ].edgeOffset ;
    const EdgeRef &er = _lstEdgeRef.pData[ off + eIndx ] ;

    return er.idx ;
}

inline VTopoGraph::VOE VTopoGraph::GetEdgeOfVertex( unsigned vIndx , unsigned eIndx ) const 
{
    unsigned off = _lstVertInfo.pData[ vIndx ].edgeOffset ;
    const EdgeRef &er = _lstEdgeRef.pData[ off + eIndx ] ;

    return VOE( er.idx , er.crd ) ;
}
//
//inline unsigned  VTopoGraph::GetNbrVertOfVertex( unsigned vIndx , unsigned eIndx ) const 
//{
//    unsigned off = _lstVertInfo.pData[ vIndx ].edgeOffset ;
//    const EdgeRef er = _lstEdgeRef.pData[ off + eIndx ];
//    unsigned idxInEdge = er.crd ^ 0x01 ;
//    return _lstEdgeInfo.pData[ er.idx ].vert[ idxInEdge ] ;
//}
//
//inline const VNVECTOR3UI & VTopoGraph::GetVertIndxOfSurf     ( unsigned sIndx ) const 
//{
//    return _Trangles[ sIndx ] ;
//}

inline const VNVECTOR3UI & VTopoGraph::GetVertNbrIndxOfSurf( unsigned sIndx ) const 
{
    return _lstSurfInfo.pData[ sIndx ].idxInVertNbr ;
} 
//
//inline unsigned VTopoGraph::GetEdgeIndxOfSurf( const EdgeOfSurf & es ) const
//{
//    return _lstSurfInfo.pData[ es.surfIndx ].edge[ es.edgeInSurf ] ;
//}

inline const VNVECTOR3UI & VTopoGraph::GetEdgeIndxOfSurf     ( unsigned sIndx ) const
{
    return _lstSurfInfo.pData[ sIndx ].edge ;
}

inline const VNVECTOR3US & VTopoGraph::GetEdgeNbrIndxOfSurf  ( unsigned sIndx ) const
{
    return _lstSurfInfo.pData[ sIndx ].idxInEdgeNbr ;
}

inline const VNVECTOR2UI & VTopoGraph::GetSurfIndxOfEdge( unsigned eIndx ) const
{
    return _lstEdgeInfo.pData[ eIndx ].nbrSurf ;
} 

inline const VNVECTOR2UI & VTopoGraph::GetSurfNbrIndxOfEdge ( unsigned eIndx ) const 
{
    return _lstEdgeInfo.pData[ eIndx ].idxInSurf ;
}

inline const VNVECTOR2UI & VTopoGraph::GetVertIndxOfEdge     ( unsigned eIndx ) const 
{
    return _lstEdgeInfo.pData[ eIndx ].vert ;
}
//
//const VNVECTOR2UI & VTopoGraph::GetVertNbrIndxOfEdge( unsigned eIndx ) const 
//{ 
//    static_assert( 0 ,"This Function is unaviliable , because without implement!" ) ;
//} 

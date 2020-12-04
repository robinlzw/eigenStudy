#pragma once
 
#include <ialgmesh/ialgmesh.h>
#include <vstd/tiaccsr.h>
 
class VTopoMesh : public VTopoGraph
{
public: 
    VTopoMesh(){}
    VTopoMesh( const VSSimpleGraph & src )  ;
    ~VTopoMesh( void ) ;

public :
    void     Rebuild( unsigned vertCount , unsigned triCount , const VNVECTOR3UI * pSurfBuff ) ;

private :
    void     constructRelation( unsigned vertCount , unsigned triCount , const VNVECTOR3UI * pSurfBuff );
    void     initVertInfo( unsigned count ) ;
    void     initSurfVert( const VNVECTOR3UI * pt , unsigned count ) ; // 返回m_lstVertSurf的长度
    void     initVertSurf( unsigned len  ) ;
    void     updateVertSurfRelation( const VNVECTOR3UI * pSurfBuff ) ;
    void     updateVertEdge() ; // 返回m_lstVertEdge应有的长度 

private :   
    std::vector< VertInfo > m_lstVertices     ;
    std::vector< SurfInfo > m_lstSurfaces     ;
    std::vector< EdgeInfo > m_lstEdges        ;
    std::vector< EdgeRef  > m_lstVertEdge     ;
    std::vector< SurfRef  > m_lstVertSurf     ; 
}; 


inline VTopoMesh::VTopoMesh( const VSSimpleGraph & src ) 
{ 
    Rebuild( src.nVertCount , src.nTriangleCount , src.pTriangles ) ;
}
inline VTopoMesh::~VTopoMesh( void )
{
} 

inline void  VTopoMesh::updateVertSurfRelation( const VNVECTOR3UI * pSurfBuff )
{  
    // m_lstVertSurf数据
    for( unsigned i = 0 ; i < m_lstSurfaces.size() ; i ++ )
    {
        SurfInfo    & sInfo = m_lstSurfaces[i] ;
        const VNVECTOR3UI & tri =  pSurfBuff[i] ;
        VNVECTOR3UI & ted =  m_lstSurfaces[i].edge ;
        VNVECTOR3US & tien =  m_lstSurfaces[i].idxInEdgeNbr ;

        for( unsigned j = 0 ; j < 3 ; j ++ )
        {
            VertInfo & vi = m_lstVertices[ tri[j] ] ;
            unsigned & ci = vi.edgeOffset ;
            SurfRef * pb = & m_lstVertSurf[ vi.surfOffset ] ;
            SurfRef & cri = pb[ci] ;

            cri.idx = i ;
            cri.crd = j ;
            cri.pre = VD_INVALID_INDEX ;
            cri.nxt = VD_INVALID_INDEX ;
            vi.edgeCount += 2 ;

            sInfo.idxInVertNbr[j] = ci ;

            for( unsigned k = 0 ; k < ci ; k ++ )
            {
                SurfRef & nri = pb[k] ;
                const VNVECTOR3UI & ntri = pSurfBuff[nri.idx];
                const VNVECTOR3UI & nted = m_lstSurfaces[nri.idx].edge ;

                if( tri[ ( j + 2 ) % 3 ] == ntri[ ( nri.crd + 1 ) % 3 ] ) 
                {
                    cri.pre = k  ;
                    nri.nxt = ci ;
                    vi.edgeCount -- ;

                    ted [ (j+1)%3 ] = nted[ ( nri.crd + 2 ) %3 ] ;
                    tien[ (j+1)%3 ] = 1 ;

                    unsigned eIndex = (j+1)%3 ;
                    EdgeInfo & ei    = m_lstEdges[ ted[ eIndex ] ] ;
                    ei.nbrSurf  [1]  = cri.idx ;
                    ei.idxInSurf[1]  = eIndex ;
                }

                if( tri[ ( j + 1 ) % 3 ] == ntri[ ( nri.crd + 2 ) % 3 ] ) 
                {
                    cri.nxt = k  ;
                    nri.pre = ci ;
                    vi.edgeCount -- ;

                    ted [ (j+2)%3 ] = nted[ ( nri.crd + 1 ) %3 ] ;
                    tien[ (j+2)%3 ] = 1 ;
                    unsigned eIndex = (j+2)%3 ;
                    EdgeInfo & ei   = m_lstEdges[ ted[ eIndex ] ] ;
                    ei.nbrSurf[1]   = cri.idx ;
                    ei.idxInSurf[1] = eIndex ;
                }
            }

            ci ++ ;
        } 

        for( unsigned j = 0 ; j < 3 ; j ++ )
        {
            if( ted[j] == VD_INVALID_INDEX )
            {
                EdgeInfo ei ;
                ei.vert[0] = tri[ ( j + 1 ) %3 ] ;
                ei.vert[1] = tri[ ( j + 2 ) %3 ] ;
                ei.nbrSurf[0]   = i ;
                ei.idxInSurf[0] = j ; 
                ei.nbrSurf[1] = VD_INVALID_INDEX ;

                //sInfo.idxInEdgeNbr[j] = 0  ;
                ted [j] = m_lstEdges.size() ;
                tien[j] = 0 ;

                m_lstEdges.push_back( ei ) ; 
            }
        }
    }
}

inline void VTopoMesh::updateVertEdge()
{   
    unsigned counter(0) ;

    m_lstVertEdge.resize( m_lstEdges.size() * 2 ) ;

    for( unsigned i = 0 ; i < m_lstVertices.size() ; i ++ )
    {
        VertInfo & vi = m_lstVertices[i] ;
        vi.edgeOffset = counter ;
        counter += vi.edgeCount ;

        vi.edgeCount = 0 ;
    }

    for( unsigned i = 0 ; i < m_lstEdges.size() ; i ++ )
    {
        const EdgeInfo & ei = m_lstEdges[i] ;

        for( int j = 0 ; j < 2 ; j ++ )
        {
            unsigned  vidx = ei.vert[j] ;
            VertInfo &vi   = m_lstVertices[ vidx ] ;
            EdgeRef * pvst = & m_lstVertEdge[ vi.edgeOffset ] ;
            EdgeRef & ri   = pvst[ vi.edgeCount ] ;
            ri.idx = i ;
            ri.crd = j ;
            vi.edgeCount ++ ;
        }
    }
}

inline void VTopoMesh::Rebuild( unsigned vertCount , unsigned triCount , const VNVECTOR3UI * pSurfBuff )
{   
    m_lstEdges.resize(0) ; 

    if( triCount > 0 )
    { 
        constructRelation( vertCount , triCount , pSurfBuff ) ; 
    }
    else
    {
        m_lstVertices.clear() ;
        m_lstSurfaces.clear() ; 
        m_lstVertEdge.clear() ;
        m_lstVertSurf.clear() ;
    }

    _lstVertInfo = VD_V2CB( m_lstVertices ) ;
    _lstSurfInfo = VD_V2CB( m_lstSurfaces ) ;
    _lstEdgeInfo = VD_V2CB( m_lstEdges    ) ;
    _lstEdgeRef  = VD_V2CB( m_lstVertEdge ) ;
    _lstSurfRef  = VD_V2CB( m_lstVertSurf ) ;   
}

inline void     VTopoMesh::constructRelation( unsigned vertCount , unsigned triCount , const VNVECTOR3UI * pSurfBuff )
{
    unsigned vc = vertCount  ;  //m_lstPoints.size() ;
    unsigned tc = triCount ;

    initVertInfo( vc ) ;
    initSurfVert( pSurfBuff , tc ) ;
    initVertSurf( tc * 3 ) ;
    updateVertSurfRelation( pSurfBuff ) ; 
    updateVertEdge() ;  
}
inline void     VTopoMesh::initVertInfo( unsigned count ) 
{
    m_lstVertices.resize( count ) ;

    // vi.pos 
    for( unsigned i = 0 ; i < count ; i ++ )   
    {
        VertInfo & vi = m_lstVertices[i] ;
        vi.surfCount = 0 ;
        vi.edgeCount = 0 ; // 用于计数
        vi.edgeOffset= 0 ; // 用于计数
    } 
}
inline void     VTopoMesh::initSurfVert( const VNVECTOR3UI * pt , unsigned count )  // 返回m_lstVertSurf的长度
{
    m_lstSurfaces.resize( count ) ; 

    // vi.surfCount
    // si.vert
    for( unsigned i = 0 ; i < count ; i ++ )
    {
        const VNVECTOR3UI & tri =  pt[i] ;
        SurfInfo & si = m_lstSurfaces[i] ;

        si.edge.x = VD_INVALID_INDEX ;
        si.edge.y = VD_INVALID_INDEX ;
        si.edge.z = VD_INVALID_INDEX ; 

        m_lstVertices[ tri[0] ].surfCount ++ ;
        m_lstVertices[ tri[1] ].surfCount ++ ;
        m_lstVertices[ tri[2] ].surfCount ++ ;
    }
}
inline void     VTopoMesh::initVertSurf( unsigned len  ) 
{
    unsigned counter = 0 ;

    m_lstVertSurf.resize( len ) ;

    // vi.surfOffset
    for( unsigned i = 0 ; i < m_lstVertices.size() ; i ++ )
    {
        VertInfo & vi = m_lstVertices[i] ;
        vi.surfOffset = counter ; 
        counter += vi.surfCount ;
    }
}   

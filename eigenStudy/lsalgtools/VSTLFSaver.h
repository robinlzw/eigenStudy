#pragma once

#include "vmath/vfmesh.h"

class VSTLFSaver 
{
public:
    VSTLFSaver( const std::string & fileName , bool bAscii );
    ~VSTLFSaver(void);

    bool Process( const VSSimpleMeshF & mesh ) ;

private:
    void FacetNormal( unsigned idex , const VFVECTOR3 *  listVertex , const VNVECTOR3UI * listIndex , std::ofstream & ofs ) ;
    void WriteFile( const VFVECTOR3 & Normal , const VFVECTOR3 & Vertex1 , const VFVECTOR3 & Vervex2 , const VFVECTOR3 & Vervex3  , std::ofstream & ofs ) ;

private:
    VFVECTOR3 computeNormal( const VFVECTOR3& pt1 , const VFVECTOR3& pt2 , const VFVECTOR3& pt3 );

protected :
    std::string    m_strFileName ;
    bool            m_bAsciiMode  ;
    

private:
    std::vector< VFVECTOR3 > m_listNormal ;
};
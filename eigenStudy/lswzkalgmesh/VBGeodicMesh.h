#pragma once

#include <ialgmesh/ialgmesh.h>

/****************************************/
//输入为一个网格和计算测地距离时的起始源点，
//输出为网格上所有点的测地距离
/****************************************/

struct geodDisNode
{
    int     Index;
    int     nodeType;   //0: far  1: open   2: dead
    float   dist;
	int     path;      //记录测地距离计算时的源点，起始点记录为其本身的索引
};

class VBGeodicMesh // : public TVGraphMeshAdaptor< VNALGMESH::VSMeshGeodic >
{  
public :
    VBGeodicMesh()
    {}
    ~VBGeodicMesh()
    {}
    void Build( VNALGMESH::VSMeshGeodic & gm , const VSGraphMesh & msh , const VSConstBuffer< unsigned > & cd ) ;

private :
    void  Init( const VSGraphMesh & msh, const VSConstBuffer< unsigned > & cd );
    void  CalEqCoef(const VSGraphMesh & msh, const int &Indx0, const int &Indx1, const int &Indx2);
	bool  UpWind(const VSGraphMesh & msh, const float & solution, const int & Indx0, const int &Indx1, const int &Indx2);
    void  CalQuadEq(const VSGraphMesh & msh, const int &Indx0, const int &Indx1, const int &Indx2);
    void  CalEq(const VSGraphMesh & msh, const int &Indx0, const int &Indx1, const int &Indx2);
    void  UpdateType(const VSGraphMesh & msh, const unsigned & vidx);
    void  CalGeodDist( const VSGraphMesh & msh, const VSConstBuffer< unsigned > & cd );

private : 
    std::vector< float > m_lstDistance ; 
    std::vector< geodDisNode >  m_lstGeodDisNode;    
    std::vector< unsigned >     m_lstNeigbNode;
    std::vector< float    >     m_lstEqCoef;
	//std::vector< geodDisNode >  m_lstTemp;
};

// For use with min-heap and sorting
class minfloatCompare {
public:
    static bool prior(geodDisNode x, geodDisNode y) { return x.dist < y.dist; }
};
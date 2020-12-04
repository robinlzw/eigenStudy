#pragma once

#include <vmath/vtopomesh.h>         //
#include "VGraphAndHeap.h"
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
using namespace VNWZKALG;

//original edge infomation
struct OrgEdge
{
    int     eIndx;
    bool    flag;       //0:ONFACE  1:ORIGINAL
    float   orgLen;
};

//shortest path infomation
struct PathInfoST
{
    float   pathLen;
    int     pathVert;
    int     pathPreVert;
};

//Vextex and SPs infomation
struct Node
{
    float   x;
    float   y;
    float   z;
    int     nodeIndx;
    int     ptrOrgElem;          //point to edge
};

struct GraphEdge                //only for G0  
{
    VNVECTOR2UI  pEdge;
    int          eGraphIndx;
    float        eLen;
};

//link infomation
struct LinkEdge                 //for subdivided edges and additional edges
{
    float        linkLen;       //link's length
    bool         linkType;      //type: original or onface
    int          ptrElem;       //edge to original edge or additional edge to surface under it
    int          linkIndx;
    VNVECTOR2UI  vertLink;      //vertex indx of link
};

class VJPathFinder 
{
private: 
    int begPoint, endPoint; 
    static float m_sMaxDist ;
    static float m_sAlpha;
    static float m_sDelta;

public:
    VJPathFinder()
    {}
    ~VJPathFinder()
    {}

public :
    bool Build(VNALGMESH::IVSurfPath & sp , const VSPerfectMesh & pm , const VNALGMESH::VSPathTerminal & path ) ;

private :
    float calEdgeLen(const int &p0Indx, const int &p1Indx);
    void  initOrgGraph(const VSPerfectMesh & pm);
    void  pathNbrInfo(const VSPerfectMesh & pm);
    void  searchLinkVertIndx(const std::vector<Node> &nd, const unsigned &p1, const unsigned &p2, unsigned *vertArryIndx1, unsigned *vertArryIndx2);
    void  preEdgeAndNode(const VSPerfectMesh & pm);   
    int   pathDijk(Graph  &G, std::vector<Node> &tn, std::vector<float> &td);
    void  extrDijkPath(const std::vector<Node> &tn, const std::vector<VSPathInfo> &pathIf, std::vector<VSPathInfo> &realPath, const int &begd);
    void  Dijkstra(Graph* G, float* D, int s);
    void  arrayBuff();    
    void  genLoopGraph(Graph* G);
    void  searchMaxIndx(const std::vector<Node> &nd, const std::vector<LinkEdge> &lkd, int *maxVert, int *maxEdge);
    void  subGraphEdge(const VSPerfectMesh & pm);
    void  calShortestPath(const VSPerfectMesh & pm);

private:
    std::vector<  int  >         m_lstGphEdgeIndxbuff;       //临时存放图中边的索引
    std::vector<  int  >         m_lstGphVertIndxbuff;
    std::vector<  int  >         m_lstGphSurfIndx;           //存放后续图中面的索引
    std::vector<  int  >         m_lstVertSPIndx;
    std::vector<  int  >         m_lstTempBuff;

    std::vector<VSPathInfo >     m_lstRealPathInfo;
    std::vector<OrgEdge  >       m_lstOrgEdge;
    std::vector<GraphEdge>       m_lstGraphEdge;
    std::vector<VSPathInfo >     m_lstPathInfo;
    std::vector<VSPathInfo >     m_lstPathInfoBuff;
    std::vector<Node     >       m_lstNode;                  //图的节点信息
    std::vector<LinkEdge >       m_lstLinkEdge;
    std::vector<LinkEdge >       m_lstLinkEdgeBuff;
    std::vector<Node     >       m_lstNodeLoop;
    std::vector<LinkEdge >       m_lstLinkEdgeLoop;

    // 
    std::vector< DijkElem > E ; // ( G->n() ) ; // 
    std::vector< float > DistBuffer ; 
};


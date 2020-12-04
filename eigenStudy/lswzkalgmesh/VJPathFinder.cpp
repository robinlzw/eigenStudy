
#include "StdAfx.h"
#include "VJPathFinder.h"


float VJPathFinder::m_sMaxDist = 100.f ;
float VJPathFinder::m_sAlpha   = 3.f;
float VJPathFinder::m_sDelta   = 1e-4f;

//#define MAX  1000
//#define EDGENUM  10000
//#define Alpha   3
//
//VJPathFinder::VJPathFinder( TVConstRef< VSPerfectMesh > & refMesh , TVConstRef< VSPathTerminal > & refTerm ) 
//    : m_refMesh( refMesh ) , m_refTerm( refTerm )
//{
//    Reset() ;
//} 
//
//VJPathFinder::~VJPathFinder(void)
//{
//}

bool VJPathFinder::Build(VNALGMESH::IVSurfPath & sp , const VSPerfectMesh & pm , const VSPathTerminal & path )
{ 
    unsigned startSufIndx = path.mpBegin.triangleIndx ;
    unsigned endSufIndx = path.mpEnd.triangleIndx ;

    begPoint = pm.pSurfaces[ startSufIndx ][0] ;
    endPoint = pm.pSurfaces[ endSufIndx   ][0] ;

    calShortestPath( pm ) ;

    return true ;
}

// Return these distances in "D".
// Dijkstra’s shortest paths algorithm with priority queue
void VJPathFinder::Dijkstra(Graph* G, float* D, int s) 
{
    int i, v, w;                               // v is current vertex
    DijkElem temp;
	VSPathInfo tempPathInfo;
      
    E.resize( G->n() ) ; 

    temp.distance = 0; 
    temp.vertex   = s;
    E[0] = temp;                               // Initialize heap array
    heap<DijkElem, DDComp> H( &E[0] , 1, G->e());    // Create heap
    for (i=0; i<G->n(); i++)                   // Now, get distances
    {                 
        do 
        {           
            if (H.size() == 0) return;         // Nothing to remove
            temp = H.removefirst();
            v    = temp.vertex;
        } while (G->getMark(v) == 1);

        tempPathInfo.pathLen     = temp.distance;
        tempPathInfo.pathVert    = temp.vertex;
        tempPathInfo.pathPreVert = temp.preVert;
        m_lstPathInfo.push_back(tempPathInfo);

        G->setMark(v, 1);
        if (D[v] == m_sMaxDist) return;                               // Unreachable vertices
        for (w = G->first(v); w < G->n(); w = G->next(v,w))                  
        {
            if (D[w] > (D[v] + G->weight(v, w))) 
            {                                                  // Update D
                D[w] = D[v] + G->weight(v, w);
                temp.distance = D[w]; 
                temp.vertex   = w;
                temp.preVert  = v;
                H.insert(temp);                               // Insert new distance in heap            
            }
        }
    }
    
}

float VJPathFinder::calEdgeLen(const int &p0Indx, const int &p1Indx)
{
    float Dis = pow((m_lstNode[p0Indx].x - m_lstNode[p1Indx].x), 2) + pow((m_lstNode[p0Indx].y - m_lstNode[p1Indx].y), 2) + 
                pow((m_lstNode[p0Indx].z - m_lstNode[p1Indx].z), 2);
    return sqrtf(Dis);
}

int VJPathFinder::pathDijk(Graph &G, std::vector<Node> &tn, std::vector<float> &td)
{
    td.resize( G.n() ) ; 
    for (int i = 0; i < G.n(); i ++)                            // Initialize
    {
        td[i] = m_sMaxDist;
    }
    int tempBegPoint = -1;
    for (unsigned i = 0; i < tn.size(); i ++)
    {
        if (tn[i].nodeIndx == begPoint)
        {
            tempBegPoint = i;
            break;
        }
    }
    td[tempBegPoint] = 0;
    VASSERT( ! DistBuffer.empty() ) ;
    Dijkstra(&G, &td[0], tempBegPoint);

    return tempBegPoint;
}

void VJPathFinder::extrDijkPath(const std::vector<Node> &tn, const std::vector<VSPathInfo> &pathIf,
                                  std::vector<VSPathInfo> &realPath, const int &begd)
{
    realPath.clear();

    int temp, tempEndPoint;
    for (unsigned i = 0; i < tn.size(); i ++)
    {
        if (tn[i].nodeIndx == endPoint)
        {
            tempEndPoint = i;
            break;
        }
    }
    for (unsigned i = 0; i < pathIf.size(); i ++)
    {
        if (pathIf[i].pathVert == tempEndPoint)
        {
            realPath.push_back(m_lstPathInfo[i]);          //终点
            temp = pathIf[i].pathPreVert;
        }
    }

    while (temp != begd) //tempBegPoint
    {
        for (unsigned i = 0; i < pathIf.size(); i ++)
        {
            if (pathIf[i].pathVert == temp)
            {
                temp = i;
                realPath.push_back(pathIf[temp]);
                temp = pathIf[temp].pathPreVert;
                break;
            }
        }
    }

    for (unsigned i = 0; i < pathIf.size(); i ++)
    {
        if (pathIf[i].pathVert == begd)   //tempBegPoint
        {
            realPath.push_back(m_lstPathInfo[i]);          //起点
            break;
        }
    }
}

void VJPathFinder::initOrgGraph(const VSPerfectMesh & pm)
{
    Node        tempOrgNode;
    OrgEdge     tempOrgEdge;
    unsigned    vertNum = pm.GetVertexCount();
    unsigned    edgeNum = pm.GetEdgeCount();
    
    Graphl orgGph(vertNum);

    for (unsigned i = 0; i < vertNum; i ++)
    {      
        tempOrgNode.ptrOrgElem = -1;
        tempOrgNode.nodeIndx   = i;
        tempOrgNode.x          = pm.pVertices[i].x;
        tempOrgNode.y          = pm.pVertices[i].y;
        tempOrgNode.z          = pm.pVertices[i].z;
        m_lstNode.push_back(tempOrgNode);                             //original vertex
    }

    for (unsigned i = 0 ; i < edgeNum; i ++)
    {
        const VNVECTOR2UI & vids = pm.GetVertIndxOfEdge( i ) ; 

        tempOrgEdge.eIndx  = i;
        tempOrgEdge.flag   = 1;                                       //original edge
        tempOrgEdge.orgLen = calEdgeLen(vids[0], vids[1]);        
        orgGph.setEdge(vids[0], vids[1], tempOrgEdge.orgLen);         //无向图
        orgGph.setEdge(vids[1], vids[0], tempOrgEdge.orgLen);
        m_lstOrgEdge.push_back(tempOrgEdge);
    }   

    int tempBgNode; 
    tempBgNode = pathDijk(orgGph, m_lstNode,  DistBuffer);

    //提取路径，从终点开始反推路径点
    extrDijkPath(m_lstNode, m_lstPathInfo, m_lstRealPathInfo, tempBgNode);

    for (unsigned i = 0; i < m_lstRealPathInfo.size(); i ++)
    {
        m_lstPathInfoBuff.push_back(m_lstRealPathInfo[i]) ;
    }

}

void VJPathFinder::pathNbrInfo(const VSPerfectMesh & pm)
{
    GraphEdge temp;

    //pathnode neighbor surface
    for (unsigned i = 0; i < m_lstRealPathInfo.size(); i ++)
    {
        const unsigned & visf = pm.GetSurfCountOfVetex(m_lstPathInfoBuff[i].pathVert);

        for (unsigned j = 0; j < visf; j ++)
        {
            int sIndx = pm.GetSurfIndxOfVertex(m_lstPathInfoBuff[i].pathVert, j);
                
            m_lstGphSurfIndx.push_back(sIndx);
        }
    }
    //删除重复的面索引号
    sort(m_lstGphSurfIndx.begin(), m_lstGphSurfIndx.end());
    m_lstGphSurfIndx.erase(unique(m_lstGphSurfIndx.begin(), m_lstGphSurfIndx.end()), m_lstGphSurfIndx.end());


    //pathnode neighbor edge
    for (unsigned i = 0; i < m_lstGphSurfIndx.size(); i ++)
    {
        const VNVECTOR3UI & sied = pm.GetEdgeIndxOfSurf(m_lstGphSurfIndx[i]);
        m_lstGphEdgeIndxbuff.push_back(sied[0]);
        m_lstGphEdgeIndxbuff.push_back(sied[1]);
        m_lstGphEdgeIndxbuff.push_back(sied[2]);
    }
    //删除重复的边索引号
    sort(m_lstGphEdgeIndxbuff.begin(), m_lstGphEdgeIndxbuff.end());
    m_lstGphEdgeIndxbuff.erase(unique(m_lstGphEdgeIndxbuff.begin(), m_lstGphEdgeIndxbuff.end()), m_lstGphEdgeIndxbuff.end());

    
    for (unsigned i = 0; i < m_lstGphEdgeIndxbuff.size(); i ++)
    {
        temp.eGraphIndx = m_lstGphEdgeIndxbuff[i];
        temp.eLen       = m_lstOrgEdge[m_lstGphEdgeIndxbuff[i]].orgLen;
        temp.pEdge      = pm.GetVertIndxOfEdge(m_lstGphEdgeIndxbuff[i]);
        m_lstGraphEdge.push_back(temp);
    }

    //vertex indx in orggraph
    for (unsigned i = 0; i < m_lstGphEdgeIndxbuff.size(); i ++)
    {
        const VNVECTOR2UI & eivt = pm.GetVertIndxOfEdge(m_lstGphEdgeIndxbuff[i]);
        m_lstGphVertIndxbuff.push_back(eivt[0]);
        m_lstGphVertIndxbuff.push_back(eivt[1]);
    }
    //删除重复的点索引号
    sort(m_lstGphVertIndxbuff.begin(), m_lstGphVertIndxbuff.end());
    m_lstGphVertIndxbuff.erase(unique(m_lstGphVertIndxbuff.begin(), m_lstGphVertIndxbuff.end()), m_lstGphVertIndxbuff.end());

}

//查找边的顶点在数组中的下标索引
void VJPathFinder::searchLinkVertIndx(const std::vector<Node> &nd, const unsigned &p1, const unsigned &p2, 
                                        unsigned *vertArryIndx1, unsigned *vertArryIndx2)
{
    for (unsigned i = 0; i < nd.size(); i ++)
    {
        if (nd[i].nodeIndx == p1)
        {
            *vertArryIndx1 = i;
        }
        if (nd[i].nodeIndx == p2)
        {
            *vertArryIndx2 = i;
        }
    }
}

void VJPathFinder::preEdgeAndNode(const VSPerfectMesh & pm)
{
    int       linkCount  = 0;
    int       numOrgVert = pm.GetVertexCount();
    Node      tempNode;
    LinkEdge  tempLink;

    m_lstNode.clear();
    for (unsigned i = 0; i < m_lstGphVertIndxbuff.size(); i ++)
    {
        const int & vidb = m_lstGphVertIndxbuff[i];

        tempNode.x          = pm.pVertices[vidb].x;
        tempNode.y          = pm.pVertices[vidb].y;
        tempNode.z          = pm.pVertices[vidb].z;
        tempNode.nodeIndx   = vidb;
        tempNode.ptrOrgElem = -1;                                                  //original vertex flag -1
        m_lstNode.push_back(tempNode);
    }

    //Add SPs
    for (unsigned i = 0; i < m_lstGraphEdge.size(); i ++)                          //逐条边操作
    {
        GraphEdge & gpd = m_lstGraphEdge[i];

        int m = (int)ceil(m_lstGraphEdge[i].eLen / m_sAlpha - 1);                          //向上取整
        for (int j = 1; j <= m; j ++)
        {
            numOrgVert ++;
            tempNode.x          = (m_lstNode[gpd.pEdge[0]].x * (m - j + 1) + m_lstNode[gpd.pEdge[1]].x * j) / (m + 1);
            tempNode.y          = (m_lstNode[gpd.pEdge[0]].y * (m - j + 1) + m_lstNode[gpd.pEdge[1]].y * j) / (m + 1);
            tempNode.z          = (m_lstNode[gpd.pEdge[0]].z * (m - j + 1) + m_lstNode[gpd.pEdge[1]].z * j) / (m + 1);
            tempNode.ptrOrgElem = gpd.eGraphIndx;                                   //SPs point to original edge indx
            tempNode.nodeIndx   = numOrgVert;                                       //在原始图中的编号
            m_lstNode.push_back(tempNode);
            
            //condition 1.2
            if (j == 1)
            {
                tempLink.linkIndx    = linkCount;
                tempLink.linkLen     = m_lstGraphEdge[i].eLen / (m + 1);
                tempLink.linkType    = 1;                                           //original edge
                tempLink.ptrElem     = m_lstGraphEdge[i].eGraphIndx;
                tempLink.vertLink[0] = m_lstGraphEdge[i].pEdge[0];
                tempLink.vertLink[1] = numOrgVert;
                linkCount ++;
                m_lstLinkEdge.push_back(tempLink);
            }

            if(j != m && j != 1)
            {
                tempLink.linkIndx    = linkCount;
                tempLink.linkLen     = m_lstGraphEdge[i].eLen / (m + 1);
                tempLink.linkType    = 1;                                           //original edge
                tempLink.ptrElem     = m_lstGraphEdge[i].eGraphIndx;
                tempLink.vertLink[0] = numOrgVert - 1;
                tempLink.vertLink[1] = numOrgVert;
                linkCount ++;
                m_lstLinkEdge.push_back(tempLink);
            }

            if (j == m && m != 1)         
            {
                tempLink.linkIndx    = linkCount;
                tempLink.linkLen     = m_lstGraphEdge[i].eLen / (m + 1);
                tempLink.linkType    = 1;                                          //original edge
                tempLink.ptrElem     = m_lstGraphEdge[i].eGraphIndx;
                tempLink.vertLink[0] = numOrgVert - 1;
                tempLink.vertLink[1] = numOrgVert;
                linkCount ++;
                m_lstLinkEdge.push_back(tempLink);
            }

            if (j == m)
            {
                tempLink.linkIndx    = linkCount;
                tempLink.linkLen     = m_lstGraphEdge[i].eLen / (m + 1);
                tempLink.linkType    = 1;                                          //original edge
                tempLink.ptrElem     = m_lstGraphEdge[i].eGraphIndx;
                tempLink.vertLink[0] = numOrgVert;
                tempLink.vertLink[1] = m_lstGraphEdge[i].pEdge[1];
                linkCount ++;
                m_lstLinkEdge.push_back(tempLink);
            }
        }
    }

    Graphl Gph1(m_lstNode.size());

    //点的索引号与顶点数组下标不一致，对图中边做操作时不能直接用点的索引号，必须使用顶点数组下标
    for (unsigned i = 0; i < m_lstLinkEdge.size(); i ++)
    {
        const VNVECTOR2UI &vtlk = m_lstLinkEdge[i].vertLink;

        unsigned vert1, vert2;
        searchLinkVertIndx(m_lstNode, vtlk[0], vtlk[1], &vert1, &vert2);

        Gph1.setEdge(vert1, vert2, m_lstLinkEdge[i].linkLen);
        Gph1.setEdge(vert2, vert1, m_lstLinkEdge[i].linkLen);        
    }


    //condition 1.1    edge onface
    for (unsigned i = 0; i < m_lstGphSurfIndx.size(); i ++)                    //逐面片操作
    {
        const VNVECTOR3UI & sied = pm.GetEdgeIndxOfSurf(m_lstGphSurfIndx[i]);
        const VNVECTOR3UI & sivn = pm.pSurfaces[m_lstGphSurfIndx[i]];
        
        //设置由顶点和对边SP点构成的边
        for (unsigned j = 0; j < 3; j ++)                                     //edge
        {
            m_lstVertSPIndx.clear();
            for (unsigned k = 0; k < 3; k ++)                                 //vertex
            {
                const VNVECTOR2UI & eivt = pm.GetVertIndxOfEdge(sied[j]);

                if (sivn[k] != eivt[0] && sivn[k] != eivt[1])                 //si.idxInVertNbr[k]的对边
                {
                    for (unsigned m = 0; m < m_lstNode.size(); m ++)
                    {
                        if (m_lstNode[m].ptrOrgElem == sied[j])
                        {
                            m_lstVertSPIndx.push_back(m_lstNode[m].nodeIndx);
                        }
                    }

                    for (unsigned n = 0; n < m_lstVertSPIndx.size(); n ++)
                    {
                        unsigned p0Indx, p1Indx;

                        searchLinkVertIndx(m_lstNode, sivn[k], m_lstVertSPIndx[n], &p0Indx, &p1Indx);

                        float weight = calEdgeLen(p0Indx, p1Indx);
                        Gph1.setEdge(p0Indx, p1Indx, weight);
                        Gph1.setEdge(p1Indx, p0Indx, weight);

                        tempLink.linkIndx    = linkCount;
                        tempLink.linkLen     = weight;
                        tempLink.linkType    = 0;                               //edge onface
                        tempLink.ptrElem     = m_lstGphSurfIndx[i];             //point to surface under it
                        tempLink.vertLink[0] = sivn[k];
                        tempLink.vertLink[1] = m_lstVertSPIndx[n];
                        linkCount ++;
                        m_lstLinkEdge.push_back(tempLink);
                    }
                }
               
            }
        }

        //设置只由SPs构成的边 
        m_lstVertSPIndx.clear();
        for (unsigned j = 0; j < 3; j ++)
        {
            for (unsigned k = 0; k < m_lstNode.size(); k ++)
            {
                if(m_lstNode[k].ptrOrgElem == sied[j])
                {
                    m_lstVertSPIndx.push_back(m_lstNode[k].nodeIndx);
                }
            }
        }

        for (unsigned k = 0; k < m_lstVertSPIndx.size(); k ++)
        {
            for (unsigned n = k; n < m_lstVertSPIndx.size(); n ++)
            {
                //点的索引号与顶点数组下标不一致，对图中边做操作时不能直接用点的索引号，必须使用顶点数组下标
                unsigned vert1, vert2;
                searchLinkVertIndx(m_lstNode, m_lstVertSPIndx[k], m_lstVertSPIndx[n], &vert1, &vert2);
                     
                if(!(Gph1.isEdge(vert1, vert2)) && vert1 != vert2)
                {
                    float weight = calEdgeLen(vert1, vert2);
                    Gph1.setEdge(vert1, vert2, weight);
                    Gph1.setEdge(vert2, vert1, weight);
                    
                    tempLink.linkIndx    = linkCount;
                    tempLink.linkLen     = weight;
                    tempLink.linkType    = 0;                              //edge onface 
                    tempLink.ptrElem     = m_lstGphSurfIndx[i];            //point to surface under it
                    tempLink.vertLink[0] = m_lstVertSPIndx[k];
                    tempLink.vertLink[1] = m_lstVertSPIndx[n];
                    linkCount ++;
                    m_lstLinkEdge.push_back(tempLink);
                }
            }
        }

    }

    m_lstPathInfo.clear();

    int tempBgNode; 
    tempBgNode = pathDijk(Gph1, m_lstNode,  DistBuffer);

    //提取路径，从终点开始反推路径点
    extrDijkPath(m_lstNode, m_lstPathInfo, m_lstRealPathInfo, tempBgNode);
}

void VJPathFinder::arrayBuff()
{
    m_lstPathInfoBuff.clear();
    m_lstNodeLoop.clear();
    m_lstLinkEdgeLoop.clear();

    //路径信息存入缓存数组
    for (unsigned i = 0; i < m_lstRealPathInfo.size(); i ++)
    {
        m_lstPathInfoBuff.push_back(m_lstRealPathInfo[i]);
    }

    //图顶点信息存入缓存数组
    for (unsigned i = 0; i < m_lstNode.size(); i ++)
    {
        m_lstNodeLoop.push_back(m_lstNode[i]);
    }

    //图边信息存入缓存数组
    for (unsigned i = 0; i < m_lstLinkEdge.size(); i ++)
    {
        m_lstLinkEdgeLoop.push_back(m_lstLinkEdge[i]);
    }

    m_lstNode.clear();
    m_lstLinkEdge.clear();
}

void VJPathFinder::genLoopGraph(Graph* G)
{
    arrayBuff();

    //查找与路径点相关联的点，如果其连接边为original
    m_lstGphVertIndxbuff.clear();
    for (unsigned i = 0; i < m_lstPathInfoBuff.size(); i ++)
    {

		VSPathInfo & pti = m_lstPathInfoBuff[i];
        m_lstGphVertIndxbuff.push_back(m_lstNodeLoop[pti.pathVert].nodeIndx);             

        //点的索引号与顶点数组下标不一致，对图中边做操作时不能直接用点的索引号，必须使用顶点数组下标
        for (unsigned j = 0; j < m_lstNodeLoop.size(); j ++)
        {           
            if (G->isEdge(pti.pathVert, j))
            {
                for (unsigned m = 0; m < m_lstLinkEdgeLoop.size(); m ++)
                {
                    LinkEdge & led = m_lstLinkEdgeLoop[m];

                    if (((m_lstNodeLoop[pti.pathVert].nodeIndx == led.vertLink[0] && m_lstNodeLoop[j].nodeIndx == led.vertLink[1]) || 
                         (m_lstNodeLoop[pti.pathVert].nodeIndx == led.vertLink[1] && m_lstNodeLoop[j].nodeIndx == led.vertLink[0])) && 
                         led.linkType == 1)                                                  //step 2
                    {
                        m_lstGphVertIndxbuff.push_back(m_lstNodeLoop[j].nodeIndx);
                    }
                }
            }
        }
    }
    //删除重复的点索引号
    sort(m_lstGphVertIndxbuff.begin(), m_lstGphVertIndxbuff.end());
    m_lstGphVertIndxbuff.erase(unique(m_lstGphVertIndxbuff.begin(), m_lstGphVertIndxbuff.end()), m_lstGphVertIndxbuff.end());

    //保存路径点及其邻接点信息
    for (unsigned i = 0; i < m_lstGphVertIndxbuff.size(); i ++)
    {
        for (unsigned j = 0; j < m_lstNodeLoop.size(); j ++)
        {
            if (m_lstNodeLoop[j].nodeIndx == m_lstGphVertIndxbuff[i])
            {
                m_lstNode.push_back(m_lstNodeLoop[j]);
            }
        }

    }

    //与路径点相连接的linkedge信息
    for (unsigned i = 0; i < m_lstNode.size(); i ++)
    {
        const unsigned & ndIndx1 = m_lstNode[i].nodeIndx;

        for (unsigned j = i; j < m_lstNode.size(); j ++)
        {
            const unsigned & ndIndx2 = m_lstNode[j].nodeIndx;

            unsigned vert1, vert2;
            searchLinkVertIndx(m_lstNodeLoop, ndIndx1, ndIndx2, &vert1, &vert2);

            if (G->isEdge(vert1, vert2) && vert1 != vert2)
            {
                for (unsigned m = 0; m < m_lstLinkEdgeLoop.size(); m ++)
                {
                    const VNVECTOR2UI & templep = m_lstLinkEdgeLoop[m].vertLink;

                    if ((ndIndx1 == templep[0] && ndIndx2 == templep[1]) ||
                        (ndIndx1 == templep[1] && ndIndx2 == templep[0]))
                    {
                        m_lstLinkEdge.push_back(m_lstLinkEdgeLoop[m]);
                    }
                }
            }
        }
    }

}

void VJPathFinder::searchMaxIndx(const std::vector<Node> &nd, const std::vector<LinkEdge> &lkd, int *maxVert, int *maxEdge)
{
    //寻找顶点的最大索引
    for (unsigned i = 0; i < nd.size(); i ++)
    {
        if (*maxVert < nd[i].nodeIndx)
        {
            *maxVert = nd[i].nodeIndx;
        }
    }

    //寻找边的最大索引
    for (unsigned i = 0; i < lkd.size(); i ++)
    {
        if (*maxEdge < lkd[i].linkIndx)
        {
            *maxEdge = lkd[i].linkIndx;
        }
    }
}

void VJPathFinder::subGraphEdge(const VSPerfectMesh & pm)
{

    int       numOrgVert = 0;       
    int       numOrgEdge = 0;      
    Node      tempNode;
    LinkEdge  tempLink;

    searchMaxIndx(m_lstNodeLoop, m_lstLinkEdgeLoop, &numOrgVert, &numOrgEdge);

    //二等分边
    //Add SPs and Additional linkedge
    m_lstLinkEdgeBuff.clear();
    for (unsigned i = 0; i < m_lstLinkEdge.size(); i ++)
    {
        LinkEdge & led = m_lstLinkEdge[i];
        unsigned vert1, vert2;

        if (led.linkType == 1)
        {
            searchLinkVertIndx(m_lstNode, led.vertLink[0], led.vertLink[1], &vert1, &vert2);

            tempNode.nodeIndx    = ++ numOrgVert;
            tempNode.ptrOrgElem  = led.ptrElem;
            tempNode.x           = (m_lstNode[vert1].x + m_lstNode[vert2].x) / 2;
            tempNode.y           = (m_lstNode[vert1].y + m_lstNode[vert2].y) / 2;
            tempNode.z           = (m_lstNode[vert1].z + m_lstNode[vert2].z) / 2;
            m_lstNode.push_back(tempNode);

            tempLink.linkIndx    = ++ numOrgEdge;
            tempLink.linkLen     = led.linkLen / 2;
            tempLink.linkType    = led.linkType;
            tempLink.ptrElem     = led.ptrElem;
            tempLink.vertLink[0] = led.vertLink[0];
            tempLink.vertLink[1] = tempNode.nodeIndx;
            m_lstLinkEdgeBuff.push_back(tempLink);
            
            tempLink.linkIndx    = ++ numOrgEdge;
            tempLink.linkLen     = led.linkLen / 2;
            tempLink.linkType    = led.linkType;
            tempLink.ptrElem     = led.ptrElem;
            tempLink.vertLink[0] = tempNode.nodeIndx;
            tempLink.vertLink[1] = led.vertLink[1];
            m_lstLinkEdgeBuff.push_back(tempLink);
        }
    }

    //将非原始边加入缓存linkedgebuff中,避免删除操作
    for (unsigned i = 0; i < m_lstLinkEdge.size(); i ++)
    {
        if (m_lstLinkEdge[i].linkType == 0)
        {
            m_lstLinkEdgeBuff.push_back(m_lstLinkEdge[i]);
        }
    }

    m_lstLinkEdge.clear();
    for (unsigned i = 0; i < m_lstLinkEdgeBuff.size(); i ++)
    {
        m_lstLinkEdge.push_back(m_lstLinkEdgeBuff[i]);
    }

    Graphl gphLoop(m_lstNode.size());
    
    for (unsigned i = 0; i < m_lstLinkEdge.size(); i ++)
    {
        const VNVECTOR2UI &vt = m_lstLinkEdge[i].vertLink;
        unsigned vert1, vert2;
        searchLinkVertIndx(m_lstNode, vt[0], vt[1], &vert1, &vert2);

        gphLoop.setEdge(vert1, vert2, m_lstLinkEdge[i].linkLen);
        gphLoop.setEdge(vert2, vert1, m_lstLinkEdge[i].linkLen);
    }

    //处理 additional edge
    for (unsigned i = 0; i < m_lstGphSurfIndx.size(); i ++)                     //逐面片操作
    {
        m_lstTempBuff.clear();

        const VNVECTOR3UI & sied = pm.GetEdgeIndxOfSurf(m_lstGphSurfIndx[i]);
        const VNVECTOR3UI & sivn = pm.pSurfaces[i];                             //面片的三个顶点

        for (unsigned j = 0; j < m_lstLinkEdge.size(); j ++)
        {
            //指定linkedge的linkType，避免指向面片的ptrElem和指向边的ptrElem重复
            if (m_lstLinkEdge[j].linkType == 1 && (m_lstLinkEdge[j].ptrElem == sied[0] || 
                m_lstLinkEdge[j].ptrElem ==  sied[1] || m_lstLinkEdge[j].ptrElem == sied[2]))
            {
                m_lstTempBuff.push_back(m_lstLinkEdge[j].vertLink[0]);
                m_lstTempBuff.push_back(m_lstLinkEdge[j].vertLink[1]);
            }
        }
        //删除重复的点索引号
        sort(m_lstTempBuff.begin(), m_lstTempBuff.end());
        m_lstTempBuff.erase(unique(m_lstTempBuff.begin(), m_lstTempBuff.end()), m_lstTempBuff.end());

        for (unsigned k = 0; k < m_lstTempBuff.size(); k ++)
        {
            for (unsigned j = k; j < m_lstTempBuff.size(); j ++)
            {
                unsigned vert1, vert2; 
                searchLinkVertIndx(m_lstNode, m_lstTempBuff[k], m_lstTempBuff[j], &vert1, &vert2);

                if (m_lstNode[vert1].ptrOrgElem == -1)
                {
                    unsigned tempNum;

                    for (unsigned n = 0; n < 3; n ++)
                    {
                        const VNVECTOR2UI & vi = pm.GetVertIndxOfEdge(sied[n]);

                        if (m_lstNode[vert1].nodeIndx != vi[0] && m_lstNode[vert1].nodeIndx != vi[1])    //查找原始点的对边
                        {
                            tempNum = sied[n];
                        }
                    }

                    if (m_lstNode[vert2].ptrOrgElem == tempNum)
                    {
                        if (!(gphLoop.isEdge(vert1, vert2)) && vert1 != vert2)
                        {
                            float weight = calEdgeLen(vert1, vert2);

                            gphLoop.setEdge(vert1, vert2, weight);
                            gphLoop.setEdge(vert2, vert1, weight);

                            tempLink.linkIndx    = ++numOrgEdge;
                            tempLink.linkLen     = weight;
                            tempLink.linkType    = 0;
                            tempLink.ptrElem     = m_lstGphSurfIndx[i];
                            tempLink.vertLink[0] = m_lstNode[vert1].nodeIndx;
                            tempLink.vertLink[1] = m_lstNode[vert2].nodeIndx;
                            m_lstLinkEdge.push_back(tempLink);
                        }
                    }
                }    
                if (m_lstNode[vert1].ptrOrgElem != -1 && m_lstNode[vert1].ptrOrgElem != m_lstNode[vert2].ptrOrgElem)
                {
                    if (!(gphLoop.isEdge(vert1, vert2)) && vert1 != vert2)
                    {
                        float weight = calEdgeLen(vert1, vert2);

                        gphLoop.setEdge(vert1, vert2, weight);
                        gphLoop.setEdge(vert2, vert1, weight);

                        tempLink.linkIndx    = ++numOrgEdge;
                        tempLink.linkLen     = weight;
                        tempLink.linkType    = 0;
                        tempLink.ptrElem     = m_lstGphSurfIndx[i];
                        tempLink.vertLink[0] = m_lstNode[vert1].nodeIndx;
                        tempLink.vertLink[1] = m_lstNode[vert2].nodeIndx;
                        m_lstLinkEdge.push_back(tempLink);
                    }
                }
            }
        }


    }

    m_lstPathInfo.clear();

    int tempBgNode; 
    tempBgNode = pathDijk(gphLoop, m_lstNode,  DistBuffer);

    //提取路径，从终点开始反推路径点
    extrDijkPath(m_lstNode, m_lstPathInfo, m_lstRealPathInfo, tempBgNode);
}

void VJPathFinder::calShortestPath(const VSPerfectMesh & pm)
{
    initOrgGraph(pm);
    pathNbrInfo(pm);
    preEdgeAndNode(pm);

    while (abs(m_lstRealPathInfo[0].pathLen - m_lstPathInfoBuff[0].pathLen) > m_sDelta) 
    {
        Graphl GPH(m_lstNode.size());

        for (unsigned i = 0; i < m_lstLinkEdge.size(); i ++)
        {
            const VNVECTOR2UI & vilk = m_lstLinkEdge[i].vertLink;

            unsigned vert1, vert2;
            searchLinkVertIndx(m_lstNode, vilk[0], vilk[1], &vert1, &vert2);

            GPH.setEdge(vert1, vert2, m_lstLinkEdge[i].linkLen);
            GPH.setEdge(vert2, vert1, m_lstLinkEdge[i].linkLen);
        }
        genLoopGraph(&GPH);
        subGraphEdge(pm); 
    }

}

VD_EXPORT_SYSTEM_SIMPLE(VJPathFinder, VNALGMESH::VRPathFinder);
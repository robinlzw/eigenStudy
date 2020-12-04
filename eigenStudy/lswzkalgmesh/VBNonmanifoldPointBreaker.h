#pragma once
#include <sys/alg/isysmesh.h>
#include <obj/alg/ialgwzk.h>
#include <vmath/vflocation.h>

class VBNonmanifoldPointBreaker
{
public:
    VBNonmanifoldPointBreaker(const VSESS< VNALGMESH::VRPerfMesh > & pm):m_PerfMesh(pm){}
    ~VBNonmanifoldPointBreaker(){}

    void Build( VSSimpleMeshF & smsh , const VSSimpleMeshF & msh );

private:
    TVR2B< VNALGMESH::VRPerfMesh > m_PerfMesh;

    std::vector<VFVECTOR3>   m_vVertices;
    std::vector<VNVECTOR3UI> m_vSurfaces;
};
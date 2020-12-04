#pragma once
#include <vector>

class HEVertex;
class HEEdge;
class HEFace;
class HalfEdge;

typedef std::vector<HalfEdge>::iterator              HalfEdgeIter;
typedef std::vector<HalfEdge>::const_iterator       HalfEdgeCIter;
typedef std::vector<HEVertex>::iterator                 VertexIter;
typedef std::vector<HEVertex>::const_iterator          VertexCIter;
typedef std::vector<HEEdge>::iterator                     EdgeIter;
typedef std::vector<HEEdge>::const_iterator              EdgeCIter;
typedef std::vector<HEFace>::iterator                     FaceIter;
typedef std::vector<HEFace>::const_iterator              FaceCIter;
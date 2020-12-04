#pragma once
#include "Type.h"

class HEEdge
{
public:

	HEEdge() : index(0) {}
	~HEEdge() {}

	HalfEdgeIter he;   // points to one of the two halfedges associated with this edge	
	unsigned index;    // unique integer ID in the range 0, ..., nEdges-1		
};
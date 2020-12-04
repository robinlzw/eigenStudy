#pragma once
#include <vmath\vfvector3.h>
#include "Type.h"
#include "HalfEdge.h"
#include "HEFace.h"

class HEVertex
{
public:
	HEVertex() : index(0), tag(false), distance(0.f) {}

	float     area(void) const;            // returns the barycentric area associated with this vertex	
	VFVector3 normal(void) const;          // returns the vertex normal	
	bool      isIsolated(void) const;      // returns true if the vertex is not contained in any face or edge; false otherwise	
	int       valence(void) const;         // returns the number of incident faces / edges	
	void      toggle();                    // toggle vertex tag 	

	HalfEdgeIter he;                 // points to the "outgoing" halfedge	
	VFVector3 position;              // location of vertex in Euclidean 3-space	
	unsigned index;                  // unique integer ID in the range 0, ..., nVertices-1	
	bool tag;                        // true if vertex is selected by the user; false otherwise	
	float distance;
};

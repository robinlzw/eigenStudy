#pragma once
#include <vmath\vfvector3.h>
#include "Type.h"
#include "HEVertex.h"

class HEFace
{
public:
	HEFace() : index(0) {}

	bool      isBoundary(void) const;   // returns true if this face corresponds to a boundary loop; false otherwise
	float     area(void) const;        // returns the triangle area
	VFVector3 normal(void) const;      // returns the unit normal associated with this face; normal
	                                   // orientation is determined by the circulation order of halfedges
	VFVector3 circumcenter(void) const;   // returns triangle circumcenter
	VFVector3 barycenter(void) const;     // returns triangle barycenter

	HalfEdgeIter he;         // points to one of the halfedges associated with this face
	unsigned index;          // unique integer ID in the range 0, ..., nFaces-1
	VFVector3  tanVector;    // tangent vector per triangle		
};
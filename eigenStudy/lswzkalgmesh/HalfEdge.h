#pragma once
#include <vmath\vfvector3.h>
#include "Type.h"
#include "HEVertex.h"

class HalfEdge
{
public:
	HalfEdge() {}
	~HalfEdge() {}

	float cotan(void) const;   // returns the cotangent of the angle opposing this edge
	VFVector3  rotateEdge(void) const;    // returns oriented edge vector rotated by PI/2 around face normal if onBoundary, then return nil  

	HalfEdgeIter  next;   // points to the next halfedge around the current face
	HalfEdgeIter  flip;   // points to the other halfedge associated with this edge
	VertexIter    vertex;  // points to the vertex at the "tail" of this halfedge
	EdgeIter      edge;   // points to the edge associated with this halfedge
	FaceIter      face;   // points to the face containing this halfedge
	bool onBoundary;      // true if this halfedge is contained in a boundary loop; false otherwise
	VFVector3  texcoord;  // texture coordinates associated with the triangle corner at the "tail" of this halfedge
};

#include "stdafx.h"
#include "HalfEdge.h"

float HalfEdge::cotan(void) const
{
	if (onBoundary)
		return 0.f;

	VFVector3 p0 = next->next->vertex->position;
	VFVector3 p1 = vertex->position;
	VFVector3 p2 = next->vertex->position;

	VFVector3 u = p1 - p0;
	VFVector3 v = p2 - p0;
	return u.Dot(v) / u.Cross(v).Magnitude();
}

VFVector3 HalfEdge::rotateEdge(void) const
{
	if (onBoundary)
		return VFVector3();

	VFVector3 n = face->normal();
	VFVector3 p0 = vertex->position;
	VFVector3 p1 = flip->vertex->position;

	return n.Cross(p1 - p0);
}

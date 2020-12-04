#include "stdafx.h"
#include "HEFace.h"

bool HEFace::isBoundary(void) const
{
	return he->onBoundary;
}

float HEFace::area(void) const
{
	VFVector3 v;
	VFVector3 p0 = he->vertex->position;
	VFVector3 p1 = he->next->vertex->position;
	VFVector3 p2 = he->next->next->vertex->position;

	v = p1 - p0;
	return v.Cross(p2 - p0).Magnitude() / 2.f;
}

VFVector3 HEFace::normal(void) const
{
	VFVector3 v;
	VFVector3 p0 = he->vertex->position;
	VFVector3 p1 = he->next->vertex->position;
	VFVector3 p2 = he->next->next->vertex->position;

	v = p1 - p0;
	return v.Cross(p2 - p0).Direction();
}

VFVector3 HEFace::circumcenter(void) const
{
	VFVector3 p0 = he->vertex->position;
	VFVector3 p1 = he->next->vertex->position;
	VFVector3 n = he->rotateEdge();
	float     h = 0.5f * he->cotan();
	return 0.5f * (p0 + p1) + h * n;
}

VFVector3 HEFace::barycenter(void) const
{
	VFVector3 p0 = he->vertex->position;
	VFVector3 p1 = he->next->vertex->position;
	VFVector3 p2 = he->next->next->vertex->position;
	return (p0 + p1 + p2) / 3.f;
}

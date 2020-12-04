#include "stdafx.h"
#include "HEVertex.h"

float HEVertex::area(void) const
{
	float A = 0.f;
	HalfEdgeCIter  h = he;
	do
	{
		if (!h->onBoundary)
			A += h->face->area();
		h = h->flip->next;
	} while (h != he);
	
	return A / 3.f;
}

VFVector3 HEVertex::normal(void) const
{
	VFVector3  N;
	HalfEdgeCIter h = he;
	do
	{
		if (!h->onBoundary)
		{
			N += h->face->normal();
			h = h->flip->next;
		}
	} while (h != he);

	return N.Direction();
}

std::vector<HalfEdge> isolated; // all isolated vertices point to isolated.begin()
bool HEVertex::isIsolated(void) const
{
	if (!isolated.size())
		return false;

	return he == isolated.begin();
}

int HEVertex::valence(void) const
{
	int n = 0;
	HalfEdgeCIter h = he;
	do
	{
		n ++;
		h = h->flip->next;
	} while (h != he);

	return n;
}

void HEVertex::toggle()
{
	tag = !tag;
}

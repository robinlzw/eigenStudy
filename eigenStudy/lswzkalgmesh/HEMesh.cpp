#include "stdafx.h"
#include "HEMesh.h"
#include "HEMeshConstruct.h"

HEMesh::HEMesh(const HEMesh & mesh)
{
	*this = mesh;
}

class HalfEdgeIterCompare { public: bool operator () (const HalfEdgeIter  & i, const HalfEdgeIter  & j) const { return &*i < &*j; } };
class HalfEdgeCIterCompare { public: bool operator () (const HalfEdgeCIter & i, const HalfEdgeCIter & j) const { return &*i < &*j; } };
class VertexIterCompare { public: bool operator () (const VertexIter    & i, const VertexIter    & j) const { return &*i < &*j; } };
class VertexCIterCompare { public: bool operator()  (const VertexCIter   & i, const VertexCIter   & j) const { return &*i < &*j; } };
class FaceIterCompare { public: bool operator()  (const FaceIter      & i, const FaceIter      & j) const { return &*i < &*j; } };
class FaceCIterCompare { public: bool operator()  (const FaceCIter     & i, const FaceCIter     & j) const { return &*i < &*j; } };
class EdgeIterCompare { public: bool operator()  (const EdgeIter      & i, const EdgeIter      & j) const { return &*i < &*j; } };
class EdgeCIterCompare { public: bool operator()  (const EdgeCIter     & i, const EdgeCIter     & j) const { return &*i < &*j; } };

const HEMesh & HEMesh::operator=(const HEMesh & mesh)
{
	std::map<HalfEdgeCIter, HalfEdgeIter, HalfEdgeCIterCompare>   halfedgeOldToNew;
	std::map<VertexCIter, VertexIter, VertexCIterCompare  >   vertexOldToNew;
	std::map<EdgeCIter, EdgeIter, EdgeCIterCompare    >   edgeOldToNew;
	std::map<FaceCIter, FaceIter, FaceCIterCompare    >   faceOldToNew;

	// copy geometry from the original mesh and create a
	// map from pointers in the original mesh to
	// those in the new mesh
	halfedges.clear();
	for (HalfEdgeCIter he = mesh.halfedges.begin(); he != mesh.halfedges.end(); he++)
		halfedgeOldToNew[he] = halfedges.insert(halfedges.end(), *he);

	vertices.clear();
	for (VertexCIter v = mesh.vertices.begin(); v != mesh.vertices.end(); v++)
		vertexOldToNew[v] = vertices.insert(vertices.end(), *v);

	edges.clear();
	for (EdgeCIter e = mesh.edges.begin(); e != mesh.edges.end(); e++)
		edgeOldToNew[e] = edges.insert(edges.end(), *e);

	faces.clear();
	for (FaceCIter f = mesh.faces.begin(); f != mesh.faces.end(); f++)
		faceOldToNew[f] = faces.insert(faces.end(), *f);

	// "search and replace" old pointers with new ones
	for (HalfEdgeIter he = halfedges.begin(); he != halfedges.end(); he++)
	{
		he->next = halfedgeOldToNew[he->next];
		he->flip = halfedgeOldToNew[he->flip];
		he->vertex = vertexOldToNew[he->vertex];
		he->edge = edgeOldToNew[he->edge];
		he->face = faceOldToNew[he->face];
	}

	for (VertexIter v = vertices.begin(); v != vertices.end(); v++) v->he = halfedgeOldToNew[v->he];
	for (EdgeIter e = edges.begin(); e != edges.end(); e++) e->he = halfedgeOldToNew[e->he];
	for (FaceIter f = faces.begin(); f != faces.end(); f++) f->he = halfedgeOldToNew[f->he];

	return *this;
}

int HEMesh::read(const VSSimpleMeshF & smesh)
{
	int rval;
	if ((rval = HEMeshConstruct::read(smesh, *this, nonmanifoldEdge)))
	{
		indexElements();
		normalize();
	}

	return rval;
}

int HEMesh::read(const std::string & filename)
{
	inputFilename = filename;
	std::ifstream in(filename.c_str());

	if (!in.is_open())
	{
		//std::cerr << "Error reading from mesh file " << filename << std::endl;
		return 1;
	}

	int rval;
	if (!(rval = HEMeshConstruct::read(in, *this)))
	{
		indexElements();
		normalize();
	}
	return rval;
}

// reads a mesh from a Wavefront OBJ file; return value is nonzero
// only if there was an error
int HEMesh::write(const std::string & filename) const
{
	std::ofstream out(filename.c_str());

	if (!out.is_open())
	{
		//std::cerr << "Error writing to mesh file " << filename << std::endl;
		return 1;
	}

	HEMeshConstruct::write(out, *this);

	return 0;
}

int HEMesh::reload(void)
{
	return read(inputFilename);
}

void HEMesh::normalize(void)
{
	// compute center of mass
	VFVector3 c(0., 0., 0.);
	for (VertexCIter v = vertices.begin(); v != vertices.end(); v++)
	{
		c += v->position;
	}
	c /= (float)vertices.size();

	// translate to origin
	for (VertexIter v = vertices.begin(); v != vertices.end(); v++)
	{
		v->position -= c;
	}

	// rescale such that the mesh sits inside the unit ball
	float rMax = 0.f;
	for (VertexCIter v = vertices.begin(); v != vertices.end(); v++)
	{
		rMax = std::max(rMax, v->position.Magnitude());
	}
	for (VertexIter v = vertices.begin(); v != vertices.end(); v++)
	{
		v->position /= rMax;
	}
}

float HEMesh::area(void) const
{
	float sum = 0.0;
	for (FaceCIter f = faces.begin(); f != faces.end(); f++)
	{
		sum += f->area();
	}
	return sum;
}

float HEMesh::meanEdgeLength(void) const
{
	float sum = 0;
	for (EdgeCIter e = edges.begin(); e != edges.end(); e++)
	{
		VertexIter v0 = e->he->vertex;
		VertexIter v1 = e->he->flip->vertex;
		sum += (v0->position - v1->position).Magnitude();
	}
	return sum / edges.size();
}

void HEMesh::indexElements(void)
{
	int nV = 0;
	for (VertexIter v = vertices.begin(); v != vertices.end(); v++)
	{
		v->index = nV;
		nV++;
	}

	int nE = 0;
	for (EdgeIter e = edges.begin(); e != edges.end(); e++)
	{
		e->index = nE;
		nE++;
	}

	int nF = 0;
	for (FaceIter f = faces.begin(); f != faces.end(); f++)
	{
		f->index = nF;
		nF++;
	}
}
#pragma once
#include <string>
#include <fstream>
#include <map>
#include <algorithm>
#include <iostream>
#include <vmath\vfmesh.h>

#include"HalfEdge.h"
#include"HEVertex.h"
#include"HEFace.h"
#include"HEEdge.h"

class HEMesh
{
public:
	HEMesh() {};
	// constructs an empty mesh

	HEMesh(const HEMesh & mesh);
	// constructs a copy of mesh

	const HEMesh & operator = (const HEMesh & mesh);
	// copies mesh

	int read(const VSSimpleMeshF & smesh);

	int read(const std::string & filename);
	// reads a mesh from a Wavefront OBJ file; return value is nonzero
	// only if there was an error

	int write(const std::string & filename) const;
	// writes a mesh to a Wavefront OBJ file; return value is nonzero
	// only if there was an errorf

	int reload(void);
	// reloads a mesh from disk using the most recent input filename

	void normalize(void);
	// centers around the origin and rescales to have unit radius

	float area(void) const;
	// returns total mesh area

	float meanEdgeLength(void) const;
	// returns mean edge lenght

	std::vector<HalfEdge>    halfedges;
	std::vector<HEVertex>      vertices;
	std::vector<HEEdge>        edges;
	std::vector<HEFace>        faces;
	std::vector<HEFace>        boundaries;
	// storage for mesh elements

	std::pair<unsigned, unsigned>     nonmanifoldEdge;
protected:
	std::string  inputFilename;

	void indexElements(void);
	// assigns a unique, 0-based index to each mesh element
};
#include "stdafx.h"
#include <set>
#include "HEMeshConstruct.h"

int HEMeshConstruct::read(const VSSimpleMeshF & smesh, HEMesh & mesh, std::pair<unsigned, unsigned> & nonmanifoldEdge)
{
	MeshData data;
	readMeshData(smesh, data);
	
	if (mbuildMesh(data, mesh, nonmanifoldEdge))
		return 1;
	
	return 0;
}



// reads a mesh from a valid, open input stream in
int HEMeshConstruct::read(std::istream & in, HEMesh & mesh)
{
	MeshData data;

	if (readMeshData(in, data))
		return 1;

	if (buildMesh(data, mesh))
		return 1;

	return 0;
}

// writes a mesh to a valid, open output stream out
void HEMeshConstruct::write(std::ostream & out, const HEMesh & mesh)
{
	int currentIndex = 1;
	std::map<VertexCIter, int> vertexIndex;

	for (VertexCIter v = mesh.vertices.begin(); v != mesh.vertices.end(); v++)
	{
		out << "v " << v->position.x << " " << v->position.y << " " << v->position.z << std::endl;
		vertexIndex[v] = currentIndex;
		currentIndex++;
	}

	for (FaceCIter f = mesh.faces.begin(); f != mesh.faces.end(); f++)
	{
		HalfEdgeIter he = f->he;

		for (unsigned i = 0; i < 3; i++)
		{
			out << "vt" << he->texcoord.x << " " << he->texcoord.y << std::endl;
			he = he->next;
		}
	}

	for (size_t i = 0; i < mesh.faces.size(); i++)
	{
		const HEFace & f(mesh.faces[i]);
		HalfEdgeIter he = f.he;

		out << "f";
		int j = 0;
		do
		{
			out << vertexIndex[he->vertex] << "/" << 1 + (i * 3 + j) << " ";
			he = he->next;
			j++;
		} while (he != f.he);

		out << std::endl;
	}
}



int HEMeshConstruct::readMeshData(const VSSimpleMeshF & smesh, MeshData & data)
{
	data.positions.resize(smesh.nVertCount);
	memcpy_s(&data.positions.front(), sizeof(VFVector3) * smesh.nVertCount, smesh.pVertices, sizeof(VFVector3) * smesh.nVertCount);

	data.indices.resize(smesh.nTriangleCount);
	std::vector<Index> faceIndices;
	faceIndices.resize(3);
	Index  tid(-1, -1, -1);
	for (unsigned i = 0; i < smesh.nTriangleCount; i++)
	{
		tid.position = smesh.pTriangles[i].x;
		faceIndices[0] = tid;

		tid.position = smesh.pTriangles[i].y;
		faceIndices[1] = tid;

		tid.position = smesh.pTriangles[i].z;
		faceIndices[2] = tid;

		data.indices[i] = faceIndices;
	}

	return 1;
}

int HEMeshConstruct::readMeshData(std::istream & in, MeshData & data)
{
	std::string line;

	while (std::getline(in, line))
	{
		std::stringstream  ss(line);
		std::string token;

		ss >> token;

		if (token == "v") { readPosition(ss, data); continue; } //vertex
		if (token == "vt") { readTexCoord(ss, data); continue; } //texture coordinate
		if (token == "vn") { readNormal(ss, data); continue; }   //vertex normal
		if (token == "f") { readFace(ss, data); continue; }     //face
		if (token[0] == '#') continue;  //comment
		if (token == "o") continue;     //object name
		if (token == "g") continue;     //group name
		if (token == "s") continue;     //smoothing group
		if (token == "mtllib") continue;//material library
		if (token == "usemtl") continue;//material
		if (token == "") continue;      //empty string

		return 1;
	}
	return 0;
}




void HEMeshConstruct::readPosition(std::stringstream & ss, MeshData & data)
{
	float x, y, z;
	ss >> x >> y >> z;
	data.positions.push_back(VFVector3(x, y, z));
}




void HEMeshConstruct::readTexCoord(std::stringstream & ss, MeshData & data)
{
	float u, v;
	ss >> u >> v;
	data.texcoords.push_back(VFVector3(u, v, 0.f));
}



void HEMeshConstruct::readNormal(std::stringstream & ss, MeshData & data)
{
	float x, y, z;
	ss >> x >> y >> z;
	data.normals.push_back(VFVector3(x, y, z));
}

void HEMeshConstruct::readFace(std::stringstream & ss, MeshData & data)
{
	std::vector<Index> faceIndices;
	std::string token;
	while (ss >> token)
	{
		faceIndices.push_back(parseFaceIndex(token));
	}
	data.indices.push_back(faceIndices);
}

Index HEMeshConstruct::parseFaceIndex(const std::string & token)
{
	// parse indices of the form
	//
	// p/[t]/[n]
	//
	// where p is an index into positions, t is an index into
	// texcoords, n is an index into normals, and [.] indicates
	// that an index is optional

	std::stringstream  in(token);
	std::string indexstring;
	int indices[3] = { -1, -1, -1 };
	int i = 0;

	while (std::getline(in, indexstring, '/'))
	{
		std::stringstream  ss(indexstring);
		ss >> indices[i++];
	}

	// decrement since indices in OBJ files are 1-based
	return Index(indices[0] - 1, indices[1] - 1, indices[2] - 1);
}

void HEMeshConstruct::preallocateMeshElements(const MeshData & data, HEMesh & mesh)
{
	// count the number of edges
	std::set<std::pair<int, int>> edges;
	for (std::vector<std::vector<Index>>::const_iterator f = data.indices.begin();
		f != data.indices.end(); f++)
	{
		for (unsigned I = 0; I < f->size(); I++)
		{
			int J = (I + 1) % f->size();
			int i = (*f)[I].position;
			int j = (*f)[J].position;

			if (i > j) std::swap(i, j);

			edges.insert(std::pair<int, int>(i, j));
		}
	}

	int nV = data.positions.size();
	int nE = edges.size();
	int nF = data.indices.size();
	int nHE = 2 * nE;
	int chi = nV - nE + nF;
	int nB = std::max(0, 2 - chi);   // (conservative approximation of number of boundary cycles)

	mesh.halfedges.clear();
	mesh.vertices.clear();
	mesh.edges.clear();
	mesh.faces.clear();
	mesh.boundaries.clear();

	/*mesh.halfedges.reserve(nHE);
	mesh.vertices.reserve(nV);
	mesh.edges.reserve(nE);
	mesh.faces.reserve(nF);*/
	mesh.boundaries.reserve(nB);
	mesh.halfedges.resize(nHE);
	mesh.vertices.resize(nV);
	mesh.edges.resize(nE);
	mesh.faces.resize(nF);
	//mesh.boundaries.resize(nB);
}

extern std::vector<HalfEdge> isolated;   // all isolated vertices point to isolated.begin()

int HEMeshConstruct::buildMesh(const MeshData & data, HEMesh & mesh)
{
	std::map<std::pair<int, int>, int>           edgeCount;
	std::map<std::pair<int, int>, HalfEdgeIter>  existingHalfEdges;
	std::map<int, VertexIter>                    indexToVertex;
	std::map<HalfEdgeIter, bool>                 hasFlipEdge;

	preallocateMeshElements(data, mesh);

	// allocate a vertex for each position in the data and construct
	// a map from vertex indices to vertex pointers
	for (unsigned i = 0; i < data.positions.size(); i++)
	{
		VertexIter newVertex = mesh.vertices.insert(mesh.vertices.end(), HEVertex());
		newVertex->position = data.positions[i];
		newVertex->he = isolated.begin();
		indexToVertex[i] = newVertex;
	}

	// insert each face into the mesh
	int faceIndex = 0;
	bool degenerateFaces = false;
	for (std::vector<std::vector<Index>>::const_iterator f = data.indices.begin();
		f != data.indices.end(); f++)
	{
		int N = f->size();

		// print an error if the face is degenerate
		if (N < 3)
		{
			//std::cerr << "Error : face " << faceIndex << " is degenerate(fewer than three vertices) !" << std::endl;
			degenerateFaces = true;
			continue;
		}

		// create a new face
		FaceIter newFace = mesh.faces.insert(mesh.faces.end(), HEFace());

		// create a new half edge for each edge of the current face
		std::vector<HalfEdgeIter>  hes(N);
		for (int i = 0; i < N; i++)
		{
			hes[i] = mesh.halfedges.insert(mesh.halfedges.end(), HalfEdge());			
		}

		// initialize these new halfedges
		for (int i = 0; i < N; i++)
		{
			// the current halfedge goes from vertex a to vertex b
			int a = (*f)[i].position;
			int b = (*f)[(i + 1) % N].position;

			// set current halfedge's attributes
			hes[i]->next = hes[(i + 1) % N];
			hes[i]->vertex = indexToVertex[a];
			int t = (*f)[i].texcoord;
			if (t >= 0) hes[i]->texcoord = data.texcoords[t];
			else        hes[i]->texcoord = VFVector3(0.f, 0.f, 0.f);
			hes[i]->onBoundary = false;

			// keep track of which halfedges have flip edges defined (for detecting boundaries)
			hasFlipEdge[hes[i]] = false;

			// point vertex a at the current halfedge
			indexToVertex[a]->he = hes[i];

			// point the new face and this half edge to each-other
			hes[i]->face = newFace;
			newFace->he = hes[i];

			// if we've created an edge between a and b in the past, it is the
			// flip edge of the current halfedge
			if (a > b) std::swap(a, b);
			if (existingHalfEdges.find(std::pair<int, int>(a, b)) != existingHalfEdges.end())
			{
				hes[i]->flip = existingHalfEdges[std::pair<int, int>(a, b)];
				hes[i]->flip->flip = hes[i];
				hes[i]->edge = hes[i]->flip->edge;
				hasFlipEdge[hes[i]] = true;
				hasFlipEdge[hes[i]->flip] = true;
			}
			else // otherwise, create an edge connected to the current halfedge
			{
				hes[i]->edge = mesh.edges.insert(mesh.edges.end(), HEEdge());
				hes[i]->edge->he = hes[i];
				edgeCount[std::pair<int, int>(a, b)] = 0;
			}

			// record the fact that we've created a halfedge from a to b
			existingHalfEdges[std::pair<int, int>(a, b)] = hes[i];

			// check for nonmanifold edges
			edgeCount[std::pair<int, int>(a, b)]++;
			if (edgeCount[std::pair<int, int>(a, b)] > 2)
			{
				//std::cerr << "Error: edge (" << a << ", " << b << ") is nonmanifold (more than two faces sharing a single edge)!" << std::endl;
				return 1;
			}
		}
		faceIndex++;
	}

	// give up now if there were degenerate faces
	if (degenerateFaces)
		return 1;

	// insert extra faces for each boundary cycle
	for (HalfEdgeIter currentHE = mesh.halfedges.begin();
		currentHE != mesh.halfedges.end(); currentHE++)
	{
		// if we find a halfedge with no flip edge defined, create
		// a new face and link it to the corresponding boundary cycle
		if (!hasFlipEdge[currentHE])
		{
			// create a new face
			FaceIter newBoundary = mesh.boundaries.insert(mesh.boundaries.end(), HEFace());

			// walk along this boundary cycle
			std::vector<HalfEdgeIter> boundaryCycle;
			HalfEdgeIter he = currentHE;
			do
			{
				// create a new halfedge on the boundary face
				HalfEdgeIter newHE = mesh.halfedges.insert(mesh.halfedges.end(), HalfEdge());				

				// mark only the halfedge on the boundary face as being on the boundary
				newHE->onBoundary = true;

				// link the current halfedge in the cycle to its new flip edge
				he->flip = newHE;

				// grab the next halfedge along the boundary by finding
				// the next halfedge around the current vertex that doesn't
				// have a flip edge defined
				HalfEdgeIter nextHE = he->next;
				while (hasFlipEdge[nextHE])
				{
					nextHE = nextHE->flip->next;
				}

				// set attributes for the flip edge (we'll set ->next below)
				newHE->flip = he;
				newHE->vertex = nextHE->vertex;
				newHE->edge = he->edge;
				newHE->face = newBoundary;
				newHE->texcoord = nextHE->texcoord;

				// point the new face to this half edge
				newBoundary->he = newHE;

				// keep track of all the new halfedges in the boundary cycle
				boundaryCycle.push_back(newHE);

				// continue to walk along the cycle
				he = nextHE;
			} while (he != currentHE);

			// link together the cycle of boundary halfedges
			unsigned N = boundaryCycle.size();
			for (unsigned i = 0; i < N; i++)
			{
				boundaryCycle[i]->next = boundaryCycle[(i + N - 1) % N];
				hasFlipEdge[boundaryCycle[i]] = true;
				hasFlipEdge[boundaryCycle[i]->flip] = true;
			}
		}
	}

	// print a warning if the input has any non-terminal defects
	checkIsolatedVertices(mesh);
	checkNonManifoldVertices(mesh);

	return 0;
}

void HEMeshConstruct::checkIsolatedVertices(const HEMesh & Mesh)
{
	// print a warning if the mesh has any isolated vertices
	int vertexIndex = 0;
	for (VertexCIter v = Mesh.vertices.begin(); v != Mesh.vertices.end(); v++)
	{
		//if (v->isIsolated())
		//{
		//	std::cerr << "Warning : vertex " << vertexIndex << " is isolated (not contained in any face)." << std::endl;
		//}
		vertexIndex++;
	}
}

void HEMeshConstruct::checkNonManifoldVertices(const HEMesh & Mesh)
{
	std::map<VertexCIter, int> nIndicentFaces;

	for (FaceCIter f = Mesh.faces.begin(); f != Mesh.faces.end(); f++)
	{
		HalfEdgeCIter he = f->he;
		do
		{
			nIndicentFaces[he->vertex] ++;
			he = he->next;
		} while (he != f->he);
	}

	for (FaceCIter f = Mesh.boundaries.begin(); f != Mesh.boundaries.end(); f++)
	{
		HalfEdgeCIter he = f->he;
		do
		{
			nIndicentFaces[he->vertex] ++;
			he = he->next;
		} while (he != f->he);
	}

	int vertexIndex = 0;
	for (VertexCIter v = Mesh.vertices.begin(); v != Mesh.vertices.end(); v++)
	{
		//if (nIndicentFaces[v] != v->valence())
		//{
		//	std::cerr << "Warning : vertex " << vertexIndex << " is nomanifold." << std::endl;
		//}
		vertexIndex++;
	}
}

int HEMeshConstruct::mbuildMesh(const MeshData & data, HEMesh & mesh, std::pair<unsigned, unsigned> & nonmanifoldEdge)
{
	std::map<std::pair<int, int>, std::vector<unsigned>>   edgeFace;

	std::map<std::pair<int, int>, int>           edgeCount;
	std::map<std::pair<int, int>, HalfEdgeIter>  existingHalfEdges;
	std::map<int, VertexIter>                    indexToVertex;
	std::map<HalfEdgeIter, bool>                 hasFlipEdge;

	preallocateMeshElements(data, mesh);
	int vertPos = 0, edgePos = 0, facePos = 0, hePos = 0, bndryPos = 0;

	// allocate a vertex for each position in the data and construct
	// a map from vertex indices to vertex pointers
	VertexIter newVertex = mesh.vertices.begin();            ////////
	for (unsigned i = 0; i < data.positions.size(); i++)
	{
		/*VertexIter newVertex = mesh.vertices.insert(mesh.vertices.end(), HEVertex());
		newVertex->position = data.positions[i];
		newVertex->he = isolated.begin();
		indexToVertex[i] = newVertex;*/
		mesh.vertices[i] = HEVertex();
		mesh.vertices[i].position = data.positions[i];
		mesh.vertices[i].he = isolated.begin();
		indexToVertex[i] = newVertex;
		newVertex++;
	}

	// insert each face into the mesh
	int faceIndex = 0;
	//bool degenerateFaces = false;
	FaceIter m_newFace = mesh.faces.begin();       ////////////
	HalfEdgeIter  m_heIter = mesh.halfedges.begin();    ///////
	EdgeIter m_edgeIter = mesh.edges.begin();           /////////
	for (std::vector<std::vector<Index>>::const_iterator f = data.indices.begin();
		f != data.indices.end(); f++)
	{
		int N = f->size();

		// print an error if the face is degenerate
		/*if (N < 3)
		{
			std::cerr << "Error : face " << faceIndex << " is degenerate(fewer than three vertices) !" << std::endl;
			degenerateFaces = true;
			continue;
		}*/

		// create a new face
		//FaceIter newFace = mesh.faces.insert(mesh.faces.end(), HEFace());
		mesh.faces[facePos] = HEFace();

		// create a new half edge for each edge of the current face
		std::vector<HalfEdgeIter>  hes(N);
		for (int i = 0; i < N; i++)
		{
			//hes[i] = mesh.halfedges.insert(mesh.halfedges.end(), HalfEdge());
			mesh.halfedges[hePos] = HalfEdge();
			hes[i] = m_heIter;
			hePos++;
			m_heIter++;
		}

		// initialize these new halfedges
		for (int i = 0; i < N; i++)
		{
			// the current halfedge goes from vertex a to vertex b
			int a = (*f)[i].position;
			int b = (*f)[(i + 1) % N].position;

			// set current halfedge's attributes
			hes[i]->next = hes[(i + 1) % N];
			hes[i]->vertex = indexToVertex[a];
			int t = (*f)[i].texcoord;
			if (t >= 0) hes[i]->texcoord = data.texcoords[t];
			else        hes[i]->texcoord = VFVector3(0.f, 0.f, 0.f);
			hes[i]->onBoundary = false;

			// keep track of which halfedges have flip edges defined (for detecting boundaries)
			hasFlipEdge[hes[i]] = false;

			// point vertex a at the current halfedge
			indexToVertex[a]->he = hes[i];

			// point the new face and this half edge to each-other
			hes[i]->face = m_newFace;
			m_newFace->he = hes[i];

			// if we've created an edge between a and b in the past, it is the
			// flip edge of the current halfedge
			if (a > b) std::swap(a, b);
			if (existingHalfEdges.find(std::pair<int, int>(a, b)) != existingHalfEdges.end())
			{
				hes[i]->flip = existingHalfEdges[std::pair<int, int>(a, b)];
				hes[i]->flip->flip = hes[i];
				hes[i]->edge = hes[i]->flip->edge;
				hasFlipEdge[hes[i]] = true;
				hasFlipEdge[hes[i]->flip] = true;
			}
			else // otherwise, create an edge connected to the current halfedge
			{
				//hes[i]->edge = mesh.edges.insert(mesh.edges.end(), HEEdge());
				mesh.edges[edgePos] = HEEdge();   ////
				hes[i]->edge = m_edgeIter;    ///////
				hes[i]->edge->he = hes[i];
				edgeCount[std::pair<int, int>(a, b)] = 0;

				edgePos++;   ////
				m_edgeIter++;    ////
			}

			// record the fact that we've created a halfedge from a to b
			existingHalfEdges[std::pair<int, int>(a, b)] = hes[i];

			// check for nonmanifold edges
			edgeCount[std::pair<int, int>(a, b)]++;
			edgeFace[std::pair<int, int>(a, b)].push_back(faceIndex);
			if (edgeCount[std::pair<int, int>(a, b)] > 2)
			{
				//std::cerr << "Error: edge (" << a << ", " << b << ") is nonmanifold (more than two faces sharing a single edge)!" << std::endl;
				
				nonmanifoldEdge.first = a;
				nonmanifoldEdge.second = b;
				return 0;
			}
		}
		faceIndex++;

		facePos++;   ////
		m_newFace++;  ////
	}

	// give up now if there were degenerate faces
	/*if (degenerateFaces)
		return 1;*/

	// insert extra faces for each boundary cycle
	//FaceIter m_bndryIter = mesh.boundaries.begin();   /////////
	for (HalfEdgeIter currentHE = mesh.halfedges.begin();
		currentHE != mesh.halfedges.end(); currentHE++)
	{
		// if we find a halfedge with no flip edge defined, create
		// a new face and link it to the corresponding boundary cycle
		if (!hasFlipEdge[currentHE])
		{
			// create a new face
			FaceIter newBoundary = mesh.boundaries.insert(mesh.boundaries.end(), HEFace());
			/*mesh.boundaries[bndryPos] = HEFace();
			FaceIter newBoundary = m_bndryIter;
			bndryPos++;
			m_bndryIter++;*/

			// walk along this boundary cycle
			std::vector<HalfEdgeIter> boundaryCycle;
			HalfEdgeIter he = currentHE;
			do
			{
				// create a new halfedge on the boundary face
				//HalfEdgeIter newHE = mesh.halfedges.insert(mesh.halfedges.end(), HalfEdge());
				mesh.halfedges[hePos] = HalfEdge();
				HalfEdgeIter newHE = m_heIter;
				hePos++;
				m_heIter++;

				// mark only the halfedge on the boundary face as being on the boundary
				newHE->onBoundary = true;

				// link the current halfedge in the cycle to its new flip edge
				he->flip = newHE;

				// grab the next halfedge along the boundary by finding
				// the next halfedge around the current vertex that doesn't
				// have a flip edge defined
				HalfEdgeIter nextHE = he->next;
				while (hasFlipEdge[nextHE])
				{
					nextHE = nextHE->flip->next;
				}

				// set attributes for the flip edge (we'll set ->next below)
				newHE->flip = he;
				newHE->vertex = nextHE->vertex;
				newHE->edge = he->edge;
				newHE->face = newBoundary;
				newHE->texcoord = nextHE->texcoord;

				// point the new face to this half edge
				newBoundary->he = newHE;

				// keep track of all the new halfedges in the boundary cycle
				boundaryCycle.push_back(newHE);

				// continue to walk along the cycle
				he = nextHE;
			} while (he != currentHE);

			// link together the cycle of boundary halfedges
			unsigned N = boundaryCycle.size();
			for (unsigned i = 0; i < N; i++)
			{
				boundaryCycle[i]->next = boundaryCycle[(i + N - 1) % N];
				hasFlipEdge[boundaryCycle[i]] = true;
				hasFlipEdge[boundaryCycle[i]->flip] = true;
			}
		}
	}

	return 1;
}

std::vector<unsigned> HEMeshConstruct::mcheckIsolatedVertices(const HEMesh & Mesh)
{
	// print a warning if the mesh has any isolated vertices
	std::vector<unsigned>   isolated;
	int vertexIndex = 0;
	for (VertexCIter v = Mesh.vertices.begin(); v != Mesh.vertices.end(); v++)
	{
		if (v->isIsolated())
			isolated.push_back(vertexIndex);

		vertexIndex++;
	}

	return isolated;
}

std::vector<unsigned> HEMeshConstruct::mcheckNonManifoldVertices(const HEMesh & Mesh)
{
	std::vector<unsigned>   nmVert;
	std::map<VertexCIter, int> nIndicentFaces;

	for (FaceCIter f = Mesh.faces.begin(); f != Mesh.faces.end(); f++)
	{
		HalfEdgeCIter he = f->he;
		do
		{
			nIndicentFaces[he->vertex] ++;
			he = he->next;
		} while (he != f->he);
	}

	for (FaceCIter f = Mesh.boundaries.begin(); f != Mesh.boundaries.end(); f++)
	{
		HalfEdgeCIter he = f->he;
		do
		{
			nIndicentFaces[he->vertex] ++;
			he = he->next;
		} while (he != f->he);
	}

	int vertexIndex = 0;
	for (VertexCIter v = Mesh.vertices.begin(); v != Mesh.vertices.end(); v++)
	{
		if (nIndicentFaces[v] != v->valence())
			nmVert.push_back(vertexIndex);

		vertexIndex++;
	}

	return nmVert;
}
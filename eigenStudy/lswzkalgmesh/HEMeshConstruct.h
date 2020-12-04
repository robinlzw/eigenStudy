#pragma once
#include <vmath\vfvector3.h>
#include "HEMesh.h"

//构建半边结构网格

class Index
{
public:
	Index(void)
	{}

	Index(int p, int t, int n)
		: position(p), texcoord(t), normal(n)
	{}

	bool operator<(const Index& i) const
	{
		if (position < i.position) return true;
		if (position > i.position) return false;
		if (texcoord < i.texcoord) return true;
		if (texcoord > i.texcoord) return false;
		if (normal < i.normal) return true;
		if (normal > i.normal) return false;
		return false;
	}

	int position;
	int texcoord;
	int normal;
};



class MeshData
{
public:
	std::vector<VFVector3> positions;
	std::vector<VFVector3> texcoords;
	std::vector<VFVector3> normals;
	std::vector< std::vector< Index > > indices;
};

class HEMeshConstruct
{
public:
	static int read(const VSSimpleMeshF & smesh, HEMesh & mesh, std::pair<unsigned, unsigned> & nonmanifoldEdge);

	static int read(std::istream& in, HEMesh & mesh);
	// reads a mesh from a valid, open input stream in

	static void write(std::ostream& out, const HEMesh & mesh);
	// writes a mesh to a valid, open output stream out
	     
	static std::vector<unsigned>  mcheckIsolatedVertices(const HEMesh& Mesh);
	static std::vector<unsigned>  mcheckNonManifoldVertices(const HEMesh& Mesh);

protected:
	static  int  readMeshData(const VSSimpleMeshF & smesh, MeshData & data);
	static  int  mbuildMesh(const MeshData& data, HEMesh& mesh, std::pair<unsigned, unsigned> & nonmanifoldEdge);

	static  int  readMeshData(std::istream& in, MeshData& data);
	static void  readPosition(std::stringstream& ss, MeshData& data);
	static void  readTexCoord(std::stringstream& ss, MeshData& data);
	static void  readNormal(std::stringstream& ss, MeshData& data);
	static void  readFace(std::stringstream& ss, MeshData& data);
	static Index parseFaceIndex(const std::string& token);
	static void  preallocateMeshElements(const MeshData& data, HEMesh& mesh);
	static  int  buildMesh(const MeshData& data, HEMesh& mesh);
	static void  checkIsolatedVertices(const HEMesh& Mesh);
	static void  checkNonManifoldVertices(const HEMesh& Mesh);
};

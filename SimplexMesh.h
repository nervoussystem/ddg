#pragma once

#include "matrix.h"
#include "ofMesh.h"

class SimplexMesh {
public:
	class NeighborIterator;
	SparseMatrix faceToEdge, edgeToFace, edgeToVert, vertToEdge; //go from edges to vertices, defines orientation of edge, exterior derivative 0 form
	SparseMatrix faceToVertex;

	Eigen::Matrix<Real, 3, Eigen::Dynamic> positions;
	Eigen::Matrix<Real, 3, Eigen::Dynamic> normals;
	Vector edgeLengths;
	Vector faceAreas;

	void loadFromOfMesh(const ofMesh & mesh);

	Vector3 getVertex(unsigned int index) {
		return positions.col(index);
	}
	void setVertex(unsigned int index, Vector3 & v) {
		//unsigned int index3 = index * 3;
		positions.col(index) = v;
		//positions.segment(index3, index3 + 2) = v;
	}

	unsigned int getNumVertices() {
		return edgeToVert.cols();
	}
	unsigned int getNumEdges() {
		return edgeToVert.rows();
	}
	unsigned int getNumFaces() {
		return faceToEdge.rows();
	}
	void computeEdgeLengths();
	void computeFaceAreas();
	Real edgeAngle(unsigned int edge);
	Real cotan(unsigned int edge, unsigned int face);
	Vector3 normal(unsigned int face);
	SparseMatrix laplacian();
	SparseMatrix hodgeStar0();
	SparseMatrix hodgeStar1();

	pair<int, int> getFacesByEdge(unsigned int edge) {
		int nonZeroes = edgeToFace.row(edge).nonZeros();

		if (nonZeroes == 0) {
			//error no faces border
			cerr << "No faces border edge " << edge << endl;
			return make_pair(-1, -1);
		}
		else if (nonZeroes == 1) {
			//boundary edge
			SparseMatrix::InnerIterator it(edgeToFace, edge);

			return make_pair(it.col(), -1);
		}
		else if (nonZeroes == 2) {
			SparseMatrix::InnerIterator it(edgeToFace, edge);
			int face1 = it.col();
			++it;
			return make_pair(face1, it.col());
		}
		else {
			cerr << "More than 2 faces border edge. That's unpossible! " << edge  << endl;
		}
		
	}

	pair<int, int> getVerticesByEdge(unsigned int edge) {
		SparseMatrix::InnerIterator it(edgeToVert, edge);
		int v1 = it.col();
		++it;
		//no error check
		return make_pair(v1, it.col());
	}

	pair<int, int> getVerticesByEdge(unsigned int edge, int dir) {
		pair<int, int> out;
		SparseMatrix::InnerIterator it(edgeToVert, edge);
		int v1 = it.col();
		
		++it;
		if (it.value() == dir) {
			out.first = v1;
			out.second = it.col();
		}
		else {
			out.second = v1;
			out.first = it.col();
		}
		//no error check
		return out;
	}

	template<class OutputIterator>
	void getEdgesByVertex(unsigned int vert, OutputIterator out) {
		for (SparseMatrix::InnerIterator it(vertToEdge, vert); it; ++it) {
			out = it.col();
		}
	}

	vector<int> getEdgesByVertex(unsigned int vert) {
		vector<int> out;
		getEdgesByVertex(vert, back_inserter(out));
		return out;
	}

	tuple<int,int,int> getEdgesByFace(unsigned int face) {
		SparseMatrix::InnerIterator it(faceToEdge, face);
		//check for 3 nonZeroes entries
		int f1 = it.col();
		++it;
		int f2 = it.col();
		++it;
		return make_tuple(f1, f2, it.col());
	}
	NeighborIterator neighborsBegin(unsigned int vert);
	

	class NeighborIterator{
		SimplexMesh *mesh;
	public:
		unsigned int edge;
		unsigned int vertex;
		int prevFace;
		unsigned int centerVertex;
		public:
			NeighborIterator() {

			}

			NeighborIterator(SimplexMesh * m, unsigned int v) {
				centerVertex = v;
				mesh = m;
				SparseMatrix::InnerIterator it(mesh->vertToEdge, v);
				edge = it.col();
				auto eVerts = mesh->getVerticesByEdge(edge);
				if (eVerts.first == v) {
					vertex = eVerts.second;
				}
				else {
					vertex = eVerts.first;
				}
				auto eFaces = mesh->getFacesByEdge(edge);
				prevFace = eFaces.first;
			}

			friend bool operator== (NeighborIterator const &lhs, NeighborIterator const &rhs) {

			}
			NeighborIterator& operator++() {
				//get edge faces
				auto edgeFaces = mesh->getFacesByEdge(edge);
				//get face != prevEdge
				int face = edgeFaces.first;
				if (face == prevFace) {
					face = edgeFaces.second;
				}
				//get face edges
				auto faceEdges = mesh->getEdgesByFace(face);
				//get edge with one pt == center and one pt != vertex
				prevFace = face;
				int nextEdge = get<0>(faceEdges);
				if (nextEdge != edge) {
					auto edgeVerts = mesh->getVerticesByEdge(nextEdge);
					if (edgeVerts.first == centerVertex) {
						edge = nextEdge;
						vertex = edgeVerts.second;
						return *this;
					}
					else if (edgeVerts.second == centerVertex) {
						edge = nextEdge;
						vertex = edgeVerts.first;
						return *this;
					}
				}
				nextEdge = get<1>(faceEdges);
				if (nextEdge != edge) {
					auto edgeVerts = mesh->getVerticesByEdge(nextEdge);
					if (edgeVerts.first == centerVertex) {
						edge = nextEdge;
						vertex = edgeVerts.second;
						return *this;
					}
					else if (edgeVerts.second == centerVertex) {
						edge = nextEdge;
						vertex = edgeVerts.first;
						return *this;
					}
				}
				nextEdge = get<2>(faceEdges);
				if (nextEdge != edge) {
					auto edgeVerts = mesh->getVerticesByEdge(nextEdge);
					if (edgeVerts.first == centerVertex) {
						edge = nextEdge;
						vertex = edgeVerts.second;
						return *this;
					}
					else if (edgeVerts.second == centerVertex) {
						edge = nextEdge;
						vertex = edgeVerts.first;
						return *this;
					}
				}
			}
	};
};	


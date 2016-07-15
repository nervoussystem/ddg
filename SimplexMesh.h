#pragma once

#include "matrix.h"
#include "ofMesh.h"

class SimplexMesh {
public:
	SparseMatrix edgeMap; //go from edges to vertices, defines orientation of edge, exterior derivative 0 form
	SparseMatrix faceMap; //go from faces to edges, encodes relative orientation of face to edge, exterior derivative 1 form

	Eigen::Matrix<Real, 3, Eigen::Dynamic> positions;
	Vector edgeLengths;
	Vector faceAreas;
	Vector normals;

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
		return edgeMap.cols();
	}
	unsigned int getNumEdges() {
		return edgeMap.rows();
	}
	unsigned int getNumFaces() {
		return faceMap.rows();
	}
	void computeEdgeLengths();
	void computeFaceAreas();

	pair<int, int> getFacesByEdge(unsigned int edge) {
		auto edgeCol = faceMap.col(edge);
		int nonZeroes = edgeCol.nonZeros();
		return make_pair(0, 0);

/*
if (nonZeroes == 0) {
			//error no faces border
			cerr << "No faces border edge " << edge << endl;
			return make_pair(-1, -1);
		}
		else if (nonZeroes == 1) {
			//boundary edge
			for (int k = 0; k<edgeCol.outerSize(); ++k)
				for (SparseMatrix::InnerIterator it(edgeCol, k); it; ++it)
				{

					it.row();   // row index
					it.col();   // col index (here it is equal to k)
					it.index(); // inner index, here it is equal to it.row()
				}
			return make_pair(edgeCol.innerNonZeroPtr()[0], -1);
		}
		else if (nonZeroes == 2) {
			return make_pair(edgeCol.innerNonZeroPtr()[0], edgeCol.innerNonZeroPtr()[1]);
		}
		else {
			cerr << "More than 2 faces border edge. That's unpossible! " << edge  << endl;
		}
		*/
	}

	pair<int, int> getVerticesByEdge(unsigned int edge) {
		SparseMatrix edgeCol = edgeMap.row(edge);
		auto nonZeroes = edgeCol.innerNonZeroPtr();
		return make_pair(nonZeroes[0], nonZeroes[1]);
	}


	class NeighborIterator{
		unsigned int edge;
		unsigned int vertex;
		int prevFace;
		unsigned int centerVertex;
		public:
			NeighborIterator() {

			}

			friend bool operator== (NeighborIterator const &lhs, NeighborIterator const &rhs) {

			}
			NeighborIterator& operator++() {
				//get edge faces
				//get face != prevEdge
				//get face edges
				//get edge with one pt == center and one pt != vertex
				//prevFace = face
				return *this;
			}
	};
};	


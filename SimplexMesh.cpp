#include "SimplexMesh.h"

void SimplexMesh::loadFromOfMesh(const ofMesh & mesh) {
	//does not account for non index based meshes
	ofIndexType numVerts = mesh.getNumVertices();
	ofIndexType numFaces = mesh.getNumIndices()/3;

	//do positions
	positions.resize(3, numVerts);
	for (int i = 0; i < numVerts; ++i) {
		ofVec3f v = mesh.getVertex(i);
		positions.col(i) << v.x, v.y, v.z;
	}

	//get edges
	SparseMatrix vv;
	vv.resize(numVerts, numVerts);
	int numEdges = 0;
	for (unsigned int i = 0; i < numFaces * 3;) {
		unsigned int fIndex = i / 3;
		ofIndexType i1 = mesh.getIndex(i++);
		ofIndexType i2 = mesh.getIndex(i++);
		ofIndexType i3 = mesh.getIndex(i++);
		if (!vv.coeff(i1, i2)) {
			numEdges++;
			vv.insert(i1, i2) = numEdges;
			vv.insert(i2, i1) = -numEdges;
		}
		if (!vv.coeff(i2, i3)) {
			numEdges++;
			vv.insert(i2, i3) = numEdges;
			vv.insert(i3, i2) = -numEdges;
		}
		if (!vv.coeff(i3, i1)) {
			numEdges++;
			vv.insert(i3, i1) = numEdges;
			vv.insert(i1, i3) = -numEdges;
		}
	}
	faceToEdge.resize(numFaces, numEdges);
	faceToEdge.reserve(numFaces * 3);
	edgeToVert.resize(numEdges, numVerts);
	faceToEdge.setZero();
	edgeToVert.setZero();
	edgeToVert.reserve(numEdges * 2);
	//inserts are inefficient
	for (unsigned int i = 0; i < numFaces * 3;) {
		unsigned int fIndex = i / 3;
		ofIndexType i1 = mesh.getIndex(i++);
		ofIndexType i2 = mesh.getIndex(i++);
		ofIndexType i3 = mesh.getIndex(i++);

		Real val = vv.coeff(i1, i2);
		//get sign for relative direction
		faceToEdge.insert(fIndex, abs(val) - 1) = (0.0 < val) - (val < 0.0);
		val = vv.coeff(i2, i3);
		faceToEdge.insert(fIndex, abs(val) - 1) = (0.0 < val) - (val < 0.0);
		val = vv.coeff(i3, i1);
		faceToEdge.insert(fIndex, abs(val) - 1) = (0.0 < val) - (val < 0.0);
	}

	for (int k = 0; k < vv.outerSize(); ++k) {
		for (SparseMatrix::InnerIterator it(vv, k); it; ++it) {
			if (it.value() > 0) {
				edgeToVert.insert(it.value()-1, it.row()) = -1;
				edgeToVert.insert(it.value() - 1, it.col()) = -1;
			}
		}
	}
	faceToEdge.makeCompressed();
	edgeToVert.makeCompressed();
	edgeToFace = faceToEdge.transpose();
	vertToEdge = edgeToVert.transpose();
}

void SimplexMesh::computeFaceAreas() {
	faceAreas.resize(getNumFaces());

	//cache this?
	faceToVertex = faceToEdge.cwiseAbs()*edgeToVert.cwiseAbs()*0.5;
	if (!faceToVertex.isCompressed()) faceToVertex.makeCompressed();
	for (int k = 0; k < faceToVertex.outerSize(); ++k) {
		auto inner = faceToVertex.innerVector(k);
		if (inner.nonZeros() != 3) {
			cout << "Error: face vertices != 3" << endl;
			continue;
		}

		SparseMatrix::InnerIterator it(faceToVertex, k);

		Vector3 p1 = getVertex(it.col());
		++it;
		Vector3 p2 = getVertex(it.col());
		++it;
		Vector3 p3 = getVertex(it.col());
		p2 -= p1;
		p3 -= p1;
		Vector3 norm = p2.cross(p3);
		Real area = norm.norm();
		
		faceAreas[k] = area*0.5;
	}
}

void SimplexMesh::computeEdgeLengths() {
	edgeLengths.resize(getNumEdges());

	for (int k = 0; k < edgeToVert.outerSize(); ++k) {
		SparseMatrix::InnerIterator it(edgeToVert, k);

		Vector3 p1 = getVertex(it.col());
		++it;
		Vector3 p2 = getVertex(it.col());
		edgeLengths[k] = (p2 - p1).norm();
	}
}

Real SimplexMesh::edgeAngle(unsigned int edge) {
	SparseMatrix::InnerIterator it(edgeToFace, edge);
	Vector3 n1 = normals.col(it.col());
	++it;
	Vector3 n2 = normals.col(it.col());
	Real cosA = n1.dot(n2);
	//oops that's not right I need to sin too
	return acos(cosA);
}

Real SimplexMesh::cotan(unsigned int edge, unsigned int face) {
	//faceToVertex.row(face).eval().indices();
	int* edgeI = edgeToVert.row(edge).innerIndexPtr();
	int* faceI = faceToVertex.row(face).innerIndexPtr();
	int indexE = edgeI[0];
	int indexF = faceI[0];
	int i = 0, j = 0;
	int opposite = faceI[0];
	if (edgeI[0] == faceI[0]) {
		if (edgeI[1] == faceI[1]) {
			opposite = faceI[2];
		}
		else {
			opposite = faceI[1];
		}
	}
	Vector3 v1 = getVertex(edgeI[0]) - getVertex(opposite);
	Vector3 v2 = getVertex(edgeI[1]) - getVertex(opposite);
	Real cosA = v1.dot(v2);
	Real cot = cosA / faceAreas(face); //cos*e1*e2/(sin*e1*e2)

	return cot;

}

Vector3 SimplexMesh::normal(unsigned int face) {

	return Vector3();
}

SparseMatrix SimplexMesh::laplacian() {
	return vertToEdge*hodgeStar0()*edgeToVert;
}

SparseMatrix SimplexMesh::hodgeStar0() {
	return SparseMatrix();
}
SparseMatrix SimplexMesh::hodgeStar1() {
	return SparseMatrix();
}

SimplexMesh::NeighborIterator SimplexMesh::neighborsBegin(unsigned int v) {
	NeighborIterator it(this, v);
	return it;
}
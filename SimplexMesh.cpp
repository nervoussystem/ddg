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
	faceMap.resize(numFaces, numEdges);
	faceMap.reserve(numFaces * 3);
	edgeMap.resize(numEdges, numVerts);
	faceMap.setZero();
	edgeMap.setZero();
	edgeMap.reserve(numEdges * 2);
	//inserts are inefficient
	for (unsigned int i = 0; i < numFaces * 3;) {
		unsigned int fIndex = i / 3;
		ofIndexType i1 = mesh.getIndex(i++);
		ofIndexType i2 = mesh.getIndex(i++);
		ofIndexType i3 = mesh.getIndex(i++);

		Real val = vv.coeff(i1, i2);
		//get sign for relative direction
		faceMap.insert(fIndex, abs(val) - 1) = (0.0 < val) - (val < 0.0);
		val = vv.coeff(i2, i3);
		faceMap.insert(fIndex, abs(val) - 1) = (0.0 < val) - (val < 0.0);
		val = vv.coeff(i3, i1);
		faceMap.insert(fIndex, abs(val) - 1) = (0.0 < val) - (val < 0.0);
	}

	for (int k = 0; k < vv.outerSize(); ++k) {
		for (SparseMatrix::InnerIterator it(vv, k); it; ++it) {
			if (it.value() > 0) {
				edgeMap.insert(it.value()-1, it.row()) = -1;
				edgeMap.insert(it.value() - 1, it.col()) = -1;
			}
		}
	}
	faceMap.makeCompressed();
	edgeMap.makeCompressed();

}

void SimplexMesh::computeFaceAreas() {
	faceAreas.resize(getNumFaces());

	//cache this?
	SparseMatrix fToV = faceMap.cwiseAbs()*edgeMap.cwiseAbs()*0.5;
	if (!fToV.isCompressed()) fToV.makeCompressed();
	for (int k = 0; k < fToV.outerSize(); ++k) {
		auto inner = fToV.innerVector(k);
		if (inner.nonZeros() != 3) {
			cout << "Error: face vertices != 3" << endl;
			continue;
		}

		SparseMatrix::InnerIterator it(fToV, k);

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

	for (int k = 0; k < edgeMap.outerSize(); ++k) {
		SparseMatrix::InnerIterator it(edgeMap, k);

		Vector3 p1 = getVertex(it.col());
		++it;
		Vector3 p2 = getVertex(it.col());
		edgeLengths[k] = (p2 - p1).norm();
	}
}
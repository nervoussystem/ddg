#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	vMesh.load("sphere.obj");
	mesh.loadFromOfMesh(vMesh);
	mesh.computeFaceAreas();
	mesh.computeEdgeLengths();
	// = mesh.faceMap.col(1);
	//const auto edgeCol = mesh.faceMap.transpose();
	auto cats = mesh.faceMap*mesh.faceMap.transpose();
	cout << "Nonzeroes " << mesh.faceMap.col(0).coeff(2) << endl;
	cout << "inner size " << mesh.faceMap.col(0).innerSize() << endl;
	cout << "outer size " << mesh.faceMap.outerSize() << endl;
	cout << "end" << endl;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

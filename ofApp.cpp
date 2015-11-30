
#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);
    ofSetCircleResolution(45); // num edges of regular polygon
    
    tet.open(); // optionally specify port -- default is 6555
    
    
    ofBackground(0);

//    // set the app fps
//    appFPS = 1;
//    ofSetFrameRate(appFPS);

    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Write the EyeTribe data to file ('e' to record eye data)
    recordEyeData();
    
    // record the path to a path object to be drawn
    // (recording automatically writes to new path)
    // ('k' to record new path without writing to file)
    recordEyePath();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    ofDisableAntiAliasing();
    ofSetColor(0);
    
    // draw live gaze data ('g' to toggle)
    drawLiveGaze();
    
    // draw eye path ('l' to draw, 'k' to record new path, 'j' for drawing type)
    drawEyePath();
    
    // debug
    ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 20);
    stringstream ss;
    ss << "DEBUG";
    if (eRecordingState) ss << endl << "Recording Eye to File";
    if (drawLiveGazeBool) ss << endl << "Drawing Live Gaze";
    if (recordEyePathBool) ss << endl << "Recording Path Only";
    if (drawEyePathBool) ss << endl << "Drawing Recorded Eye Path";
    ofDrawBitmapStringHighlight(ss.str(), 10, 50);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == 'f') ofToggleFullscreen();
    if (key == 'o') tet.open();
    if (key == 'c') tet.close();
    if (key == 's') tet.startServer();
    if (key == 'e' || key == 'r') { // 'e' toggles eye recording -- writes to new file ea. time
        if (!eRecordingState) {
            eRecordingState = true;
            eyePath.clear();
            recordEyePathBool = false;
            drawEyePathBool = false;
            
            stringstream eStreamTitle;
            eStreamTitle << ofGetYear() << "-";
            eStreamTitle <<  setw(2) << setfill('0') << ofGetMonth() << "-";
            eStreamTitle <<  setw(2) << setfill('0') << ofGetDay() << " ";
            eStreamTitle <<  setw(2) << setfill('0') << ofGetHours() << "-";
            eStreamTitle <<  setw(2) << setfill('0') << ofGetMinutes() << "-";
            eStreamTitle <<  setw(2) << setfill('0') << ofGetSeconds();
            efileTitle = "EyeTribe " + eRecordingTitle + " " + eStreamTitle.str();
            eCsv.createFile(ofToDataPath(efileTitle));
        } else {
            eRecordingState = false;
            //            drawEyePathBool = false;
            eCsv.saveFile(ofToDataPath(efileTitle));
            eCounter = 0;
        }
    }
    if (key == 'g') {
        drawLiveGazeBool = !drawLiveGazeBool;
    }

    
}

//--------------------------------------------------------------

//--------------------------------------------------------------

// Write Eye Data to eCsv
void ofApp::recordEyeData() {
    // if 'e' is pressed, record data
    if (eRecordingState) {
        if (eCounter == 0) { // record time zero as reference in first reading
            eTimeZero = tet.getTimestamp();
            eCsv.setFloat(eCounter, 0, (tet.getTimestamp() - eTimeZero) / 1000.0);
            eCsv.setFloat(eCounter, 1, tet.getPoint2dAvg().x);
            eCsv.setFloat(eCounter, 2, tet.getPoint2dAvg().y);
            eCounter++;
        } else { // other times, just write to eCsv
            float timeNow = (tet.getTimestamp() - eTimeZero) / 1000;
            if (timeNow != eCsv.getFloat(eCounter - 1, 0)) { // remove duplicates
                if (tet.getPoint2dAvg() != origin) { // remove pts at origin
                    eCsv.setFloat(eCounter, 0, timeNow);
                    eCsv.setFloat(eCounter, 1, tet.getPoint2dAvg().x);
                    eCsv.setFloat(eCounter, 2, tet.getPoint2dAvg().y);
                    eCounter++;
                }
            }
        }
    }
}

//--------------------------------------------------------------

// draw real time gaze
void ofApp::drawLiveGaze() {
    if (drawLiveGazeBool) {
        if (tet.isConnected()) {
            ofFill();
            
            // draw raw gaze point
            ofSetColor(ofColor::red);
            ofCircle(tet.getPoint2dRaw(), 5);
            
            // draw smoothed gaze point
            ofSetColor(ofColor::green);
            ofCircle(tet.getPoint2dAvg(), 10);
            
            // draw circle in avg point when fixed
            if (tet.isFix()) {
                ofSetColor(0);
                ofCircle(tet.getPoint2dAvg(), 5);
            }
        }
    }
}

//--------------------------------------------------------------

// draw recorded or live eye path to the display window
void ofApp::drawEyePath() {
    if (drawEyePathBool) { // toggle drawing the eye path
        switch (drawEyePathType) {
            case 0: { // simple line
                eyePath.setFilled(false);
                eyePath.setStrokeWidth(1);
                eyePath.setStrokeColor(0);
                eyePath.draw();
            }
                break;
            case 1: { // points on line
                eyePath.setFilled(false);
                eyePath.setStrokeWidth(1);
                ofColor col;
                eyePath.setStrokeColor(ofColor(0, 75));
                eyePath.draw();
                vector<ofPolyline> tempPoly = eyePath.getOutline();
                vector<ofPoint> ptArray = tempPoly[0].getVertices();
                for (int i = 0; i < ptArray.size(); i++) {
                    ofSetColor(0);
                    ofCircle(ptArray[i], 2.5);
                }
            }
                break;
            case 2: { // large transparent circles
                eyePath.setFilled(false);
                eyePath.setStrokeWidth(0);
                vector<ofPolyline> tempPoly = eyePath.getOutline();
                vector<ofPoint> ptArray = tempPoly[0].getVertices();
                for (int i = 0; i < ptArray.size(); i++) {
                    ofSetColor(0, 7);
                    ofCircle(ptArray[i], 35);
                }
            }
                break;
        }
    }
}

//--------------------------------------------------------------

// record eye data to a path object so it can be drawn
void ofApp::recordEyePath() {
    // start new path each new recording
    if (eRecordingState) {
        if (tet.getPoint2dAvg() != origin) {
            eyePath.lineTo(tet.getPoint2dAvg());
        }
    } else if (recordEyePathBool) { // otherwise, manually start new path w/ 'k'
        if (tet.getPoint2dAvg() != origin) {
            eyePath.lineTo(tet.getPoint2dAvg());
        }
    }



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
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}
#pragma once

#include "ofMain.h"
#include "ofxEyeTribe.h"
#include "ofxCsv.h"

// e for eye
class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofxEyeTribe tet;
    
    wng::ofxCsv eCsv;
    
    float eTimeZero;
    int eCounter = 0;
    
    void recordEyeData();
    string eRecordingTitle = "data"; // eye only recording title
    Boolean eRecordingState = false;
    string efileTitle;
    
    void drawLiveGaze();
    Boolean drawLiveGazeBool = true;
    
    ofPath eyePath;
    ofPoint origin = ofVec3f(0.0, 0.0);
    void recordEyePath();
    Boolean recordEyePathBool = false; // toggle with 'k' for Klear
    
    void drawEyePath();
    Boolean drawEyePathBool = false; // toggle with 'l' for draw Line
    int drawEyePathType = 0; // cycle with 'j'
    
    
    
};
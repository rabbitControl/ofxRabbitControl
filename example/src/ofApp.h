#pragma once

//#include "ofMain.h"
#include "ofApp.h"
#include "ofBaseApp.h"
#include "ofParameter.h"

#include "ofxGui.h"
#include "ofxRabbitControl.h"

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
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);


    ofxPanel gui;

    ofParameterGroup group;
    ofParameterGroup group1;

    ofParameter<char> charParam;
    ofParameter<int> int32Param;
    ofParameter<double> dblParam;
    ofParameter<float> floatParam;
    ofParameter<bool> boolParam;
    ofParameter<std::string> strParam;
    ofParameter<ofColor> clrParam;

    ofxRabbitControlServer rabbit;
    websocketServerTransporter transporter;
};

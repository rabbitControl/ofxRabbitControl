#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    // setup parameter
    charParam.set("char param", 10);

    // setup gui
    gui.setup("panel");
    gui.add(int32Param.set("int 32", 1100, 1010, 1510));
    gui.add(boolParam.set("BOOOL", true));
    gui.add(dblParam.set("DOUBLE", 3.1415, 0.0, 10.0));
    gui.add(floatParam.set("float parameter", 1.111, 0.0, 10.0));
    gui.add(strParam.set("some text", "bla"));
    gui.add(clrParam.set("Color", ofColor::azure));


    //----------------------------------------
    // setup rabbitserver

    // add a transporter
    rabbit.addTransporter(transporter);

    // bind to port
    transporter.bind(10000);

    // expose ofParameter
    rabbit.expose(boolParam);

    // setup rcp parameter
    auto rabbitStr = rabbit.expose(strParam);
    // change label
    rabbitStr.setLabel("new string label");
    // set a label in french
    rabbitStr.setLanguageLabel("fra", "une chose variable");
    // add a description
    rabbitStr.setDescription("this is a bool with a french label");
    // set some tags
    rabbitStr.setTags("desc lableled some");

    // setup a ofParameterGroup
    group.setName("group");
    group1.setName("group1 - all the ints");

    // add parameter to group1
    group1.add(charParam);
    group1.add(int32Param);

    // add group1 to group
    group.add(group1);

    // add other parameters to group
    group.add(dblParam);
    group.add(floatParam);
    group.add(clrParam);

    // expose whole group via rabbit
    rabbit.expose(group);

    // update rabbit
    rabbit.update();
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofClear(charParam.get());

    gui.draw();

    ofSetColor(255);
    ofDrawBitmapStringHighlight(strParam.get(), 10, 450);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if (key == ' ') {
        // remove some parameter
        rabbit.remove(boolParam);
//        rabbit.remove(strParam);
        rabbit.update();

    } else if (key == 'a') {
        // add them
        rabbit.expose(boolParam);
//        rabbit.expose(strParam);
        rabbit.update();
    } else if (key == 'r') {

        // set random values
        if (ofRandomf() > 0) boolParam.set(true);
        else boolParam.set(false);
        strParam.set(ofToString(ofRandomf()));

        // update rabbit
        rabbit.update();
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

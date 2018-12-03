/*
********************************************************************
* ofxRabbitControl
*
* written by: Ingo Randolf - 2018
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following
* conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*********************************************************************
*/

#include "ofLog.h"
#include "ofxRabbitControl.h"

#include "rabbitControl/parameter_intern.h"

int16_t ofxRabbitControlServer::findParam(void* paramAdr) {

    auto it = paramIdMap.find(paramAdr);
    if (it != paramIdMap.end()) {
        // found
        return it->second;
    }

    return 0;
}


//----------------------------------------------------
//----------------------------------------------------
// group
//----------------------------------------------------
//----------------------------------------------------
rcp::GroupParameterPtr ofxRabbitControlServer::expose(ofParameterGroup& group, const rcp::GroupParameterPtr& rabbitgroup) {

    auto it = groupIdMap.find((void*)&group);
    if (it != groupIdMap.end()) {
        // already exposed - return parameter
        return std::dynamic_pointer_cast<rcp::GroupParameter>(ParameterServer::getParameter(it->second));
    }

    rcp::GroupParameterPtr gp;
    if (rabbitgroup) {
        gp = ParameterServer::createGroupParameter(group.getName(), const_cast<rcp::GroupParameterPtr&>(rabbitgroup));
    } else {
        gp = ParameterServer::createGroupParameter(group.getName());
    }

    std::size_t s = group.size();
    for (std::size_t i = 0; i < group.size(); i++) {

        std::string t = group[i].valueType();

        if (t == "a" || t == "c") {
            auto& p = group.getChar(group[i].getName());
            expose(p, gp);
        } else if (t == "h") {
            // uint8, usigned char
            ofLogNotice() << "unhandled uint8\n";
        } else if (t == "s") {
            // short
            ofLogNotice() << "unhandled short\n";
        } else if (t == "t") {
            // unsigned short
            ofLogNotice() << "unhandled unsigned short\n";
        } else if (t == "i") {
            auto& p = group.getInt(group[i].getName());
            expose(p, gp);
        } else if (t == "j") {
            // unsigned int
            ofLogNotice() << "unhandled unsigned int\n";
        } else if (t == "x") {
            // int64
            ofLogNotice() << "unhandled int64\n";
        } else if (t == "y") {
            // uint64
            ofLogNotice() << "unhandled unsigned int64\n";
        } else if (t == "d") {
            auto& p = static_cast< ofParameter<double>& >(group[i]);
            expose(p, gp);
        } else if (t == "f") {
            auto& p = group.getFloat(group[i].getName());
            expose(p, gp);
        } else if (t == "b") {
            auto& p = group.getBool(group[i].getName());
            expose(p, gp);
        } else if (t.find("basic_string") != std::string::npos) {
            auto& p = group.getString(group[i].getName());
            expose(p, gp);
        } else if (t.find("ofColor") != std::string::npos) {
            auto& p = group.getColor(group[i].getName());
            expose(p, gp);
        } else if (t.find("ofParameterGroup") != std::string::npos) {
            auto& p = group.getGroup(group[i].getName());
            expose(p, gp);
        } else {
            // unknown
            ofLogNotice() << "unknown type: " << t << "\n";
        }
    }

    groupIdMap[(void*)&group] = gp->getId();

    return gp;
}

void ofxRabbitControlServer::remove(ofParameterGroup& group) {

    auto it = groupIdMap.find((void*)&group);
    if (it != groupIdMap.end()) {
        // found

        // fist, send remove parameter from clients
        ParameterServer::removeParameter(it->second);

        // remove from maps
        groupIdMap.erase((void*)(&group));
    }
}


//----------------------------------------------------
//----------------------------------------------------
// bool
//----------------------------------------------------
//----------------------------------------------------
rcp::BooleanParameter ofxRabbitControlServer::expose(ofParameter<bool> & param, const rcp::GroupParameterPtr& rabbitgroup)
{
    auto it = paramIdMap.find((void*)&param.get());
    if (it != paramIdMap.end()) {
        // already exposed
        return *std::dynamic_pointer_cast<rcp::BooleanParameter>(ParameterServer::getParameter(it->second));
    }

    // setup
    rcp::BooleanParameter p(0);
    if (rabbitgroup) {
        p = ParameterServer::createBooleanParameter(param.getName(), const_cast<rcp::GroupParameterPtr&>(rabbitgroup));
    } else {
        p = ParameterServer::createBooleanParameter(param.getName());
    }
    p.setValue(param.get());

    p.addValueUpdatedCb([&](bool& v) {
        param.set(v);
    });

    p.addUpdatedCb([&]() {
        param.setName(p.getLabel());
    });


    // set change listener to update rcp parameter
    param.addListener(this, &ofxRabbitControlServer::paramBoolChanged);

    // insert into maps
    paramIdMap[(void*)&param.get()] = p.getId();

    return p;
}

void ofxRabbitControlServer::remove(ofParameter<bool> & param)
{
    ofParameterRemove(param);
    param.removeListener(this, &ofxRabbitControlServer::paramBoolChanged);
}

void ofxRabbitControlServer::paramBoolChanged(bool & value)
{
    int16_t id = findParam(&value);
    if (!id) {
        return;
    }

    auto param = std::dynamic_pointer_cast<rcp::BooleanParameter >(ParameterServer::getParameter(id));
    if (param) {
        param->setValue(value);
        // update
        ParameterServer::update();
    }
}



//----------------------------------------------------
//----------------------------------------------------
// int8
//----------------------------------------------------
//----------------------------------------------------
rcp::Int8Parameter ofxRabbitControlServer::expose(ofParameter<char> & param, const rcp::GroupParameterPtr& rabbitgroup) {

    auto it = paramIdMap.find((void*)&param.get());
    if (it != paramIdMap.end()) {
        // already exposed
        return *std::dynamic_pointer_cast<rcp::Int8Parameter>(ParameterServer::getParameter(it->second));
    }

    // setup
    rcp::Int8Parameter p(0);
    if (rabbitgroup) {
        p  = ParameterServer::createInt8Parameter(param.getName(), const_cast<rcp::GroupParameterPtr&>(rabbitgroup));
    } else {
        p  = ParameterServer::createInt8Parameter(param.getName());
    }
    p.setMinimum(param.getMin());
    p.setMaximum(param.getMax());
    p.setValue(param.get());

    p.addValueUpdatedCb([&](int8_t& v) {
        param.set(v);
    });

    p.addUpdatedCb([&]() {
        param.setName(p.getLabel());
    });

    // set change listener to update rcp parameter
    param.addListener(this, &ofxRabbitControlServer::paramInt8Changed);

    // insert into maps
    paramIdMap[(void*)&param.get()] = p.getId();

    return p;
}

void ofxRabbitControlServer::remove(ofParameter<char> & param){
    ofParameterRemove(param);
    param.removeListener(this, &ofxRabbitControlServer::paramInt8Changed);
}

void ofxRabbitControlServer::paramInt8Changed(char & value)
{
    int16_t id = findParam(&value);
    if (!id) {
        return;
    }

    auto param = std::dynamic_pointer_cast<rcp::Int8Parameter>(ParameterServer::getParameter(id));
    if (param) {
        param->setValue(value);
        // update
        ParameterServer::update();
    }
}


//----------------------------------------------------
//----------------------------------------------------
// int32
//----------------------------------------------------
//----------------------------------------------------
rcp::Int32Parameter ofxRabbitControlServer::expose(ofParameter<int> & param, const rcp::GroupParameterPtr& rabbitgroup)
{
    auto it = paramIdMap.find((void*)&param.get());
    if (it != paramIdMap.end()) {
        // already exposed
        return *std::dynamic_pointer_cast<rcp::Int32Parameter>(ParameterServer::getParameter(it->second));
    }

    // setup
    rcp::Int32Parameter p(0);
    if (rabbitgroup) {
        p  = ParameterServer::createInt32Parameter(param.getName(), const_cast<rcp::GroupParameterPtr&>(rabbitgroup));
    } else {
        p  = ParameterServer::createInt32Parameter(param.getName());
    }
    p.setMinimum(param.getMin());
    p.setMaximum(param.getMax());
    p.setValue(param.get());

    p.addValueUpdatedCb([&](int32_t& v) {
        param.set(v);
    });

    p.addUpdatedCb([&]() {
        param.setName(p.getLabel());
    });

    // set change listener to update rcp parameter
    param.addListener(this, &ofxRabbitControlServer::paramInt32Changed);

    // insert into maps
    paramIdMap[(void*)&param.get()] = p.getId();

    return p;
}

void ofxRabbitControlServer::remove(ofParameter<int> & param){
    ofParameterRemove(param);
    param.removeListener(this, &ofxRabbitControlServer::paramInt32Changed);
}

void ofxRabbitControlServer::paramInt32Changed(int & value)
{
    int16_t id = findParam(&value);
    if (!id) {
        return;
    }

    auto param = std::dynamic_pointer_cast<rcp::Int32Parameter>(ParameterServer::getParameter(id));
    if (param) {
        param->setValue(value);
        // update
        ParameterServer::update();
    }
}



//----------------------------------------------------
//----------------------------------------------------
// float
//----------------------------------------------------
//----------------------------------------------------
rcp::Float32Parameter ofxRabbitControlServer::expose(ofParameter<float> & param, const rcp::GroupParameterPtr& rabbitgroup)
{
    auto it = paramIdMap.find((void*)&param.get());
    if (it != paramIdMap.end()) {
        // already exposed
        return *std::dynamic_pointer_cast<rcp::Float32Parameter>(ParameterServer::getParameter(it->second));
    }

    // setup
    rcp::Float32Parameter p(0);
    if (rabbitgroup) {
        p  = ParameterServer::createFloat32Parameter(param.getName(), const_cast<rcp::GroupParameterPtr&>(rabbitgroup));
    } else {
        p  = ParameterServer::createFloat32Parameter(param.getName());
    }
    p.setMinimum(param.getMin());
    p.setMaximum(param.getMax());
    p.setValue(param.get());

    p.getMinimum();

    p.addValueUpdatedCb([&](float& v) {
        param.set(v);
    });

    p.addUpdatedCb([&]() {
        param.setName(p.getLabel());
    });

    // set change listener to update rcp parameter
    param.addListener(this, &ofxRabbitControlServer::paramfloatChanged);

    // insert into maps
    paramIdMap[(void*)&param.get()] = p.getId();

    return p;
}

void ofxRabbitControlServer::remove(ofParameter<float> & param){
    ofParameterRemove(param);
    param.removeListener(this, &ofxRabbitControlServer::paramfloatChanged);
}

void ofxRabbitControlServer::paramfloatChanged(float & value)
{
    int16_t id = findParam(&value);
    if (!id) {
        return;
    }

    auto param = std::dynamic_pointer_cast<rcp::Float32Parameter>(ParameterServer::getParameter(id));
    if (param) {
        param->setValue(value);
        // update
        ParameterServer::update();
    }
}


//----------------------------------------------------
//----------------------------------------------------
// double
//----------------------------------------------------
//----------------------------------------------------
rcp::Float64Parameter ofxRabbitControlServer::expose(ofParameter<double> & param, const rcp::GroupParameterPtr& rabbitgroup)
{
    auto it = paramIdMap.find((void*)&param.get());
    if (it != paramIdMap.end()) {
        // already exposed
        return *std::dynamic_pointer_cast<rcp::Float64Parameter>(ParameterServer::getParameter(it->second));
    }

    // setup
    rcp::Float64Parameter p(0);
    if (rabbitgroup) {
        p  = ParameterServer::createFloat64Parameter(param.getName(), const_cast<rcp::GroupParameterPtr&>(rabbitgroup));
    } else {
        p  = ParameterServer::createFloat64Parameter(param.getName());
    }
    p.setMinimum(param.getMin());
    p.setMaximum(param.getMax());
    p.setValue(param.get());

    p.addValueUpdatedCb([&](double& v) {
        param.set(v);
    });

    p.addUpdatedCb([&]() {
        param.setName(p.getLabel());
    });

    // set change listener to update rcp parameter
    param.addListener(this, &ofxRabbitControlServer::paramDoubleChanged);

    // insert into maps
    paramIdMap[(void*)&param.get()] = p.getId();

    return p;
}

void ofxRabbitControlServer::remove(ofParameter<double> & param){
    ofParameterRemove(param);
    param.removeListener(this, &ofxRabbitControlServer::paramDoubleChanged);
}

void ofxRabbitControlServer::paramDoubleChanged(double & value)
{
    int16_t id = findParam(&value);
    if (!id) {
        return;
    }

    auto param = std::dynamic_pointer_cast<rcp::Float64Parameter>(ParameterServer::getParameter(id));
    if (param) {
        param->setValue(value);
        // update
        ParameterServer::update();
    }
}


//----------------------------------------------------
//----------------------------------------------------
// string
//----------------------------------------------------
//----------------------------------------------------
rcp::StringParameter ofxRabbitControlServer::expose(ofParameter<std::string> & param, const rcp::GroupParameterPtr& rabbitgroup)
{
    auto it = paramIdMap.find((void*)&param.get());
    if (it != paramIdMap.end()) {
        // already exposed
        return *std::dynamic_pointer_cast<rcp::StringParameter>(ParameterServer::getParameter(it->second));
    }

    // setup
    rcp::StringParameter p(0);
    if (rabbitgroup) {
        p = ParameterServer::createStringParameter(param.getName(), const_cast<rcp::GroupParameterPtr&>(rabbitgroup));
    } else {
        p = ParameterServer::createStringParameter(param.getName());
    }
    p.setValue(param.get());

    p.addValueUpdatedCb([&](std::string& v) {
        param.set(v);
    });

    p.addUpdatedCb([&]() {
        param.setName(p.getLabel());
    });

    // set change listener to update rcp parameter
    param.addListener(this, &ofxRabbitControlServer::paramStringChanged);

    // insert into maps
    paramIdMap[(void*)&param.get()] = p.getId();

    return p;
}

void ofxRabbitControlServer::remove(ofParameter<std::string> & param){
    ofParameterRemove(param);
    param.removeListener(this, &ofxRabbitControlServer::paramStringChanged);
}

void ofxRabbitControlServer::paramStringChanged(std::string & value)
{

    int16_t id = findParam(&value);
    if (!id) {
        return;
    }

    auto param = std::dynamic_pointer_cast<rcp::StringParameter>(ParameterServer::getParameter(id));
    if (param) {
        param->setValue(value);
        // update
        ParameterServer::update();
    }
}


//----------------------------------------------------
//----------------------------------------------------
// color
//----------------------------------------------------
//----------------------------------------------------
rcp::RGBAParameter ofxRabbitControlServer::expose(ofParameter<ofColor> & param, const rcp::GroupParameterPtr& rabbitgroup)
{
    auto it = paramIdMap.find((void*)&param.get());
    if (it != paramIdMap.end()) {
        // already exposed
        return *std::dynamic_pointer_cast<rcp::RGBAParameter>(ParameterServer::getParameter(it->second));
    }

    // setup
    rcp::RGBAParameter p(0);
    if (rabbitgroup) {
        p  = ParameterServer::createRGBAParameter(param.getName(), const_cast<rcp::GroupParameterPtr&>(rabbitgroup));
    } else {
        p  = ParameterServer::createRGBAParameter(param.getName());
    }

    uint32_t cv = param.get().r + (param.get().g << 8) + (param.get().b << 16) + (param.get().a << 24);
    p.setValue(rcp::Color(cv));

    p.addValueUpdatedCb([&](rcp::Color& v) {

        uint32_t cv = v.getValue();
        float r = cv & 0xFF;
        float g = (cv >> 8) & 0xFF;
        float b = (cv >> 16) & 0xFF;
        float a = (cv >> 24) & 0xFF;

        param.set(ofColor(r, g, b, a));
    });

    p.addUpdatedCb([&]() {
        param.setName(p.getLabel());
    });

    // set change listener to update rcp parameter
    param.addListener(this, &ofxRabbitControlServer::paramColorChanged);

    // insert into maps
    paramIdMap[(void*)&param.get()] = p.getId();

    return p;
}

void ofxRabbitControlServer::remove(ofParameter<ofColor> & param){
    ofParameterRemove(param);
    param.removeListener(this, &ofxRabbitControlServer::paramColorChanged);
}

void ofxRabbitControlServer::paramColorChanged(ofColor & value)
{
    int16_t id = findParam(&value);
    if (!id) {
        return;
    }

    auto param = std::dynamic_pointer_cast<rcp::RGBAParameter>(ParameterServer::getParameter(id));
    if (param) {

        int r = value.r;
        int g = value.g;
        int b = value.b;
        int a = value.a;

        uint32_t cv = r + (g << 8) + (b << 16) + (a << 24);
        param->setValue(rcp::Color(cv));
        // update
        ParameterServer::update();
    }
}


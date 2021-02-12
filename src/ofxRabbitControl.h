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

#ifndef OFXRABBITCONTROL_H
#define OFXRABBITCONTROL_H

#include "ofParameter.h"

#include "websocketServerTransporter.h"
#include "rabbitholeWsServerTransporter.h"
#include "rabbitControl/parameterserver.h"

class ofxRabbitControlServer : public rcp::ParameterServer
{
public:
    ofxRabbitControlServer() {}

    rcp::GroupParameterPtr expose(ofParameterGroup& group, const rcp::GroupParameterPtr& rabbitgroup = rcp::GroupParameterPtr());
    void remove(ofParameterGroup& group);

    rcp::BooleanParameterPtr expose(ofParameter<bool> & param, const rcp::GroupParameterPtr& rabbitgroup = rcp::GroupParameterPtr());
    rcp::Int8ParameterPtr expose(ofParameter<char> & param, const rcp::GroupParameterPtr& rabbitgroup = rcp::GroupParameterPtr());
    rcp::Int32ParameterPtr expose(ofParameter<int> & param, const rcp::GroupParameterPtr& rabbitgroup = rcp::GroupParameterPtr());
    rcp::Float32ParameterPtr expose(ofParameter<float> & param, const rcp::GroupParameterPtr& rabbitgroup = rcp::GroupParameterPtr());
    rcp::Float64ParameterPtr expose(ofParameter<double> & param, const rcp::GroupParameterPtr& rabbitgroup = rcp::GroupParameterPtr());
    rcp::StringParameterPtr expose(ofParameter<std::string> & param, const rcp::GroupParameterPtr& rabbitgroup = rcp::GroupParameterPtr());
    rcp::RGBAParameterPtr expose(ofParameter<ofColor> & param, const rcp::GroupParameterPtr& rabbitgroup = rcp::GroupParameterPtr());

    void remove(ofParameter<bool> & param);
    void remove(ofParameter<char> & param);
    void remove(ofParameter<int> & param);
    void remove(ofParameter<float> & param);
    void remove(ofParameter<double> & param);
    void remove(ofParameter<std::string> & param);
    void remove(ofParameter<ofColor> & param);

    void paramBoolChanged(bool & value);
    void paramInt8Changed(char & value);
    void paramInt32Changed(int & value);
    void paramfloatChanged(float & value);
    void paramDoubleChanged(double & value);
    void paramStringChanged(std::string & value);
    void paramColorChanged(ofColor & value);

private:
    int16_t findParam(void* paramAdr);

    template <typename T>
    void ofParameterRemove(ofParameter<T> param) {
        int16_t id = findParam((void*)(&param.get()));
        if (!id) {
            return;
        }
        // fist, send remove parameter from clients
        ParameterServer::removeParameter(id);

        // remove from maps
        paramIdMap.erase((void*)(&param.get()));
    }

    std::map<void*, int16_t > paramIdMap;
    std::map<void*, int16_t > groupIdMap;
};



class rabbitClient
{
    // TODO
};


#endif // OFXRABBITCONTROL_H

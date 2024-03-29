/*
********************************************************************
* rabbitcontrol cpp
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

#ifndef RCPSERVER_H
#define RCPSERVER_H

#include <set>

#include "servertransporter.h"
#include "parametermanager.h"
#include "rcp_error_listener.h"

namespace rcp {

    class Packet;
class ParameterServer : public ServerTransporterReceiver
{
public:
    ParameterServer();
    ParameterServer(ServerTransporter& transporter);
    ~ParameterServer();

public:
    void dispose();
    void clear();

    bool addTransporter(ServerTransporter& transporter);
    bool removeTransporter(ServerTransporter& transporter);
    int getConnectionCount();

    virtual bool update();

public:
    // ServerTransporterReceiver
    void received(std::istream& data, ServerTransporter& transporter, void* id);

public:
    GroupParameterPtr& getRoot() { return root; }

    ParameterPtr getParameter(const short& id) {
        return parameterManager->getParameter(id);
    }

    //
    void removeParameter(IParameter& parameter) {
        parameterManager->removeParameter(parameter);
    }
    void removeParameter(short id) {
        parameterManager->removeParameter(id);
    }

    // parameter creater
    BooleanParameterPtr createBooleanParameter(const std::string& label) {
        return parameterManager->createBooleanParameter(label, root);
    }
    BooleanParameterPtr createBooleanParameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createBooleanParameter(label, group);
    }

    Int8ParameterPtr createInt8Parameter(const std::string& label) {
        return parameterManager->createInt8Parameter(label, root);
    }
    Int8ParameterPtr createInt8Parameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createInt8Parameter(label, group);
    }

    Int16ParameterPtr createInt16Parameter(const std::string& label) {
        return parameterManager->createInt16Parameter(label, root);
    }
    Int16ParameterPtr createInt16Parameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createInt16Parameter(label, group);
    }

    Int32ParameterPtr createInt32Parameter(const std::string& label) {
        return parameterManager->createInt32Parameter(label, root);
    }
    Int32ParameterPtr createInt32Parameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createInt32Parameter(label, group);
    }

    Int64ParameterPtr createInt64Parameter(const std::string& label) {
        return parameterManager->createInt64Parameter(label, root);
    }
    Int64ParameterPtr createInt64Parameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createInt64Parameter(label, group);
    }

    Float32ParameterPtr createFloat32Parameter(const std::string& label) {
        return parameterManager->createFloat32Parameter(label, root);
    }
    Float32ParameterPtr createFloat32Parameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createFloat32Parameter(label, group);
    }

    Float64ParameterPtr createFloat64Parameter(const std::string& label) {
        return parameterManager->createFloat64Parameter(label, root);
    }
    Float64ParameterPtr createFloat64Parameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createFloat64Parameter(label, group);
    }


    StringParameterPtr createStringParameter(const std::string& label) {
        return parameterManager->createStringParameter(label, root);
    }
    StringParameterPtr createStringParameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createStringParameter(label, group);
    }


    RGBAParameterPtr createRGBAParameter(const std::string& label) {
        return parameterManager->createRGBAParameter(label, root);
    }
    RGBAParameterPtr createRGBAParameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createRGBAParameter(label, group);
    }



    BangParameterPtr createBangParameter(const std::string& label) {
        return parameterManager->createBangParameter(label, root);
    }
    BangParameterPtr createBangParameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createBangParameter(label, group);
    }

    GroupParameterPtr createGroupParameter(const std::string& label) {
        return parameterManager->createGroupParameter(label, root);
    }
    GroupParameterPtr createGroupParameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createGroupParameter(label, group);
    }

    void addParsingErrorCb(ParsingErrorListener* c, void(ParsingErrorListener::* func)()) {
        parsing_error_cb[c] = func;
    }
    void removeParsingErrorCb(ParsingErrorListener* c) {
        parsing_error_cb.erase(c);
    }


    void dumpHierarchy() {
        root->dumpChildren(0);
        std::flush(std::cout);
    }

    void setApplicationId(const std::string& appid) {
        m_applicationId = appid;
    }
    std::string getApplicationId() {
        return m_applicationId;
    }

protected:
	std::shared_ptr<GroupParameter> root;
	std::shared_ptr<ParameterManager> parameterManager;
	std::vector<std::reference_wrapper<ServerTransporter> > transporterList;
	
private:
    void _init(ServerTransporter& transporter, void *id);
    bool _update(Packet& Packet, ServerTransporter& transporter, void *id);
    void _version(Packet& packet, ServerTransporter& transporter, void *id);
    void _sendParameterFull(ParameterPtr& parameter, ServerTransporter& transporter, void *id);
    void sendPacket(Packet& packet, void *id=nullptr);

    std::string m_applicationId;
//    Events:
    std::map<ParsingErrorListener*, void(ParsingErrorListener::*)()> parsing_error_cb;
//    onError(Exception ex);
};

}
#endif // RCPSERVER_H

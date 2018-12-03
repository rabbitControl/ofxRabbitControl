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

namespace rcp {

    class Packet;
class ParameterServer : public ServerTransporterReceiver
{
public:
    ParameterServer();
    ParameterServer(ServerTransporter& transporter);
    ~ParameterServer();

    void dispose();

    bool addTransporter(ServerTransporter& transporter);
    bool removeTransporter(ServerTransporter& transporter);
    int getConnectionCount();

    bool update();


    // ServerTransporterReceiver
    void received(std::istream& data, ServerTransporter& transporter, void* id);


    GroupParameterPtr& getRoot() { return root; }

    ParameterPtr getParameter(const short& id) {
        return parameterManager->getParameter(id);
    }

    //
    void addParameter(IParameter& parameter) {
        parameterManager->addParameter(parameter, root);
    }
    void addParameter(IParameter& parameter, GroupParameterPtr& group) {
        parameterManager->addParameter(parameter, group);
    }
    void removeParameter(IParameter& parameter) {
        parameterManager->removeParameter(parameter);
    }
    void removeParameter(short id) {
        parameterManager->removeParameter(id);
    }

    // parameter creater
    BooleanParameter& createBooleanParameter(const std::string& label) {
        return parameterManager->createBooleanParameter(label, root);
    }
    BooleanParameter& createBooleanParameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createBooleanParameter(label, group);
    }

    Int8Parameter& createInt8Parameter(const std::string& label) {
        return parameterManager->createInt8Parameter(label, root);
    }
    Int8Parameter& createInt8Parameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createInt8Parameter(label, group);
    }

    Int16Parameter& createInt16Parameter(const std::string& label) {
        return parameterManager->createInt16Parameter(label, root);
    }
    Int16Parameter& createInt16Parameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createInt16Parameter(label, group);
    }

    Int32Parameter& createInt32Parameter(const std::string& label) {
        return parameterManager->createInt32Parameter(label, root);
    }
    Int32Parameter& createInt32Parameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createInt32Parameter(label, group);
    }

    Int64Parameter& createInt64Parameter(const std::string& label) {
        return parameterManager->createInt64Parameter(label, root);
    }
    Int64Parameter& createInt64Parameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createInt64Parameter(label, group);
    }

    Float32Parameter& createFloat32Parameter(const std::string& label) {
        return parameterManager->createFloat32Parameter(label, root);
    }
    Float32Parameter& createFloat32Parameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createFloat32Parameter(label, group);
    }

    Float64Parameter& createFloat64Parameter(const std::string& label) {
        return parameterManager->createFloat64Parameter(label, root);
    }
    Float64Parameter& createFloat64Parameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createFloat64Parameter(label, group);
    }


    StringParameter& createStringParameter(const std::string& label) {
        return parameterManager->createStringParameter(label, root);
    }
    StringParameter& createStringParameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createStringParameter(label, group);
    }


    RGBAParameter& createRGBAParameter(const std::string& label) {
        return parameterManager->createRGBAParameter(label, root);
    }
    RGBAParameter& createRGBAParameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createRGBAParameter(label, group);
    }



    BangParameter& createBangParameter(const std::string& label) {
        return parameterManager->createBangParameter(label, root);
    }
    BangParameter& createBangParameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createBangParameter(label, group);
    }

    GroupParameterPtr createGroupParameter(const std::string& label) {
        return parameterManager->createGroupParameter(label, root);
    }
    GroupParameterPtr createGroupParameter(const std::string& label, GroupParameterPtr& group) {
        return parameterManager->createGroupParameter(label, group);
    }


    void dumpHierarchy() {
        root->dumpChildren(0);
        std::flush(std::cout);
    }

private:
    void _init(ServerTransporter& transporter, void *id);
    bool _update(Packet& Packet, ServerTransporter& transporter, void *id);
    void _version(ServerTransporter& transporter, void *id);
    void _sendParameterFull(ParameterPtr& parameter, ServerTransporter& transporter, void *id);
    void sendPacket(Packet& packet, void *id=nullptr);

    std::shared_ptr<GroupParameter> root;
    std::shared_ptr<ParameterManager> parameterManager;
    std::vector<std::reference_wrapper<ServerTransporter> > transporterList;

//    Events:
//    onError(Exception ex);
};

}
#endif // RCPSERVER_H

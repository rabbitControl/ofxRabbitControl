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

#ifndef RCPMANAGER_H
#define RCPMANAGER_H

#include <map>
#include <vector>
#include <unordered_set>
#include <climits>

#include "parameter_intern.h"
#include "parameterfactory.h"
#include "iparametermanager.h"

namespace rcp {

class ParameterManager :
        public IParameterManager,
        public std::enable_shared_from_this<ParameterManager>
{
public:    

    static bool isValid(const IParameter& parameter) {
        return parameter.getTypeDefinition().getDatatype() != DATATYPE_MAX_ &&
                parameter.getId() != 0;
    }

    static bool isValid(const ParameterPtr& parameter) {
        return parameter->getTypeDefinition().getDatatype() != DATATYPE_MAX_ &&
                parameter->getId() != 0;
    }

    static bool isGroup(const ParameterPtr& parameter) {
        return isValid(parameter) && parameter->getDatatype() == DATATYPE_GROUP;
    }

    ParameterManager();
    ~ParameterManager();

    // IParameterManager
    virtual ParameterPtr getParameter(const short& id);


    //
    void removeParameter(IParameter& parameter);
    void removeParameter(short id);

    //
    BooleanParameterPtr createBooleanParameter(const std::string& label, GroupParameterPtr& group);
    Int8ParameterPtr createInt8Parameter(const std::string& label, GroupParameterPtr& group);
    Int16ParameterPtr createInt16Parameter(const std::string& label, GroupParameterPtr& group);
    Int32ParameterPtr createInt32Parameter(const std::string& label, GroupParameterPtr& group);
    Int64ParameterPtr createInt64Parameter(const std::string& label, GroupParameterPtr& group);
    Float32ParameterPtr createFloat32Parameter(const std::string& label, GroupParameterPtr& group);
    Float64ParameterPtr createFloat64Parameter(const std::string& label, GroupParameterPtr& group);

    StringParameterPtr createStringParameter(const std::string& label, GroupParameterPtr& group);

    RGBAParameterPtr createRGBAParameter(const std::string& label, GroupParameterPtr& group);

    BangParameterPtr createBangParameter(const std::string& label, GroupParameterPtr& group);

    GroupParameterPtr createGroupParameter(const std::string& label, GroupParameterPtr& group);


    template<typename> friend class Parameter;
    friend class ParameterServer;
    friend class ParameterClient;

private:
    std::shared_ptr<ParameterManager> getShared() {
        return shared_from_this();
    }

    //--------
    short getNextId();
    void _addParameter(ParameterPtr& parameter);
    void _addParameter(ParameterPtr& parameter, GroupParameterPtr& group);
    void _addParameterDirect(const std::string& label, ParameterPtr& parameter, GroupParameterPtr& group);
    void clear();

    virtual void setParameterDirty(IParameter& parameter);
    virtual void setParameterRemoved(ParameterPtr& parameter);

    void removeParameterDirect(ParameterPtr& parameter);

    //--------
    std::unordered_set<short> ids;
    std::map<short, ParameterPtr > params;
    std::map<short, ParameterPtr > dirtyParameter;
    std::map<short, ParameterPtr > removedParameter;
};

}

#endif // RCPMANAGER_H

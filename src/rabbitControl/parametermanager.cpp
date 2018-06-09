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

#include "parametermanager.h"
#include "stringstreamwriter.h"

namespace rcp {

    ParameterManager::ParameterManager() {
    }

    ParameterManager::~ParameterManager() {
        params.clear();
        ids.clear();
    }


    // parameter management
    void ParameterManager::addParameter(IParameter& parameter, GroupParameterPtr& group) {

        if (!isValid(parameter)) return;

        ParameterPtr p = parameter.newReference();

        _addParameter(p, group);
    }

    void ParameterManager::removeParameter(IParameter& parameter) {
        removeParameter(parameter.getId());
    }

    void ParameterManager::removeParameter(short id) {

        if (id == 0) return;

        auto it = params.find(id);
        if (it == params.end()) {
            return;
        }

        // add it to removed
        setParameterRemoved(params[id]);

        removeParameterDirect(params[id]);
    }

    void ParameterManager::removeParameterDirect(ParameterPtr& parameter) {

        if (!isValid(*parameter)) return;

        // erase from available ids
        auto id_it = std::find(ids.begin(), ids.end(), parameter->getId());
        if (id_it != ids.end()) {
            ids.erase(id_it);
        } else {
            std::cerr << "could not find id in id list\n";
        }

        if (auto p = parameter->getParent().lock()) {
            p->removeChild(parameter);
        }
        parameter->dispose();
        params.erase(parameter->getId());

        // check if this is a group... if so, remove all children without!! adding them to the removed-list
        if (parameter->getTypeDefinition().getDatatype() == DATATYPE_GROUP) {
            GroupParameterPtr gp = std::dynamic_pointer_cast<GroupParameter>(parameter);
            for (auto& child : gp->children()) {
                removeParameterDirect(child.second);
            }
        }
    }


    //--------------------------------------------
    //--------------------------------------------
    // parameter creation
    //--------------------------------------------
    //--------------------------------------------
    BooleanParameter& ParameterManager::createBooleanParameter(const std::string& label, GroupParameterPtr& group) {

        short id = getNextId();
        if (id != 0) {

            std::shared_ptr<BooleanParameter > p = std::make_shared<BooleanParameter>(id);
            _addParameterDirect(label, (ParameterPtr&)p, group);

            return *p.get();
        }

        // ?? - yeah? or use some options here?
        throw std::runtime_error("no valid id...");
    }

    Int8Parameter& ParameterManager::createInt8Parameter(const std::string& label, GroupParameterPtr& group) {

        short id = getNextId();
        if (id != 0) {

            std::shared_ptr<Int8Parameter > p = std::make_shared<Int8Parameter>(id);
            _addParameterDirect(label, (ParameterPtr&)p, group);

            return *p.get();
        }

        // ?? - yeah? or use some options here?
        throw std::runtime_error("no valid id...");
    }

    Int16Parameter& ParameterManager::createInt16Parameter(const std::string& label, GroupParameterPtr& group) {

        short id = getNextId();
        if (id != 0) {

            std::shared_ptr<Int16Parameter > p = std::make_shared<Int16Parameter>(id);
            _addParameterDirect(label, (ParameterPtr&)p, group);

            return *p.get();
        }

        // ?? - yeah? or use some options here?
        throw std::runtime_error("no valid id...");
    }

    Int32Parameter& ParameterManager::createInt32Parameter(const std::string& label, GroupParameterPtr& group) {

        short id = getNextId();
        if (id != 0) {

            std::shared_ptr<Int32Parameter > p = std::make_shared<Int32Parameter>(id);
            _addParameterDirect(label, (ParameterPtr&)p, group);

            return *p.get();
        }

        // ?? - yeah? or use some options here?
        throw std::runtime_error("no valid id...");
    }

    Int64Parameter& ParameterManager::createInt64Parameter(const std::string& label, GroupParameterPtr& group) {

        short id = getNextId();
        if (id != 0) {

            std::shared_ptr<Int64Parameter > p = std::make_shared<Int64Parameter>(id);
            _addParameterDirect(label, (ParameterPtr&)p, group);

            return *p.get();
        }

        // ?? - yeah? or use some options here?
        throw std::runtime_error("no valid id...");
    }

    Float32Parameter& ParameterManager::createFloat32Parameter(const std::string& label, GroupParameterPtr& group) {

        short id = getNextId();
        if (id != 0) {

            std::shared_ptr<Float32Parameter > p = std::make_shared<Float32Parameter>(id);
            _addParameterDirect(label, (ParameterPtr&)p, group);

            return *p.get();
        }

        // ?? - yeah? or use some options here?
        throw std::runtime_error("no valid id...");
    }

    Float64Parameter& ParameterManager::createFloat64Parameter(const std::string& label, GroupParameterPtr& group) {

        short id = getNextId();
        if (id != 0) {

            std::shared_ptr<Float64Parameter > p = std::make_shared<Float64Parameter>(id);
            _addParameterDirect(label, (ParameterPtr&)p, group);

            return *p.get();
        }

        // ?? - yeah? or use some options here?
        throw std::runtime_error("no valid id...");
    }


    StringParameter& ParameterManager::createStringParameter(const std::string& label, GroupParameterPtr& group) {

        short id = getNextId();
        if (id != 0) {

            std::shared_ptr<StringParameter > p = std::make_shared<StringParameter>(id);
            _addParameterDirect(label, (ParameterPtr&)p, group);

            return *p.get();
        }

        // ?? - yeah? or use some options here?
        throw std::runtime_error("no valid id...");
    }

    RGBAParameter& ParameterManager::createRGBAParameter(const std::string& label, GroupParameterPtr& group) {

        short id = getNextId();
        if (id != 0) {

            std::shared_ptr<RGBAParameter > p = std::make_shared<RGBAParameter>(id);
            _addParameterDirect(label, (ParameterPtr&)p, group);

            return *p.get();
        }

        // ?? - yeah? or use some options here?
        throw std::runtime_error("no valid id...");
    }

    BangParameter& ParameterManager::createBangParameter(const std::string& label, GroupParameterPtr& group) {

        short id = getNextId();
        if (id != 0) {

            std::shared_ptr<BangParameter > p = std::make_shared<BangParameter>(id);
            _addParameterDirect(label, (ParameterPtr&)p, group);

            return *p.get();
        }

        // ?? - yeah? or use some options here?
        throw std::runtime_error("no valid id...");
    }

    GroupParameterPtr ParameterManager::createGroupParameter(const std::string& label, GroupParameterPtr& group) {

        short id = getNextId();
        if (id != 0) {

            GroupParameterPtr p = std::make_shared<GroupParameter>(id);
            _addParameterDirect(label, (ParameterPtr&)p, group);

            return p;
        }

        // ?? - yeah? or use some options here?
        throw std::runtime_error("no valid id...");

    }


    ParameterPtr ParameterManager::getParameter(const short& id) {

        // erase from available ids
        auto it = params.find(id);
        if (it != params.end()) {
            return it->second;
        }

        std::cerr << "could not find id in id list\n";
        return std::make_shared<InvalidParameter>(0);
    }


// private functions


    short ParameterManager::getNextId() {
        for(unsigned short i=1; i<USHRT_MAX; i++) {
            auto search = ids.find(static_cast<short>(i));
            if(search == ids.end()) {
                short i_s = static_cast<short>(i);
                ids.insert(i_s);
                return i_s;
            }
        }

        // return invalid id 0
        return 0;
    }


    void ParameterManager::_addParameter(ParameterPtr& parameter) {

        // check if already in map
        auto it = params.find(parameter->getId());
        if (it != params.end()) {
            // already in map... ignore
            return;
        }

        // need to reserve id
        if (std::find(ids.begin(), ids.end(), parameter->getId()) != ids.end()) {
            // huh - parameter is not in parameter cache, but id already taken!?
            std::cout << "consistency in id/parameter list\n";
        }

        // in any case: reserve that id
        ids.insert(parameter->getId());

        // add it
        parameter->setManager(getShared());

        // parsed parameters are proxy parameter until they are in params-map
        // proxy parameter are not set as children...
        // so: add parameter to its parent
        if (auto parent = parameter->getParent().lock()) {
            parent->addChild(parameter);
        }

        params[parameter->getId()] = parameter;
    }

    void ParameterManager::_addParameter(ParameterPtr& parameter, GroupParameterPtr& group) {

        auto it = params.find(parameter->getId());
        if (it == params.end()) {

            params[parameter->getId()] = parameter;

            // check?
            if (std::find(ids.begin(), ids.end(), parameter->getId()) != ids.end()) {
                std::cout << "consistency in id/parameter list\n";
            }
            ids.insert(parameter->getId());
        } else {
            // already in there!!
            std::cout << "param already in map: " << parameter->getId() << "\n";
        }

        // adding to group...
        group->addChild(parameter);
    }

    void ParameterManager::_addParameterDirect(const std::string& label, ParameterPtr& parameter, GroupParameterPtr& group) {

        parameter->setManager(getShared());
        parameter->setLabel(label);

        // add to group
        group->addChild(parameter);

        params[parameter->getId()] = parameter;
    }


    void ParameterManager::setParameterDirty(IParameter& parameter) {

        // only add if not already removed
        if (removedParameter.find(parameter.getId()) != removedParameter.end()) {
            // parameter is removed, don't add

            std::cout << "already dirty: " << parameter.getId() << "\n";

            return;
        }

        dirtyParameter[parameter.getId()] = parameter.newReference();
    }

    void ParameterManager::setParameterRemoved(ParameterPtr& parameter) {

        // check if parameter is dirty...
        auto it = dirtyParameter.find(parameter->getId());
        if (it != dirtyParameter.end()) {
            // remove parameter from dirties
            dirtyParameter.erase(it);
        }

        removedParameter[parameter->getId()] = parameter;
    }
}

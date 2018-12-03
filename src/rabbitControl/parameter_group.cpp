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

#include "parameter_intern.h"

namespace rcp {

    void GroupParameter::addChild(IParameter& child) {

        ParameterPtr p = child.newReference();
        addChild(p);
    }

    void GroupParameter::addChild(ParameterPtr& child) {

        std::map<short, std::shared_ptr<IParameter > >::iterator it = obj->children.find(child->getId());
        if (it == obj->children.end()) {
            obj->children[child->getId()] = child;
        }

        // set parent in child
        child->setParent(*this);
    }

    void GroupParameter::removeChild(IParameter& child) {

        auto it = obj->children.find(child.getId());
        if (it != obj->children.end()) {
            // found! - remove
            child.clearParent();
            obj->children.erase(child.getId());
        }
    }

    void GroupParameter::removeChild(ParameterPtr& child) {
        removeChild(*child.get());
    }

    void GroupParameter::dumpChildren(int indent) {
        std::string in = "";
        for (int i=0; i<indent; i++) {
            in += "  ";
        }

        std::cout << in << getLabel() << " (" << getId() << "):\n";

        in += "  ";

        for (auto& child : obj->children) {

            if (child.second->getTypeDefinition().getDatatype() == DATATYPE_GROUP) {
                GroupParameter* group = (GroupParameter*)child.second.get();
                group->dumpChildren(indent+1);
            } else {
                std::cout << in << child.second->getLabel() << " (" << child.second->getId() << ")\n";
            }
        }
    }
}


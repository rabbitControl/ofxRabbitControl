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

#ifndef RCP_PARAMETERPARSER_H
#define RCP_PARAMETERPARSER_H

#include "specializetypes.h"
#include "parameterfactory.h"

namespace rcp {

    class ParameterParser {
    public:
        static ParameterPtr parse(std::istream& is) {

            // get id and type
            int16_t parameter_id = readFromStream(is, parameter_id);

            if (is.eof()) {
                return nullptr;
            }

            // get parameter type_id
            datatype_t type_id = static_cast<datatype_t>(is.get());

            // validity check
            if (is.eof() ||
                type_id < 0 ||
                type_id >= DATATYPE_MAX_)
            {
                // fail
                return nullptr;
            }

            //
            ParameterPtr param;

            // handle certain datatypes
            if (type_id == DATATYPE_RANGE) {

                // get subtype
                datatype_t element_type_id = static_cast<datatype_t>(is.get());

                param = ParameterFactory::createRangeParameter(parameter_id, element_type_id);
                if (!param) {
                    std::cerr << "could not create rangeparameter with element_type: " << element_type_id << "\n";
                    return nullptr;
                }

                param->getTypeDefinition().parseOptions(is);

            } else if (type_id == DATATYPE_ARRAY) {
                // TODO
            } else if (type_id == DATATYPE_LIST) {
                // TODO
            } else {

                param = ParameterFactory::createParameter(parameter_id, type_id);

                if (!param) {
                    return nullptr;
                }

                param->getTypeDefinition().parseOptions(is);
            }

            if (param) {
                param->parseOptions(is);
            }

            return param;
        }
    };

}

#endif

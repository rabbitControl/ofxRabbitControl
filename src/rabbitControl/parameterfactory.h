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

#ifndef RCP_PARAMETERFACTORY_H
#define RCP_PARAMETERFACTORY_H

#include <inttypes.h>

#include "types.h"
#include "iparameter.h"
#include "parameter_range.h"
#include "parameter_custom.h"

namespace rcp {

    class ParameterFactory
    {
    public:
        template<typename T>
        static ParameterPtr readValue(const T& p, std::istream& is) {
            p->setValue(p->getDefaultTypeDefinition().readValue(is));
            return p;
        }

        static ParameterPtr createParameter(int16_t parameter_id, datatype_t type_id);
        static ParameterPtr createParameterReadValue(int16_t parameter_id, datatype_t type_id, std::istream& is);


        template<typename T>
        static ParameterPtr createRangeParameter(int16_t id, T type) {
            return std::make_shared<RangeParameter<T> >(id);
        }

        static ParameterPtr createRangeParameter(int16_t parameter_id, datatype_t type_id);
        static ParameterPtr createRangeParameterReadValue(int16_t parameter_id, datatype_t type_id, std::istream& is);
    };

}

#endif

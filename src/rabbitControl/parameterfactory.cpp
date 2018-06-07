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

#include "parameterfactory.h"

#include "parameter_range.h"
#include "parameter_custom.h"

namespace rcp {

    ParameterPtr ParameterFactory::createParameter(int16_t parameter_id, datatype_t type_id) {

        switch (type_id) {
        case DATATYPE_BOOLEAN:
            return BooleanParameter::create(parameter_id);

        case DATATYPE_INT8:
            return Int8Parameter::create(parameter_id);

        case DATATYPE_UINT8:
            return UInt8Parameter::create(parameter_id);

        case DATATYPE_INT16:
            return Int16Parameter::create(parameter_id);

        case DATATYPE_UINT16:
            return UInt16Parameter::create(parameter_id);

        case DATATYPE_INT32:
            return Int32Parameter::create(parameter_id);

        case DATATYPE_UINT32:
            return UInt32Parameter::create(parameter_id);

        case DATATYPE_INT64:
            return Int64Parameter::create(parameter_id);

        case DATATYPE_UINT64:
            return UInt64Parameter::create(parameter_id);

        case DATATYPE_FLOAT32:
            return Float32Parameter::create(parameter_id);

        case DATATYPE_FLOAT64:
            return Float64Parameter::create(parameter_id);

        case DATATYPE_RANGE:
            //return RangeParameter::create(parameter_id);

        case DATATYPE_STRING:
            return StringParameter::create(parameter_id);


        case DATATYPE_ENUM:
            return EnumParameter::create(parameter_id);


        case DATATYPE_RGB:
            return RGBParameter::create(parameter_id);

        case DATATYPE_RGBA:
            return RGBAParameter::create(parameter_id);


        case DATATYPE_URI:
            return URIParameter::create(parameter_id);

        case DATATYPE_IPV4:
            return IPv4Parameter::create(parameter_id);

        case DATATYPE_IPV6:
            return IPv6Parameter::create(parameter_id);


        case DATATYPE_BANG:
            return BangParameter::create(parameter_id);

        case DATATYPE_GROUP:
            return GroupParameter::create(parameter_id);

        case DATATYPE_VECTOR2I32:
        case DATATYPE_VECTOR2F32:
        case DATATYPE_VECTOR3I32:
        case DATATYPE_VECTOR3F32:
        case DATATYPE_VECTOR4I32:
        case DATATYPE_VECTOR4F32:

        case DATATYPE_CUSTOMTYPE:

        default:
            break;
        }

        return nullptr;
    }

    ParameterPtr ParameterFactory::createRangeParameter(int16_t parameter_id, datatype_t type_id) {

        switch (type_id) {
        case DATATYPE_INT8:
            return RangeParameter<int8_t>::create(parameter_id);

        case DATATYPE_UINT8:
            return RangeParameter<uint8_t>::create(parameter_id);

        case DATATYPE_INT16:
            return RangeParameter<int16_t>::create(parameter_id);

        case DATATYPE_UINT16:
            return RangeParameter<uint16_t>::create(parameter_id);

        case DATATYPE_INT32:
            return RangeParameter<int32_t>::create(parameter_id);

        case DATATYPE_UINT32:
            return RangeParameter<uint32_t>::create(parameter_id);

        case DATATYPE_INT64:
            return RangeParameter<int64_t>::create(parameter_id);

        case DATATYPE_UINT64:
            return RangeParameter<uint64_t>::create(parameter_id);

        case DATATYPE_FLOAT32:
            return RangeParameter<float>::create(parameter_id);

        case DATATYPE_FLOAT64:
            return RangeParameter<double>::create(parameter_id);

        default:
            // error...?
            break;
        }

        return nullptr;
    }
}
